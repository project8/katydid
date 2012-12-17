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
    class KTFrequencySpectrum;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTSlidingWindowFFTW;
    class KTSlidingWindowFSData;
    class KTSlidingWindowFSDataFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTWriteableData;

    typedef std::pair< UInt_t, UInt_t > KTWVPair;

    class KTWignerVille : public KTFFT, public KTProcessor
    {
        private:
            typedef KTSignal< void (const KTWriteableData*) >::signal WVSignal;
            typedef std::vector< KTWVPair > PairVector;

        private:
            typedef std::map< std::string, Int_t > TransformFlagMap;

        public:
            KTWignerVille();
            virtual ~KTWignerVille();

            Bool_t Configure(const KTPStoreNode* node);

            void AddPair(const KTWVPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        private:
            PairVector fPairs;

            std::string fInputDataName;
            std::string fOutputDataName;

        public:
            void InitializeFFT();
            void RecreateFFT();

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Bool_t GetUseWisdom() const;
            const std::string& GetWisdomFilename() const;

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
            void SetUseWisdom(Bool_t flag);
            void SetWisdomFilename(const std::string& fname);
            void SetWindowSize(UInt_t nBins);
            void SetWindowLength(Double_t wlTime);
            void SetOverlap(UInt_t nBins);
            void SetOverlap(Double_t overlapTime);
            void SetOverlapFrac(Double_t overlapFrac);
            void SetUseOverlapFrac(Bool_t useOverlapFrac);
            void SetWindowFunction(KTEventWindowFunction* wf);

        private:
            void SetupTransformFlagMap(); // do not make this virtual (called from the constructor)

            UInt_t CalculateNFrequencyBins(UInt_t nTimeBins) const;

            fftw_plan fFTPlan;
            fftw_complex* fInputArray;
            fftw_complex* fOutputArray;

            std::string fTransformFlag;
            TransformFlagMap fTransformFlagMap;

            Bool_t fIsInitialized;
            Bool_t fUseWisdom;
            std::string fWisdomFilename;

            UInt_t fOverlap;
            Double_t fOverlapFrac;
            Bool_t fUseOverlapFrac;

            KTEventWindowFunction* fWindowFunction;


        public:
            /// Performs the W-V transform on the given time series data.
            /// In the process, the data is FFTed, and then reverse FFTed; if you want to keep the intermediate frequency spectrum, pass a KTFrequencySpectrumDataFFTW** as the second parameter..
            /// @note A frequency spectrum data object can still be returned even if the full W-V transform fails!
            KTSlidingWindowFSDataFFTW* TransformData(const KTTimeSeriesData* data);

        private:
            /// Calculates the AA and returns the new time series; the intermediate FS is assigned to the given output pointer.
            KTTimeSeriesFFTW* CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);

            void CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset);
            KTFrequencySpectrumFFTW* ExtractTransformResult(Double_t freqMin, Double_t freqMax) const;



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

    };

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

    inline UInt_t KTWignerVille::GetTimeSize() const
    {
        if (fWindowFunction == NULL) return 0;
        return fWindowFunction->GetSize();
    }

    inline UInt_t KTWignerVille::GetFrequencySize() const
    {
        //return CalculateNFrequencyBins(GetTimeSize());
        return GetTimeSize();
    }

    inline Double_t KTWignerVille::GetMinFrequency(Double_t timeBinWidth) const
    {
        //return -0.5 * GetFrequencyBinWidth(timeBinWidth);
        /**/
        // There's one bin at the center, always: the DC bin.
        // # of bins on the negative side is nFreqBins/2 (rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the negative frequency side.
        return -GetFrequencyBinWidth(timeBinWidth) * (Double_t(GetTimeSize()/2) + 0.5);
        /**/
    }

    inline Double_t KTWignerVille::GetMaxFrequency(Double_t timeBinWidth) const
    {
        //return GetFrequencyBinWidth(timeBinWidth) * ((Double_t)GetFrequencySize() - 0.5);
        /**/
        // There's one bin at the center, always: the DC bin.
        // # of bins on the positive side is nFreqBins/2 if the number of bins is odd, and nFreqBins/2-1 if the number of bins is even (division rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the positive frequency side.
        UInt_t nBins = GetTimeSize();
        UInt_t nBinsToSide = nBins / 2;
        return GetFrequencyBinWidth(timeBinWidth) * (Double_t(nBinsToSide*2 == nBins ? nBinsToSide - 1 : nBinsToSide) + 0.5);
        /**/
    }

    inline UInt_t KTWignerVille::CalculateNFrequencyBins(UInt_t nTimeBins) const
    {
        // Integer division is rounded down, per FFTW's instructions
        return nTimeBins / 2 + 1;
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

    inline Bool_t KTWignerVille::GetUseWisdom() const
    {
        return fUseWisdom;
    }

    inline const std::string& KTWignerVille::GetWisdomFilename() const
    {
        return fWisdomFilename;
    }

    inline void KTWignerVille::SetUseWisdom(Bool_t flag)
    {
        fUseWisdom = flag;
        return;
    }

    inline void KTWignerVille::SetWisdomFilename(const std::string& fname)
    {
        fWisdomFilename = fname;
        return;
    }

} /* namespace Katydid */
#endif /* KTWIGNERVILLE_HH_ */
