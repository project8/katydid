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
    class KTTimeSeriesFFTW;
    class KTTimeSeriesDataFFTW;
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
     \li \c direction -- select if the forward ("FORWARD") or reverse ("BACKWARD") transform is performed.

     Transform flags control how FFTW performs the FFT.
     Currently only the following "rigor" flags are available:
     \li \c ESTIMATE -- "A simple heuristic is used to pick a (probably sub-optimal) plan quickly."
     \li \c MEASURE --  "Find[s] an optimized plan by actually computing several FFTs and measuring their execution time. Depending on your machine, this can take some time (often a few seconds)." This is the default option.
     \li \c PATIENT -- "Considers a wider range of algorithms and often produces a “more optimal” plan (especially for large transforms), but at the expense of several times longer planning time (especially for large transforms)."
     \li \c EXHAUSTIVE -- "Considers an even wider range of algorithms, including many that we think are unlikely to be fast, to produce the most optimal plan but with a substantially increased planning time."
     These flag descriptions are quoted from the FFTW3 manual (http://www.fftw.org/fftw3_doc/Planner-Flags.html#Planner-Flags)

     FFTW_PRESERVE_INPUT is automatically added to the transform flag so that, particularly for the reverse transform, the input data is not destroyed.

     Slots:
     \li \c void ProcessHeader(const KTEggHeader* header)
     \li \c void ProcessEvent(UInt_t iEvent, const KTEvent* event)
     \li \c void ProcessTimeSeriesData(const KTTimeSeriesDataFFTW* data)

     Signals:
     \li \c void (UInt_t, const KTComplexFFTW*) emitted upon performance of a transform.
    */

    class KTComplexFFTW : public KTFFT, public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTWriteableData*) >::signal FFTSignal;

        protected:
            typedef std::map< std::string, UInt_t > TransformFlagMap;
            typedef std::map< std::string, UInt_t > DirectionMap;

        public:
            KTComplexFFTW();
            KTComplexFFTW(UInt_t timeSize);
            virtual ~KTComplexFFTW();

            Bool_t Configure(const KTPStoreNode* node);

            virtual void InitializeFFT();

            virtual KTFrequencySpectrumDataFFTW* TransformData(const KTTimeSeriesDataFFTW* tsData);

            KTFrequencySpectrumFFTW* Transform(const KTTimeSeriesFFTW* data) const;

            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;

            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized to kFALSE.
            void SetTimeSize(UInt_t nBins);

            const std::string& GetDirection() const;
            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;

            /// note: SetDirection does NOT affect fIsInitialized.
            void SetDirection(const std::string& dir);

            /// note: SetTransoformFlag sets fIsInitialized to false.
            void SetTransformFlag(const std::string& flag);

        protected:
            void AllocateArrays();
            UInt_t CalculateNFrequencyBins(UInt_t nTimeBins) const; // do not make this virtual (called from the constructor)
            void SetupInternalMaps(); // do not make this virtual (called from the constructor)

            fftw_plan fFTPlan[2];
            UInt_t fActivePlanIndex;

            UInt_t fTimeSize;
            fftw_complex* fInputArray;
            fftw_complex* fOutputArray;

            std::string fDirection;
            DirectionMap fDirectionMap;

            std::string fTransformFlag;
            TransformFlagMap fTransformFlagMap;

            Bool_t fIsInitialized;

            //***************
            // Signals
            //***************

        private:
            FFTSignal fFFTSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessHeader(const KTEggHeader* header);
            void ProcessEvent(KTEvent* event);
            void ProcessTimeSeriesData(const KTTimeSeriesDataFFTW* tsData);

    };


    inline UInt_t KTComplexFFTW::GetTimeSize() const
    {
        return fTimeSize;
    }

    inline UInt_t KTComplexFFTW::GetFrequencySize() const
    {
        return fTimeSize;
    }

    inline const std::string& KTComplexFFTW::GetDirection() const
    {
        return fDirection;
    }

    inline const std::string& KTComplexFFTW::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTComplexFFTW::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline UInt_t KTComplexFFTW::CalculateNFrequencyBins(UInt_t nTimeBins) const
    {
        // Integer division is rounded down, per FFTW's instructions
        return nTimeBins / 2 + 1;
    }

} /* namespace Katydid */
#endif /* KTCOMPLEXFFTW_HH_ */
