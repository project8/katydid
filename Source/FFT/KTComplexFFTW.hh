/**
 @file KTComplexFFTW.hh
 @brief Contains KTComplexFFTW
 @details Calculates a 1-dimensional FFT on a set of real data.
 @author: N. S. Oblath
 @date: Sep 12, 2011
 */

#ifndef KTCOMPLEXFFTW_HH_
#define KTCOMPLEXFFTW_HH_

#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTLogger.hh"
#include "KTSlot.hh"

#include <fftw3.h>

#include <map>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(fftlog_comp, "KTComplexFFTW");

    class KTAnalyticAssociateData;
    class KTEggHeader;
    class KTParamNode;
    class KTTimeSeriesFFTW;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;

    /*!
     @class KTComplexFFTW
     @author N. S. Oblath

     @brief A one-dimensional real-to-complex FFT class.

     @details
     KTComplexFFTW performs a real-to-complex FFT on a one-dimensional array of doubles.

     The FFT is implemented using FFTW.

     Configuration name: "complex-fftw"

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
     - "header": void (KTEggHeader*) -- Initialize the FFT from an Egg header
     - "ts": void (KTDataPtr) -- Perform a forward FFT on the time series; Requires KTTimeSeriesData; Adds KTFrequencySpectrumPolar; Emits signal "fft-forward"
     - "aa": void (KTDataPtr) -- Perform a forward FFT on an analytic associate data; Requires KTAnalyticAssociateData; Adds KTFrequencySpectrumPolar; Emits signal "fft-forward"
     - "fs-fftw": void (KTDataPtr) -- Perform a reverse FFT on the frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTTimeSeriesData; Emits signal "fft-reverse"

     Signals:
     - "fft-forward": void (KTDataPtr) -- Emitted upon performance of a forward transform; Guarantees KTFrequencySpectrumDataFFTW.
     - "fft-reverse": void (KTDataPtr) -- Emitted upon performance of a reverse transform; Guarantees KTTimeSeriesData.
    */

    class KTComplexFFTW : public KTFFTW, public KTProcessor
    {
        protected:
            typedef std::map< std::string, unsigned > TransformFlagMap;

        public:
            KTComplexFFTW(const std::string& name = "complex-fftw");
            virtual ~KTComplexFFTW();

            bool Configure(const KTParamNode* node);

            void InitializeFFT();
            void InitializeWithHeader(KTEggHeader* header);

            /// Forward FFT
            bool TransformData(KTTimeSeriesData& tsData);
            bool TransformData(KTAnalyticAssociateData& aaData);
            /// Reverse FFT
            bool TransformData(KTFrequencySpectrumDataFFTW& fsData);

            /// Forward FFT
            KTFrequencySpectrumFFTW* Transform(const KTTimeSeriesFFTW* ts) const;
            void DoTransform(const KTTimeSeriesFFTW* tsIn, KTFrequencySpectrumFFTW* fsOut) const;
            /// Reverse FFT
            KTTimeSeriesFFTW* Transform(const KTFrequencySpectrumFFTW* fs) const;
            void DoTransform(const KTFrequencySpectrumFFTW* fsIn, KTTimeSeriesFFTW* tsOut) const;

            virtual unsigned GetSize() const;
            virtual unsigned GetTimeSize() const;
            virtual unsigned GetFrequencySize() const;
            virtual double GetMinFrequency(double timeBinWidth) const;
            virtual double GetMaxFrequency(double timeBinWidth) const;

            const std::string& GetTransformFlag() const;
            bool GetIsInitialized() const;
            bool GetUseWisdom() const;
            const std::string& GetWisdomFilename() const;

            /// note: SetSize creates a new fTransform.
            ///       It also sets fIsInitialized to false.
            void SetSize(unsigned nBins);

            /// note: SetTransoformFlag sets fIsInitialized to false.
            void SetTransformFlag(const std::string& flag);
            void SetUseWisdom(bool flag);
            void SetWisdomFilename(const std::string& fname);

        protected:
            void AllocateArrays();
            void FreeArrays();
            void SetupInternalMaps(); // do not make this virtual (called from the constructor)

            fftw_plan fForwardPlan;
            fftw_plan fReversePlan;

            unsigned fSize;
            fftw_complex* fInputArray;
            fftw_complex* fOutputArray;

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

            //***************
            // Slots
            //***************

        private:
            KTSlotOneArg< void (KTEggHeader*) > fHeaderSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
            KTSlotDataOneType< KTAnalyticAssociateData > fAASlot;
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;

    };


    inline unsigned KTComplexFFTW::GetSize() const
    {
        return fSize;
    }

    inline unsigned KTComplexFFTW::GetTimeSize() const
    {
        return fSize;
    }

    inline unsigned KTComplexFFTW::GetFrequencySize() const
    {
        return fSize;
    }

    inline double KTComplexFFTW::GetMinFrequency(double timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the negative side is nFreqBins/2 (rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the negative frequency side.
        return -GetFrequencyBinWidth(timeBinWidth) * (double(fSize/2) + 0.5);
    }

    inline double KTComplexFFTW::GetMaxFrequency(double timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the positive side is nFreqBins/2 if the number of bins is odd, and nFreqBins/2-1 if the number of bins is even (division rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the positive frequency side.
        unsigned nBinsToSide = fSize / 2;
        return GetFrequencyBinWidth(timeBinWidth) * (double(nBinsToSide*2 == fSize ? nBinsToSide - 1 : nBinsToSide) + 0.5);
    }

    inline const std::string& KTComplexFFTW::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline bool KTComplexFFTW::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline bool KTComplexFFTW::GetUseWisdom() const
    {
        return fUseWisdom;
    }

    inline const std::string& KTComplexFFTW::GetWisdomFilename() const
    {
        return fWisdomFilename;
    }

    inline void KTComplexFFTW::SetUseWisdom(bool flag)
    {
        fUseWisdom = flag;
        return;
    }

    inline void KTComplexFFTW::SetWisdomFilename(const std::string& fname)
    {
        fWisdomFilename = fname;
        return;
    }

} /* namespace Katydid */
#endif /* KTCOMPLEXFFTW_HH_ */
