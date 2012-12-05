/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVille.hh"

#include "KTComplexFFTW.hh"
#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFFTW.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTTimeSeriesPairedData.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <set>

using std::map;
using std::set;
using std::string;

using boost::shared_ptr;

// I can't just use boost::spirit::qi because of naming conflicts with std
using boost::spirit::qi::int_;
// I had to take this out because of a naming conflict with boost::bind
//using boost::spirit::qi::_1;
using boost::spirit::qi::phrase_parse;
using boost::spirit::ascii::space;
using boost::phoenix::ref;


namespace Katydid
{
    KTLOGGER(wvlog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTWignerVille > sWVRegistrar("wigner-ville");

    KTWignerVille::KTWignerVille() :
            KTFFT(),
            KTProcessor(),
            fFullFFT(new KTComplexFFTW()),
            fWindowedFFT(new KTSlidingWindowFFTW()),
            fSaveAAFrequencySpectrum(false),
            fSaveAnalyticAssociate(false),
            fSaveCrossMultipliedTimeSeries(false),
            fInputDataName("time-series"),
            fOutputDataName("wigner-ville"),
            fAAFSOutputDataName("aafs-wigner-ville"),
            fAATSOutputDataName("aats-wigner-ville"),
            fCMTSOutputDataName("cmts-wigner-ville"),
            fFTPlan(NULL),
            fInputArray(NULL),
            fOutputArray(NULL),
            fTransformFlag("MEASURE"),
            fIsInitialized(kFALSE),
            fOverlap(0),
            fOverlapFrac(0.),
            fUseOverlapFrac(kFALSE),
            fWindowFunction(NULL),
            fWVSignal()
    {
        fConfigName = "wigner-ville";

        RegisterSignal("wigner-ville", &fWVSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTWignerVille::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTWignerVille::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        //RegisterSlot("fs-data", this, &KTWignerVille::ProcessFrequencySpectrumData, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSlot("event", this, &KTWignerVille::ProcessEvent, "void (KTEvent*)");

        SetupTransformFlagMap();
    }

    KTWignerVille::~KTWignerVille()
    {
        delete fFullFFT;
        fftw_destroy_plan(fFTPlan);
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
        delete fWindowFunction;
        delete fWindowedFFT;
    }

    Bool_t KTWignerVille::Configure(const KTPStoreNode* node)
    {
        SetSaveAAFrequencySpectrum(node->GetData< Bool_t >("save-frequency-spectrum", fSaveAAFrequencySpectrum));
        SetSaveAnalyticAssociate(node->GetData< Bool_t >("save-analytic-associate", fSaveAnalyticAssociate));
        SetSaveCrossMultipliedTimeSeries(node->GetData< Bool_t >("save-cross-multiplied-time-series", fSaveCrossMultipliedTimeSeries));

        if (node->HasData("overlap-time")) SetOverlap(node->GetData< Double_t >("overlap-time", 0));
        if (node->HasData("overlap-size")) SetOverlap(node->GetData< UInt_t >("overlap-size", 0));
        if (node->HasData("overlap-frac")) SetOverlapFrac(node->GetData< Double_t >("overlap-frac", 0.));

        string windowType = node->GetData< string >("window-function-type", "rectangular");
        KTEventWindowFunction* tempWF = KTFactory< KTEventWindowFunction >::GetInstance()->Create(windowType);
        if (tempWF == NULL)
        {
            KTERROR(fftlog_sw_fftw, "Invalid window function type given: <" << windowType << ">.");
            return false;
        }
        SetWindowFunction(tempWF);

        const KTPStoreNode* childNode = node->GetChild("window-function");
        if (childNode != NULL)
        {
            fWindowFunction->Configure(childNode);
        }

        SetInputDataName(node->GetData< string >("input-data-name", fInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

        SetAAFSOutputDataName(node->GetData< string >("aa-ts-output-data-name", fAAFSOutputDataName));
        SetAATSOutputDataName(node->GetData< string >("aa-fs-output-data-name", fAATSOutputDataName));
        SetCMTSOutputDataName(node->GetData< string >("cm-ts-output-data-name", fCMTSOutputDataName));

        KTPStoreNode::csi_pair itPair = node->EqualRange("wv-pair");
        for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; citer++)
        {
            string pairString(citer->second.get_value< string >());
            UInt_t first = 0, second = 0;
            Bool_t parsed = phrase_parse(pairString.begin(), pairString.end(),
                    (int_[ref(first)=boost::spirit::qi::_1] >> ',' >> int_[ref(second) = boost::spirit::qi::_1]),
                    space);
            if (! parsed)
            {
                KTWARN(wvlog, "Unable to parse WV pair: " << pairString);
                continue;
            }
            KTINFO(wvlog, "Adding WV pair " << first << ", " << second);
            this->AddPair(KTWVPair(first, second));
        }

        const KTPStoreNode* fftNode = node->GetChild("complex-fftw");
        if (fftNode != NULL)
        {
            delete fFullFFT;
            fFullFFT = new KTComplexFFTW();
            if (! fFullFFT->Configure(fftNode)) return false;
        }

        fftNode = node->GetChild("sliding-window-fftw");
        if (fftNode != NULL)
        {
            delete fWindowedFFT;
            fWindowedFFT = new KTSlidingWindowFFTW();
            if (! fWindowedFFT->Configure(fftNode)) return false;
        }

        return true;
    }

    void KTWignerVille::InitializeFFT()
    {
        if (fWindowFunction == NULL)
        {
            KTERROR(wvlog, "No window function has been set. The FFT has not been initialized.");
            return;
        }

        // fTransformFlag is guaranteed to be valid in the Set method.
        KTDEBUG(wvlog, "Transform flag: " << fTransformFlag);
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find(fTransformFlag);
        Int_t transformFlag = iter->second;

        KTDEBUG(wvlog, "Creating plan: " << fWindowFunction->GetSize() << " bins; forward FFT");
        if (fFTPlan != NULL)
            fftw_destroy_plan(fFTPlan);
        fFTPlan = fftw_plan_dft_1d(fWindowFunction->GetSize(), fInputArray, fOutputArray, FFTW_FORWARD, transformFlag);
        if (fFTPlan != NULL)
        {
            KTDEBUG(wvlog, "FFTW plan created");
            fIsInitialized = true;
        }
        else
        {
            KTWARN(wvlog, "Unable to create FFTW plan!");
            fIsInitialized = false;
        }
        return;
    }



    KTSlidingWindowFSDataFFTW* KTWignerVille::TransformData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData, KTTimeSeriesData** outputAAData, KTTimeSeriesData** outputCMTSData)
    {
        if (! GetIsInitialized())
        {
            KTERROR(wvlog, "The FFT for the Wigner-Ville transform is not initialized! Aborting.");
            return NULL;
        }

        // # of time bins in each windowed FFT, and the number of bins in the data
        UInt_t windowSize = fWindowFunction->GetSize();
        UInt_t dataSize = data->GetTimeSeries(0)->GetNTimeBins();
        if (windowSize >= dataSize)
        {
            KTERROR(wvlog, "Window size is larger than time data: " << windowSize << " > " << dataSize << "\n" <<
                    "No transform was performed!");
            return NULL;
        }

        if (fFullFFT == NULL)
        {
            KTERROR(wvlog, "Full FFT is not initialized; cannot perform the transform.");
            return NULL;
        }
        if (! fFullFFT->GetIsInitialized())
        {
            fFullFFT->InitializeFFT();
            if (! fFullFFT->GetIsInitialized())
            {
                KTERROR(wvlog, "Unable to initialize full FFT.");
                return NULL;
            }
        }

        /*
        if (fWindowedFFT == NULL)
        {
            KTERROR(wvlog, "Windowed FFT is not present; cannot perform the transform.");
            return NULL;
        }
        if (! fWindowedFFT->GetIsInitialized())
        {
            fWindowedFFT->InitializeFFT();
            if (! fWindowedFFT->GetIsInitialized())
            {
                KTERROR(wvlog, "Unable to initialize windowed FFT.");
                return NULL;
            }
        }
         */

        if (fPairs.empty())
        {
            KTWARN(wvlog, "No Wigner-Ville pairs specified; no transforms performed.");
            return NULL;
        }

        if (fSaveAAFrequencySpectrum && outputFSData == NULL)
        {
            KTWARN(wvlog, "The flag for saving the AA frequency spectrum is set, but no KTFrequencySpectrumDataFFTW** was provided;\n"
                    << "\tThe frequency spectrum will not be saved."
                    << "\tfSaveFrequencySpectrum is being set to false");
            fSaveAAFrequencySpectrum = false;
        }

        if (fSaveAnalyticAssociate && outputAAData == NULL)
        {
            KTWARN(wvlog, "The flag for saving the analytic associate is set, but no KTTimeSeriesData** was provided;\n"
                    << "\tThe analytic associate will not be saved."
                    << "\tfSaveAnalyticAssociate is being set to false");
            fSaveAnalyticAssociate = false;
        }

        if (fSaveCrossMultipliedTimeSeries && outputCMTSData == NULL)
        {
            KTWARN(wvlog, "The flag for saving the cross-multiplied time series is set, but no KTTimeSeriesData** was provided;\n"
                    << "\tThe cross-multiplied time series will not be saved."
                    << "\tfSaveCrossMultipliedTimeSeries is being set to false");
            fSaveCrossMultipliedTimeSeries = false;
        }

        // make a set of the channel numbers in use so that we can make the analytic associates of only the channels we need
        set< UInt_t > channelsInUse;
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            channelsInUse.insert((*pairIt).first);
            channelsInUse.insert((*pairIt).second);
        }

        // New data to hold the time series of the analytic associate
        KTBasicTimeSeriesData* aaTSData = NULL;
        if (fSaveAnalyticAssociate)
        {
            aaTSData = new KTBasicTimeSeriesData(data->GetNTimeSeries());
            (*outputAAData) = (KTTimeSeriesData*)aaTSData;
        }

        KTFrequencySpectrumDataFFTW* aaFSData = NULL;
        if (fSaveAAFrequencySpectrum)
        {
            aaFSData = new KTFrequencySpectrumDataFFTW(data->GetNTimeSeries());
            aaFSData->SetName(fAAFSOutputDataName);
            (*outputFSData) = aaFSData;
        }

        // Calculate the analytic associates
        // Use this map to hold pointers to them so we don't have to dynamic cast later.
        map< UInt_t, const KTTimeSeriesFFTW* > channelAAs;
        for (set< UInt_t >::const_iterator channelIt = channelsInUse.begin(); channelIt != channelsInUse.end(); channelIt++)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(data->GetTimeSeries(*channelIt));
            if (nextInput == NULL)
            {
                KTERROR(wvlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                delete aaTSData;
                return NULL;
            }

            KTFrequencySpectrumFFTW* newFS = NULL;
            KTTimeSeriesFFTW* newTS = NULL;
            if (fSaveAAFrequencySpectrum)
            {
                newTS = CalculateAnalyticAssociate(nextInput, &newFS);
                aaFSData->SetSpectrum(newFS, *channelIt);
            }
            else
            {
                newTS = CalculateAnalyticAssociate(nextInput);
            }

            if (newTS == NULL)
            {
                KTERROR(wvlog, "Channel <" << *channelIt << "> did not transform correctly.");
                delete aaTSData;
                return NULL;
            }

            if (fSaveAnalyticAssociate)
                aaTSData->SetTimeSeries(newTS, *channelIt);
            channelAAs[*channelIt] = newTS;
        }




        // Characteristics of the whole windowed FFT
        UInt_t windowShift = windowSize - GetEffectiveOverlap();
        UInt_t nWindows = (dataSize - windowSize) / windowShift + 1; // integer arithmetic gets truncated to the nearest integer
        UInt_t nTimeBinsUsed = windowSize + (nWindows - 1) * windowShift;
        UInt_t nTimeBinsNotUsed = dataSize - nTimeBinsUsed;

        // Characteristics of the frequency spectrum
        Double_t timeBinWidth = data->GetTimeSeries(0)->GetTimeBinWidth();
        Double_t freqMin = GetMinFrequency(timeBinWidth);
        Double_t freqMax = GetMaxFrequency(timeBinWidth);

        Double_t timeMin = 0.;
        Double_t timeMax = nTimeBinsUsed * timeBinWidth;

        KTDEBUG(wvlog, "Performing windowed FFT characteristics:\n"
                << "\tWindow size: " << windowSize << '\n'
                << "\tWindow shift: " << windowShift << '\n'
                << "\t# of windows: " << nWindows << '\n'
                << "\t# of unused bins: " << nTimeBinsNotUsed)

        KTSlidingWindowFSDataFFTW* newData = new KTSlidingWindowFSDataFFTW(fPairs.size());

        // Do WV transform for each pair
        UInt_t iPair = 0;
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            UInt_t firstChannel = (*pairIt).first;
            UInt_t secondChannel = (*pairIt).second;

            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >(nWindows, timeMin, timeMax);

            KTDEBUG(wvlog, "Performing windowed FFT for channels " << firstChannel << " and " << secondChannel)

            UInt_t windowStart = 0;
            for (UInt_t iWindow = 0; iWindow < nWindows; iWindow++)
            {
                //KTDEBUG(wvlog, "Window: " << iWindow << "; first bin: " << windowStart);
                CrossMultiplyToInputArray(channelAAs[firstChannel], channelAAs[secondChannel], windowStart);
                fftw_execute(fFTPlan);
                (*newSpectra)(iWindow) = ExtractTransformResult(freqMin, freqMax);
                windowStart += windowShift;
            }
            KTINFO(wvlog, "Windowed FFT complete (channels " << firstChannel << " and " << secondChannel << "); windows used: " << nWindows << "; time bins not used: " << nTimeBinsNotUsed);
            newData->SetSpectra(newSpectra, iPair);
            iPair++;
        }

        newData->SetEvent(data->GetEvent());
        newData->SetName(fOutputDataName);

        fWVSignal(newData);

        return newData;





        /*
        // new KTTimeSeriesPairedData to hold the results of the cross multiplication
        KTTimeSeriesPairedData* crossMultipliedData = new KTTimeSeriesPairedData(fPairs.size());
        if (fSaveCrossMultipliedTimeSeries)
        {
            crossMultipliedData->SetName(fAATSOutputDataName);
            (*outputCMTSData) = (KTTimeSeriesData*)crossMultipliedData;
        }

        // Cross-multiply pairs of channels
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            UInt_t firstChannel = (*pairIt).first;
            UInt_t secondChannel = (*pairIt).second;

            KTTimeSeriesFFTW* newTS = CrossMultiply(channelAAs[firstChannel], channelAAs[secondChannel]);
            if (newTS == NULL)
            {
                KTERROR(wvlog, "Something went wrong in the cross multiplication of channels <" << firstChannel << "> and < " << secondChannel << ">.");
                delete aaTSData;
                delete crossMultipliedData;
                return NULL;
            }

            crossMultipliedData->SetTimeSeries(newTS, firstChannel, secondChannel);
        }

        // Windowed FFT
        KTSlidingWindowFSDataFFTW* newSWFSData = fWindowedFFT->TransformData(crossMultipliedData);

        if (newSWFSData == NULL)
        {
            KTERROR(wvlog, "Something went wrong in the windowed FFT.");
            delete crossMultipliedData;
            return NULL;
        }

        if (! fSaveCrossMultipliedTimeSeries)
        {
            delete crossMultipliedData;
        }
        else
        {
            crossMultipliedData->SetName(fCMTSOutputDataName);
        }

        newSWFSData->SetEvent(data->GetEvent());
        newSWFSData->SetName(fOutputDataName);

        KTDEBUG(fftlog_comp, "W-V transform complete; " << newSWFSData->GetNChannels() << " pair(s) transformed");

        fWVSignal(newSWFSData);

        return newSWFSData;
         */
    }
    /*
    KTTimeSeriesFFTW* KTWignerVille::Transform(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS)
    {
        if (fFullFFT == NULL)
        {
            KTERROR(wvlog, "FFT is not initialized; cannot perform a transform on time series data.");
            return NULL;
        }

        if (fSaveFrequencySpectrum && outputFS == NULL)
        {
            KTWARN(wvlog, "The flag for saving the frequency spectrum is set, but no KTFrequencySpectrumFFTW** was provide;\n"
                    << "\tThe frequency spectrum will not be saved."
                    << "\tfSaveFrequencySpectrum is being set to false");
            fSaveFrequencySpectrum = false;
        }
        else if (! fSaveFrequencySpectrum && outputFS != NULL)
        {
            KTWARN(wvlog, "A KTFrequencySpectrumDataFFTW** was supplied to store the intermediate frequency spectrum, but fSaveFrequencySpectrum is false."
                    << "\tTo avoid a potential memory leak, the frequency spectra will not be saved.");
        }

        // Calculate the analytic associate
        KTFrequencySpectrumFFTW* tempOutputFS = NULL;
        KTTimeSeriesFFTW* outputTS = CalculateAnalyticAssociate(inputTS, &tempOutputFS);

        // Delete or reassign the intermediate FS
        if (outputFS == NULL) delete tempOutputFS;
        else outputFS = &tempOutputFS;

        return outputTS;
    }

    Bool_t KTWignerVille::Transform(KTFrequencySpectrumFFTW* freqSpectrum)
    {
        if (freqSpectrum == NULL)
        {
            KTERROR(wvlog, "Input frequency spectrum was NULL");
            return false;
        }

        return CalculateAnalyticAssociate(freqSpectrum);
    }
     */

    KTTimeSeriesFFTW* KTWignerVille::CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS)
    {
        // Forward FFT
        KTFrequencySpectrumFFTW* freqSpec = fFullFFT->Transform(inputTS);
        if (freqSpec == NULL)
        {
            KTERROR(wvlog, "Something went wrong with the forward FFT on the time series.");
            return NULL;
        }
        // copy the address of the frequency spectrum to outputFS
        outputFS = &freqSpec;

        // perform the actual W-V transform
        if (! CalculateAnalyticAssociate(freqSpec))
        {
            KTERROR(wvlog, "Something went wrong with the W-V transform of the frequency spectrum.");
            if (outputFS == NULL) delete freqSpec;
            return NULL;
        }

        // reverse FFT
        KTTimeSeriesFFTW* outputTS = fFullFFT->Transform(freqSpec);
        if (outputTS == NULL)
        {
            KTERROR(wvlog, "Something went wrong with the reverse FFT on the frequency spectrum.");
            if (outputFS == NULL) delete freqSpec;
            return NULL;
        }

        return outputTS;
    }


    Bool_t KTWignerVille::CalculateAnalyticAssociate(KTFrequencySpectrumFFTW* freqSpectrum)
    {
        // Note: the data storage array is accessed directly, so the FFTW data storage format is used.
        // Nyquist bin(s) and negative frequency bins are set to 0 (from size/2 to the end of the array)
        // DC bin stays as is (array position 0).
        // Positive frequency bins are multiplied by 2 (from array position 1 to size/2).
        fftw_complex* data = freqSpectrum->GetData();
        UInt_t arraySize = freqSpectrum->size();
        UInt_t nyquistPos = arraySize / 2; // either the sole nyquist bin (if even # of bins) or the first of the two (if odd # of bins; bins are sequential in the array).
        for (UInt_t arrayPos=1; arrayPos<nyquistPos; arrayPos++)
        {
            data[arrayPos][0] = data[arrayPos][0] * 2.;
            data[arrayPos][1] = data[arrayPos][1] * 2.;
        }
        for (UInt_t arrayPos=nyquistPos; arrayPos<arraySize; arrayPos++)
        {
            data[arrayPos][0] = 0.;
            data[arrayPos][1] = 0.;
        }
        return true;
    }

    KTTimeSeriesFFTW* KTWignerVille::CrossMultiply(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2)
    {
        KTTimeSeriesFFTW* product = new KTTimeSeriesFFTW(*data1);

        UInt_t size = data1->size();
        UInt_t iBin2 = size - 1;
        for (UInt_t iBin1 = 0; iBin1 < size; iBin1++)
        {
            // data1 * CC(data2)
            (*product)(iBin1)[0] = (*data1)(iBin1)[0] * (*data2)(iBin2)[0] + (*data1)(iBin1)[1] * (*data2)(iBin2)[1];
            (*product)(iBin1)[1] = (*data1)(iBin1)[0] * (*data2)(iBin2)[1] - (*data1)(iBin1)[1] * (*data2)(iBin2)[0];
            iBin2--;
        }

        return product;
    }

    void KTWignerVille::CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset)
    {
        UInt_t size = GetSize();
        UInt_t iPoint1 = offset;
        UInt_t iPoint2 = size - 1 + offset;
        for (UInt_t inPoint = 0; inPoint < size; inPoint++)
        {
            fInputArray[inPoint][0] = (*data1)(iPoint1)[0] * (*data2)(iPoint2)[0] + (*data1)(iPoint1)[1] * (*data2)(iPoint2)[1];
            fInputArray[inPoint][1] = (*data1)(iPoint1)[0] * (*data2)(iPoint2)[1] - (*data1)(iPoint1)[1] * (*data2)(iPoint2)[0];
            iPoint1++;
            iPoint2--;
        }
        return;
    }


    KTFrequencySpectrumFFTW* KTWignerVille::ExtractTransformResult(Double_t freqMin, Double_t freqMax) const
    {
        UInt_t freqSize = GetFrequencySize();
        Double_t normalization = sqrt(2. / (Double_t)GetTimeSize());

        KTFrequencySpectrumFFTW* newSpect = new KTFrequencySpectrumFFTW(freqSize, freqMin, freqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            (newSpect->GetData())[iPoint][0] = fOutputArray[iPoint][0] * normalization;
            (newSpect->GetData())[iPoint][1] = fOutputArray[iPoint][1] * normalization;
        }

        return newSpect;
    }

    void KTWignerVille::ProcessHeader(const KTEggHeader* header)
    {
        fFullFFT->ProcessHeader(header);

        fWindowFunction->SetBinWidth(1. / header->GetAcquisitionRate());
        RecreateFFT();
        InitializeFFT();

        //fWindowedFFT->ProcessHeader(header);
        return;
    }

    void KTWignerVille::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        //Pass these pointers in case the user wants to save these data.
        KTFrequencySpectrumDataFFTW* saveAAFreqSpec = NULL;
        KTTimeSeriesData* saveAA = NULL;
        KTTimeSeriesData* saveCMTS = NULL;

        KTSlidingWindowFSDataFFTW* newData = TransformData(tsData, &saveAAFreqSpec, &saveAA, &saveCMTS);

        if (newData == NULL)
        {
            KTERROR(wvlog, "Unable to transform data");
            return;
        }

        KTEvent* event = tsData->GetEvent();
        if (event != NULL)
        {
            event->AddData(newData);

            if (fSaveAAFrequencySpectrum) event->AddData(saveAAFreqSpec);
            else delete saveAAFreqSpec;

            if (fSaveAnalyticAssociate) event->AddData(saveAA);
            else delete saveAA;

            if (fSaveCrossMultipliedTimeSeries) event->AddData(saveCMTS);
            else delete saveCMTS;
        }

        return;
    }
    /*
    void KTWignerVille::ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData)
    {
        KTSlidingWindowFSDataFFTW* newData = TransformData(fsData);
        if (fsData->GetEvent() != NULL)
            fsData->GetEvent()->AddData(newData);
        return;
    }
     */
    void KTWignerVille::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTTimeSeriesData* tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(fInputDataName));
        if (tsData == NULL)
        {
            KTWARN(wvlog, "No time series data named <" << fInputDataName << "> was available in the event");
            return;
        }

        ProcessTimeSeriesData(tsData);
        return;
    }

    KTComplexFFTW* KTWignerVille::GetFullFFT() const
    {
        return fFullFFT;
    }

    KTSlidingWindowFFTW* KTWignerVille::GetWindowedFFT() const
    {
        return fWindowedFFT;
    }

    void KTWignerVille::RecreateFFT()
    {
        if (fWindowFunction == NULL)
        {
            KTERROR(fftlog_sw_fftw, "No window function has been set. The FFT has not been recreated.");
            return;
        }

        fftw_destroy_plan(fFTPlan);
        fFTPlan = NULL;
        fftw_free(fInputArray);
        fftw_free(fOutputArray);
        fInputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fWindowFunction->GetSize());
        fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fWindowFunction->GetSize());
        fIsInitialized = false;
    }

    void KTWignerVille::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(wvlog, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }
        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    void KTWignerVille::SetWindowSize(UInt_t nBins)
    {
        if (fWindowFunction == NULL)
        {
            KTERROR(fftlog_sw_fftw, "Window function has not been set.");
            return;
        }
        fWindowFunction->SetSize(nBins);
        RecreateFFT();
        return;
    }

    void KTWignerVille::SetWindowLength(Double_t wlTime)
    {
        if (fWindowFunction == NULL)
        {
            KTERROR(fftlog_sw_fftw, "Window function has not been set.");
            return;
        }
        fWindowFunction->SetLength(wlTime);
        RecreateFFT();
        return;
    }

    void KTWignerVille::SetWindowFunction(KTEventWindowFunction* wf)
    {
        delete fWindowFunction;
        fWindowFunction = wf;
        RecreateFFT();
        return;
    }

    void KTWignerVille::SetupTransformFlagMap()
    {
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }


} /* namespace Katydid */
