/*
 * KTSlidingWindowFFT.hh
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#ifndef KTSLIDINGWINDOWFFT_HH_
#define KTSLIDINGWINDOWFFT_HH_

#include "KTFFT.hh"

#include "KTPowerSpectrum.hh"

#include "TFFTRealComplex.h"

#include <string>
#include <vector>
using std::vector;

class TH2D;

namespace Katydid
{
    //class KTPowerSpectrum;
    class KTEvent;
    class KTWindowFunction;

    class KTSlidingWindowFFT : public KTFFT
    {
        public:
            KTSlidingWindowFFT();
            virtual ~KTSlidingWindowFFT();

            virtual void InitializeFFT();

            virtual Bool_t TakeData(const KTEvent* event);
            virtual Bool_t TakeData(const std::vector< Double_t >& data);
            //virtual Bool_t TakeData(const TArray* data);

            virtual Bool_t Transform();

            virtual TH2D* CreatePowerSpectrumHistogram(const std::string& name) const;
            virtual TH2D* CreatePowerSpectrumHistogram() const;

            /// for this FFT, the "TimeSize" is the window size. The "FullTimeSize" is different.
            virtual Int_t GetTimeSize() const;
            virtual Int_t GetFrequencySize() const;

            UInt_t GetWindowSize() const;
            UInt_t GetFullTimeSize() const;
            UInt_t GetOverlap() const;
            KTWindowFunction* GetWindowFunction() const;
            const std::vector< Double_t >& GetTimeData() const;
            const KTPowerSpectrum* GetPowerSpectrum(Int_t spect) const;

            const TFFTRealComplex* GetFFT() const;
            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Bool_t GetIsDataReady() const;
            Double_t GetFreqBinWidth() const;

            /// note: SetTransformFlag sets fIsInitialized and fIsDataReady to kFALSE.
            void SetTransformFlag(const string& flag);
            void SetFreqBinWidth(Double_t bw);
            void SetWindowSize(UInt_t nBins);
            void SetWindowLength(Double_t wlTime);
            void SetOverlap(UInt_t nBins);
            void SetOverlap(Double_t overlapTime);
            void SetWindowFunction(KTWindowFunction* wf);

        protected:
            virtual KTPowerSpectrum* CreatePowerSpectrum() const;

            TFFTRealComplex* fTransform;

            std::string fTransformFlag;

            Bool_t fIsInitialized;
            Bool_t fIsDataReady;

            Double_t fFreqBinWidth;
            UInt_t fOverlap;

            KTWindowFunction* fWindowFunction;
            std::vector< Double_t > fTimeData;
            std::vector< KTPowerSpectrum* > fPowerSpectra;

            ClassDef(KTSlidingWindowFFT, 2);
    };


    inline Int_t KTSlidingWindowFFT::GetTimeSize() const
    {
        return fTransform->GetSize();
    }

    inline Int_t KTSlidingWindowFFT::GetFrequencySize() const
    {
        return fTransform->GetSize() / 2 + 1;
    }

    inline const TFFTRealComplex* KTSlidingWindowFFT::GetFFT() const
    {
        return fTransform;
    }

    inline const string& KTSlidingWindowFFT::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTSlidingWindowFFT::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline Bool_t KTSlidingWindowFFT::GetIsDataReady() const
    {
        return fIsDataReady;
    }

    inline Double_t KTSlidingWindowFFT::GetFreqBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline UInt_t KTSlidingWindowFFT::GetFullTimeSize() const
    {
        return (UInt_t)fTimeData.size();
    }

    inline UInt_t KTSlidingWindowFFT::GetOverlap() const
    {
        return fOverlap;
    }

    inline const vector< Double_t >& KTSlidingWindowFFT::GetTimeData() const
    {
        return fTimeData;
    }

    inline const KTPowerSpectrum* KTSlidingWindowFFT::GetPowerSpectrum(Int_t spec) const
    {
        if (spec >= 0 && spec < (Int_t)fPowerSpectra.size()) return fPowerSpectra[spec];
        return NULL;
    }

    inline KTWindowFunction* KTSlidingWindowFFT::GetWindowFunction() const
    {
        return fWindowFunction;
    }

    inline void KTSlidingWindowFFT::SetTransformFlag(const string& flag)
    {
        fTransformFlag = flag;
        fIsInitialized = kFALSE;
        return;
    }

    inline void KTSlidingWindowFFT::SetOverlap(UInt_t nBins)
    {
        fOverlap = nBins;
        return;
    }

    inline void KTSlidingWindowFFT::SetFreqBinWidth(Double_t bw)
    {
        fFreqBinWidth = bw;
        return;
    }

} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFT_HH_ */
