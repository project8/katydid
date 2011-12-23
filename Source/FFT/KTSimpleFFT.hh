/*
 * KTSimpleFFT.hh
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#ifndef KTSIMPLEFFT_HH_
#define KTSIMPLEFFT_HH_

#include "KTFFT.hh"

#include "TFFTRealComplex.h"

#include <string>
using std::string;

//class TArray;
class TH1D;

namespace Katydid
{
    class KTPowerSpectrum;
    class KTEvent;

    class KTSimpleFFT : public KTFFT
    {
        public:
            KTSimpleFFT();
            KTSimpleFFT(UInt_t timeSize);
            virtual ~KTSimpleFFT();

            virtual void InitializeFFT();

            virtual void TakeData(const KTEvent* event);
            virtual void TakeData(const vector< Double_t >& data);
            //virtual void TakeData(const TArray* data);

            virtual void Transform();

            virtual TH1D* CreatePowerSpectrumHistogram() const;

            virtual KTPowerSpectrum* CreatePowerSpectrum() const;
            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;

            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized and fIsDataReady to kFALSE.
            virtual void SetTimeSize(UInt_t nBins);

            const TFFTRealComplex& GetFFT() const;
            const string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Bool_t GetIsDataReady() const;
            Double_t GetFreqBinWidth() const;

            /// note: SetTransoformFlag sets fIsInitialized and fIsDataReady to kFALSE.
            void SetTransformFlag(const string& flag);
            void SetFreqBinWidth(Double_t bw);

        protected:

            TFFTRealComplex fTransform;

            string fTransformFlag;

            Bool_t fIsInitialized;
            Bool_t fIsDataReady;

            Double_t fFreqBinWidth;

            ClassDef(KTSimpleFFT, 2);
    };


    inline UInt_t KTSimpleFFT::GetTimeSize() const
    {
        return fTransform.GetSize();
    }

    inline UInt_t KTSimpleFFT::GetFrequencySize() const
    {
        return fTransform.GetSize() / 2 + 1;
    }

    inline void KTSimpleFFT::SetTimeSize(UInt_t nBins)
    {
        fTransform = TFFTRealComplex((Int_t)nBins, kFALSE);
        fIsInitialized = kFALSE;
        fIsDataReady = kFALSE;
        return;
    }

    inline const TFFTRealComplex& KTSimpleFFT::GetFFT() const
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

    inline Double_t KTSimpleFFT::GetFreqBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline void KTSimpleFFT::SetTransformFlag(const string& flag)
    {
        fTransformFlag = flag;
        fIsInitialized = kFALSE;
        fIsDataReady = kFALSE;
        return;
    }

    inline void KTSimpleFFT::SetFreqBinWidth(Double_t bw)
    {
        fFreqBinWidth = bw;
        return;
    }

} /* namespace Katydid */
#endif /* KTSIMPLEFFT_HH_ */
