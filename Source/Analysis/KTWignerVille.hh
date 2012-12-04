/*
 * KTWignerVille.hh
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#ifndef KTWIGNERVILLE_HH_
#define KTWIGNERVILLE_HH_

#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTMath.hh"

#include "KTEventWindowFunction.hh"

#include <boost/shared_ptr.hpp>

#include <complex>
#include <fftw3.h>


namespace Katydid
{
    class KTComplexFFTW;
    class KTEggHeader;
    class KTEvent;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTSlidingWindowFFTW;
    class KTSlidingWindowFSDataFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTWriteableData;

    typedef std::pair< UInt_t, UInt_t > KTWVPair;

    class KTWignerVille : public KTFFT, public KTProcessor
    {
        protected:
            typedef KTSignal< void (const KTWriteableData*) >::signal WVSignal;
            typedef std::vector< KTWVPair > PairVector;

        protected:
            typedef std::map< std::string, Int_t > TransformFlagMap;

        public:
            KTWignerVille();
            virtual ~KTWignerVille();

            Bool_t Configure(const KTPStoreNode* node);

            KTComplexFFTW* GetFullFFT() const;
            KTSlidingWindowFFTW* GetWindowedFFT() const;

            Bool_t GetSaveAAFrequencySpectrum() const;
            Bool_t GetSaveAnalyticAssociate() const;
            Bool_t GetSaveCrossMultipliedTimeSeries() const;

            void SetSaveAAFrequencySpectrum(Bool_t flag);
            void SetSaveAnalyticAssociate(Bool_t flag);
            void SetSaveCrossMultipliedTimeSeries(Bool_t flag);

            void AddPair(const KTWVPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

            const std::string& GetAAFSOutputDataName() const;
            void SetAAFSOutputDataName(const std::string& name);

            const std::string& GetAATSOutputDataName() const;
            void SetAATSOutputDataName(const std::string& name);

            const std::string& GetCMTSOutputDataName() const;
            void SetCMTSOutputDataName(const std::string& name);

        protected:
            PairVector fPairs;


        protected:
            KTComplexFFTW* fFullFFT;
            KTSlidingWindowFFTW* fWindowedFFT;

            Bool_t fSaveAAFrequencySpectrum;
            Bool_t fSaveAnalyticAssociate;
            Bool_t fSaveCrossMultipliedTimeSeries;

            std::string fInputDataName;
            std::string fOutputDataName;

            std::string fAAFSOutputDataName;
            std::string fAATSOutputDataName;
            std::string fCMTSOutputDataName;

        public:
            /// Performs the W-V transform on the given time series data.
            /// In the process, the data is FFTed, and then reverse FFTed; if you want to keep the intermediate frequency spectrum, pass a KTFrequencySpectrumDataFFTW** as the second parameter..
            /// @note A frequency spectrum data object can still be returned even if the full W-V transform fails!
            KTSlidingWindowFSDataFFTW* TransformData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData=NULL, KTTimeSeriesData** outputAAData=NULL, KTTimeSeriesData** outputCMTSData=NULL);

            /// Performs the W-V transform on the given time series.
            /// In the process, the data is FFTed, and then reverse FFTed. If you want to keep the intermediate frequency spectrum, pass a KTFrequencySpectrumFFTW** as the second parameter.
            /// @note A frequency spectrum object can still be returned even if the full W-V transform fails.
            //KTTimeSeriesFFTW* Transform(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);

            /// Performs the W-V transform on the given frequency spectrum (in place! does NOT create a new FS)
            //Bool_t Transform(KTFrequencySpectrumFFTW* freqSpectrum);

            void InitializeFFT();
            void RecreateFFT();

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;

            UInt_t GetSize() const;
            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;
            virtual Double_t GetMinFrequency(Double_t timeBinWidth) const;
            virtual Double_t GetMaxFrequency(Double_t timeBinWidth) const;

            UInt_t GetWindowSize() const;
            UInt_t GetOverlap() const;
            UInt_t GetEffectiveOverlap() const;
            Double_t GetOverlapFrac() const;
            Bool_t GetUseOverlapFrac() const;
            KTEventWindowFunction* GetWindowFunction() const;

            void SetTransformFlag(const std::string& flag);
            void SetWindowSize(UInt_t nBins);
            void SetWindowLength(Double_t wlTime);
            void SetOverlap(UInt_t nBins);
            void SetOverlap(Double_t overlapTime);
            void SetOverlapFrac(Double_t overlapFrac);
            void SetUseOverlapFrac(Bool_t useOverlapFrac);
            void SetWindowFunction(KTEventWindowFunction* wf);


        private:
            /// Calculates the AA and returns the new time series; the intermediate FS is assigned to the given output pointer.
            KTTimeSeriesFFTW* CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);
            /// Calculates the AA in place.
            Bool_t CalculateAnalyticAssociate(KTFrequencySpectrumFFTW* freqSpectrum);

            KTTimeSeriesFFTW* CrossMultiply(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2);

            void CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset);
            KTFrequencySpectrumFFTW* ExtractTransformResult(Double_t freqMin, Double_t freqMax) const;
            void SetupTransformFlagMap(); // do not make this virtual (called from the constructor)

            fftw_plan fFTPlan;
            fftw_complex* fInputArray;
            fftw_complex* fOutputArray;

            std::string fTransformFlag;
            TransformFlagMap fTransformFlagMap;

            Bool_t fIsInitialized;

            UInt_t fOverlap;
            Double_t fOverlapFrac;
            Bool_t fUseOverlapFrac;

            KTEventWindowFunction* fWindowFunction;



            //***************
             // Signals
             //***************

         private:
             WVSignal fWVSignal;

             //***************
             // Slots
             //***************

         public:
             void ProcessHeader(const KTEggHeader* header);
             void ProcessEvent(boost::shared_ptr<KTEvent> event);
             void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);
             //void ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData);

    };

    inline Bool_t KTWignerVille::GetSaveAAFrequencySpectrum() const
    {
        return fSaveAAFrequencySpectrum;
    }

    inline void KTWignerVille::SetSaveAAFrequencySpectrum(Bool_t flag)
    {
        fSaveAAFrequencySpectrum = flag;
        return;
    }

    inline Bool_t KTWignerVille::GetSaveAnalyticAssociate() const
    {
        return fSaveAnalyticAssociate;
    }

    inline void KTWignerVille::SetSaveAnalyticAssociate(Bool_t flag)
    {
        fSaveAnalyticAssociate = flag;
        return;
    }

    inline Bool_t KTWignerVille::GetSaveCrossMultipliedTimeSeries() const
    {
        return fSaveCrossMultipliedTimeSeries;
    }

    inline void KTWignerVille::SetSaveCrossMultipliedTimeSeries(Bool_t flag)
    {
        fSaveCrossMultipliedTimeSeries = flag;
        return;
    }

    inline void KTWignerVille::AddPair(const KTWVPair& pair)
    {
        fPairs.push_back(pair);
        return;
    }

    inline void KTWignerVille::SetPairVector(const PairVector& pairs)
    {
        fPairs = pairs;
        return;
    }

    inline const KTWignerVille::PairVector& KTWignerVille::GetPairVector() const
    {
        return fPairs;
    }

    inline void KTWignerVille::ClearPairs()
    {
        fPairs.clear();
        return;
    }

    inline const std::string& KTWignerVille::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTWignerVille::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTWignerVille::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTWignerVille::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

    inline const std::string& KTWignerVille::GetAAFSOutputDataName() const
    {
        return fAAFSOutputDataName;
    }

    inline void KTWignerVille::SetAAFSOutputDataName(const std::string& name)
    {
        fAAFSOutputDataName = name;
        return;
    }

    inline const std::string& KTWignerVille::GetAATSOutputDataName() const
    {
        return fAATSOutputDataName;
    }

    inline void KTWignerVille::SetAATSOutputDataName(const std::string& name)
    {
        fAATSOutputDataName = name;
        return;
    }

    inline const std::string& KTWignerVille::GetCMTSOutputDataName() const
    {
        return fCMTSOutputDataName;
    }

    inline void KTWignerVille::SetCMTSOutputDataName(const std::string& name)
    {
        fCMTSOutputDataName = name;
        return;
    }

    inline UInt_t KTWignerVille::GetSize() const
    {
        if (fWindowFunction == NULL) return 0;
        return fWindowFunction->GetSize();
    }

    inline UInt_t KTWignerVille::GetTimeSize() const
    {
        return GetSize();
    }

    inline UInt_t KTWignerVille::GetFrequencySize() const
    {
        return GetSize();
    }

    inline Double_t KTWignerVille::GetMinFrequency(Double_t timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the negative side is nFreqBins/2 (rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the negative frequency side.
        return -GetFrequencyBinWidth(timeBinWidth) * (Double_t(GetSize()/2) + 0.5);
    }

    inline Double_t KTWignerVille::GetMaxFrequency(Double_t timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the positive side is nFreqBins/2 if the number of bins is odd, and nFreqBins/2-1 if the number of bins is even (division rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the positive frequency side.
        UInt_t nBins = GetSize();
        UInt_t nBinsToSide = nBins / 2;
        return GetFrequencyBinWidth(timeBinWidth) * (Double_t(nBinsToSide*2 == nBins ? nBinsToSide - 1 : nBinsToSide) + 0.5);
   }

    inline const std::string& KTWignerVille::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTWignerVille::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline UInt_t KTWignerVille::GetOverlap() const
    {
        return fOverlap;
    }

    inline Double_t KTWignerVille::GetOverlapFrac() const
    {
        return fOverlapFrac;
    }

    inline Bool_t KTWignerVille::GetUseOverlapFrac() const
    {
        return fUseOverlapFrac;
    }

    inline UInt_t KTWignerVille::GetEffectiveOverlap() const
    {
        if (fUseOverlapFrac) return (UInt_t)KTMath::Nint(fOverlapFrac * (Double_t)this->fWindowFunction->GetSize());
        return fOverlap;
    }

    inline KTEventWindowFunction* KTWignerVille::GetWindowFunction() const
    {
        return fWindowFunction;
    }

    inline void KTWignerVille::SetOverlap(UInt_t nBins)
    {
        fOverlap = nBins;
        fUseOverlapFrac = false;
        return;
    }

    inline void KTWignerVille::SetOverlap(Double_t overlapTime)
    {
        this->SetOverlap((UInt_t)KTMath::Nint(overlapTime / fWindowFunction->GetBinWidth()));
        fUseOverlapFrac = false;
        return;
    }

    inline void KTWignerVille::SetOverlapFrac(Double_t overlapFrac)
    {
        fOverlapFrac = overlapFrac;
        fUseOverlapFrac = false;
        return;
    }

    inline void KTWignerVille::SetUseOverlapFrac(Bool_t useOverlapFrac)
    {
        fUseOverlapFrac = useOverlapFrac;
        return;
    }

} /* namespace Katydid */
#endif /* KTWIGNERVILLE_HH_ */
