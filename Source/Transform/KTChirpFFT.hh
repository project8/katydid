/**
 @file KTChirpFFT.hh
 @brief Contains KTChirpFFT
 @details Calculates a 1 to 2-dimensional FFT on a set of real or complex data.
 @author: J. K. Gaison
 @date: Apr 5, 2022 
 */

#ifndef KTCHIRPFFT_HH_
#define KTCHIRPFFT_HH_

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
    class KTChirpSpaceFFT;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;

    /*!
     @class KTChirpFFT
     @author J. K. Gaison

     @brief A chirplet-based FFT class.

     @details
     KTChirpFFT performs a 1D-to-2D chirplet-based FFT on a one-dimensional time series.

     The basic options for use are:
     - If you have real data, you'll do a real-to-complex transform.  Initialize with InitializeForRealTDD(), and
       transform with TransformRealData() and related functions.
       The output will be a KTChirpSpaceFFT with size N/2 + 1, where N is the size of the time series.
     - If you have complex data, you'll do a complex-to-complex transform.  Initialize with InitializeForComplexTDD(), and
       transform with TransformComplexData() and related functions.
       The output will be a KTChirpSpaceFFT with size N.

     The above are the two highly highly recommended modes of operation.  However, there's one more option:
     if you have real data and want to pretend it's complex, you can do a real-as-complex-to-complex transform.
     Initialize with InitializeForRealAsComplexTDD(), and transform with TransformRealAsComplexData() and related functions.
     The output will be a KTChirpSpaceFFT with size N.

     If you're using the signal/slot interface, and initializing the FFT with the Egg header (slot "header"), you have two options for specifying the type of transform:
     - Use the type recorded in the Egg header.  The choice will be based on whether the time-domain-data is real, complex (not IQ), or IQ.
     - Specify the "transform-state" in the processor configuration.  You can then also specify whether to "transform-complex-as-iq".

     The FFT is implemented based on "The Chirplet Transform: A Generalization of Gabor's Logon Transform" by Mann and Haykin.

     Configuration name: "chirp-fft"

     Available configuration values:
     - "transform_flag": string -- flag that determines how much planning is done prior to any transforms (see below)
     - "use-wisdom": bool -- whether or not to use FFTW wisdom to improve FFT performance
     - "wisdom-filename": string -- filename for loading/saving FFTW wisdom
     - "transform-state": string -- "r2c", "c2c", or "rasc2c"; specify the transform state, regardless of the time domain type listed in the egg header; this is useful when a new time domain data type (e.g. aa) has been added to the data object and is being transformed.
     - "transform-complex-as-iq": bool -- specify whether to treat complex data as IQ: the negative frequency bins are assumed to be a continuous extension of the positive frequency bins, and the whole spectrum is shifted so that it starts at DC; this is only used if the transform state has also been specified.

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
     - "ts-real": void (Nymph::KTDataPtr) -- Perform a forward FFT on a real time series; Requires KTTimeSeriesData; Adds KTChirpSpaceFFT; Emits signal "fft"
     - "ts-fftw": void (Nymph::KTDataPtr) -- Perform a forward FFT on a complex time series; Requires KTTimeSeriesData; Adds KTChirpSpaceFFT; Emits signal "fft"
     - "aa": void (Nymph::KTDataPtr) -- Perform a forward FFT on an analytic associate data; Requires KTAnalyticAssociateData; Adds KTChirpSpaceFFT; Emits signal "fft"
     - "ts-real-as-complex": void (Nymph::KTDataPtr) -- Perform a forward FFT on a real time series; Requires KTTimeSeriesData; Adds KTChirpSpaceFFT; Emits signal "fft"

     Signals:
     - "fft": void (Nymph::KTDataPtr) -- Emitted upon performance of a forward transform; Guarantees KTFrequencySpectrumDataFFTW.
    */

    class KTChirpFFT : public KTFFTW, public Nymph::KTProcessor
    {
        private:
            typedef std::map< std::string, unsigned > TransformFlagMap;

        public:
            enum State
            {
                kNone,
                kR2C,
                kRasC2C,
                kC2C
            };

        public:
            KTChirpFFT(const std::string& name = "chirp-fft");
            virtual ~KTChirpFFT();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(bool, UseWisdom);
            MEMBERVARIABLEREF(std::string, WisdomFilename);

            MEMBERVARIABLE(bool, ComplexAsIQ);

            MEMBERVARIABLE_NOSET(unsigned, TimeSize);
            MEMBERVARIABLE_NOSET(unsigned, FrequencySize);

            MEMBERVARIABLEREF_NOSET(std::string, TransformFlag);

        public:
            /// Set the number of time bins; FFT must be initialized after calling this.
            void SetTimeSize(unsigned nBins);
            /// Change the transform flag; FFT must be initialized after calling this.
            void SetTransformFlag(const std::string& flag);

        private:
            /// note: does not change the state
            void SetTimeSizeForState(unsigned nBins, KTChirpFFT::State intendedState);

            TransformFlagMap fTransformFlagMap;

        public:
            /// Initialize the FFT for real time domain data; optionally specify a new time size (the default value of 0 will leave it unchanged)
            bool InitializeForRealTDD(unsigned timeSize = 0);
            /// Initialize the FFT for real time domain data interpreted as complex; optionally specify a new time size (the default value of 0 will leave it unchanged)
            bool InitializeForRealAsComplexTDD(unsigned timeSize = 0);
            /// Initialize the FFT for complex time domain data; optionally specify a new time size (the default value of 0 will leave it unchanged)
            bool InitializeForComplexTDD(unsigned timeSize = 0);
            /// Initialize the FFT using a KTEggHeader object.
            bool InitializeWithHeader(KTEggHeader& header);

            virtual double GetMinFrequency(double timeBinWidth) const;
            virtual double GetMaxFrequency(double timeBinWidth) const;

            MEMBERVARIABLE_NOSET(KTChirpFFT::State, State);
            MEMBERVARIABLE_NOSET(bool, IsInitialized);

        private:
            bool InitializeFFT(KTChirpFFT::State intendedState, unsigned timeSize = 0);

        public:
            /// Forward FFT - Real Time Data
            bool TransformRealData(KTTimeSeriesData& tsData);
            /// Forward FFT - Real Time Series
            KTChirpSpaceFFT* Transform(const KTTimeSeriesReal* ts) const;
            /// Forward FFT - Real Time Series - No size or bin width checks
            KTChirpSpaceFFT*  FastTransform(const KTTimeSeriesReal* ts) const;
            /// Forward FFT - Real Time Series - Output must exist - No size or bin width checks
            void DoTransform(const KTTimeSeriesReal* tsIn, KTChirpSpaceFFT* fsOut) const;

            /// Forward FFT - Real Time Data as Complex
            bool TransformRealDataAsComplex(KTTimeSeriesData& tsData);
            /// Forward FFT - Real-as-Complex Time Series
            KTChirpSpaceFFT* TransformAsComplex(const KTTimeSeriesReal* ts) const;
            /// Forward FFT - Real-as-Complex Time Series - No size or bin width checks
            KTChirpSpaceFFT*  FastTransformAsComplex(const KTTimeSeriesReal* ts) const;
            /// Forward FFT - Real-as-Complex Time Series - Output must exist - No size or bin width checks
            void DoTransformAsComplex(const KTTimeSeriesReal* tsIn, KTChirpSpaceFFT* fsOut) const;

            /// Forward FFT - Complex Time Data
            bool TransformComplexData(KTTimeSeriesData& tsData);
            /// Forward FFT - Complex Analytic Associate Data
            bool TransformComplexData(KTAnalyticAssociateData& aaData);
            /// Forward FFT - Complex Time Series
            KTChirpSpaceFFT* Transform(const KTTimeSeriesFFTW* ts) const;
            /// Forward FFT - Complex Time Series - No size or bin width checks
            KTChirpSpaceFFT*  FastTransform(const KTTimeSeriesFFTW* ts) const;
            /// Forward FFT - Complex Time Series - Output must exist - No size or bin width checks
            void DoTransform(const KTTimeSeriesFFTW* tsIn, KTChirpSpaceFFT* fsOut) const;

        private:
            // binning cache
            void UpdateBinningCache(double timeBinWidth) const;
            mutable double fTimeBinWidthCache;
            mutable double fFreqMinCache;
            mutable double fFreqMaxCache;

            /// Allocate memory in the i/o arrays for an intended state if provided, or (default) the current state
            /// note: does not change the state
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
            Nymph::KTSignalData fFFTSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTEggHeader > fHeaderSlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTSRealSlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTSComplexSlot;
            Nymph::KTSlotDataOneType< KTAnalyticAssociateData > fAASlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTSRealAsComplexSlot;

    };


    inline double KTChirpFFT::GetMinFrequency(double timeBinWidth) const
    {
        if (fState == kR2C || (fState == kC2C && fComplexAsIQ))
        {
            // DC bin is centered at 0, with half a bin width on either side
            return -0.5 * GetFrequencyBinWidth(timeBinWidth);
        }
        else // frequencies symmetric about DC
        {
            // There's one bin at the center, always: the DC bin.
            // # of bins on the negative side is nFreqBins/2 (rounded down because of integer division).
            // 0.5 is added to the # of bins because of the half of the DC bin on the negative frequency side.
            return -GetFrequencyBinWidth(timeBinWidth) * (double(fFrequencySize/2) + 0.5);
        }
    }

    inline double KTChirpFFT::GetMaxFrequency(double timeBinWidth) const
    {
        if (fState == kR2C || (fState == kC2C && fComplexAsIQ))
        {
            return GetFrequencyBinWidth(timeBinWidth) * ((double)fFrequencySize - 0.5);
        }
        else // frequencies symmetric about DC
        {
            // There's one bin at the center, always: the DC bin.
            // # of bins on the positive side is nFreqBins/2 if the number of bins is odd, and nFreqBins/2-1 if the number of bins is even (division rounded down because of integer division).
            // 0.5 is added to the # of bins because of the half of the DC bin on the positive frequency side.
            unsigned nBinsToSide = fFrequencySize / 2;
            return GetFrequencyBinWidth(timeBinWidth) * (double(nBinsToSide*2 == fFrequencySize ? nBinsToSide - 1 : nBinsToSide) + 0.5);
        }
    }

    inline void KTChirpFFT::UpdateBinningCache(double timeBinWidth) const
    {
        if (timeBinWidth == fTimeBinWidthCache) return;
        fTimeBinWidthCache = timeBinWidth;
        fFreqMinCache = GetMinFrequency(timeBinWidth);
        fFreqMaxCache = GetMaxFrequency(timeBinWidth);
        return;
    }

} /* namespace Katydid */

#endif /* KTCHIRPFFT_HH_ */
