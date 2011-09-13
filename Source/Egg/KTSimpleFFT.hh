/*
 * KTSimpleFFT.hh
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#ifndef KTSIMPLEFFT_HH_
#define KTSIMPLEFFT_HH_

#include "TFFTRealComplex.h"

#include <string>
using std::string;

class TArray;

namespace Katydid
{
    class KTPowerSpectrum;
    class KTEvent;

    class KTSimpleFFT
    {
        public:
            KTSimpleFFT();
            KTSimpleFFT(Int_t timeSize);
            virtual ~KTSimpleFFT();

            virtual void InitializeFFT();

            virtual void TakeData(const KTEvent* event);
            virtual void TakeData(const TArray* data);

            virtual void Transform();

            KTPowerSpectrum* CreatePowerSpectrum() const;
            Int_t GetTimeSize() const;
            Int_t GetFrequencySize() const;

            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized and fIsDataReady to kFALSE.
            virtual void SetTimeSize(Int_t nBins);

            const TFFTRealComplex* GetFFT() const;
            const string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Bool_t GetIsDataReady() const;
            Double_t GetBinWidth() const;

            /// note: SetTransoformFlag sets fIsInitialized and fIsDataReady to kFALSE.
            void SetTransformFlag(const string& flag);
            void SetBinWidth(Double_t bw);

        protected:

            TFFTRealComplex* fTransform;

            string fTransformFlag;

            Bool_t fIsInitialized;
            Bool_t fIsDataReady;

            Double_t fBinWidth;

            ClassDef(KTSimpleFFT, 1);
    };

    inline Int_t KTSimpleFFT::GetTimeSize() const
    {
        return fTransform->GetSize();
    }

    inline Int_t KTSimpleFFT::GetFrequencySize() const
    {
        return fTransform->GetSize() / 2 + 1;
    }

    inline void KTSimpleFFT::SetTimeSize(Int_t nBins)
    {
        delete fTransform;
        fTransform = new TFFTRealComplex(nBins, kFALSE);
        fIsInitialized = kFALSE;
        fIsDataReady = kFALSE;
        return;
    }

    inline const TFFTRealComplex* KTSimpleFFT::GetFFT() const
    {
        return fTransform;
    }

    inline const string& KTSimpleFFT::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTSimpleFFT::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline Bool_t KTSimpleFFT::GetIsDataReady() const
    {
        return fIsDataReady;
    }

    inline Double_t KTSimpleFFT::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTSimpleFFT::SetTransformFlag(const string& flag)
    {
        fTransformFlag = flag;
        fIsInitialized = kFALSE;
        fIsDataReady = kFALSE;
        return;
    }

    inline void KTSimpleFFT::SetBinWidth(Double_t bw)
    {
        fBinWidth = bw;
        return;
    }

} /* namespace Katydid */
#endif /* KTSIMPLEFFT_HH_ */
