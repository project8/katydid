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
                kR2C,
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
            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized to false.
            void SetTimeSize(unsigned nBins);
            void SetFrequencySize(unsigned nBins);
            /// note: SetTransoformFlag sets fIsInitialized to false.
            void SetTransformFlag(const std::string& flag);

        private:
            TransformFlagMap fTransformFlagMap;

        public:
            bool InitializeForRealTDD();
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
            /// Forward FFT - Complex Time Data
            bool TransformComplexData(KTTimeSeriesData& tsData);
            /// Forward FFT - Complex Analytic Associate Data
            bool TransformComplexData(KTAnalyticAssociateData& aaData);

            /// Forward FFT - Real Data
            KTFrequencySpectrumFFTW* Transform(const KTTimeSeriesReal* ts) const;
            void DoTransform(const KTTimeSeriesReal* tsIn, KTFrequencySpectrumFFTW* fsOut) const;
            /// Forward FFT - Complex Data
            KTFrequencySpectrumFFTW* Transform(const KTTimeSeriesFFTW* ts) const;
            void DoTransform(const KTTimeSeriesFFTW* tsIn, KTFrequencySpectrumFFTW* fsOut) const;

        private:
            void AllocateArrays();
            void FreeArrays();
            void SetupInternalMaps(); // do not make this virtual (called from the constructor)

            fftw_plan fForwardPlan;

            double* fTSArray;
            fftw_complex* fFSArray;

            fftw_complex* fInputArray;
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
        // There's one bin at the center, always: the DC bin.
        // # of bins on the negative side is nFreqBins/2 (rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the negative frequency side.
        return -GetFrequencyBinWidth(timeBinWidth) * (double(fTimeSize/2) + 0.5);
    }

    inline double KTForwardFFTW::GetMaxFrequency(double timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the positive side is nFreqBins/2 if the number of bins is odd, and nFreqBins/2-1 if the number of bins is even (division rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the positive frequency side.
        unsigned nBinsToSide = fTimeSize / 2;
        return GetFrequencyBinWidth(timeBinWidth) * (double(nBinsToSide*2 == fTimeSize ? nBinsToSide - 1 : nBinsToSide) + 0.5);
    }

} /* namespace Katydid */

#endif /* KTFORWARDFFTW_HH_ */
