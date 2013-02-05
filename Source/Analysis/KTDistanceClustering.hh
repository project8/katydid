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
    class KTCluster1DData;
    //class KTCluster2DData;
    class KTDiscriminatedPoints1DData;
    //class KTDiscriminatedPoints2DData;
    class KTBundle;

    class KTDistanceClustering : public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTCluster1DData*) >::signal Cluster1DSignal;
            //typedef KTSignal< void (const KTCluster2DData*) >::signal Cluster2DSignal;


        public:
            KTDistanceClustering();
            virtual ~KTDistanceClustering();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetMaxFrequencyDistance() const;
            void SetMaxFrequencyDistance(Double_t freq);

            UInt_t GetMaxBinDistance() const;
            void SetMaxBinDistance(UInt_t bin);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        private:

            Double_t fMaxFrequencyDistance;
            UInt_t fMaxBinDistance;
            Bool_t fCalculateMaxBinDistance;

            std::string fInputDataName;
            std::string fOutputDataName;

        public:
            KTCluster1DData* FindClusters(const KTDiscriminatedPoints1DData* data);
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
            void ProcessEvent(boost::shared_ptr<KTBundle> bundle);
            void Process1DData(const KTDiscriminatedPoints1DData* data);
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
    inline const std::string& KTDistanceClustering::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTDistanceClustering::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTDistanceClustering::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTDistanceClustering::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

} /* namespace Katydid */
#endif /* KTDISTANCECLUSTERING_HH_ */
