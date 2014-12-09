/**
 @file KTForwardFFTW.hh
 @brief Contains KTForwardFFTW
 @details Calculates a 1-dimensional FFT on a set of real or complex data.
 @author: N. S. Oblath
 @date: Sep 12, 2011 (original); Dec 5, 2014 (KTForwardFFTW)
 */

#ifndef KTFORWARDFFTW_HH_
#define KTFORWARDFFTW_HH_

#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include <fftw3.h>

#include <map>
#include <string>
#include <vector>

namespace Katydid
{
    class KTAnalyticAssociateData;
    class KTEggHeader;
    class KTFrequencySpectrumFFTW;
    class KTParamNode;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;

    /*!
     @class KTForwardFFTW
     @author N. S. Oblath

     @brief A forward FFT class.

     @details
     KTForwardFFTW performs a real-to-complex and complex-to-complex FFTs on a one-dimensional time series.

     The FFT is implemented using FFTW.

     Configuration name: "forward-fftw"

     Available configuration values:
     - "transform_flag": string -- flag that determines how much planning is done prior to any transforms (see below)
     - "use-wisdom": bool -- whether or not to use FFTW wisdom to improve FFT performance
     - "wisdom-filename": string -- filename for loading/saving FFTW wisdom

     Transform flags control how FFTW performs the FFT.
     Currently only the following "rigor" flags are available:
     - ESTIMATE -- "A simple heuristic is used to pick a (probably sub-optimal) plan quickly."
     - MEASURE --  "Find[s] an optimized plan by actually computing several FFTs and measuring their execution time. Depending on your machine, this can take some time (often a few seconds)." This is the default option.
     - PATIENT -- "Considers a wider range of algorithms and often produces a more optimal plan (especially for large transforms), but at the expense of several times longer planning time (especially for large transforms)."
     - EXHAUSTIVE -- "Considers an even wider range of algorithms, including many that we think are unlikely to be fast, to produce the most optimal plan but with a substantially increased planning time."
     These flag descriptions are quoted from the FFTW3 manual (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags)

     FFTW_PRESERVE_INPUT is automatically added to the transform flag so that, particularly for the reverse transform, the input data is not destroyed.

     Slots:
     - "header": void (KTDataPtr) -- Initialize the FFT from an Egg header; Requires KTEggHeader
     - "ts": void (KTDataPtr) -- Perform a forward FFT on the time series; Requires KTTimeSeriesData; Adds KTFrequencySpectrumPolar; Emits signal "fft-forward"
     - "aa": void (KTDataPtr) -- Perform a forward FFT on an analytic associate data; Requires KTAnalyticAssociateData; Adds KTFrequencySpectrumPolar; Emits signal "fft-forward"

     Signals:
     - "fft-forward": void (KTDataPtr) -- Emitted upon performance of a forward transform; Guarantees KTFrequencySpectrumDataFFTW.
    */

    class KTForwardFFTW : public KTFFTW, public KTProcessor
    {
        private:
            typedef std::map< std::string, unsigned > TransformFlagMap;

            enum State
            {
                kNone,
                kR2C,
                kRasC2C,
                kC2C
            };

        public:
            KTForwardFFTW(const std::string& name = "forward-fftw");
            virtual ~KTForwardFFTW();

            bool Configure(const KTParamNode* node);

            MEMBERVARIABLE(bool, UseWisdom);
            MEMBERVARIABLEREF(std::string, WisdomFilename);

            MEMBERVARIABLE_NOSET(unsigned, TimeSize);
            MEMBERVARIABLE_NOSET(unsigned, FrequencySize);

            MEMBERVARIABLEREF_NOSET(std::string, TransformFlag);

        public:
            /// Set the number of time bins; FFT must be initialized after calling this.
            void SetTimeSize(unsigned nBins);
            /// Change the transform flag; FFT must be initialized after calling this.
            void SetTransformFlag(const std::string& flag);

        private:
            TransformFlagMap fTransformFlagMap;

        public:
            bool InitializeForRealTDD();
            bool InitializeForRealAsComplexTDD();
            bool InitializeForComplexTDD();
            bool InitializeWithHeader(KTEggHeader& header);

            virtual double GetMinFrequency(double timeBinWidth) const;
            virtual double GetMaxFrequency(double timeBinWidth) const;

            MEMBERVARIABLE_NOSET(KTForwardFFTW::State, State);
            MEMBERVARIABLE_NOSET(bool, IsInitialized);

        private:
            bool InitializeFFT(KTForwardFFTW::State intendedState);

        public:
            /// Forward FFT - Real Time Data
            bool TransformRealData(KTTimeSeriesData& tsData);
            /// Forward FFT - Real Data
            KTFrequencySpectrumFFTW* Transform(const KTTimeSeriesReal* ts) const;
            /// Forward FFT - Real Data
            KTFrequencySpectrumFFTW*  FastTransform(const KTTimeSeriesReal* ts) const;
            void DoTransform(const KTTimeSeriesReal* tsIn, KTFrequencySpectrumFFTW* fsOut) const;

            /// Forward FFT - Real Time Data as Complex
            bool TransformRealDataAsComplex(KTTimeSeriesData& tsData);
            /// Forward FFT - Real-as-Complex Data
            KTFrequencySpectrumFFTW* TransformAsComplex(const KTTimeSeriesReal* ts) const;
            /// Forward FFT - Real-as-Complex Data
            KTFrequencySpectrumFFTW*  FastTransformAsComplex(const KTTimeSeriesReal* ts) const;
            void DoTransformAsComplex(const KTTimeSeriesReal* tsIn, KTFrequencySpectrumFFTW* fsOut) const;

            /// Forward FFT - Complex Time Data
            bool TransformComplexData(KTTimeSeriesData& tsData);
            /// Forward FFT - Complex Analytic Associate Data
            bool TransformComplexData(KTAnalyticAssociateData& aaData);
            /// Forward FFT - Complex Data
            KTFrequencySpectrumFFTW* Transform(const KTTimeSeriesFFTW* ts) const;
            /// Forward FFT - Complex Data
            KTFrequencySpectrumFFTW*  FastTransform(const KTTimeSeriesFFTW* ts) const;
            void DoTransform(const KTTimeSeriesFFTW* tsIn, KTFrequencySpectrumFFTW* fsOut) const;

        private:
            // binning cache
            void UpdateBinningCache(double timeBinWidth) const;
            mutable double fTimeBinWidthCache;
            mutable double fFreqMinCache;
            mutable double fFreqMaxCache;

            /// Allocate memory in the i/o arrays for an intended state if provided, or (default) the current state
            bool AllocateArrays(State intendedState = kNone);
            void FreeArrays();
            void SetupInternalMaps(); // do not make this virtual (called from the constructor)

            fftw_plan fForwardPlan;

            double*       fRInputArray;
            fftw_complex* fCInputArray;
            fftw_complex* fOutputArray;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fFFTSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTEggHeader > fHeaderSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTSRealSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTSComplexSlot;
            KTSlotDataOneType< KTAnalyticAssociateData > fAASlot;

    };


    inline double KTForwardFFTW::GetMinFrequency(double timeBinWidth) const
    {
        // DC bin is centered at 0
        return -0.5 * GetFrequencyBinWidth(timeBinWidth);
    }

    inline double KTForwardFFTW::GetMaxFrequency(double timeBinWidth) const
    {
        return GetFrequencyBinWidth(timeBinWidth) * ((double)fFrequencySize - 0.5);
    }

    inline void KTForwardFFTW::UpdateBinningCache(double timeBinWidth) const
    {
        if (timeBinWidth == fTimeBinWidthCache) return;
        fTimeBinWidthCache = timeBinWidth;
        fFreqMinCache = GetMinFrequency(timeBinWidth);
        fFreqMaxCache = GetMaxFrequency(timeBinWidth);
        return;
    }

} /* namespace Katydid */

#endif /* KTFORWARDFFTW_HH_ */
