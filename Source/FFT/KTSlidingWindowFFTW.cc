/*
 * KTSlidingWindowFFTW.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFTW.hh"

#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesReal.hh"
#include "KTFactory.hh"
#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTWindowFunction.hh"

using std::string;
using std::vector;

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTSlidingWindowFFTW > sSWFFTRegistrar("sliding-window-fft");

    KTSlidingWindowFFTW::KTSlidingWindowFFTW() :
            KTFFT(),
            KTProcessor(),
            fFTPlan(),
            fTimeSize(0),
            fInputArray(NULL),
            fOutputArray(NULL),
            fTransformFlag("MEASURE"),
            fIsInitialized(kFALSE),
            fOverlap(0),
            fOverlapFrac(0.),
            fUseOverlapFrac(kFALSE),
            fWindowFunction(NULL),
            fSingleFFTSignal(),
            fFullFFTSignal()
    {
        RegisterSignal("single_fft", &fSingleFFTSignal, "void (UInt_t, KTFrequencySpectrum*)");
        RegisterSignal("full_fft", &fFullFFTSignal, "void (KTSlidingWindowFSData*)");

        RegisterSlot("header", this, &KTSlidingWindowFFTW::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("time-series", this, &KTSlidingWindowFFTW::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("event", this, &KTSlidingWindowFFTW::ProcessEvent, "void (KTEvent*)");
        RegisterSlot("event-named-data", this, &KTSlidingWindowFFTW::ProcessEvent, "void (KTEvent*, const string&)");

        SetupTransformFlagMap();
    }

    KTSlidingWindowFFTW::~KTSlidingWindowFFTW()
    {
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
        delete fWindowFunction;
        //ClearPowerSpectra();
    }

    Bool_t KTSlidingWindowFFTW::Configure(const KTPStoreNode* node)
    {
        // Config-file options
        SetTransformFlag(node->GetData< string >("transform-flag", fTransformFlag));

        if (node->HasData("overlap-time")) SetOverlap(node->GetData< Double_t >("overlap-time", 0));
        if (node->HasData("overlap-size")) SetOverlap(node->GetData< UInt_t >("overlap-size", 0));
        if (node->HasData("overlap-frac")) SetOverlapFrac(node->GetData< Double_t >("overlap-frac", 0.));

        string windowType = node->GetData< string >("window-function-type", "rectangular");
        KTEventWindowFunction* tempWF = KTFactory< KTEventWindowFunction >::GetInstance()->Create(windowType);
        if (tempWF == NULL)
        {
            KTERROR(fftlog_sw, "Invalid window function type given: <" << windowType << ">.");
            return false;
        }
        SetWindowFunction(tempWF);

        const KTPStoreNode* childNode = node->GetChild("window-function");
        if (childNode != NULL)
        {
            fWindowFunction->Configure(childNode);
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

    void KTSlidingWindowFFTW::ProcessEvent(KTEvent* event, const string& dataName)
    {
        const KTTimeSeriesData* tsData = NULL;
        if (dataName.empty())
        {
            tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(KTProgenitorTimeSeriesData::StaticGetName()));
            if (tsData == NULL)
                tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(KTBasicTimeSeriesData::StaticGetName()));
            if (tsData == NULL)
                tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(KTTimeSeriesPairedData::StaticGetName()));
        }
        else
        {
            tsData = dynamic_cast< KTTimeSeriesData* >(event->GetData(dataName));
        }

        if (tsData == NULL)
        {
            KTWARN(fftlog_simp, "No time series data was available in the event");
            return;
        }

        KTSlidingWindowFSDataFFTW* newData = TransformData(tsData);
        event->AddData(newData);
        return;
    }


    void KTSlidingWindowFFTW::InitializeFFT()
    {
        // fTransformFlag is guaranteed to be valid in the Set method.
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find(fTransformFlag);
        Int_t transformFlag = iter->second;

        fFTPlan = fftw_plan_dft_r2c_1d(fTimeSize, fInputArray, fOutputArray, transformFlag);
        if (fFTPlan != NULL)
        {
            fIsInitialized = true;
        }
        else
        {
            fIsInitialized = false;
        }
        return;
    }

    KTSlidingWindowFSData* KTSlidingWindowFFTW::TransformData(const KTTimeSeriesData* tsData)
    {
        if (! fIsInitialized)
        {
            KTWARN(fftlog_sw, "FFT must be initialized before the transform is performed.\n" <<
                    "Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return NULL;
        }

        KTSlidingWindowFSData* newData = new KTSlidingWindowFSData(tsData->GetNTimeSeries());

        for (UInt_t iChannel = 0; iChannel < tsData->GetNTimeSeries(); iChannel++)
        {
            const KTTimeSeriesReal* nextInput = dynamic_cast< const KTTimeSeriesReal* >(tsData->GetTimeSeries(iChannel));
            if (nextInput == NULL)
            {
                KTERROR(fftlog_sw, "Incorrect time series type: time series did not cast to KTTimeSeriesReal.");
                return NULL;
            }
            KTPhysicalArray< 1, KTFrequencySpectrum* >* newResults = NULL;
            try
            {
                newResults = Transform(nextInput);
            }
            catch (std::exception& e)
            {
                KTERROR(fftlog_sw, "Channel " << iChannel << " did not transform correctly:\n" << e.what());
                return NULL;
            }
            newData->SetSpectra(newResults, iChannel);
        }

        newData->SetEvent(tsData->GetEvent());

        fFullFFTSignal(newData);

        return newData;
    }

    KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTSlidingWindowFFTW::Transform(const KTTimeSeriesFFTW* data) const
    {
        UInt_t nTimeBins = fWindowFunction->GetSize();
        if (nTimeBins < data->size())
        {
            UInt_t windowShift = nTimeBins - GetEffectiveOverlap();
            UInt_t nWindows = (data->size() - nTimeBins) / windowShift + 1; // integer arithmetic gets truncated to the nearest integer
            UInt_t nTimeBinsNotUsed = data->size() - (nWindows - 1) * windowShift + fWindowFunction->GetSize();

            Double_t timeBinWidth = data->GetBinWidth();
            Double_t freqBinWidth = GetFrequencyBinWidth(timeBinWidth);
            Double_t freqMin = GetMinFrequency(timeBinWidth);
            Double_t freqMax = GetMaxFrequency(timeBinWidth);

            Double_t timeMin = 0.;
            Double_t timeMax = ((nWindows - 1) * windowShift + fWindowFunction->GetSize()) * fWindowFunction->GetBinWidth();
            KTPhysicalArray< 1, KTFrequencySpectrum* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrum* >(data->size(), timeMin, timeMax);

            UInt_t windowStart = 0;
            for (UInt_t iWindow = 0; iWindow < nWindows; iWindow++)
            {
                copy(data->begin() + windowStart, data->begin() + windowStart + fWindowFunction->GetSize(), fInputArray);
                fftw_execute(fFTPlan);
                (*newSpectra)(iWindow) = ExtractTransformResult(freqMin, freqMax);
                // emit a signal that the FFT was performed, for any connected slots
                fSingleFFTSignal(iWindow, (*newSpectra)(iWindow));
                windowStart += windowShift;
            }
            KTINFO(fftlog_sw, "FFTs complete; windows used: " << nWindows << "; time bins not used: " << nTimeBinsNotUsed);
            return newSpectra;
       }

       KTERROR(fftlog_sw, "Window size is larger than time data: " << nTimeBins << " > " << data->size() << "\n" <<
              "No transform was performed!");
       throw(std::length_error("Window size is larger than time data"));
       return NULL;
    }

    KTFrequencySpectrumFFTW* KTSlidingWindowFFTW::ExtractTransformResult(Double_t freqMin, Double_t freqMax) const
    {
        UInt_t freqSize = GetFrequencySize();
        Double_t normalization = sqrt(2. / (Double_t)GetTimeSize());

        Double_t tempReal, tempImag;
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(freqSize, freqMin, freqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            (*newSpect)(iPoint).set_rect(fOutputArray[iPoint][0], fOutputArray[iPoint][1]);
            (*newSpect)(iPoint) *= normalization;
        }

        return newSpect;
    }

    void KTSlidingWindowFFTW::SetWindowSize(UInt_t nBins)
    {
        fWindowFunction->SetSize(nBins);
        RecreateFFT();
        return;
    }

    void KTSlidingWindowFFTW::SetWindowLength(Double_t wlTime)
    {
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
        fftw_destroy_plan(fFTPlan);
        fftw_free(fInputArray);
        fftw_free(fOutputArray);
        fInputArray = (double*) fftw_malloc(sizeof(double) * fTimeSize);
        fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * CalculateNFrequencyBins(fTimeSize));
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
