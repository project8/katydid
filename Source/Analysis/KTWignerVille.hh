/*
 * KTWignerVille.hh
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#ifndef KTWIGNERVILLE_HH_
#define KTWIGNERVILLE_HH_

#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTMath.hh"

#include "KTEventWindowFunction.hh"

#include <boost/shared_ptr.hpp>

#include <complex>
#include <fftw3.h>


namespace Katydid
{
    class KTComplexFFTW;
    class KTEggHeader;
    class KTEvent;
    class KTFrequencySpectrum;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTSlidingWindowFFTW;
    class KTSlidingWindowFSData;
    class KTSlidingWindowFSDataFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTWriteableData;

    typedef std::pair< UInt_t, UInt_t > KTWVPair;

    class KTWignerVille : public KTProcessor
    {
        private:
            typedef KTSignal< void (const KTWriteableData*) >::signal WVSignal;
            typedef std::vector< KTWVPair > PairVector;

        private:
            typedef std::map< std::string, Int_t > TransformFlagMap;

        public:
            KTWignerVille();
            virtual ~KTWignerVille();

            Bool_t Configure(const KTPStoreNode* node);

            void AddPair(const KTWVPair& pair);
            void SetPairVector(const PairVector& pairs);
            const PairVector& GetPairVector() const;
            void ClearPairs();

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

            KTComplexFFTW* GetFFT();
            const KTComplexFFTW* GetFFT() const;

        private:
            PairVector fPairs;

            std::string fInputDataName;
            std::string fOutputDataName;

            KTComplexFFTW* fFFT;
            KTTimeSeriesFFTW* fInputArray;


        public:
            /// Performs the W-V transform on the given time series data.
            /// @note A frequency spectrum data object can still be returned even if the full W-V transform fails!
            KTFrequencySpectrumDataFFTW* TransformData(const KTTimeSeriesData* data);

        private:
            void CrossMultiplyToInputArray(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset);



            //***************
             // Signals
             //***************

         private:
             WVSignal fWVSignal;

             //***************
             // Slots
             //***************

         public:
             void ProcessHeader(const KTEggHeader* header);
             void ProcessEvent(boost::shared_ptr<KTEvent> event);
             void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);

    };

    inline void KTWignerVille::AddPair(const KTWVPair& pair)
    {
        fPairs.push_back(pair);
        return;
    }

    inline void KTWignerVille::SetPairVector(const PairVector& pairs)
    {
        fPairs = pairs;
        return;
    }

    inline const KTWignerVille::PairVector& KTWignerVille::GetPairVector() const
    {
        return fPairs;
    }

    inline void KTWignerVille::ClearPairs()
    {
        fPairs.clear();
        return;
    }

    inline const std::string& KTWignerVille::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTWignerVille::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTWignerVille::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTWignerVille::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

    inline KTComplexFFTW* KTWignerVille::GetFFT()
    {
        return fFFT;
    }

    inline const KTComplexFFTW* KTWignerVille::GetFFT() const
    {
        return fFFT;
    }


} /* namespace Katydid */
#endif /* KTWIGNERVILLE_HH_ */
