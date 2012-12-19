/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVille.hh"

#include "KTCacheDirectory.hh"
#include "KTComplexFFTW.hh"
#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFFTW.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTTimeSeriesPairedData.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <set>

#include <iostream>

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
            fInputDataName("time-series"),
            fOutputDataName("wigner-ville"),
            fFTPlan(NULL),
            fInputArray(NULL),
            fOutputArray(NULL),
            fTransformFlag("MEASURE"),
            fIsInitialized(kFALSE),
            fUseWisdom(true),
            fWisdomFilename("wisdom_wignerville.fftw3"),
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
        RegisterSlot("event", this, &KTWignerVille::ProcessEvent, "void (KTEvent*)");

        SetupTransformFlagMap();
    }

    KTWignerVille::~KTWignerVille()
    {
        fftw_destroy_plan(fFTPlan);
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
        delete fWindowFunction;
    }

    Bool_t KTWignerVille::Configure(const KTPStoreNode* node)
    {
        SetTransformFlag(node->GetData< string >("transform-flag", fTransformFlag));

        SetUseWisdom(node->GetData<Bool_t>("use-wisdom", fUseWisdom));
        SetWisdomFilename(node->GetData<string>("wisdom-filename", fWisdomFilename));

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

        if (fUseWisdom)
        {
            if (! KTCacheDirectory::GetInstance()->PrepareForUse())
            {
                KTWARN(wvlog, "Unable to use wisdom because cache directory is not ready.");
                fUseWisdom = false;
            }
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

        if (fUseWisdom)
        {
            KTDEBUG(wvlog, "Reading wisdom from file <" << fWisdomFilename << ">");
            if (fftw_import_wisdom_from_filename(fWisdomFilename.c_str()) == 0)
            {
                KTWARN(wvlog, "Unable to read FFTW wisdom from file <" << fWisdomFilename << ">");
            }
        }

        KTDEBUG(wvlog, "Creating plan: " << fWindowFunction->GetSize() << " bins; forward FFT");
        if (fFTPlan != NULL)
            fftw_destroy_plan(fFTPlan);
        fFTPlan = fftw_plan_dft_1d(GetTimeSize(), fInputArray, fOutputArray, FFTW_FORWARD, transformFlag);
        if (fFTPlan != NULL)
        {
            KTDEBUG(wvlog, "FFTW plan created");
            fIsInitialized = true;
            if (fUseWisdom)
            {
                if (fftw_export_wisdom_to_filename(fWisdomFilename.c_str()) == 0)
                {
                    KTWARN(wvlog, "Unable to write FFTW wisdom to file <" << fWisdomFilename << ">");
                }
            }
        }
        else
        {
            KTWARN(wvlog, "Unable to create FFTW plan!");
            fIsInitialized = false;
        }
        return;
    }



    KTSlidingWindowFSDataFFTW* KTWignerVille::TransformData(const KTTimeSeriesData* data)
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

        if (fPairs.empty())
        {
            KTWARN(wvlog, "No Wigner-Ville pairs specified; no transforms performed.");
            return NULL;
        }

        // cast all time series into KTTimeSeriesFFTW
        vector< const KTTimeSeriesFFTW* > timeSeries(data->GetNTimeSeries());
        for (UInt_t iTS=0; iTS < timeSeries.size(); iTS++)
        {
            timeSeries[iTS] = dynamic_cast< const KTTimeSeriesFFTW* >(data->GetTimeSeries(iTS));
            if (timeSeries[iTS] == NULL)
            {
                KTERROR(wvlog, "Time series " << iTS << " did not cast to a const KTTimeSeriesFFTW*. No transforms performed.");
                return NULL;
            }
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

        //KTSlidingWindowFSData* newData = new KTSlidingWindowFSData(fPairs.size());
        KTSlidingWindowFSDataFFTW* newData = new KTSlidingWindowFSDataFFTW(fPairs.size());

        // Do WV transform for each pair
        UInt_t iPair = 0;
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            UInt_t firstChannel = (*pairIt).first;
            UInt_t secondChannel = (*pairIt).second;

            //KTPhysicalArray< 1, KTFrequencySpectrum* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrum* >(nWindows, timeMin, timeMax);
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >(nWindows, timeMin, timeMax);

            KTDEBUG(wvlog, "Performing windowed FFT for channels " << firstChannel << " and " << secondChannel)

            UInt_t windowStart = 0;
            for (UInt_t iWindow = 0; iWindow < nWindows; iWindow++)
            {
                //KTDEBUG(wvlog, "Window: " << iWindow << "; first bin: " << windowStart);
                CrossMultiplyToInputArray(timeSeries[firstChannel], timeSeries[secondChannel], windowStart);
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

    }

    void KTWignerVille::CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset)
    {
        UInt_t size = GetTimeSize();
        UInt_t iPoint1 = offset;
        UInt_t iPoint2 = size - 1 + offset;
        /*
        fInputArray[0][0] = (*data1)(offset)[0] * (*data2)(size - 1 + offset)[0] + (*data1)(offset)[1] * (*data2)(size - 1 + offset)[1];
        fInputArray[0][1] = (*data1)(offset)[1] * (*data2)(size - 1 + offset)[0] - (*data1)(offset)[0] * (*data2)(size - 1 + offset)[1];
        if (offset == 0)
        {
            std::cout << "0  " << offset << "  " << size - 1 + offset << "  --   " << fInputArray[0][0] << ", " << fInputArray[0][1] << std::endl;
        }
        UInt_t iPoint1 = offset + 1;
        UInt_t iPoint2 = size - 2 + offset;
        Double_t real1, real2, imag1, imag2;
        */
        //for (UInt_t inPoint = 2; inPoint < 2.*size; inPoint+=2)
        for (UInt_t inPoint = 0; inPoint < size; inPoint++)
        {
            /**/
            fInputArray[inPoint][0] = (*data1)(iPoint1)[0] * (*data2)(iPoint2)[0] + (*data1)(iPoint1)[1] * (*data2)(iPoint2)[1];
            fInputArray[inPoint][1] = (*data1)(iPoint1)[1] * (*data2)(iPoint2)[0] - (*data1)(iPoint1)[0] * (*data2)(iPoint2)[1];
            if (offset == 0)
            {
                std::cout << inPoint << "  " << iPoint1 << "  " << iPoint2 << "  --   " << fInputArray[inPoint][0] << ", " << fInputArray[inPoint][1] << std::endl;
            }
            /**/
            /*
            fInputArray[inPoint][0] = (*data1)(iPoint1)[0] * (*data2)(iPoint2)[0] + (*data1)(iPoint1)[1] * (*data2)(iPoint2)[1];
            fInputArray[inPoint][1] = (*data1)(iPoint1)[1] * (*data2)(iPoint2)[0] - (*data1)(iPoint1)[0] * (*data2)(iPoint2)[1];
            fInputArray[inPoint-1][0] = 0.5 * (fInputArray[inPoint-2][0] + fInputArray[inPoint][0]);
            fInputArray[inPoint-1][1] = 0.5 * (fInputArray[inPoint-2][1] + fInputArray[inPoint][1]);
            */
            /*
            real1 = (*data1)(iPoint1)[0];
            imag1 = (*data1)(iPoint1)[1];
            real2 = (*data2)(iPoint2)[0];
            imag2 = (*data2)(iPoint2)[1];
            fInputArray[inPoint][0] = real1 * real2 + imag1 * imag2;
            fInputArray[inPoint][1] = imag1 * real2 - real1 * imag2;
            real1 = 0.5 * ((*data1)(iPoint1-1)[0] + real1);
            imag1 = 0.5 * ((*data1)(iPoint1-1)[1] + imag1);
            real2 = 0.5 * ((*data2)(iPoint2+1)[0] + real2);
            imag2 = 0.5 * ((*data2)(iPoint2+1)[1] + imag2);
            fInputArray[inPoint-1][0] = real1 * real2 + imag1 * imag2;
            fInputArray[inPoint-1][1] = imag1 * real2 - real1 * imag2;
            if (offset == 0)
            {
                std::cout << " " << inPoint-1 << "  " << iPoint1 << "  " << iPoint2 << "  --   " << fInputArray[inPoint-1][0] << ", " << fInputArray[inPoint-1][1] << std::endl;
                std::cout << inPoint << "  " << iPoint1 << "  " << iPoint2 << "  --   " << fInputArray[inPoint][0] << ", " << fInputArray[inPoint][1] << std::endl;
            }
            */
            iPoint1++;
            iPoint2--;
        }
        return;
    }


    //KTFrequencySpectrum* KTWignerVille::ExtractTransformResult(Double_t freqMin, Double_t freqMax) const
    KTFrequencySpectrumFFTW* KTWignerVille::ExtractTransformResult(Double_t freqMin, Double_t freqMax) const
    {
        //UInt_t freqSize = GetFrequencySize();
        //Double_t normalization = 4. * sqrt(2. / (Double_t)GetTimeSize());
        UInt_t freqSize = GetFrequencySize();
        Double_t normalization = sqrt(2. / (Double_t)GetTimeSize());

        //KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(freqSize, freqMin, freqMax);
        KTFrequencySpectrumFFTW* newSpect = new KTFrequencySpectrumFFTW(freqSize, freqMin, freqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            (newSpect->GetData())[iPoint][0] = fOutputArray[iPoint][0] * normalization;
            (newSpect->GetData())[iPoint][1] = fOutputArray[iPoint][1] * normalization;
            //(newSpect->GetData())[iPoint].set_rect(fOutputArray[iPoint][0] * normalization, fOutputArray[iPoint][1] * normalization);
        }

        return newSpect;
    }

    void KTWignerVille::ProcessHeader(const KTEggHeader* header)
    {
        fWindowFunction->SetBinWidth(1. / header->GetAcquisitionRate());
        RecreateFFT();
        InitializeFFT();
        return;
    }

    void KTWignerVille::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTSlidingWindowFSDataFFTW* newData = TransformData(tsData);

        if (newData == NULL)
        {
            KTERROR(wvlog, "Unable to transform data");
            return;
        }

        KTEvent* event = tsData->GetEvent();
        if (event != NULL)
        {
            event->AddData(newData);
        }

        return;
    }

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
        fInputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * GetTimeSize());
        fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * GetTimeSize());
        /*
        for (UInt_t iPoint=0; iPoint < 2. * fWindowFunction->GetSize(); iPoint++)
        {
            fInputArray[iPoint][0] = 0.;
            fInputArray[iPoint][1] = 0.;
        }
        */
        /*
        for (UInt_t iPoint=0; iPoint < 2. * GetTimeSize(); iPoint++)
        {
            fOutputArray[iPoint][0] = 0.;
            fOutputArray[iPoint][1] = 0.;
        }
        */
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
