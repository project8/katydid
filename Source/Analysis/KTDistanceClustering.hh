/**
 @file KTDistanceClustering.hh
 @brief Contains KTDistanceClustering
 @details Simple 1-D clustering based on distance between discriminated points from a frequency spectrum
 @author: N. S. Oblath
 @date: Dec, 17, 2012
 */


#ifndef KTDISTANCECLUSTERING_HH_
#define KTDISTANCECLUSTERING_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>


namespace Katydid
{
    struct KTData;
    class KTDiscriminatedPoints1DData;
    //class KTDiscriminatedPoints2DData;

    /*!
     @class KTDistanceClustering
     @author N. S. Oblath

     @brief Simple 1-D clustering based on distance between discriminated points from a frequency spectrum

     @details

     Configuration name: "distance-clustering"

     Available configuration values:
     - "max-frequency-distance": double -- Set maximum separation within a cluster by frequency
     - "max-bin-distance": unsigned int -- Set maximum separation within a cluster by bin

      Slots:
     - "disc-1d": void (shared_ptr< KTData >) -- Cluster 1D discriminated points; Requires KTDiscriminatedPoints1DData; Adds KTCluster1DData

     Signals:
     - "cluster-1d": void (shared_ptr< KTData >) -- Emitted after forming clusters; Guarantees KTCluster1DData
    */
    class KTDistanceClustering : public KTProcessor
    {
        public:
            KTDistanceClustering(const std::string& name = "distance-clustering");
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
            KTSignalData fCluster1DSignal;
            //KTSignalData fCluster2DSignal;

            //***************
            // Slots
            //***************

        public:
            void Process1DData(boost::shared_ptr< KTData > data);
            //void Process2DData(const KTDiscriminatedPoints2DData* data);

        private:
            KTSlotDataOneType< KTDiscriminatedPoints1DData > fDiscPoints1DSlot;
            //KTSlotDataOneType< KTDiscriminatedPoints2DData > fDiscPoints2DSlot;

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
