/**
 @file KTSimpleFFT.hh
 @brief Contains KTSimpleFFT
 @details Calculates a 1-dimensional FFT on a set of real data.
 @author: N. S. Oblath
 @date: Sep 12, 2011
 */

#ifndef KTSIMPLEFFT_HH_
#define KTSIMPLEFFT_HH_

#include "KTFFT.hh"

#include "TFFTRealComplex.h"

#include <string>

//class TArray;
class TH1D;

namespace Katydid
{
    class KTComplexVector;
    class KTPowerSpectrum;
    class KTPStoreNode;
    class KTEvent;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    /*!
     @class KTSimpleFFT
     @author N. S. Oblath

     @brief A one-dimensional real-to-complex FFT class.

     @details
     KTSimpleFFT performs a real-to-complex FFT on a one-dimensional array of doubles.

     The FFT is currently performed by ROOT's adaptation of FFTW. Specifically the TFFTRealComplex class.

     Available configuration values:
     \li \c transform_flag -- flag that determines how much planning is done prior to any transforms

    */

    class KTSimpleFFT : public KTFFT
    {
        public:
            KTSimpleFFT();
            KTSimpleFFT(UInt_t timeSize);
            virtual ~KTSimpleFFT();

            Bool_t Configure(const KTPStoreNode* node);

            void InitializeFFT();

            virtual Bool_t TakeData(const KTEvent* event);
            virtual Bool_t TakeData(const vector< Double_t >& data);
            //virtual Bool_t TakeData(const TArray* data);

            virtual Bool_t Transform();

            TH1D* CreatePowerSpectrumHistogram(const std::string& name) const;
            TH1D* CreatePowerSpectrumHistogram() const;

            KTPhysicalArray< 1, Double_t >* CreatePowerSpectrumPhysArr() const;

            KTPowerSpectrum* CreatePowerSpectrum() const;
            UInt_t GetTimeSize() const;
            UInt_t GetFrequencySize() const;

            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized and fIsDataReady to kFALSE.
            void SetTimeSize(UInt_t nBins);

            const TFFTRealComplex* GetFFT() const;
            const KTComplexVector* GetTransformResult() const;
            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Bool_t GetIsDataReady() const;
            Double_t GetFreqBinWidth() const;

            /// note: SetTransoformFlag sets fIsInitialized and fIsDataReady to kFALSE.
            void SetTransformFlag(const std::string& flag);
            void SetFreqBinWidth(Double_t bw);

        protected:
            void ExtractTransformResult();

            TFFTRealComplex* fTransform;
            KTComplexVector* fTransformResult;

            std::string fTransformFlag;

            Bool_t fIsInitialized;
            Bool_t fIsDataReady;

            Double_t fFreqBinWidth;

            ClassDef(KTSimpleFFT, 2);
    };


    inline UInt_t KTSimpleFFT::GetTimeSize() const
    {
        return fTransform->GetSize();
    }

    inline UInt_t KTSimpleFFT::GetFrequencySize() const
    {
        return fTransform->GetSize() / 2 + 1;
    }

    inline void KTSimpleFFT::SetTimeSize(UInt_t nBins)
    {
        delete fTransform;
        fTransform = new TFFTRealComplex((Int_t)nBins, kFALSE);
        fIsInitialized = kFALSE;
        fIsDataReady = kFALSE;
        return;
    }

    inline const TFFTRealComplex* KTSimpleFFT::GetFFT() const
    {
        return fTransform;
    }

    inline const KTComplexVector* KTSimpleFFT::GetTransformResult() const
    {
        return fTransformResult;
    }

    inline const std::string& KTSimpleFFT::GetTransformFlag() const
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

    inline void KTSimpleFFT::SetTransformFlag(const std::string& flag)
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
