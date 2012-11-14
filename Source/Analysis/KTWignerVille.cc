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
            fWVSignal()
    {
        fConfigName = "wigner-ville";

        RegisterSignal("wigner-ville", &fWVSignal, "void (const KTWriteableData*)");

        RegisterSlot("header", this, &KTWignerVille::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTWignerVille::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        //RegisterSlot("fs-data", this, &KTWignerVille::ProcessFrequencySpectrumData, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSlot("event", this, &KTWignerVille::ProcessEvent, "void (KTEvent*)");
    }

    KTWignerVille::~KTWignerVille()
    {
        delete fFullFFT;
    }

    Bool_t KTWignerVille::Configure(const KTPStoreNode* node)
    {
        SetSaveAAFrequencySpectrum(node->GetData< Bool_t >("save-frequency-spectrum", fSaveAAFrequencySpectrum));
        SetSaveAnalyticAssociate(node->GetData< Bool_t >("save-analytic-associate", fSaveAnalyticAssociate));
        SetSaveCrossMultipliedTimeSeries(node->GetData< Bool_t >("save-cross-multiplied-time-series", fSaveCrossMultipliedTimeSeries));

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

        const KTPStoreNode* fftNode = node->GetChild("complex-fft");
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

    KTSlidingWindowFSDataFFTW* KTWignerVille::TransformData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData, KTTimeSeriesData** outputAAData, KTTimeSeriesData** outputCMTSData)
    {
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

    void KTWignerVille::ProcessHeader(const KTEggHeader* header)
    {
        fFullFFT->ProcessHeader(header);
        fWindowedFFT->ProcessHeader(header);
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
    void KTWignerVille::ProcessEvent(KTEvent* event)
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

} /* namespace Katydid */
