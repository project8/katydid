/*
 * KTSlidingWindowFFT.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTSlidingWindowFFT.hh"

#include "KTEggHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTFactory.hh"
#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"
#include "KTWindowFunction.hh"

#include "TH2D.h"

using std::string;
using std::vector;

namespace Katydid
{
    KTSlidingWindowFFT::KTSlidingWindowFFT() :
            KTFFT(),
            KTProcessor(),
            KTConfigurable(),
            fTransform(new TFFTRealComplex()),
            fTransformFlag(string("")),
            fIsInitialized(kFALSE),
            fFreqBinWidth(0.),
            fOverlap(0),
            fOverlapFrac(0.),
            fUseOverlapFrac(kFALSE),
            fWindowFunction(NULL),
            fPowerSpectra(),
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
        ClearPowerSpectra();
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
        return;
    }

    void KTSlidingWindowFFT::ProcessEvent(UInt_t iEvent, const KTTimeSeriesData* tsData)
    {
        TransformData(tsData);
        fFullFFTSignal(iEvent, this);
        return;
    }


    void KTSlidingWindowFFT::InitializeFFT()
    {
        fTransform->Init(fTransformFlag.c_str(), 0, NULL);
        fIsInitialized = kTRUE;
        return;
    }

    Bool_t KTSlidingWindowFFT::TransformData(const KTTimeSeriesData* tsData)
    {
        if (! fIsInitialized)
        {
            KTWARN(fftlog_sw, "FFT must be initialized before the transform is performed.\n" <<
                    "Please first call InitializeFFT(), then use a TakeData method to set the data, and then finally perform the transform.");
            return kFALSE;
        }

        ClearPowerSpectra();

        fFreqBinWidth = tsData->GetSampleRate() / (Double_t)fWindowFunction->GetSize();

        for (UInt_t iChannel = 0; iChannel < tsData->GetNRecords(); iChannel++)
        {
            vector< KTPowerSpectrum* >* newResults = new vector< KTPowerSpectrum* >();
            try
            {
                Transform(tsData->GetRecord(iChannel), newResults);
            }
            catch (std::exception& e)
            {
                KTERROR(fftlog_sw, "Channel " << iChannel << " did not transform correctly:\n" << e.what());
                return false;
            }
            AddTransformResult(newResults);
        }

        return true;
    }

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

    KTPowerSpectrum* KTSlidingWindowFFT::ExtractPowerSpectrum() const
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
        powerSpec->SetBinWidth(fFreqBinWidth);
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


} /* namespace Katydid */
