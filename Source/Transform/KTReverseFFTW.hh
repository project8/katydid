/**
 @file KTReverseFFTW.hh
 @brief Contains KTReverseFFTW
 @details Calculates a 1-dimensional FFT on a set of real data.
 @author: N. S. Oblath
 @date: Sep 12, 2011
 */

#ifndef KTREVERSEFFTW_HH_
#define KTREVERSEFFTW_HH_

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
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;

    /*!
     @class KTReverseFFTW
     @author N. S. Oblath

     @brief A one-dimensional complex-to-real or complex-to-complex reverse FFT class.

     @details
     KTReverseFFTW performs a complex-to-real or complex-to-complex reverse FFT on a one-dimensional frequency spectrum.

     The FFT is implemented using FFTW.

     Configuration name: "reverse-fftw"

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

     FFTW_PRESERVE_INPUT is automatically added to the transform flag when necessary so that the input data is not destroyed.

     Slots:
     - "header": void (Nymph::KTDataPtr) -- Initialize the FFT from an Egg header; Requires KTEggHeader
     - "fs-fftw-to-real": void (Nymph::KTDataPtr) -- Perform a reverse FFT on the frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTTimeSeriesData; Emits signal "fft"
     - "fs-fftw-to-complex": void (Nymph::KTDataPtr) -- Perform a reverse FFT on the frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTTimeSeriesData; Emits signal "fft"

     Signals:
     - "fft": void (Nymph::KTDataPtr) -- Emitted upon performance of a reverse transform; Guarantees KTTimeSeriesData.
    */

    class KTReverseFFTW : public KTFFTW, public Nymph::KTProcessor
    {
        private:
            typedef std::map< std::string, unsigned > TransformFlagMap;

        public:
            enum State
            {
                kNone,
                kC2R,
                kC2C
            };

        public:
            KTReverseFFTW(const std::string& name = "reverse-fftw");
            virtual ~KTReverseFFTW();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(bool, UseWisdom);
            MEMBERVARIABLE_REF(std::string, WisdomFilename);

            MEMBERVARIABLE(KTReverseFFTW::State, RequestedState);

            MEMBERVARIABLE_NOSET(unsigned, TimeSize);
            MEMBERVARIABLE_NOSET(unsigned, FrequencySize);

            MEMBERVARIABLE_REF_CONST(std::string, TransformFlag);

        public:
            /// Set the number of time bins (also sets the number of frequency bins).
            /// In r-to-c mode, number of frequency bins is N/2 + 1.
            /// In c-to-c or r-as-c-to-c mode, number of frequency bins is N.
            /// FFT must be initialized after calling this.
            void SetTimeSize(unsigned nBins);
            /// Set the number of frequency bins (also sets the number of time bins).
            /// In c-to-r mode, number of time bins is (N-1) * 2  (NOTE: number of time bins is always even!)
            /// In c-to-c mode, number of time bins is N.
            /// FFT must be initialized after calling this.
            void SetFrequencySize(unsigned nBins);
            /// Change the transform flag; FFT must be initialized after calling this.
            void SetTransformFlag(const std::string& flag);

        private:
            /// note: does not change the state
            void SetTimeSizeForState(unsigned nBins, KTReverseFFTW::State intendedState);
            /// note: does not change the state
            void SetFrequencySizeForState(unsigned nBins, KTReverseFFTW::State intendedState);

            TransformFlagMap fTransformFlagMap;

        public:
            /// Initialize the FFT for real time domain data; optionally specify a new time size (the default value of 0 will leave it unchanged)
            bool InitializeForRealTDD(unsigned timeSize = 0);
            /// Initialize the FFT for complex time domain data; optionally specify a new time size (the default value of 0 will leave it unchanged)
            bool InitializeForComplexTDD(unsigned timeSize = 0);
            /// Initialize the FFT based on the requested state; optionally specify a new time size (the default value of 0 will leave it unchanged)
            bool InitializeFromRequestedState(unsigned timeSize = 0);
            /// Initialize the FFT using a KTEggHeader object.
            bool InitializeWithHeader(KTEggHeader& header);

            virtual double GetMinFrequency(double timeBinWidth) const;
            virtual double GetMaxFrequency(double timeBinWidth) const;

            MEMBERVARIABLE_NOSET(KTReverseFFTW::State, State);
            MEMBERVARIABLE_NOSET(bool, IsInitialized);

        private:
            bool InitializeFFT(KTReverseFFTW::State intendedState, unsigned timeSize = 0);

        public:
            /// Reverse FFT - To Real Time Data
            bool TransformDataToReal(KTFrequencySpectrumDataFFTW& fsData);
            /// Reverse FFT - To Real Time Series
            KTTimeSeriesReal* TransformToReal(const KTFrequencySpectrumFFTW* fs) const;
            /// Reverse FFT - To Real Time Series - No size or bin width checks
            KTTimeSeriesReal* FastTransformToReal(const KTFrequencySpectrumFFTW* fs) const;
            /// Reverse FFT - To Real Time Series - Output must exist - No size or bin width checks
            void DoTransform(const KTFrequencySpectrumFFTW* fsIn, KTTimeSeriesReal* tsOut) const;

            /// Reverse FFT - To Complex Time Data
            bool TransformDataToComplex(KTFrequencySpectrumDataFFTW& fsData);
            /// Reverse FFT - To Complex Time Series
            KTTimeSeriesFFTW* TransformToComplex(const KTFrequencySpectrumFFTW* fs) const;
            /// Reverse FFT - To Complex Time Series - No size or bin width checks
            KTTimeSeriesFFTW* FastTransformToComplex(const KTFrequencySpectrumFFTW* fs) const;
            /// Reverse FFT - To Complex Time Series - Output must exist - No size or bin width checks
            void DoTransform(const KTFrequencySpectrumFFTW* fsIn, KTTimeSeriesFFTW* tsOut) const;

        private:
            // binning cache
            void UpdateBinningCache(double freqBinWidth) const;
            mutable double fFreqBinWidthCache;
            mutable double fTimeMinCache;
            mutable double fTimeMaxCache;

            /// Allocate memory in the i/o arrays for an intended state if provided, or (default) the current state
            /// note: does not change the state
            bool AllocateArrays(State intendedState = kNone);
            void FreeArrays();
            void SetupInternalMaps(); // do not make this virtual (called from the constructor)

            fftw_plan fReversePlan;

            fftw_complex* fInputArray;
            double*       fROutputArray;
            fftw_complex* fCOutputArray;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fFFTSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTEggHeader > fHeaderSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWToRealSlot;
            Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWToComplexSlot;

    };


    inline double KTReverseFFTW::GetMinFrequency(double timeBinWidth) const
    {
        // DC bin is centered at 0, with half a bin width on either side
        return -0.5 * GetFrequencyBinWidth(timeBinWidth);
    }

    inline double KTReverseFFTW::GetMaxFrequency(double timeBinWidth) const
    {
        return GetFrequencyBinWidth(timeBinWidth) * ((double)fFrequencySize - 0.5);
    }

    inline void KTReverseFFTW::UpdateBinningCache(double freqBinWidth) const
    {
        if (freqBinWidth == fFreqBinWidthCache) return;
        fFreqBinWidthCache = freqBinWidth;
        fTimeMinCache = GetMinTime();
        fTimeMaxCache = GetMaxTime(freqBinWidth);
        return;
    }

} /* namespace Katydid */

#endif /* KTREVERSEFFTW_HH_ */
