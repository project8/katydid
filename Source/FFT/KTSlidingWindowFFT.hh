/**
 @file KTSlidingWindowFFT.hh
 @brief Contains KTSlidingWindowFFT
 @details Creates a 2-D (frequency vs. time) power spectrum from an event
 @author: N. S. Oblath
 @date: Sep 12, 2011
 */

#ifndef KTSLIDINGWINDOWFFT_HH_
#define KTSLIDINGWINDOWFFT_HH_

//#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTMath.hh"
#include "KTLogger.hh"
#include "KTFrequencySpectrum.hh"
#include "KTEventWindowFunction.hh"

#include <complex>
#include <fftw3/fftw3.h>

#include <stdexcept>
#include <string>

class TH2D;

namespace Katydid
{
    KTLOGGER(fftlog_sw, "katydid.fft");

    class KTEggHeader;
    class KTSlidingWindowFSData;
    class KTTimeSeries;
    class KTTimeSeriesData;
    class KTPStoreNode;

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
     \li \c transform_flag -- flag that determines how much planning is done prior to any transforms
     \li \c overlap_time -- sets the overlap in time units
     \li \c overlap_size -- sets the overlap in number of bins
     \li \c overlap_frac -- sets the overlap in fraction of the window length
     \li \c window_function_type -- sets the type of window function to be used
     \li \c window_function -- parent node for the window function configuration
    */

   class KTSlidingWindowFFT : /*public KTFFT,*/ public KTProcessor
    {
        public:
            typedef KTSignal< void (UInt_t, KTFrequencySpectrum*) >::signal SingleFFTSignal;
            typedef KTSignal< void (KTSlidingWindowFSData*) >::signal FullFFTSignal;

        protected:
            typedef std::map< std::string, Int_t > TransformFlagMap;

        public:
            KTSlidingWindowFFT();
            virtual ~KTSlidingWindowFFT();

            Bool_t Configure(const KTPStoreNode* node);

            virtual void InitializeFFT();
            virtual void RecreateFFT();

            virtual KTSlidingWindowFSData* TransformData(const KTTimeSeriesData* tsData);

            KTPhysicalArray< 1, KTFrequencySpectrum* >* Transform(const KTTimeSeries* data) const;

            /// for this FFT, the "TimeSize" is the window size. The "FullTimeSize" is different.
            virtual Int_t GetTimeSize() const;
            virtual Int_t GetFrequencySize() const;

            UInt_t GetWindowSize() const;
            UInt_t GetOverlap() const;
            UInt_t GetEffectiveOverlap() const;
            Double_t GetOverlapFrac() const;
            Bool_t GetUseOverlapFrac() const;
            KTEventWindowFunction* GetWindowFunction() const;

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Double_t GetFreqBinWidth() const;
            Double_t GetFreqMin() const;
            Double_t GetFreqMax() const;

            /// note: SetTransformFlag sets fIsInitialized to kFALSE.
            void SetTransformFlag(const std::string& flag);
            void SetFreqBinWidth(Double_t bw);
            void SetFreqMin(Double_t fm);
            void SetFreqMax(Double_t fm);
            void SetWindowSize(UInt_t nBins);
            void SetWindowLength(Double_t wlTime);
            void SetOverlap(UInt_t nBins);
            void SetOverlap(Double_t overlapTime);
            void SetOverlapFrac(Double_t overlapFrac);
            void SetUseOverlapFrac(Bool_t useOverlapFrac);
            void SetWindowFunction(KTEventWindowFunction* wf);

        protected:
            UInt_t CalculateNFrequencyBins(UInt_t nTimeBins) const; // do not make this virtual (called from the constructor)
            virtual KTFrequencySpectrum* ExtractTransformResult() const;
            void SetupTransformFlagMap(); // do not make this virtual (called from the constructor)

            fftw_plan fFTPlan;
            UInt_t fTimeSize;
            Double_t* fInputArray;
            fftw_complex* fOutputArray;

            std::string fTransformFlag;
            TransformFlagMap fTransformFlagMap;

            Bool_t fIsInitialized;

            Double_t fFreqBinWidth;
            Double_t fFreqMin;
            Double_t fFreqMax;
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
            void ProcessEvent(UInt_t iEvent, const KTTimeSeriesData* tsData);

    };


    inline Int_t KTSlidingWindowFFT::GetTimeSize() const
    {
        return fTimeSize;
    }

    inline Int_t KTSlidingWindowFFT::GetFrequencySize() const
    {
        return CalculateNFrequencyBins(fTimeSize);
    }

    inline const std::string& KTSlidingWindowFFT::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTSlidingWindowFFT::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline Double_t KTSlidingWindowFFT::GetFreqBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline Double_t KTSlidingWindowFFT::GetFreqMin() const
    {
        return fFreqMin;
    }

    inline Double_t KTSlidingWindowFFT::GetFreqMax() const
    {
        return fFreqMax;
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

    inline UInt_t KTSlidingWindowFFT::GetWindowSize() const
    {
        return (UInt_t)fWindowFunction->GetSize();
    }

    inline void KTSlidingWindowFFT::SetTransformFlag(const std::string& flag)
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

    inline void KTSlidingWindowFFT::SetFreqBinWidth(Double_t bw)
    {
        fFreqBinWidth = bw;
        return;
    }

    inline void KTSlidingWindowFFT::SetFreqMin(Double_t fm)
    {
        fFreqMin = fm;
        return;
    }

    inline void KTSlidingWindowFFT::SetFreqMax(Double_t fm)
    {
        fFreqMax = fm;
        return;
    }

    inline UInt_t KTSlidingWindowFFT::CalculateNFrequencyBins(UInt_t nTimeBins) const
    {
        // Integer division is rounded down, per FFTW's instructions
        return nTimeBins / 2 + 1;
    }

} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFT_HH_ */
