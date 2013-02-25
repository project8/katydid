/*
 * KTHoughTransform.hh
 *
 *  Created on: Nov 14, 2012
 *      Author: nsoblath
 */

#ifndef KTHOUGHTRANSFORM_HH_
#define KTHOUGHTRANSFORM_HH_

#include "KTProcessor.hh"

#include "KTDiscriminatedPoints2DData.hh"
#include "KTPhysicalArray.hh"

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTData;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    //class KTSlidingWindowFSDataFFTW;

    class KTHoughTransform : public KTProcessor
    {
        public:
            typedef KTDiscriminatedPoints2DData::SetOfPoints SetOfPoints;

        protected:
            typedef KTSignalConcept< void (boost::shared_ptr< KTData >) >::signal HTSignal;

        public:
            KTHoughTransform();
            virtual ~KTHoughTransform();

            Bool_t Configure(const KTPStoreNode* node);

            UInt_t GetNThetaPoints() const;
            void SetNThetaPoints(UInt_t nPoints);

            UInt_t GetNRPoints() const;
            void SetNRPoints(UInt_t nPoints);

        protected:

            UInt_t fNThetaPoints;
            UInt_t fNRPoints;

        public:
            //Bool_t TransformData(KTSlidingWindowFSDataFFTW& data);
            //KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* TransformSpectrum(const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* powerSpectrum);

            Bool_t TransformData(KTDiscriminatedPoints2DData& data);
            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* TransformSetOfPoints(const SetOfPoints& points, UInt_t nTimeBins, UInt_t nFreqBins);


        protected:
            //KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* RemoveNegativeFrequencies(const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* inputSpectrum);

            //***************
             // Signals
             //***************

         private:
             HTSignal fHTSignal;

             //***************
             // Slots
             //***************

         public:
             //void ProcessSWFSData(const KTSlidingWindowFSDataFFTW* data);
             void ProcessDiscriminatedData(boost::shared_ptr< KTData > data);


    };

    inline UInt_t KTHoughTransform::GetNThetaPoints() const
    {
        return fNThetaPoints;
    }

    inline void KTHoughTransform::SetNThetaPoints(UInt_t nPoints)
    {
        fNThetaPoints = nPoints;
        return;
    }

    inline UInt_t KTHoughTransform::GetNRPoints() const
    {
        return fNRPoints;
    }

    inline void KTHoughTransform::SetNRPoints(UInt_t nPoints)
    {
        fNRPoints = nPoints;
        return;
    }

} /* namespace Katydid */
#endif /* KTHOUGHTRANSFORM_HH_ */
