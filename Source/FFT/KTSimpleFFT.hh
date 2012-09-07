/**
 @file KTSimpleFFT.hh
 @brief Contains KTSimpleFFT
 @details Calculates a 1-dimensional FFT on a set of real data.
 @author: N. S. Oblath
 @date: Sep 12, 2011
 */

#ifndef KTSIMPLEFFT_HH_
#define KTSIMPLEFFT_HH_

#include "KTProcessor.hh"
#include "KTConfigurable.hh"

#include "KTLogger.hh"
#include "KTFrequencySpectrum.hh"

#include <complex> // add this before including fftw3.h to use std::complex as FFTW's complex type
#include <fftw3/fftw3.h>

#include <map>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(fftlog_simp, "katydid.fft");

    class KTEggHeader;
    class KTEvent;
    class KTPStoreNode;
    class KTTimeSeries;
    class KTTimeSeriesData;
    class KTFrequencySpectrumData;
    class KTWriteableData;

    /*!
     @class KTSimpleFFT
     @author N. S. Oblath

     @brief A one-dimensional real-to-complex FFT class.

     @details
     KTSimpleFFT performs a real-to-complex FFT on a one-dimensional array of doubles.

     The FFT is implemented using FFTW.

     Available configuration values:
     \li \c transform_flag -- flag that determines how much planning is done prior to any transforms

     Transform flags control how FFTW performs the FFT.
     Currently only the following "rigor" flags are available:
     \li \c ESTIMATE -- "A simple heuristic is used to pick a (probably sub-optimal) plan quickly."
     \li \c MEASURE --  "Find[s] an optimized plan by actually computing several FFTs and measuring their execution time. Depending on your machine, this can take some time (often a few seconds)." This is the default option.
     \li \c PATIENT -- "Considers a wider range of algorithms and often produces a “more optimal” plan (especially for large transforms), but at the expense of several times longer planning time (especially for large transforms)."
     \li \c EXHAUSTIVE -- "Considers an even wider range of algorithms, including many that we think are unlikely to be fast, to produce the most optimal plan but with a substantially increased planning time."
     These flag descriptions are quoted from the FFTW3 manual (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags)

     Slots:
     \li \c void ProcessHeader(const KTEggHeader* header)
     \li \c void ProcessEvent(UInt_t iEvent, const KTEvent* event)

     Signals:
     \li \c void (UInt_t, const KTSimpleFFT*) emitted upon performance of a transform.
    */

    class KTSimpleFFT : /*public KTFFT,*/ public KTProcessor, public KTConfigurable
    {
        public:
            typedef KTSignal< void (const KTWriteableData*) >::signal FFTSignal;

        protected:
            typedef std::map< std::string, Int_t > TransformFlagMap;

        public:
            KTSimpleFFT();
            KTSimpleFFT(UInt_t timeSize);
            virtual ~KTSimpleFFT();

            Bool_t Configure(const KTPStoreNode* node);

            virtual void InitializeFFT();

            virtual KTFrequencySpectrumData* TransformData(const KTTimeSeriesData* tsData);

            KTFrequencySpectrum* Transform(const KTTimeSeries* data) const;

            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;

            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized to kFALSE.
            void SetTimeSize(UInt_t nBins);

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Double_t GetFreqBinWidth() const;
            Double_t GetFreqMin() const;
            Double_t GetFreqMax() const;

            /// note: SetTransoformFlag sets fIsInitialized and fIsDataReady to kFALSE.
            void SetTransformFlag(const std::string& flag);
            void SetFreqBinWidth(Double_t bw);
            void SetFreqMin(Double_t fm);
            void SetFreqMax(Double_t fm);

        protected:
            UInt_t CalculateNFrequencyBins(UInt_t nTimeBins) const; // do not make this virtual (called from the constructor)
            KTFrequencySpectrum* ExtractTransformResult() const;
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

            //***************
            // Signals
            //***************

        private:
            FFTSignal fFFTSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessHeader(const KTEggHeader* header);
            void ProcessEvent(KTEvent* event);
            void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);

    };


    inline UInt_t KTSimpleFFT::GetTimeSize() const
    {
        return fTimeSize;
    }

    inline UInt_t KTSimpleFFT::GetFrequencySize() const
    {
        return CalculateNFrequencyBins(fTimeSize);
    }

    inline void KTSimpleFFT::SetTimeSize(UInt_t nBins)
    {
        fTimeSize = nBins;
        fftw_free(fInputArray);
        fftw_free(fOutputArray);
        fInputArray = (double*) fftw_malloc(sizeof(double) * fTimeSize);
        fOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * CalculateNFrequencyBins(fTimeSize));
        fIsInitialized = false;
        return;
    }

    inline const std::string& KTSimpleFFT::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTSimpleFFT::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline Double_t KTSimpleFFT::GetFreqBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline Double_t KTSimpleFFT::GetFreqMin() const
    {
        return fFreqMin;
    }

    inline Double_t KTSimpleFFT::GetFreqMax() const
    {
        return fFreqMax;
    }

    inline void KTSimpleFFT::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(fftlog_simp, "Invalid tranform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }
        fTransformFlag = flag;
        fIsInitialized = false;
        return;
    }

    inline void KTSimpleFFT::SetFreqBinWidth(Double_t bw)
    {
        fFreqBinWidth = bw;
        return;
    }

    inline void KTSimpleFFT::SetFreqMin(Double_t fm)
    {
        fFreqMin = fm;
        return;
    }

    inline void KTSimpleFFT::SetFreqMax(Double_t fm)
    {
        fFreqMax = fm;
        return;
    }

    inline UInt_t KTSimpleFFT::CalculateNFrequencyBins(UInt_t nTimeBins) const
    {
        // Integer division is rounded down, per FFTW's instructions
        return nTimeBins / 2 + 1;
    }

} /* namespace Katydid */
#endif /* KTSIMPLEFFT_HH_ */
