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

#include <fftw3.h>

#include <map>
#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(fftlog_comp, "katydid.fft");

    class KTEggHeader;
    class KTEvent;
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
     \li \c transform_flag -- flag that determines how much planning is done prior to any transforms (see below)

     Transform flags control how FFTW performs the FFT.
     Currently only the following "rigor" flags are available:
     \li \c ESTIMATE -- "A simple heuristic is used to pick a (probably sub-optimal) plan quickly."
     \li \c MEASURE --  "Find[s] an optimized plan by actually computing several FFTs and measuring their execution time. Depending on your machine, this can take some time (often a few seconds)." This is the default option.
     \li \c PATIENT -- "Considers a wider range of algorithms and often produces a “more optimal” plan (especially for large transforms), but at the expense of several times longer planning time (especially for large transforms)."
     \li \c EXHAUSTIVE -- "Considers an even wider range of algorithms, including many that we think are unlikely to be fast, to produce the most optimal plan but with a substantially increased planning time."
     These flag descriptions are quoted from the FFTW3 manual (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags)

     FFTW_PRESERVE_INPUT is automatically added to the transform flag so that, particularly for the reverse transform, the input data is not destroyed.

     Slots:
     \li \c "header": void ProcessHeader(const KTEggHeader* header)
     \li \c "event-forward": void ProcessEventForward(const KTEvent* event)
     \li \c "event-reverse": void ProcessEventReverse(const KTEvent* event)
     \li \c "ts-data": void ProcessTimeSeriesData(const KTTimeSeriesDataFFTW* data)
     \li \c "fs-data": void ProcessFrequencySpectrumData(const KTTimeSeriesDataFFTW* data)

     Signals:
     \li \c "fft-forward": void (const KTWriteableData*) emitted upon performance of a forward transform.
     \li \c "fft-reverse": void (const KTWriteableData*) emitted upon performance of a reverse transform.
    */

    class KTComplexFFTW : public KTFFT, public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTWriteableData*) >::signal FFTSignal;

        protected:
            typedef std::map< std::string, UInt_t > TransformFlagMap;

        public:
            KTComplexFFTW();
            KTComplexFFTW(UInt_t size);
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

            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;

            /// note: SetSize creates a new fTransform.
            ///       It also sets fIsInitialized to kFALSE.
            void SetSize(UInt_t nBins);

            /// note: SetTransoformFlag sets fIsInitialized to false.
            void SetTransformFlag(const std::string& flag);

        protected:
            void AllocateArrays();
            void SetupInternalMaps(); // do not make this virtual (called from the constructor)

            fftw_plan fForwardPlan;
            fftw_plan fReversePlan;

            UInt_t fSize;
            fftw_complex* fInputArray;
            fftw_complex* fOutputArray;

            std::string fTransformFlag;
            TransformFlagMap fTransformFlagMap;

            Bool_t fIsInitialized;

            //***************
            // Signals
            //***************

        private:
            FFTSignal fFFTForwardSignal;
            FFTSignal fFFTReverseSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessHeader(const KTEggHeader* header);
            void ProcessEventForward(KTEvent* event);
            void ProcessEventReverse(KTEvent* event);
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

    inline const std::string& KTComplexFFTW::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTComplexFFTW::GetIsInitialized() const
    {
        return fIsInitialized;
    }

} /* namespace Katydid */
#endif /* KTCOMPLEXFFTW_HH_ */
