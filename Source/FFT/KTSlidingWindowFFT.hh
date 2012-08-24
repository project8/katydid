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
#include "KTConfigurable.hh"

#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"
#include "KTEventWindowFunction.hh"

#include "TFFTRealComplex.h"
#include "TMath.h"

#include <stdexcept>
#include <string>
#include <vector>

class TH2D;

namespace Katydid
{
    KTLOGGER(fftlog_sw, "katydid.fft");

    class KTEggHeader;
    class KTTimeSeriesData;
    class KTPStoreNode;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

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

   class KTSlidingWindowFFT : public KTFFT, public KTProcessor, public KTConfigurable
    {
        public:
            typedef KTSignal< void (UInt_t, KTPowerSpectrum*) >::signal SingleFFTSignal;
            typedef KTSignal< void (UInt_t, const KTSlidingWindowFFT*) >::signal FullFFTSignal;

        public:
            KTSlidingWindowFFT();
            virtual ~KTSlidingWindowFFT();

            Bool_t Configure(const KTPStoreNode* node);

            virtual void InitializeFFT();
            virtual void RecreateFFT();

            virtual Bool_t TransformData(const KTTimeSeriesData* tsData);

            template< typename XDataType >
            void Transform(const std::vector< XDataType >& data, vector< KTPowerSpectrum* >* powerSpectra);

            void AddTransformResult(std::vector< KTPowerSpectrum* >* newResults);

            virtual TH2D* CreatePowerSpectrumHistogram(const std::string& name, UInt_t channelNum = 0) const;
            virtual TH2D* CreatePowerSpectrumHistogram(UInt_t channelNum = 0) const;

            virtual KTPhysicalArray< 2, Double_t >* CreatePowerSpectrumPhysArr(UInt_t channelNum = 0) const;

            /// for this FFT, the "TimeSize" is the window size. The "FullTimeSize" is different.
            virtual Int_t GetTimeSize() const;
            virtual Int_t GetFrequencySize() const;

            UInt_t GetWindowSize() const;
            UInt_t GetOverlap() const;
            UInt_t GetEffectiveOverlap() const;
            Double_t GetOverlapFrac() const;
            Bool_t GetUseOverlapFrac() const;
            KTEventWindowFunction* GetWindowFunction() const;
            KTPowerSpectrum* GetPowerSpectrum(Int_t spect, UInt_t channelNum = 0) const;
            UInt_t GetNPowerSpectra(UInt_t channelNum = 0) const;

            const TFFTRealComplex* GetFFT() const;
            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Double_t GetFreqBinWidth() const;

            /// note: SetTransformFlag sets fIsInitialized to kFALSE.
            void SetTransformFlag(const string& flag);
            void SetFreqBinWidth(Double_t bw);
            void SetWindowSize(UInt_t nBins);
            void SetWindowLength(Double_t wlTime);
            void SetOverlap(UInt_t nBins);
            void SetOverlap(Double_t overlapTime);
            void SetOverlapFrac(Double_t overlapFrac);
            void SetUseOverlapFrac(Bool_t useOverlapFrac);
            void SetWindowFunction(KTEventWindowFunction* wf);

        protected:
            virtual KTPowerSpectrum* ExtractPowerSpectrum() const;
            void ClearPowerSpectra();

            TFFTRealComplex* fTransform;

            std::string fTransformFlag;

            Bool_t fIsInitialized;

            Double_t fFreqBinWidth;
            UInt_t fOverlap;
            Double_t fOverlapFrac;
            Bool_t fUseOverlapFrac;

            KTEventWindowFunction* fWindowFunction;
            std::vector< std::vector< KTPowerSpectrum* >* > fPowerSpectra;


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


    template< typename XDataType >
    void KTSlidingWindowFFT::Transform(const std::vector< XDataType >& data, vector< KTPowerSpectrum* >* powerSpectra)
    {
       if (fWindowFunction->GetSize() < data.size())
       {
           Int_t windowShift = fWindowFunction->GetSize() - GetEffectiveOverlap();
           UInt_t iWindow = 0;
           for (unsigned int windowStart=0; windowStart + fWindowFunction->GetSize() <= data.size(); windowStart += windowShift)
           {
               for (unsigned int iPoint=windowStart; iPoint<windowStart+fWindowFunction->GetSize(); iPoint++)
               {
                   fTransform->SetPoint(iPoint-windowStart, Double_t(data[iPoint]) * fWindowFunction->GetWeight(iPoint-windowStart));
               }
               fTransform->Transform();
               powerSpectra->push_back(ExtractPowerSpectrum());
               // emit a signal that the FFT was performed, for any connected slots
               fSingleFFTSignal(iWindow, powerSpectra->back());
               iWindow++;
           }
           KTINFO(fftlog_sw, "FFTs complete; windows used: " << iWindow);
           return;
       }

       KTERROR(fftlog_sw, "Window size is larger than time data: " << fWindowFunction->GetSize() << " > " << data.size() << "\n" <<
              "No transform was performed!");
       throw(std::length_error("Window size is larger than time data"));
       return;
    }



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

    inline Double_t KTSlidingWindowFFT::GetFreqBinWidth() const
    {
        return fFreqBinWidth;
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

    inline KTPowerSpectrum* KTSlidingWindowFFT::GetPowerSpectrum(Int_t spec, UInt_t channelNum) const
    {
        if (spec >= 0 && spec < (Int_t)(*fPowerSpectra[channelNum]).size()) return (*fPowerSpectra[channelNum])[spec];
        return NULL;
    }

    inline UInt_t KTSlidingWindowFFT::GetNPowerSpectra(UInt_t channelNum) const
    {
        return (UInt_t)(*fPowerSpectra[channelNum]).size();
    }

    inline KTEventWindowFunction* KTSlidingWindowFFT::GetWindowFunction() const
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

} /* namespace Katydid */
#endif /* KTSLIDINGWINDOWFFT_HH_ */
