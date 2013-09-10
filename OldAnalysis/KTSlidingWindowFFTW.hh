/**
 @file KTSlidingWindowFFTW.hh
 @brief Contains KTSlidingWindowFFTW
 @details Creates a 2-D (frequency vs. time) power spectrum from an bundle
 @author: N. S. Oblath
 @date: Sep 12, 2011
 */

#ifndef KTSLIDINGWINDOWFFTW_HH_
#define KTSLIDINGWINDOWFFTW_HH_

#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTMath.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTBundleWindowFunction.hh"

#include <complex>
#include <fftw3.h>

#include <stdexcept>
#include <string>

class TH2D;

namespace Katydid
{
    KTLOGGER(fftlog_sw_fftw, "katydid.fft");

    class KTEggHeader;
    class KTBundle;
    class KTSlidingWindowFSDataFFTW;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesData;
    class KTPStoreNode;
    class KTWriteableData;

    /*!
     @class KTSlidingWindowFFTW
     @author N. S. Oblath

     @brief Creates a 2-D (frequency vs. time) power spectrum from an bundle

     @details
     Slides a window along the length of the (time-domain) data, performing Fourier Transforms as the window moves.
     The distance the window slides between transforms is given by the overlap or overlap fraction.
     \li The overlap is the number of bins or time units of overlap between the window from one transform and the next.
     \li The overlap fraction gives the overlap between the window from one transform and the next as a fraction of the window length.
     Any type of window function (inheriting from KTWindowFunction) can be used.

     Available configuration values:
     \li \c "transform-flag": string -- flag that determines how much planning is done prior to any transforms
     \li \c "use-wisdom": bool -- whether or not to use FFTW wisdom to improve FFT performance
     \li \c "wisdom-filename": string -- filename for loading/saving FFTW wisdom
     \li \c "overlap-time": float -- sets the overlap in time units
     \li \c "overlap-size": int -- sets the overlap in number of bins
     \li \c "overlap-frac": float -- sets the overlap in fraction of the window length
     \li \c "window-function-type": string -- sets the type of window function to be used
     \li \c "window-function": subtree -- parent node for the window function configuration
     \li \c "input-data-name": string -- name used to find data when processing bundles
    */

   class KTSlidingWindowFFTW : public KTFFT, public KTProcessor
    {
        public:
            typedef KTSignalConcept< void (UInt_t, KTFrequencySpectrumFFTW*) >::signal SingleFFTSignal;
            typedef KTSignalConcept< void (const KTWriteableData*) >::signal FullFFTSignal;

        protected:
            typedef std::map< std::string, Int_t > TransformFlagMap;

        public:
            KTSlidingWindowFFTW();
            virtual ~KTSlidingWindowFFTW();

            Bool_t Configure(const KTPStoreNode* node);

            void InitializeFFT();
            void RecreateFFT();

            KTSlidingWindowFSDataFFTW* TransformData(const KTTimeSeriesData* tsData);

            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* Transform(const KTTimeSeriesFFTW* data) const;

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
            KTBundleWindowFunction* GetWindowFunction() const;

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Bool_t GetUseWisdom() const;
            const std::string& GetWisdomFilename() const;

            /// note: SetTransformFlag sets fIsInitialized to kFALSE.
            void SetTransformFlag(const std::string& flag);
            void SetUseWisdom(Bool_t flag);
            void SetWisdomFilename(const std::string& fname);
            void SetWindowSize(UInt_t nBins);
            void SetWindowLength(Double_t wlTime);
            void SetOverlap(UInt_t nBins);
            void SetOverlap(Double_t overlapTime);
            void SetOverlapFrac(Double_t overlapFrac);
            void SetUseOverlapFrac(Bool_t useOverlapFrac);
            void SetWindowFunction(KTBundleWindowFunction* wf);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        protected:
            virtual KTFrequencySpectrumFFTW* ExtractTransformResult(Double_t freqMin, Double_t freqMax) const;
            void SetupTransformFlagMap(); // do not make this virtual (called from the constructor)

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

            KTBundleWindowFunction* fWindowFunction;
            //std::vector< std::vector< KTPowerSpectrum* >* > fPowerSpectra;

            std::string fInputDataName;
            std::string fOutputDataName;


            //***************
            // Signals
            //***************

        private:
            SingleFFTSignal fSingleFFTSignal;
            FullFFTSignal fFullFFTSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessHeader(const KTEggHeader* header);
            void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);
            void ProcessBundle(KTBundle* bundle);

    };

    inline UInt_t KTSlidingWindowFFTW::GetSize() const
    {
        if (fWindowFunction == NULL) return 0;
        return fWindowFunction->GetSize();
    }

    inline UInt_t KTSlidingWindowFFTW::GetTimeSize() const
    {
        return GetSize();
    }

    inline UInt_t KTSlidingWindowFFTW::GetFrequencySize() const
    {
        return GetSize();
    }

    inline Double_t KTSlidingWindowFFTW::GetMinFrequency(Double_t timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the negative side is nFreqBins/2 (rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the negative frequency side.
        return -GetFrequencyBinWidth(timeBinWidth) * (Double_t(GetSize()/2) + 0.5);
    }

    inline Double_t KTSlidingWindowFFTW::GetMaxFrequency(Double_t timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the positive side is nFreqBins/2 if the number of bins is odd, and nFreqBins/2-1 if the number of bins is even (division rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the positive frequency side.
        UInt_t nBins = GetSize();
        UInt_t nBinsToSide = nBins / 2;
        return GetFrequencyBinWidth(timeBinWidth) * (Double_t(nBinsToSide*2 == nBins ? nBinsToSide - 1 : nBinsToSide) + 0.5);
   }

    inline const std::string& KTSlidingWindowFFTW::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTSlidingWindowFFTW::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline UInt_t KTSlidingWindowFFTW::GetOverlap() const
    {
        return fOverlap;
    }

    inline Double_t KTSlidingWindowFFTW::GetOverlapFrac() const
    {
        return fOverlapFrac;
    }

    inline Bool_t KTSlidingWindowFFTW::GetUseOverlapFrac() const
    {
        return fUseOverlapFrac;
    }

    inline UInt_t KTSlidingWindowFFTW::GetEffectiveOverlap() const
    {
        if (fUseOverlapFrac) return (UInt_t)KTMath::Nint(fOverlapFrac * (Double_t)this->fWindowFunction->GetSize());
        return fOverlap;
    }

    inline KTBundleWindowFunction* KTSlidingWindowFFTW::GetWindowFunction() const
    {
        return fWindowFunction;
    }

    inline void KTSlidingWindowFFTW::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(fftlog_sw_fftw, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }
        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    inline void KTSlidingWindowFFTW::SetOverlap(UInt_t nBins)
    {
        fOverlap = nBins;
        fUseOverlapFrac = false;
        return;
    }

    inline void KTSlidingWindowFFTW::SetOverlap(Double_t overlapTime)
    {
        this->SetOverlap((UInt_t)KTMath::Nint(overlapTime / fWindowFunction->GetBinWidth()));
        fUseOverlapFrac = false;
        return;
    }

    inline void KTSlidingWindowFFTW::SetOverlapFrac(Double_t overlapFrac)
    {
        fOverlapFrac = overlapFrac;
        fUseOverlapFrac = false;
        return;
    }

    inline void KTSlidingWindowFFTW::SetUseOverlapFrac(Bool_t useOverlapFrac)
    {
        fUseOverlapFrac = useOverlapFrac;
        return;
    }

    inline Bool_t KTSlidingWindowFFTW::GetUseWisdom() const
    {
        return fUseWisdom;
    }

    inline const std::string& KTSlidingWindowFFTW::GetWisdomFilename() const
    {
        return fWisdomFilename;
    }

    inline void KTSlidingWindowFFTW::SetUseWisdom(Bool_t flag)
    {
        fUseWisdom = flag;
        return;
    }

    inline void KTSlidingWindowFFTW::SetWisdomFilename(const std::string& fname)
    {
        fWisdomFilename = fname;
        return;
    }

    inline const std::string& KTSlidingWindowFFTW::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTSlidingWindowFFTW::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTSlidingWindowFFTW::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTSlidingWindowFFTW::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFTW_HH_ */
