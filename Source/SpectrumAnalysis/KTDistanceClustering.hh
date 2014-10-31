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

#include "KTData.hh"
#include "KTSlot.hh"


namespace Katydid
{
    class KTDiscriminatedPoints1DData;
    //class KTDiscriminatedPoints2DData;

    /*!
     @class KTDistanceClustering
     @author N. S. Oblath

     @brief Simple 1-D clustering based on distance between discriminated points from a frequency spectrum

     @details
     NOTE: the output data class, KTCluster1DData, has a single threshold for all points (as of 6/23/14).
     Therefore, this processor is only suitable for use with a flat threshold (e.g. KTSpectrumDiscriminator).

     Configuration name: "distance-clustering"

     Available configuration values:
     - "max-frequency-distance": double -- Set maximum separation within a cluster by frequency
     - "max-bin-distance": unsigned int -- Set maximum separation within a cluster by bin

      Slots:
     - "disc-1d": void (KTDataPtr) -- Cluster 1D discriminated points; Requires KTDiscriminatedPoints1DData; Adds KTCluster1DData

     Signals:
     - "cluster-1d": void (KTDataPtr) -- Emitted after forming clusters; Guarantees KTCluster1DData
    */
    class KTDistanceClustering : public KTProcessor
    {
        public:
            KTDistanceClustering(const std::string& name = "distance-clustering");
            virtual ~KTDistanceClustering();

            bool Configure(const KTParamNode* node);

            double GetMaxFrequencyDistance() const;
            void SetMaxFrequencyDistance(double freq);

            unsigned GetMaxBinDistance() const;
            void SetMaxBinDistance(unsigned bin);

        private:

            double fMaxFrequencyDistance;
            unsigned fMaxBinDistance;
            bool fCalculateMaxBinDistance;

        public:
            bool FindClusters(KTDiscriminatedPoints1DData& data);
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
            void Process1DData(KTDataPtr data);
            //void Process2DData(const KTDiscriminatedPoints2DData* data);

        private:
            KTSlotDataOneType< KTDiscriminatedPoints1DData > fDiscPoints1DSlot;
            //KTSlotDataOneType< KTDiscriminatedPoints2DData > fDiscPoints2DSlot;

    };

    inline double KTDistanceClustering::GetMaxFrequencyDistance() const
    {
        return fMaxFrequencyDistance;
    }

    inline void KTDistanceClustering::SetMaxFrequencyDistance(double freq)
    {
        fMaxFrequencyDistance = freq;
        fCalculateMaxBinDistance = true;
        return;
    }

    inline unsigned KTDistanceClustering::GetMaxBinDistance() const
    {
        return fMaxBinDistance;
    }

    inline void KTDistanceClustering::SetMaxBinDistance(unsigned bin)
    {
        fMaxBinDistance = bin;
        fCalculateMaxBinDistance = false;
        return;
    }

} /* namespace Katydid */
#endif /* KTDISTANCECLUSTERING_HH_ */
