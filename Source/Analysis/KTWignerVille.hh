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

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTMath.hh"

#include <boost/shared_ptr.hpp>

#include <complex>
#include <utility>


namespace Katydid
{
    class KTWignerVilleData : public KTFrequencySpectrumDataFFTWCore, public KTExtensibleData< KTWignerVilleData >
    {
        public:
            KTWignerVilleData() :
                    KTFrequencySpectrumDataFFTWCore(),
                    KTExtensibleData< KTWignerVilleData >()
            {}
            virtual ~KTWignerVilleData()
            {}

            const std::pair< UInt_t, UInt_t >& GetInputPair(UInt_t component = 0) const;

            void SetInputPair(UInt_t first, UInt_t second, UInt_t component = 0);

            KTWignerVilleData& SetNComponents(UInt_t components);

        protected:
            std::vector< std::pair< UInt_t, UInt_t > > fWVComponentData;
    };

    inline const std::pair< UInt_t, UInt_t >& KTWignerVilleData::GetInputPair(UInt_t component) const
    {
        return fWVComponentData[component];
    }

    inline void KTWignerVilleData::SetInputPair(UInt_t first, UInt_t second, UInt_t component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fWVComponentData[component].first = first;
        fWVComponentData[component].second = second;
        return;
    }

    inline KTWignerVilleData& KTWignerVilleData::SetNComponents(UInt_t components)
    {
        fSpectra.resize(components);
        fWVComponentData.resize(components);
        return *this;
    }



    class KTComplexFFTW;
    class KTData;
    class KTEggHeader;
    //class KTFrequencySpectrumPolar;
    //class KTFrequencySpectrumDataFFTW;
    //class KTFrequencySpectrumFFTW;
    //class KTSlidingWindowFFTW;
    //class KTSlidingWindowFSData;
    //class KTSlidingWindowFSDataFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;

    typedef std::pair< UInt_t, UInt_t > KTWVPair;

    class KTWignerVille : public KTProcessor
    {
        private:
            typedef KTSignal< void (boost::shared_ptr< KTData >) >::signal WVSignal;
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

            KTComplexFFTW* GetFFT();
            const KTComplexFFTW* GetFFT() const;

        private:
            PairVector fPairs;

            KTComplexFFTW* fFFT;
            KTTimeSeriesFFTW* fInputArray;


        public:
            /// Performs the W-V transform on the given time series data.
            Bool_t TransformData(KTTimeSeriesData& data);

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
             void ProcessTimeSeriesData(boost::shared_ptr< KTData > data);

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
