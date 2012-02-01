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
#include "KTWindowFunction.hh"

#include "TFFTRealComplex.h"
#include "TMath.h"

#include "boost/signals2.hpp"

#include <string>
#include <vector>
using std::vector;

class TH2D;

namespace Katydid
{
    //class KTPowerSpectrum;
    class KTEvent;

    class KTSlidingWindowFFT : public KTFFT
    {
        public:
            typedef boost::signals2::signal< void (UInt_t, KTPowerSpectrum*) > SingleFFTSignal;

        public:
            KTSlidingWindowFFT();
            virtual ~KTSlidingWindowFFT();

            virtual void InitializeFFT();
            virtual void RecreateFFT();

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
            UInt_t GetEffectiveOverlap() const;
            Double_t GetOverlapFrac() const;
            Bool_t GetUseOverlapFrac() const;
            KTWindowFunction* GetWindowFunction() const;
            const std::vector< Double_t >& GetTimeData() const;
            KTPowerSpectrum* GetPowerSpectrum(Int_t spect) const;

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
            void SetOverlapFrac(Double_t overlapFrac);
            void SetUseOverlapFrac(Bool_t useOverlapFrac);
            void SetWindowFunction(KTWindowFunction* wf);

        protected:
            virtual KTPowerSpectrum* CreatePowerSpectrum() const;
            void ClearPowerSpectra();

            TFFTRealComplex* fTransform;

            std::string fTransformFlag;

            Bool_t fIsInitialized;
            Bool_t fIsDataReady;

            Double_t fFreqBinWidth;
            UInt_t fOverlap;
            Double_t fOverlapFrac;
            Bool_t fUseOverlapFrac;

            KTWindowFunction* fWindowFunction;
            std::vector< Double_t > fTimeData;
            std::vector< KTPowerSpectrum* > fPowerSpectra;


            //***************
            // Signals
            //***************

        public:
            boost::signals2::connection ConnectToFFTSignal(const SingleFFTSignal::slot_type &subscriber);
            boost::signals2::connection ConnectToFFTSignal(Int_t group, const SingleFFTSignal::slot_type &subscriber);

        private:
            SingleFFTSignal fSingleFFTSignal;

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

    inline Double_t KTSlidingWindowFFT::GetOverlapFrac() const
    {
        return fOverlapFrac;
    }

    inline Bool_t KTSlidingWindowFFT::GetUseOverlapFrac() const
    {
        return fUseOverlapFrac;
    }

    inline UInt_t KTSlidingWindowFFT::GetEffectiveOverlap() const
    {
        if (fUseOverlapFrac) return (UInt_t)TMath::Nint(fOverlapFrac * (Double_t)this->fWindowFunction->GetSize());
        return fOverlap;
    }

    inline const vector< Double_t >& KTSlidingWindowFFT::GetTimeData() const
    {
        return fTimeData;
    }

    inline KTPowerSpectrum* KTSlidingWindowFFT::GetPowerSpectrum(Int_t spec) const
    {
        if (spec >= 0 && spec < (Int_t)fPowerSpectra.size()) return fPowerSpectra[spec];
        return NULL;
    }

    inline KTWindowFunction* KTSlidingWindowFFT::GetWindowFunction() const
    {
        return fWindowFunction;
    }

    inline UInt_t KTSlidingWindowFFT::GetWindowSize() const
    {
        return (UInt_t)fWindowFunction->GetSize();
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
        fUseOverlapFrac = kFALSE;
        return;
    }

    inline void KTSlidingWindowFFT::SetOverlap(Double_t overlapTime)
    {
        this->SetOverlap((UInt_t)TMath::Nint(overlapTime / fWindowFunction->GetBinWidth()));
        fUseOverlapFrac = kFALSE;
        return;
    }

    inline void KTSlidingWindowFFT::SetOverlapFrac(Double_t overlapFrac)
    {
        fOverlapFrac = overlapFrac;
        fUseOverlapFrac = kTRUE;
        return;
    }

    inline void KTSlidingWindowFFT::SetUseOverlapFrac(Bool_t useOverlapFrac)
    {
        fUseOverlapFrac = useOverlapFrac;
        return;
    }

    inline void KTSlidingWindowFFT::SetFreqBinWidth(Double_t bw)
    {
        fFreqBinWidth = bw;
        return;
    }

    inline boost::signals2::connection KTSlidingWindowFFT::ConnectToFFTSignal(const SingleFFTSignal::slot_type &subscriber)
    {
        return fSingleFFTSignal.connect(subscriber);
    }

    inline boost::signals2::connection KTSlidingWindowFFT::ConnectToFFTSignal(Int_t group, const SingleFFTSignal::slot_type &subscriber)
    {
        return fSingleFFTSignal.connect(group, subscriber);
    }


} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFT_HH_ */
