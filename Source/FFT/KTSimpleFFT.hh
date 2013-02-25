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
#include "KTProcessor.hh"

#include "KTFrequencySpectrumPolar.hh"
#include "KTLogger.hh"
#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

#include <complex> // add this before including fftw3.h to use std::complex as FFTW's complex type
#include <fftw3.h>

#include <map>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(fftlog_simp, "katydid.fft");

    class KTData;
    class KTEggHeader;
    class KTPStoreNode;
    class KTTimeSeriesReal;
    class KTTimeSeriesData;
    class KTFrequencySpectrumDataPolar;

    /*!
     @class KTSimpleFFT
     @author N. S. Oblath

     @brief A one-dimensional real-to-complex FFT class.

     @details
     KTSimpleFFT performs a real-to-complex FFT on a one-dimensional array of doubles.

     The FFT is implemented using FFTW.

     Available configuration values:
     \li \c "transform_flag": string -- flag that determines how much planning is done prior to any transforms
     \li \c "use-wisdom": bool -- whether or not to use FFTW wisdom to improve FFT performance
     \li \c "wisdom-filename": string -- filename for loading/saving FFTW wisdom

     Transform flags control how FFTW performs the FFT.
     Currently only the following "rigor" flags are available:
     \li \c ESTIMATE -- "A simple heuristic is used to pick a (probably sub-optimal) plan quickly."
     \li \c MEASURE --  "Find[s] an optimized plan by actually computing several FFTs and measuring their execution time. Depending on your machine, this can take some time (often a few seconds)." This is the default option.
     \li \c PATIENT -- "Considers a wider range of algorithms and often produces a “more optimal” plan (especially for large transforms), but at the expense of several times longer planning time (especially for large transforms)."
     \li \c EXHAUSTIVE -- "Considers an even wider range of algorithms, including many that we think are unlikely to be fast, to produce the most optimal plan but with a substantially increased planning time."
     These flag descriptions are quoted from the FFTW3 manual (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags)

     Slots:
     \li \c "header" -- Initialize the FFT from an Egg file header
     \li \c "ts" -- Perform an FFT; Argument is shared_ptr<KTData>; Requires KTTimeSeriesData; Adds KTFrequencySpectrumDataPolar; Emits signal "fft"

     Signals:
     \li \c "fft" -- Emitted upon successful performance of an FFT; Argument is shared_ptr<KTData>; Includes KTFrequencySpectrumDataPolar
    */

    class KTSimpleFFT : public KTFFT, public KTProcessor
    {
        protected:
            typedef std::map< std::string, UInt_t > TransformFlagMap;

        public:
            KTSimpleFFT();
            virtual ~KTSimpleFFT();

            Bool_t Configure(const KTPStoreNode* node);

            void InitializeFFT();
            void InitializeWithHeader(const KTEggHeader* header);

            Bool_t TransformData(KTTimeSeriesData& tsData);

            KTFrequencySpectrumPolar* Transform(const KTTimeSeriesReal* data) const;

            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;
            virtual Double_t GetMinFrequency(Double_t timeBinWidth) const;
            virtual Double_t GetMaxFrequency(Double_t timeBinWidth) const;

            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized to kFALSE.
            void SetTimeSize(UInt_t nBins);

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Bool_t GetUseWisdom() const;
            const std::string& GetWisdomFilename() const;

            /// note: SetTransoformFlag sets fIsInitialized to false.
            void SetTransformFlag(const std::string& flag);
            void SetUseWisdom(Bool_t flag);
            void SetWisdomFilename(const std::string& fname);

        protected:
            UInt_t CalculateNFrequencyBins(UInt_t nTimeBins) const; // do not make this virtual (called from the constructor)
            KTFrequencySpectrumPolar* ExtractTransformResult(Double_t freqMin, Double_t freqMax) const;
            void SetupTransformFlagMap(); // do not make this virtual (called from the constructor)

            fftw_plan fFTPlan;
            UInt_t fTimeSize;
            Double_t* fInputArray;
            fftw_complex* fOutputArray;

            std::string fTransformFlag;
            TransformFlagMap fTransformFlagMap;

            Bool_t fIsInitialized;
            Bool_t fUseWisdom;
            std::string fWisdomFilename;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fFFTSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotOneArg< KTSimpleFFT, const KTEggHeader*, void > fHeaderSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
    };


    inline UInt_t KTSimpleFFT::GetTimeSize() const
    {
        return fTimeSize;
    }

    inline UInt_t KTSimpleFFT::GetFrequencySize() const
    {
        return CalculateNFrequencyBins(fTimeSize);
    }

    inline Double_t KTSimpleFFT::GetMinFrequency(Double_t timeBinWidth) const
    {
        return -0.5 * GetFrequencyBinWidth(timeBinWidth);
    }

    inline Double_t KTSimpleFFT::GetMaxFrequency(Double_t timeBinWidth) const
    {
        return GetFrequencyBinWidth(timeBinWidth) * ((Double_t)GetFrequencySize() - 0.5);
    }

    inline const std::string& KTSimpleFFT::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTSimpleFFT::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline Bool_t KTSimpleFFT::GetUseWisdom() const
    {
        return fUseWisdom;
    }

    inline const std::string& KTSimpleFFT::GetWisdomFilename() const
    {
        return fWisdomFilename;
    }

    inline void KTSimpleFFT::SetUseWisdom(Bool_t flag)
    {
        fUseWisdom = flag;
        return;
    }

    inline void KTSimpleFFT::SetWisdomFilename(const std::string& fname)
    {
        fWisdomFilename = fname;
        return;
    }

    inline UInt_t KTSimpleFFT::CalculateNFrequencyBins(UInt_t nTimeBins) const
    {
        // Integer division is rounded down, per FFTW's instructions
        return nTimeBins / 2 + 1;
    }

} /* namespace Katydid */
#endif /* KTSIMPLEFFT_HH_ */
