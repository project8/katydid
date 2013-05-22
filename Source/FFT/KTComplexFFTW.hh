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

#include <boost/shared_ptr.hpp>

#include <fftw3.h>

#include <map>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(fftlog_comp, "katydid.fft");

    class KTAnalyticAssociateData;
    class KTData;
    class KTEggHeader;
    class KTPStoreNode;
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
     - PATIENT -- "Considers a wider range of algorithms and often produces a “more optimal” plan (especially for large transforms), but at the expense of several times longer planning time (especially for large transforms)."
     - EXHAUSTIVE -- "Considers an even wider range of algorithms, including many that we think are unlikely to be fast, to produce the most optimal plan but with a substantially increased planning time."
     These flag descriptions are quoted from the FFTW3 manual (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags)

     FFTW_PRESERVE_INPUT is automatically added to the transform flag so that, particularly for the reverse transform, the input data is not destroyed.

     Slots:
     - "header": void (const KTEggHeader* header) -- Initialize the FFT from an Egg header
     - "ts": void (shared_ptr<KTData>) -- Perform a forward FFT on the time series; Requires KTTimeSeriesData; Adds KTFrequencySpectrumPolar; Emits signal "fft-forward"
     - "aa": void (shared_ptr<KTData>) -- Perform a forward FFT on an analytic associate data; Requires KTAnalyticAssociateData; Adds KTFrequencySpectrumPolar; Emits signal "fft-forward"
     - "fs-fftw": void (shared_ptr<KTData>) -- Perform a reverse FFT on the frequency spectrum; Requires KTFrequencySpectrumDataFFTW; Adds KTTimeSeriesData; Emits signal "fft-reverse"

     Signals:
     - "fft-forward": void (shared_ptr<KTData>) -- Emitted upon performance of a forward transform; Guarantees KTFrequencySpectrumDataFFTW.
     - "fft-reverse": void (shared_ptr<KTData>) -- Emitted upon performance of a reverse transform; Guarantees KTTimeSeriesData.
    */

    class KTComplexFFTW : public KTFFTW, public KTProcessor
    {
        protected:
            typedef std::map< std::string, UInt_t > TransformFlagMap;

        public:
            KTComplexFFTW(const std::string& name = "complex-fftw");
            virtual ~KTComplexFFTW();

            Bool_t Configure(const KTPStoreNode* node);

            void InitializeFFT();
            void InitializeWithHeader(const KTEggHeader* header);

            /// Forward FFT
            Bool_t TransformData(KTTimeSeriesData& tsData);
            Bool_t TransformData(KTAnalyticAssociateData& aaData);
            /// Reverse FFT
            Bool_t TransformData(KTFrequencySpectrumDataFFTW& fsData);

            /// Forward FFT
            KTFrequencySpectrumFFTW* Transform(const KTTimeSeriesFFTW* data) const;
            /// Reverse FFT
            KTTimeSeriesFFTW* Transform(const KTFrequencySpectrumFFTW* data) const;

            virtual UInt_t GetSize() const;
            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;
            virtual Double_t GetMinFrequency(Double_t timeBinWidth) const;
            virtual Double_t GetMaxFrequency(Double_t timeBinWidth) const;

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Bool_t GetUseWisdom() const;
            const std::string& GetWisdomFilename() const;

            /// note: SetSize creates a new fTransform.
            ///       It also sets fIsInitialized to kFALSE.
            void SetSize(UInt_t nBins);

            /// note: SetTransoformFlag sets fIsInitialized to false.
            void SetTransformFlag(const std::string& flag);
            void SetUseWisdom(Bool_t flag);
            void SetWisdomFilename(const std::string& fname);

        protected:
            void AllocateArrays();
            void FreeArrays();
            void SetupInternalMaps(); // do not make this virtual (called from the constructor)

            fftw_plan fForwardPlan;
            fftw_plan fReversePlan;

            UInt_t fSize;
            fftw_complex* fInputArray;
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
            KTSignalData fFFTForwardSignal;
            KTSignalData fFFTReverseSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;
            KTSlotDataOneType< KTAnalyticAssociateData > fAASlot;
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;

    };


    inline UInt_t KTComplexFFTW::GetSize() const
    {
        return fSize;
    }

    inline UInt_t KTComplexFFTW::GetTimeSize() const
    {
        return fSize;
    }

    inline UInt_t KTComplexFFTW::GetFrequencySize() const
    {
        return fSize;
    }

    inline Double_t KTComplexFFTW::GetMinFrequency(Double_t timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the negative side is nFreqBins/2 (rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the negative frequency side.
        return -GetFrequencyBinWidth(timeBinWidth) * (Double_t(fSize/2) + 0.5);
    }

    inline Double_t KTComplexFFTW::GetMaxFrequency(Double_t timeBinWidth) const
    {
        // There's one bin at the center, always: the DC bin.
        // # of bins on the positive side is nFreqBins/2 if the number of bins is odd, and nFreqBins/2-1 if the number of bins is even (division rounded down because of integer division).
        // 0.5 is added to the # of bins because of the half of the DC bin on the positive frequency side.
        UInt_t nBinsToSide = fSize / 2;
        return GetFrequencyBinWidth(timeBinWidth) * (Double_t(nBinsToSide*2 == fSize ? nBinsToSide - 1 : nBinsToSide) + 0.5);
    }

    inline const std::string& KTComplexFFTW::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTComplexFFTW::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline Bool_t KTComplexFFTW::GetUseWisdom() const
    {
        return fUseWisdom;
    }

    inline const std::string& KTComplexFFTW::GetWisdomFilename() const
    {
        return fWisdomFilename;
    }

    inline void KTComplexFFTW::SetUseWisdom(Bool_t flag)
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
