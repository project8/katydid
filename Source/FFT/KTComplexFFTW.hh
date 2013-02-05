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
#include "KTFrequencySpectrum.hh"

#include <boost/shared_ptr.hpp>

#include <fftw3.h>

#include <map>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(fftlog_comp, "katydid.fft");

    class KTEggHeader;
    class KTBundle;
    class KTPStoreNode;
    class KTBasicTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesDataFFTW;
    class KTBasicTimeSeriesDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumDataFFTW;
    class KTWriteableData;

    /*!
     @class KTComplexFFTW
     @author N. S. Oblath

     @brief A one-dimensional real-to-complex FFT class.

     @details
     KTComplexFFTW performs a real-to-complex FFT on a one-dimensional array of doubles.

     The FFT is implemented using FFTW.

     Available configuration values:
     \li \c "transform_flag": string -- flag that determines how much planning is done prior to any transforms (see below)
     \li \c "use-wisdom": bool -- whether or not to use FFTW wisdom to improve FFT performance
     \li \c "wisdom-filename": string -- filename for loading/saving FFTW wisdom
     \li \c "forward-input-data-name": string -- name of the data to find if processing an bundle in the forward direction
     \li \c "forward-output-data-name": string -- name to give to the data produced by a forward FFT
     \li \c "reverse-input-data-name": string -- name of the data to find if processing an bundle in the reverse direction
     \li \c "reverse-output-data-name": string -- name of give to the data produced by a reverse FFT

     Transform flags control how FFTW performs the FFT.
     Currently only the following "rigor" flags are available:
     \li \c ESTIMATE -- "A simple heuristic is used to pick a (probably sub-optimal) plan quickly."
     \li \c MEASURE --  "Find[s] an optimized plan by actually computing several FFTs and measuring their execution time. Depending on your machine, this can take some time (often a few seconds)." This is the default option.
     \li \c PATIENT -- "Considers a wider range of algorithms and often produces a �more optimal� plan (especially for large transforms), but at the expense of several times longer planning time (especially for large transforms)."
     \li \c EXHAUSTIVE -- "Considers an even wider range of algorithms, including many that we think are unlikely to be fast, to produce the most optimal plan but with a substantially increased planning time."
     These flag descriptions are quoted from the FFTW3 manual (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags)

     FFTW_PRESERVE_INPUT is automatically added to the transform flag so that, particularly for the reverse transform, the input data is not destroyed.

     Slots:
     \li \c "header": void ProcessHeader(const KTEggHeader* header)
     \li \c "bundle-forward": void ProcessEventForward(const KTBundle* bundle)
     \li \c "bundle-reverse": void ProcessEventReverse(const KTBundle* bundle)
     \li \c "ts-data": void ProcessTimeSeriesData(const KTTimeSeriesDataFFTW* data)
     \li \c "fs-data": void ProcessFrequencySpectrumData(const KTTimeSeriesDataFFTW* data)

     Signals:
     \li \c "fft-forward": void (const KTFrequencySpectrumDataFFTW*) emitted upon performance of a forward transform.
     \li \c "fft-reverse": void (const KTWriteableData*) emitted upon performance of a reverse transform.
    */

    class KTComplexFFTW : public KTFFT, public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTFrequencySpectrumDataFFTW*) >::signal FFTForwardSignal;
            typedef KTSignal< void (const KTTimeSeriesData*) >::signal FFTReverseSignal;

        protected:
            typedef std::map< std::string, UInt_t > TransformFlagMap;

        public:
            KTComplexFFTW();
            virtual ~KTComplexFFTW();

            Bool_t Configure(const KTPStoreNode* node);

            virtual void InitializeFFT();

            /// Forward FFT
            virtual KTFrequencySpectrumDataFFTW* TransformData(const KTTimeSeriesData* tsData);
            /// Reverse FFT
            virtual KTTimeSeriesData* TransformData(const KTFrequencySpectrumDataFFTW* fsData);

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

            const std::string& GetForwardInputDataName() const;
            void SetForwardInputDataName(const std::string& name);

            const std::string& GetForwardOutputDataName() const;
            void SetForwardOutputDataName(const std::string& name);

            const std::string& GetReverseInputDataName() const;
            void SetReverseInputDataName(const std::string& name);

            const std::string& GetReverseOutputDataName() const;
            void SetReverseOutputDataName(const std::string& name);

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

            std::string fForwardInputDataName;
            std::string fForwardOutputDataName;

            std::string fReverseInputDataName;
            std::string fReverseOutputDataName;

            //***************
            // Signals
            //***************

        private:
            FFTForwardSignal fFFTForwardSignal;
            FFTReverseSignal fFFTReverseSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessHeader(const KTEggHeader* header);
            void ProcessEventForward(boost::shared_ptr<KTBundle> bundle);
            void ProcessEventReverse(boost::shared_ptr<KTBundle> bundle);
            void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);
            void ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData);

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

    inline const std::string& KTComplexFFTW::GetForwardInputDataName() const
    {
        return fForwardInputDataName;
    }

    inline void KTComplexFFTW::SetForwardInputDataName(const std::string& name)
    {
        fForwardInputDataName = name;
        return;
    }

    inline const std::string& KTComplexFFTW::GetForwardOutputDataName() const
    {
        return fForwardOutputDataName;
    }

    inline void KTComplexFFTW::SetForwardOutputDataName(const std::string& name)
    {
        fForwardOutputDataName = name;
        return;
    }

    inline const std::string& KTComplexFFTW::GetReverseInputDataName() const
    {
        return fReverseInputDataName;
    }

    inline void KTComplexFFTW::SetReverseInputDataName(const std::string& name)
    {
        fReverseInputDataName = name;
        return;
    }

    inline const std::string& KTComplexFFTW::GetReverseOutputDataName() const
    {
        return fReverseOutputDataName;
    }

    inline void KTComplexFFTW::SetReverseOutputDataName(const std::string& name)
    {
        fReverseOutputDataName = name;
        return;
    }

} /* namespace Katydid */
#endif /* KTCOMPLEXFFTW_HH_ */
