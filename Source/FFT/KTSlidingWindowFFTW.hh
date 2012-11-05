/**
 @file KTSlidingWindowFFTW.hh
 @brief Contains KTSlidingWindowFFTW
 @details Creates a 2-D (frequency vs. time) power spectrum from an event
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
    class KTSlidingWindowFSDataFFTW;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesData;
    class KTPStoreNode;

    /*!
     @class KTSlidingWindowFFTW
     @author N. S. Oblath

     @brief Creates a 2-D (frequency vs. time) power spectrum from an event

     @details
     Slides a window along the length of the (time-domain) data, performing Fourier Transforms as the window moves.
     The distance the window slides between transforms is given by the overlap or overlap fraction.
     \li The overlap is the number of bins or time units of overlap between the window from one transform and the next.
     \li The overlap fraction gives the overlap between the window from one transform and the next as a fraction of the window length.
     Any type of window function (inheriting from KTWindowFunction) can be used.

     Available configuration values:
     \li \c transform_flag -- flag that determines how much planning is done prior to any transforms
     \li \c overlap_time -- sets the overlap in time units
     \li \c overlap_size -- sets the overlap in number of bins
     \li \c overlap_frac -- sets the overlap in fraction of the window length
     \li \c window_function_type -- sets the type of window function to be used
     \li \c window_function -- parent node for the window function configuration
    */

   class KTSlidingWindowFFTW : public KTFFT, public KTProcessor
    {
        public:
            typedef KTSignal< void (UInt_t, KTFrequencySpectrumFFTW*) >::signal SingleFFTSignal;
            typedef KTSignal< void (KTSlidingWindowFSDataFFTW*) >::signal FullFFTSignal;

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

            /// for this FFT, the "TimeSize" is the window size. The "FullTimeSize" is different.
            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;

            UInt_t GetWindowSize() const;
            UInt_t GetOverlap() const;
            UInt_t GetEffectiveOverlap() const;
            Double_t GetOverlapFrac() const;
            Bool_t GetUseOverlapFrac() const;
            KTEventWindowFunction* GetWindowFunction() const;

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;

            /// note: SetTransformFlag sets fIsInitialized to kFALSE.
            void SetTransformFlag(const std::string& flag);
            void SetWindowSize(UInt_t nBins);
            void SetWindowLength(Double_t wlTime);
            void SetOverlap(UInt_t nBins);
            void SetOverlap(Double_t overlapTime);
            void SetOverlapFrac(Double_t overlapFrac);
            void SetUseOverlapFrac(Bool_t useOverlapFrac);
            void SetWindowFunction(KTEventWindowFunction* wf);

        protected:
            UInt_t CalculateNFrequencyBins(UInt_t nTimeBins) const; // do not make this virtual (called from the constructor)
            virtual KTFrequencySpectrumFFTW* ExtractTransformResult(Double_t freqMin, Double_t freqMax) const;
            void SetupTransformFlagMap(); // do not make this virtual (called from the constructor)

            fftw_plan fFTPlan;
            UInt_t fTimeSize;
            Double_t* fInputArray;
            fftw_complex* fOutputArray;

            std::string fTransformFlag;
            TransformFlagMap fTransformFlagMap;

            Bool_t fIsInitialized;

            UInt_t fOverlap;
            Double_t fOverlapFrac;
            Bool_t fUseOverlapFrac;

            KTEventWindowFunction* fWindowFunction;
            //std::vector< std::vector< KTPowerSpectrum* >* > fPowerSpectra;


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
            void ProcessEvent(KTEvent* event, const std::string& dataName="");

    };


    inline UInt_t KTSlidingWindowFFTW::GetTimeSize() const
    {
        return fTimeSize;
    }

    inline UInt_t KTSlidingWindowFFTW::GetFrequencySize() const
    {
        return CalculateNFrequencyBins(fTimeSize);
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

    inline KTEventWindowFunction* KTSlidingWindowFFTW::GetWindowFunction() const
    {
        return fWindowFunction;
    }

    inline UInt_t KTSlidingWindowFFTW::GetWindowSize() const
    {
        return (UInt_t)fWindowFunction->GetSize();
    }

    inline void KTSlidingWindowFFTW::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(fftlog_sw, "Invalid tranform flag requested: " << flag << "\n\tNo change was made.");
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

    inline UInt_t KTSlidingWindowFFTW::CalculateNFrequencyBins(UInt_t nTimeBins) const
    {
        // Integer division is rounded down, per FFTW's instructions
        return nTimeBins / 2 + 1;
    }

} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFTW_HH_ */
