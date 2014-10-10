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

#include <complex> // add this before including fftw3.h to use std::complex as FFTW's complex type
#include <fftw3.h>

#include <map>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(fftlog_simp, "KTSimpleFFT");

    class KTCorrelationData;
    class KTCorrelationTSData;
    class KTEggHeader;
    class KTFrequencySpectrumDataPolar;
    class KTParamNode;
    class KTTimeSeriesReal;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTFrequencySpectrumDataPolar;

    /*!
     @class KTSimpleFFT
     @author N. S. Oblath

     @brief A one-dimensional real-to-complex FFT class.

     @details
     KTSimpleFFT performs a real-to-complex forward FFT on a one-dimensional array of doubles,
     or a complex-to-real reverse FFT on a one-dimensional array of complexpolar pairs.

     The FFT is implemented using FFTW.

     Configuration name: "simple-fft"

     Available configuration values:
     - "transform_flag": string -- flag that determines how much planning is done prior to any transforms
     - "use-wisdom": bool -- whether or not to use FFTW wisdom to improve FFT performance
     - "wisdom-filename": string -- filename for loading/saving FFTW wisdom

     Transform flags control how FFTW performs the FFT.
     Currently only the following "rigor" flags are available:
     - ESTIMATE -- "A simple heuristic is used to pick a (probably sub-optimal) plan quickly."
     - MEASURE --  "Find[s] an optimized plan by actually computing several FFTs and measuring their execution time. Depending on your machine, this can take some time (often a few seconds)." This is the default option.
     - PATIENT -- "Considers a wider range of algorithms and often produces a more optimal plan (especially for large transforms), but at the expense of several times longer planning time (especially for large transforms)."
     - EXHAUSTIVE -- "Considers an even wider range of algorithms, including many that we think are unlikely to be fast, to produce the most optimal plan but with a substantially increased planning time."
     These flag descriptions are quoted from the FFTW3 manual (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags)

     Slots:
     - "header": void (KTDataPtr) -- Initialize the FFT from an Egg file header; Requires KTEggHeader
     - "ts": void (KTDataPtr) -- Perform an FFT; Requires KTTimeSeriesData; Adds KTFrequencySpectrumDataPolar; Emits signal "fft-forward"
     - "fs-polar": void (KTDataPtr) -- Perform a reverse FFT; Requires KTFrequencySpectrumDataPolar; Emits signal signal "fft-reverse"

     Signals:
     - "fft-forward": void (KTDataPtr) -- Emitted upon successful performance of a forward FFT; Guarantees KTFrequencySpectrumDataPolar
     - "fft-reverse": void (KTDataPtr) -- Emitted upon successful performance of a reverse FFT; Guarantees KTFrequencySpectrumDataPolar
    */

    class KTSimpleFFT : public KTFFT, public KTProcessor
    {
        protected:
            typedef std::map< std::string, unsigned > TransformFlagMap;

        public:
            KTSimpleFFT(const std::string& name = "simple-fft");
            virtual ~KTSimpleFFT();

            bool Configure(const KTParamNode* node);

            void InitializeFFT();
            bool InitializeWithHeader(KTEggHeader& header);

            /// Forward FFT
            bool TransformData(KTTimeSeriesData& tsData);

            /// Reverse FFT
            bool TransformData(KTFrequencySpectrumDataPolar& fsData);
            bool TransformData(KTCorrelationData& corrData);

            /// Forward FFT on a frequency spectrum
            /// NOTE: will REPLACE existing time series in the data object with KTTimeSeriesFFTW
            bool TransformDataAgain(KTFrequencySpectrumDataPolar& fsData, KTTimeSeriesData& tsData);

            /// Forward FFT
            KTFrequencySpectrumPolar* Transform(const KTTimeSeriesReal* data) const;

            /// Reverse FFT
            KTTimeSeriesReal* Transform(const KTFrequencySpectrumPolar* data) const;

            /// Forward FFT on a frequency spectrum
            KTTimeSeriesFFTW* TransformAgain(const KTFrequencySpectrumPolar* data) const;

            virtual unsigned GetTimeSize() const;
            virtual unsigned GetFrequencySize() const;
            virtual double GetMinFrequency(double timeBinWidth) const;
            virtual double GetMaxFrequency(double timeBinWidth) const;

            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized to false.
            void SetTimeSize(unsigned nBins);

            const std::string& GetTransformFlag() const;
            bool GetIsInitialized() const;
            bool GetUseWisdom() const;
            const std::string& GetWisdomFilename() const;

            /// note: SetTransoformFlag sets fIsInitialized to false.
            void SetTransformFlag(const std::string& flag);
            void SetUseWisdom(bool flag);
            void SetWisdomFilename(const std::string& fname);

        protected:
            unsigned CalculateNFrequencyBins(unsigned nTimeBins) const; // do not make this virtual (called from the constructor)
            KTFrequencySpectrumPolar* ExtractForwardTransformResult(double freqMin, double freqMax) const;
            void SetupTransformFlagMap(); // do not make this virtual (called from the constructor)

            fftw_plan fForwardPlan;
            fftw_plan fReversePlan;
            unsigned fTimeSize;
            double* fTSArray;
            fftw_complex* fFSArray;

            std::string fTransformFlag;
            TransformFlagMap fTransformFlagMap;

            bool fIsInitialized;
            bool fUseWisdom;
            std::string fWisdomFilename;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fFFTForwardSignal;
            KTSignalData fFFTReverseSignal;
            KTSignalData fFFTReverseCorrSignal;
            KTSignalData fFFTForwardAgainSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTEggHeader > fHeaderSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;
            KTSlotDataOneType< KTCorrelationData > fCorrSlot;
            KTSlotDataTwoTypes< KTFrequencySpectrumDataPolar, KTTimeSeriesData > fFSPolarForwardSlot;
    };


    inline unsigned KTSimpleFFT::GetTimeSize() const
    {
        return fTimeSize;
    }

    inline unsigned KTSimpleFFT::GetFrequencySize() const
    {
        return CalculateNFrequencyBins(fTimeSize);
    }

    inline double KTSimpleFFT::GetMinFrequency(double timeBinWidth) const
    {
        return -0.5 * GetFrequencyBinWidth(timeBinWidth);
    }

    inline double KTSimpleFFT::GetMaxFrequency(double timeBinWidth) const
    {
        return GetFrequencyBinWidth(timeBinWidth) * ((double)GetFrequencySize() - 0.5);
    }

    inline const std::string& KTSimpleFFT::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline bool KTSimpleFFT::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline bool KTSimpleFFT::GetUseWisdom() const
    {
        return fUseWisdom;
    }

    inline const std::string& KTSimpleFFT::GetWisdomFilename() const
    {
        return fWisdomFilename;
    }

    inline void KTSimpleFFT::SetUseWisdom(bool flag)
    {
        fUseWisdom = flag;
        return;
    }

    inline void KTSimpleFFT::SetWisdomFilename(const std::string& fname)
    {
        fWisdomFilename = fname;
        return;
    }

    inline unsigned KTSimpleFFT::CalculateNFrequencyBins(unsigned nTimeBins) const
    {
        // Integer division is rounded down, per FFTW's instructions
        return nTimeBins / 2 + 1;
    }

} /* namespace Katydid */
#endif /* KTSIMPLEFFT_HH_ */
