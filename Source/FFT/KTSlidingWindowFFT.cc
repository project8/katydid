/*
 * KTSlidingWindowFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFT.hh"

//#include "KTArrayUC.hh"
#include "KTEvent.hh"
//#include "KTPowerSpectrum.hh"
#include "KTWindowFunction.hh"

#include "TH2D.h"

#include <iostream>

using std::string;
using std::vector;

namespace Katydid
{

    KTSlidingWindowFFT::KTSlidingWindowFFT() :
            KTFFT(),
            fTransform(new TFFTRealComplex()),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fIsDataReady(kFALSE),
            fOverlap(0),
            fOverlapFrac(0.),
            fUseOverlapFrac(kFALSE),
            fWindowFunction(NULL),
            fTimeData(),
            fPowerSpectra(),
            fSingleFFTSignal()
    {
    }

    KTSlidingWindowFFT::~KTSlidingWindowFFT()
    {
        delete fTransform;
        delete fWindowFunction;
        while (! fPowerSpectra.empty())
        {
            delete fPowerSpectra.back();
            fPowerSpectra.pop_back();
        }
    }

    void KTSlidingWindowFFT::InitializeFFT()
    {
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
        return;
    }

    Bool_t KTSlidingWindowFFT::TakeData(const KTEvent* event)
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSlidingWindowFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return kFALSE;
        }

        if (event->GetBinWidth() != fWindowFunction->GetBinWidth())
        {
            std::cerr << "Warning from KTSlidingWindowFFT::TakeData: Bin widths are mismatched between the given event (" << event->GetBinWidth() << ") and the window function (" << fWindowFunction->GetBinWidth() << ")." << std::endl;
            return kFALSE;
        }

        fFreqBinWidth = event->GetSampleRate() / (Double_t)fWindowFunction->GetSize();

        unsigned int nBins = event->GetRecordSize();
        fTimeData.resize(nBins);
        for (unsigned int iPoint=0; iPoint<nBins; iPoint++)
        {
            fTimeData[iPoint] = event->GetRecordAt< Double_t >(iPoint);
        }
        fIsDataReady = kTRUE;
        return kTRUE;
    }

    Bool_t KTSlidingWindowFFT::TakeData(const vector< Double_t >& data)
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSimpleFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return kFALSE;
        }

        unsigned int nBins = (unsigned int)data.size();
        fTimeData.resize(nBins);
        for (unsigned int iPoint=0; iPoint<nBins; iPoint++)
        {
            fTimeData[iPoint] = (Double_t)(data[iPoint]);
        }
        fIsDataReady = kTRUE;
        return kTRUE;
    }
    /*
    void KTSlidingWindowFFT::TakeData(const TArray* array)
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSlidingWindowFFT::TakeData: FFT must be initialized before setting the data" << std::endl;
            std::cerr << "   Please first call InitializeFFT, and then use the TakeData method of your choice to set the data" << std::endl;
            return;
        }

        if (fTimeData == NULL) fTimeData = new TArrayD(array->GetSize());
        else if (fTimeData->GetSize() != array->GetSize())
        {
            delete fTimeData;
            fTimeData = new TArrayD(array->GetSize());
        }

        for (Int_t iPoint=0; iPoint<array->GetSize(); iPoint++)
        {
            (*fTimeData)[iPoint] = (Double_t)(array->GetAt(iPoint));
        }
        fIsDataReady = kTRUE;
        return;
    }
    */
    Bool_t KTSlidingWindowFFT::Transform()
    {
        if (! fIsInitialized)
        {
            std::cerr << "Warning from KTSlidingWindowFFT::Transform: FFT must be initialized before the transform is performed" << std::endl;
            std::cerr << "   Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform." << std::endl;
            return kFALSE;
        }

        if (! fIsDataReady)
        {
            std::cerr << "Warning from KTSlidingWindowFFT::Transform: The data for the transform is not ready"<< std::endl;
            std::cerr << "   Please first call TakeData, and then perform the transform" << std::endl;
            return kFALSE;
        }

        while (! fPowerSpectra.empty())
        {
            std::cout << "power spectrum: " << fPowerSpectra.back() << std::endl;
            delete fPowerSpectra.back();
            fPowerSpectra.pop_back();
        }

        if (fWindowFunction->GetSize() < this->GetFullTimeSize())
        {
            //std::cout << fWindowFunction->GetSize() << "  " << fWindowFunction->GetBinWidth() << "  " << fWindowFunction->GetLength() << "  " << GetEffectiveOverlap() << "  " << GetFullTimeSize() << "  " << GetTimeSize() << std::endl;
            Int_t windowShift = fWindowFunction->GetSize() - GetEffectiveOverlap();
            //std::cout << "window shift: " << windowShift << std::endl;
            //Int_t nWindows = 1 + TMath::FloorNint((Double_t)(this->GetFullTimeSize() - fWindowFunction->GetSize()) / (Double_t)windowShift);
            UInt_t iWindow = 0;
            for (unsigned int windowStart=0; windowStart + fWindowFunction->GetSize() <= this->GetFullTimeSize(); windowStart += windowShift)
            {
                //std::cout << "window: " << iWindow+1 << "; window start: " << windowStart << std::endl;
                for (unsigned int iPoint=windowStart; iPoint<windowStart+fWindowFunction->GetSize(); iPoint++)
                {
                    fTransform->SetPoint(iPoint-windowStart, fTimeData[iPoint] * fWindowFunction->GetWeight(iPoint-windowStart));
                    //std::cout << "Setting a point in the transform: " << iPoint-windowStart << "  " << fTransform->GetPointReal(iPoint-windowStart, kTRUE) << std::endl;
                }
                fTransform->Transform();
                fPowerSpectra.push_back(this->CreatePowerSpectrum());
                // emit a signal that the FFT was performed, for any connected slots
                fSingleFFTSignal(iWindow, fPowerSpectra.back());
                iWindow++;
            }
            std::cout << "FFTs complete; windows used: " << iWindow << std::endl;
        }
        else
        {
            std::cout << "Warning from KTSlidingWindowFFT: window size is larger than time data: " << fWindowFunction->GetSize() << " > " << this->GetFullTimeSize() << std::endl;
            std::cout << "   No transform was performed!" << std::endl;
            return kFALSE;
        }

        return kTRUE;
    }

    TH2D* KTSlidingWindowFFT::CreatePowerSpectrumHistogram(const string& name) const
    {
        if (fPowerSpectra.empty()) return NULL;

        // plot in MHz, instead of Hz
        Double_t freqMult = 1.e-6;

        Double_t effTimeWidth = (Double_t)(fPowerSpectra.size() * fWindowFunction->GetSize() - (fPowerSpectra.size()-1) * GetEffectiveOverlap());
        effTimeWidth *= fWindowFunction->GetBinWidth();
        TH2D* hist = new TH2D(name.c_str(), "Power Spectra",
                fPowerSpectra.size(), 0., effTimeWidth,
                this->GetFrequencySize(), -0.5 * fFreqBinWidth * freqMult, fFreqBinWidth * ((Double_t)this->GetFrequencySize()-0.5) * freqMult);
        std::cout << "Frequency axis: " << this->GetFrequencySize() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " MHz" << std::endl;
        std::cout << "Time axis: " << fPowerSpectra.size() << " bins; range: 0 - " << effTimeWidth << " s" << std::endl;

        for (Int_t iBinX=1; iBinX<=(Int_t)fPowerSpectra.size(); iBinX++)
        {
            KTPowerSpectrum* ps = fPowerSpectra[iBinX-1];
            for (Int_t iBinY=1; iBinY<=this->GetFrequencySize(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, ps->GetMagnitudeAt(iBinY-1));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (MHz)");
        return hist;
    }

    TH2D* KTSlidingWindowFFT::CreatePowerSpectrumHistogram() const
    {
        return CreatePowerSpectrumHistogram("hPowerSpectra_SlidingWindowFFT");
    }

    KTPowerSpectrum* KTSlidingWindowFFT::CreatePowerSpectrum() const
    {
        // Extract the transformed data
        UInt_t freqSize = this->GetFrequencySize();
        Double_t* freqSpecReal = new Double_t [freqSize];
        Double_t* freqSpecImag = new Double_t [freqSize];
        fTransform->GetPointsComplex(freqSpecReal, freqSpecImag);

        KTComplexVector freqSpec(freqSize, freqSpecReal, freqSpecImag, "R");
        delete [] freqSpecReal; delete [] freqSpecImag;
        freqSpec *= 1. / (Double_t)this->GetTimeSize();

        KTPowerSpectrum* powerSpec = new KTPowerSpectrum();
        powerSpec->TakeFrequencySpectrum(freqSpec);
        powerSpec->SetBinWidth(fWindowFunction->GetBinWidth());
        return powerSpec;
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

    void KTSlidingWindowFFT::SetWindowFunction(KTWindowFunction* wf)
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
        fIsInitialized = kFALSE;
        fIsDataReady = kFALSE;
    }


} /* namespace Katydid */
