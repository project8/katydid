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

#include "TH2D.h"

using std::string;
using std::vector;

namespace Katydid
{
    KTSlidingWindowFFT::KTSlidingWindowFFT() :
            //KTFFT(),
            KTProcessor(),
            KTConfigurable(),
            fTransform(new TFFTRealComplex()),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fFreqBinWidth(1.),
            fFreqMin(0.),
            fFreqMax(1.),
            fOverlap(0),
            fOverlapFrac(0.),
            fUseOverlapFrac(kFALSE),
            fWindowFunction(NULL),
            //fPowerSpectra(),
            fSingleFFTSignal(),
            fFullFFTSignal()
    {
        RegisterSignal("single_fft", &fSingleFFTSignal);
        RegisterSignal("full_fft", &fFullFFTSignal);

        RegisterSlot("header", this, &KTSlidingWindowFFT::ProcessHeader);
        RegisterSlot("event", this, &KTSlidingWindowFFT::ProcessEvent);
    }

    KTSlidingWindowFFT::~KTSlidingWindowFFT()
    {
        delete fTransform;
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
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
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
                for (UInt_t iPoint=windowStart; iPoint<windowStart+fWindowFunction->GetSize(); iPoint++)
                {
                    fTransform->SetPoint(iPoint-windowStart, Double_t((*data)[iPoint]) * fWindowFunction->GetWeight(iPoint-windowStart));
                }
                fTransform->Transform();
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


    /*
    void KTSlidingWindowFFT::AddTransformResult(vector< KTPowerSpectrum* >* newResults)
    {
        if (newResults != NULL)
        {
            this->fPowerSpectra.push_back(newResults);
        }
        return;
    }


    TH2D* KTSlidingWindowFFT::CreatePowerSpectrumHistogram(const string& name,  UInt_t channelNum) const
    {
        if (channelNum >= fPowerSpectra.size()) return NULL;
        if (fPowerSpectra[channelNum]->empty()) return NULL;

        // plot in MHz, instead of Hz
        Double_t freqMult = 1.e-6;

        Double_t effTimeWidth = (Double_t)(fPowerSpectra[channelNum]->size() * fWindowFunction->GetSize() - (fPowerSpectra[channelNum]->size()-1) * GetEffectiveOverlap());
        effTimeWidth *= fWindowFunction->GetBinWidth();
        TH2D* hist = new TH2D(name.c_str(), "Power Spectra",
                fPowerSpectra[channelNum]->size(), 0., effTimeWidth,
                this->GetFrequencySize(), -0.5 * fFreqBinWidth * freqMult, fFreqBinWidth * ((Double_t)this->GetFrequencySize()-0.5) * freqMult);

        KTINFO("Frequency axis: " << this->GetFrequencySize() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " MHz");
        KTINFO("Time axis: " << fPowerSpectra[channelNum]->size() << " bins; range: 0 - " << effTimeWidth << " s");

        for (Int_t iBinX=1; iBinX<=(Int_t)fPowerSpectra[channelNum]->size(); iBinX++)
        {
            KTPowerSpectrum* ps = (*fPowerSpectra[channelNum])[iBinX-1];
            for (Int_t iBinY=1; iBinY<=this->GetFrequencySize(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, ps->GetMagnitudeAt(iBinY-1));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (MHz)");
        return hist;
    }

    TH2D* KTSlidingWindowFFT::CreatePowerSpectrumHistogram(UInt_t channelNum) const
    {
        return CreatePowerSpectrumHistogram("hPowerSpectra_SlidingWindowFFT", channelNum);
    }

    KTPhysicalArray< 2, Double_t >* KTSlidingWindowFFT::CreatePowerSpectrumPhysArr(UInt_t channelNum) const
    {
        if (channelNum >= fPowerSpectra.size()) return NULL;
        if (fPowerSpectra[channelNum]->empty()) return NULL;

        // plot in MHz, instead of Hz
        Double_t freqMult = 1.e-6;

        Double_t effTimeWidth = (Double_t)(fPowerSpectra[channelNum]->size() * fWindowFunction->GetSize() - (fPowerSpectra[channelNum]->size()-1) * GetEffectiveOverlap());
        effTimeWidth *= fWindowFunction->GetBinWidth();
        KTPhysicalArray< 2, Double_t >* array = new KTPhysicalArray< 2, Double_t >(fPowerSpectra[channelNum]->size(), 0., effTimeWidth,
                this->GetFrequencySize(), -0.5 * fFreqBinWidth * freqMult, fFreqBinWidth * ((Double_t)this->GetFrequencySize()-0.5) * freqMult);

        KTINFO("Frequency axis: " << this->GetFrequencySize() << " bins; range: " << array->GetRangeMin(2) << " - " << array->GetRangeMax(2) << " MHz");
        KTINFO("Time axis: " << fPowerSpectra[channelNum]->size() << " bins; range: 0 - " << effTimeWidth << " s");

        for (Int_t iBinX=1; iBinX<=(Int_t)fPowerSpectra[channelNum]->size(); iBinX++)
        {
            KTPowerSpectrum* ps = (*fPowerSpectra[channelNum])[iBinX-1];
            for (Int_t iBinY=1; iBinY<=this->GetFrequencySize(); iBinY++)
            {
                (*array)(iBinX, iBinY) = ps->GetMagnitudeAt(iBinY-1);
            }
        }

        return array;
    }
    */
    KTFrequencySpectrum* KTSlidingWindowFFT::ExtractTransformResult() const
    {
        UInt_t freqSize = this->GetFrequencySize();
        Double_t normalization = 1. / (Double_t)GetTimeSize();

        Double_t tempReal, tempImag;
        KTFrequencySpectrum* newSpect = new KTFrequencySpectrum(freqSize, fFreqMin, fFreqMax);
        for (Int_t iPoint = 0; iPoint<freqSize; iPoint++)
        {
            fTransform->GetPointComplex(iPoint, tempReal, tempImag);
            (*newSpect)[iPoint].set_rect(tempReal, tempImag);
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
        delete fTransform;
        fTransform = new TFFTRealComplex(fWindowFunction->GetSize(), kFALSE);
        fIsInitialized = false;
    }
    /*
    void KTSlidingWindowFFT::ClearPowerSpectra()
    {
        while (! fPowerSpectra.empty())
        {
            while (! fPowerSpectra.back()->empty())
            {
                delete fPowerSpectra.back()->back();
                fPowerSpectra.back()->pop_back();
            }
            delete fPowerSpectra.back();
            fPowerSpectra.pop_back();
        }
        return;
    }
    */

} /* namespace Katydid */
