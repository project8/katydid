/*
 * KTSlidingWindowFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFT.hh"

#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTTimeSeriesData.hh"
#include "KTFactory.hh"
#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTWindowFunction.hh"

using std::string;
using std::vector;

namespace Katydid
{
    KTSlidingWindowFFT::KTSlidingWindowFFT() :
            KTProcessor(),
            KTConfigurable(),
            fFTPlan(),
            fTimeSize(0),
            fInputArray(NULL),
            fOutputArray(NULL),
            fTransformFlag("MEASURE"),
            fIsInitialized(kFALSE),
            fFreqBinWidth(1.),
            fFreqMin(0.),
            fFreqMax(1.),
            fOverlap(0),
            fOverlapFrac(0.),
            fUseOverlapFrac(kFALSE),
            fWindowFunction(NULL),
            fSingleFFTSignal(),
            fFullFFTSignal()
    {
        RegisterSignal("single_fft", &fSingleFFTSignal, "void (UInt_t, KTFrequencySpectrum*)");
        RegisterSignal("full_fft", &fFullFFTSignal, "void (KTSlidingWindowFSData*)");

        RegisterSlot("header", this, &KTSlidingWindowFFT::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("event", this, &KTSlidingWindowFFT::ProcessEvent, "void (UInt_t iEvent, const KTTimeSeriesData*)");

        SetupTransformFlagMap();
    }

    KTSlidingWindowFFT::~KTSlidingWindowFFT()
    {
        if (fInputArray != NULL) fftw_free(fInputArray);
        if (fOutputArray != NULL) fftw_free(fOutputArray);
        delete fWindowFunction;
        //ClearPowerSpectra();
    }

    Bool_t KTSlidingWindowFFT::Configure(const KTPStoreNode* node)
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

    void KTSlidingWindowFFT::ProcessHeader(const KTEggHeader* header)
    {
        fWindowFunction->SetBinWidth(1. / header->GetAcquisitionRate());
        RecreateFFT();
        InitializeFFT();
        SetFreqBinWidth(header->GetAcquisitionRate() / (Double_t)fWindowFunction->GetSize());
        fFreqMin = -0.5 * fFreqBinWidth;
        fFreqMax = fFreqBinWidth * ((Double_t)fWindowFunction->GetSize()-0.5);
        return;
    }

    void KTSlidingWindowFFT::ProcessEvent(UInt_t iEvent, const KTTimeSeriesData* tsData)
    {
        KTSlidingWindowFSData* newData = TransformData(tsData);
        tsData->GetEvent()->AddData(newData);
        return;
    }


    void KTSlidingWindowFFT::InitializeFFT()
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
        return;
    }

    KTSlidingWindowFSData* KTSlidingWindowFFT::TransformData(const KTTimeSeriesData* tsData)
    {
        if (! fIsInitialized)
        {
            KTWARN(fftlog_sw, "FFT must be initialized before the transform is performed.\n" <<
                    "Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return NULL;
        }

        fFreqBinWidth = tsData->GetSampleRate() / (Double_t)fWindowFunction->GetSize();
        fFreqMin = -0.5 * fFreqBinWidth;
        fFreqMax = fFreqBinWidth * ((Double_t)fWindowFunction->GetSize()-0.5);

        KTSlidingWindowFSData* newData = new KTSlidingWindowFSData(tsData->GetNChannels());

        for (UInt_t iChannel = 0; iChannel < tsData->GetNChannels(); iChannel++)
        {
            KTPhysicalArray< 1, KTFrequencySpectrum* >* newResults = NULL;
            try
            {
                newResults = Transform(tsData->GetRecord(iChannel));
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

    KTPhysicalArray< 1, KTFrequencySpectrum* >* KTSlidingWindowFFT::Transform(const KTTimeSeries* data) const
    {
        if (fWindowFunction->GetSize() < data->size())
        {
            UInt_t windowShift = fWindowFunction->GetSize() - GetEffectiveOverlap();
            UInt_t nWindows = (data->size() - fWindowFunction->GetSize()) / windowShift + 1;
            UInt_t nTimeBinsNotUsed = data->size() - (nWindows - 1) * windowShift + fWindowFunction->GetSize();
            Double_t timeMin = 0.;
            Double_t timeMax = ((nWindows - 1) * windowShift + fWindowFunction->GetSize()) * fWindowFunction->GetBinWidth();
            KTPhysicalArray< 1, KTFrequencySpectrum* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrum* >(data->size(), timeMin, timeMax);
            UInt_t windowStart = 0;
            for (UInt_t iWindow = 0; iWindow < nWindows; iWindow++)
            {
                copy(data->begin() + windowStart, data->begin() + windowStart + fWindowFunction->GetSize(), fInputArray);
                fftw_execute(fFTPlan);
                (*newSpectra)[iWindow] = ExtractTransformResult();
                // emit a signal that the FFT was performed, for any connected slots
                fSingleFFTSignal(iWindow, (*newSpectra)[iWindow]);
                windowStart += windowShift;
            }
            KTINFO(fftlog_sw, "FFTs complete; windows used: " << nWindows << "; time bins not used: " << nTimeBinsNotUsed);
            return newSpectra;
       }

       KTERROR(fftlog_sw, "Window size is larger than time data: " << fWindowFunction->GetSize() << " > " << data->size() << "\n" <<
              "No transform was performed!");
       throw(std::length_error("Window size is larger than time data"));
       return NULL;
    }

    KTFrequencySpectrum* KTSlidingWindowFFT::ExtractTransformResult() const
    {
        UInt_t freqSize = this->GetFrequencySize();
        Double_t normalization = sqrt(2. / (Double_t)GetTimeSize());

        Double_t tempReal, tempImag;
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(freqSize, fFreqMin, fFreqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            (*newSpect)[iPoint].set_rect(fOutputArray[iPoint][0], fOutputArray[iPoint][1]);
            (*newSpect)[iPoint] *= normalization;
        }

        return newSpect;
    }

    void KTSlidingWindowFFT::SetWindowSize(UInt_t nBins)
    {
        fWindowFunction->SetSize(nBins);
        RecreateFFT();
        return;
    }

    void KTSlidingWindowFFT::SetWindowLength(Double_t wlTime)
    {
        fWindowFunction->SetLength(wlTime);
        RecreateFFT();
        return;
    }

    void KTSlidingWindowFFT::SetWindowFunction(KTEventWindowFunction* wf)
    {
        delete fWindowFunction;
        fWindowFunction = wf;
        RecreateFFT();
        return;
    }

    void KTSlidingWindowFFT::RecreateFFT()
    {
        fftw_destroy_plan(fFTPlan);
        fftw_free(fInputArray);
        fftw_free(fOutputArray);
        fInputArray = (double*) fftw_malloc(sizeof(double) * fTimeSize);
        fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * CalculateNFrequencyBins(fTimeSize));
        fIsInitialized = false;
    }

     void KTSlidingWindowFFT::SetupTransformFlagMap()
     {
         fTransformFlagMap.clear();
         fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
         fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
         fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
         fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
         return;
     }


} /* namespace Katydid */
