/**
 @file KTSlidingWindowFFT.hh
 @brief Contains KTSlidingWindowFFT
 @details Creates a 2-D (frequency vs. time) power spectrum from an event
 @author: N. S. Oblath
 @date: Sep 12, 2011
 */

#ifndef KTSLIDINGWINDOWFFT_HH_
#define KTSLIDINGWINDOWFFT_HH_

#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTMath.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrum.hh"
#include "KTEventWindowFunction.hh"

#include <complex>
#include <fftw3.h>

#include <stdexcept>
#include <string>

class TH2D;

namespace Katydid
{
    KTLOGGER(fftlog_sw, "katydid.fft");

    class KTEggHeader;
    class KTEvent;
    class KTSlidingWindowFSData;
    class KTTimeSeriesReal;
    class KTTimeSeriesData;
    class KTPStoreNode;
    class KTWriteableData;

    /*!
     @class KTSlidingWindowFFT
     @author N. S. Oblath

     @brief Creates a 2-D (frequency vs. time) power spectrum from an event

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
     \li \c "input-data-name": string -- name used to find data when processing events
    */

   class KTSlidingWindowFFT : public KTFFT, public KTProcessor
    {
        public:
            typedef KTSignal< void (UInt_t, KTFrequencySpectrum*) >::signal SingleFFTSignal;
            typedef KTSignal< void (const KTWriteableData*) >::signal FullFFTSignal;

        protected:
            typedef std::map< std::string, Int_t > TransformFlagMap;

        public:
            KTSlidingWindowFFT();
            virtual ~KTSlidingWindowFFT();

            Bool_t Configure(const KTPStoreNode* node);

            void InitializeFFT();
            void RecreateFFT();

            KTSlidingWindowFSData* TransformData(const KTTimeSeriesData* tsData);

            KTPhysicalArray< 1, KTFrequencySpectrum* >* Transform(const KTTimeSeriesReal* data) const;

            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;
            virtual Double_t GetMinFrequency(Double_t timeBinWidth) const;
            virtual Double_t GetMaxFrequency(Double_t timeBinWidth) const;

            UInt_t GetOverlap() const;
            UInt_t GetEffectiveOverlap() const;
            Double_t GetOverlapFrac() const;
            Bool_t GetUseOverlapFrac() const;
            KTEventWindowFunction* GetWindowFunction() const;

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
            void SetWindowFunction(KTEventWindowFunction* wf);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        protected:
            UInt_t CalculateNFrequencyBins(UInt_t nTimeBins) const; // do not make this virtual (called from the constructor)
            virtual KTFrequencySpectrum* ExtractTransformResult(Double_t freqMin, Double_t freqMax) const;
            void SetupTransformFlagMap(); // do not make this virtual (called from the constructor)

            fftw_plan fFTPlan;
            Double_t* fInputArray;
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
            void ProcessEvent(KTEvent* event);

    };


    inline UInt_t KTSlidingWindowFFT::GetTimeSize() const
    {
        if (fWindowFunction == NULL) return 0;
        return fWindowFunction->GetSize();
    }

    inline UInt_t KTSlidingWindowFFT::GetFrequencySize() const
    {
        if (fWindowFunction == NULL) return 0;
        return CalculateNFrequencyBins(fWindowFunction->GetSize());
    }

    inline Double_t KTSlidingWindowFFT::GetMinFrequency(Double_t timeBinWidth) const
    {
        return -0.5 * GetFrequencyBinWidth(timeBinWidth);
    }

    inline Double_t KTSlidingWindowFFT::GetMaxFrequency(Double_t timeBinWidth) const
    {
        return GetFrequencyBinWidth(timeBinWidth) * ((Double_t)GetFrequencySize() - 0.5);
    }

    inline const std::string& KTSlidingWindowFFT::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTSlidingWindowFFT::GetIsInitialized() const
    {
        return fIsInitialized;
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
        if (fUseOverlapFrac) return (UInt_t)KTMath::Nint(fOverlapFrac * (Double_t)this->fWindowFunction->GetSize());
        return fOverlap;
    }

    inline KTEventWindowFunction* KTSlidingWindowFFT::GetWindowFunction() const
    {
        return fWindowFunction;
    }

    inline void KTSlidingWindowFFT::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(fftlog_sw, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }
        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    inline void KTSlidingWindowFFT::SetOverlap(UInt_t nBins)
    {
        fOverlap = nBins;
        fUseOverlapFrac = false;
        return;
    }

    inline void KTSlidingWindowFFT::SetOverlap(Double_t overlapTime)
    {
        this->SetOverlap((UInt_t)KTMath::Nint(overlapTime / fWindowFunction->GetBinWidth()));
        fUseOverlapFrac = false;
        return;
    }

    inline void KTSlidingWindowFFT::SetOverlapFrac(Double_t overlapFrac)
    {
        fOverlapFrac = overlapFrac;
        fUseOverlapFrac = false;
        return;
    }

    inline void KTSlidingWindowFFT::SetUseOverlapFrac(Bool_t useOverlapFrac)
    {
        fUseOverlapFrac = useOverlapFrac;
        return;
    }

    inline Bool_t KTSlidingWindowFFT::GetUseWisdom() const
    {
        return fUseWisdom;
    }

    inline const std::string& KTSlidingWindowFFT::GetWisdomFilename() const
    {
        return fWisdomFilename;
    }

    inline void KTSlidingWindowFFT::SetUseWisdom(Bool_t flag)
    {
        fUseWisdom = flag;
        return;
    }

    inline void KTSlidingWindowFFT::SetWisdomFilename(const std::string& fname)
    {
        fWisdomFilename = fname;
        return;
    }

    inline const std::string& KTSlidingWindowFFT::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTSlidingWindowFFT::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTSlidingWindowFFT::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTSlidingWindowFFT::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

    inline UInt_t KTSlidingWindowFFT::CalculateNFrequencyBins(UInt_t nTimeBins) const
    {
        // Integer division is rounded down, per FFTW's instructions
        return nTimeBins / 2 + 1;
    }

} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFT_HH_ */
