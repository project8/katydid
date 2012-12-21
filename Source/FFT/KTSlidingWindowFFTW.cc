/*
 * KTSlidingWindowFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFTW.hh"

#include "KTCacheDirectory.hh"
#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTTimeSeriesPairedData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTFactory.hh"
#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTWindowFunction.hh"

#include <cstring>

using std::string;
using std::vector;

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTSlidingWindowFFTW > sSWFFTRegistrar("sliding-window-fftw");

    KTSlidingWindowFFTW::KTSlidingWindowFFTW() :
            KTFFT(),
            KTProcessor(),
            fFTPlan(NULL),
            fInputArray(NULL),
            fOutputArray(NULL),
            fTransformFlag("MEASURE"),
            fIsInitialized(kFALSE),
            fUseWisdom(true),
            fWisdomFilename("wisdom_slidingwindowfftw.fftw3"),
            fOverlap(0),
            fOverlapFrac(0.),
            fUseOverlapFrac(kFALSE),
            fWindowFunction(NULL),
            fInputDataName("time-series"),
            fOutputDataName("sw-frequency-spectrum"),
            fSingleFFTSignal(),
            fFullFFTSignal()
    {
        fConfigName = "sliding-window-fftw";

        RegisterSignal("single_fft", &fSingleFFTSignal, "void (UInt_t, KTFrequencySpectrum*)");
        RegisterSignal("full_fft", &fFullFFTSignal, "void (KTWriteableData*)");

        RegisterSlot("header", this, &KTSlidingWindowFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("time-series", this, &KTSlidingWindowFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("event", this, &KTSlidingWindowFFTW::ProcessEvent, "void (KTEvent*)");

        SetupTransformFlagMap();
    }

    KTSlidingWindowFFTW::~KTSlidingWindowFFTW()
    {
        fftw_destroy_plan(fFTPlan);
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
        delete fWindowFunction;
    }

    Bool_t KTSlidingWindowFFTW::Configure(const KTPStoreNode* node)
    {
        // Config-file options
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

        if (fUseWisdom)
        {
            if (! KTCacheDirectory::GetInstance()->PrepareForUse())
            {
                KTWARN(fftlog_sw_fftw, "Unable to use wisdom because cache directory is not ready.");
                fUseWisdom = false;
            }
        }

        // No command-line options

        return true;
    }

    void KTSlidingWindowFFTW::ProcessHeader(const KTEggHeader* header)
    {
        fWindowFunction->SetBinWidth(1. / header->GetAcquisitionRate());
        RecreateFFT();
        InitializeFFT();
        return;
    }

    void KTSlidingWindowFFTW::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTSlidingWindowFSDataFFTW* newData = TransformData(tsData);
        if (tsData->GetEvent() != NULL)
            tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTSlidingWindowFFTW::ProcessEvent(KTEvent* event)
    {
        const KTTimeSeriesData* tsData = event->GetData< KTTimeSeriesData >(fInputDataName);
        if (tsData == NULL)
        {
            KTWARN(fftlog_sw_fftw, "No time series data named <" << fInputDataName << "> was available in the event");
            return;
        }

        KTSlidingWindowFSDataFFTW* newData = TransformData(tsData);
        event->AddData(newData);
        return;
    }


    void KTSlidingWindowFFTW::InitializeFFT()
    {
        if (fWindowFunction == NULL)
        {
            KTERROR(fftlog_sw_fftw, "No window function has been set. The FFT has not been initialized.");
            return;
        }

        // fTransformFlag is guaranteed to be valid in the Set method.
        KTDEBUG(fftlog_sw_fftw, "Transform flag: " << fTransformFlag);
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find(fTransformFlag);
        Int_t transformFlag = iter->second;

        if (fUseWisdom)
        {
            KTDEBUG(fftlog_sw_fftw, "Reading wisdom from file <" << fWisdomFilename << ">");
            if (fftw_import_wisdom_from_filename(fWisdomFilename.c_str()) == 0)
            {
                KTWARN(fftlog_sw_fftw, "Unable to read FFTW wisdom from file <" << fWisdomFilename << ">");
            }
        }

        KTDEBUG(fftlog_sw_fftw, "Creating plan: " << fWindowFunction->GetSize() << " bins; forward FFT");
        if (fFTPlan != NULL)
            fftw_destroy_plan(fFTPlan);
        fFTPlan = fftw_plan_dft_1d(fWindowFunction->GetSize(), fInputArray, fOutputArray, FFTW_FORWARD, transformFlag);
        if (fFTPlan != NULL)
        {
            KTDEBUG(fftlog_sw_fftw, "FFTW plan created");
            if (fUseWisdom)
            {
                if (fftw_export_wisdom_to_filename(fWisdomFilename.c_str()) == 0)
                {
                    KTWARN(fftlog_sw_fftw, "Unable to write FFTW wisdom to file <" << fWisdomFilename << ">");
                }
            }
            fIsInitialized = true;
        }
        else
        {
            KTWARN(fftlog_sw_fftw, "Unable to create FFTW plan!");
            fIsInitialized = false;
        }
        return;
    }

    KTSlidingWindowFSDataFFTW* KTSlidingWindowFFTW::TransformData(const KTTimeSeriesData* tsData)
    {
        if (! fIsInitialized)
        {
            KTWARN(fftlog_sw_fftw, "FFT must be initialized before the transform is performed.\n" <<
                    "Please first call InitializeFFT(), and then perform the transform.");
            return NULL;
        }

        KTSlidingWindowFSDataFFTW* newData = new KTSlidingWindowFSDataFFTW(tsData->GetNTimeSeries());

        for (UInt_t iChannel = 0; iChannel < tsData->GetNTimeSeries(); iChannel++)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(tsData->GetTimeSeries(iChannel));
            if (nextInput == NULL)
            {
                KTERROR(fftlog_sw_fftw, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                return NULL;
            }
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* newResults = NULL;
            try
            {
                newResults = Transform(nextInput);
            }
            catch (std::exception& e)
            {
                KTERROR(fftlog_sw_fftw, "Channel " << iChannel << " did not transform correctly:\n" << e.what());
                return NULL;
            }
            newData->SetSpectra(newResults, iChannel);
        }

        newData->SetEvent(tsData->GetEvent());
        newData->SetName(fOutputDataName);

        fFullFFTSignal(newData);

        return newData;
    }

    KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTSlidingWindowFFTW::Transform(const KTTimeSeriesFFTW* data) const
    {
        // # of time bins in each FFT, and the number of bins in the data
        UInt_t windowSize = fWindowFunction->GetSize();
        UInt_t dataSize = data->size();
        if (windowSize < dataSize)
        {
            // Characteristics of the whole windowed FFT
            UInt_t windowShift = windowSize - GetEffectiveOverlap();
            UInt_t nWindows = (dataSize - windowSize) / windowShift + 1; // integer arithmetic gets truncated to the nearest integer
            UInt_t nTimeBinsUsed = windowSize + (nWindows - 1) * windowShift;
            UInt_t nTimeBinsNotUsed = dataSize - nTimeBinsUsed;

            // Characteristics of the frequency spectrum
            Double_t timeBinWidth = data->GetTimeBinWidth();
            Double_t freqMin = GetMinFrequency(timeBinWidth);
            Double_t freqMax = GetMaxFrequency(timeBinWidth);

            Double_t timeMin = 0.;
            Double_t timeMax = nTimeBinsUsed * timeBinWidth;
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >(nWindows, timeMin, timeMax);

            KTDEBUG(fftlog_sw_fftw, "Performing windowed FFT\n"
                    << "\tWindow size: " << windowSize << '\n'
                    << "\tWindow shift: " << windowShift << '\n'
                    << "\t# of windows: " << nWindows << '\n'
                    << "\t# of unused bins: " << nTimeBinsNotUsed)

            UInt_t windowStart = 0;
            for (UInt_t iWindow = 0; iWindow < nWindows; iWindow++)
            {
                KTDEBUG(fftlog_sw_fftw, "Window: " << iWindow << "; first bin: " << windowStart);
                memcpy(fInputArray, data->GetData() + windowStart, windowSize * sizeof(fftw_complex));
                fftw_execute(fFTPlan);
                (*newSpectra)(iWindow) = ExtractTransformResult(freqMin, freqMax);
                // emit a signal that the FFT was performed, for any connected slots
                fSingleFFTSignal(iWindow, (*newSpectra)(iWindow));
                windowStart += windowShift;
            }
            KTINFO(fftlog_sw_fftw, "FFTs complete; windows used: " << nWindows << "; time bins not used: " << nTimeBinsNotUsed);
            return newSpectra;
       }

       KTERROR(fftlog_sw_fftw, "Window size is larger than time data: " << windowSize << " > " << dataSize << "\n" <<
              "No transform was performed!");
       throw(std::length_error("Window size is larger than time data"));
       return NULL;
    }

    KTFrequencySpectrumFFTW* KTSlidingWindowFFTW::ExtractTransformResult(Double_t freqMin, Double_t freqMax) const
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

    void KTSlidingWindowFFTW::SetWindowSize(UInt_t nBins)
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

    void KTSlidingWindowFFTW::SetWindowLength(Double_t wlTime)
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

    void KTSlidingWindowFFTW::SetWindowFunction(KTEventWindowFunction* wf)
    {
        delete fWindowFunction;
        fWindowFunction = wf;
        RecreateFFT();
        return;
    }

    void KTSlidingWindowFFTW::RecreateFFT()
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

     void KTSlidingWindowFFTW::SetupTransformFlagMap()
     {
         fTransformFlagMap.clear();
         fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
         fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
         fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
         fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
         return;
     }


} /* namespace Katydid */
