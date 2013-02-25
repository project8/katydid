/*
 * KTDistanceClustering.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#ifndef KTDISTANCECLUSTERING_HH_
#define KTDISTANCECLUSTERING_HH_

#include "KTProcessor.hh"

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    class KTData;
    class KTDiscriminatedPoints1DData;
    //class KTDiscriminatedPoints2DData;

    class KTDistanceClustering : public KTProcessor
    {
        public:
            typedef KTSignalConcept< void (boost::shared_ptr< KTData >) >::signal Cluster1DSignal;
            //typedef KTSignalConcept< void (const KTCluster2DData*) >::signal Cluster2DSignal;


        public:
            KTDistanceClustering();
            virtual ~KTDistanceClustering();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetMaxFrequencyDistance() const;
            void SetMaxFrequencyDistance(Double_t freq);

            UInt_t GetMaxBinDistance() const;
            void SetMaxBinDistance(UInt_t bin);

        private:

            Double_t fMaxFrequencyDistance;
            UInt_t fMaxBinDistance;
            Bool_t fCalculateMaxBinDistance;

        public:
            Bool_t FindClusters(KTDiscriminatedPoints1DData& data);
            //KTCluster2DData* FindClusters(const KTDiscriminatedPoints2DData* data);

            //***************
            // Signals
            //***************

        private:
            Cluster1DSignal fCluster1DSignal;
            //Cluster2DSignal fCluster2DSignal;

            //***************
            // Slots
            //***************

        public:
            void Process1DData(boost::shared_ptr< KTData > data);
            //void Process2DData(const KTDiscriminatedPoints2DData* data);

    };

    inline Double_t KTDistanceClustering::GetMaxFrequencyDistance() const
    {
        return fMaxFrequencyDistance;
    }

    inline void KTDistanceClustering::SetMaxFrequencyDistance(Double_t freq)
    {
        fMaxFrequencyDistance = freq;
        fCalculateMaxBinDistance = true;
        return;
    }

    inline UInt_t KTDistanceClustering::GetMaxBinDistance() const
    {
        return fMaxBinDistance;
    }

    inline void KTDistanceClustering::SetMaxBinDistance(UInt_t bin)
    {
        fMaxBinDistance = bin;
        fCalculateMaxBinDistance = false;
        return;
    }

} /* namespace Katydid */
#endif /* KTDISTANCECLUSTERING_HH_ */
