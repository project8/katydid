/**
 @file KTMultiSliceClustering.hh
 @brief Contains KTMultiSliceClustering
 @details Simple cluster-finding algorithm that works by looking for lines of high-peaked bins increasing in frequency
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTMULTISLICECLUSTERING_HH_
#define KTMULTISLICECLUSTERING_HH_

#include "KTDataQueueProcessor.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTMath.hh"

#include <boost/shared_ptr.hpp>

#include <deque>
#include <list>
//#include <map>
//#include <set>
#include <utility>

namespace Katydid
{
    class KTCorrelationData;
    class KTData;
    class KTFrequencySpectrumData;

    class KTMultiSliceClustering : public KTDataQueueProcessorTemplate< KTMultiSliceClustering >
    {
        public:
            typedef KTDiscriminatedPoints1DData::SetOfPoints SetOfDiscriminatedPoints;

            struct ClusterPoint
            {
                 UInt_t fTimeBin;
                 UInt_t fFreqBin;
                 Double_t fAmplitude;
                 boost::shared_ptr<KTFrequencySpectrumData> fDataPtr;
            };
            //typedef std::deque< ClusterPoint > Cluster;
            struct Cluster
            {
                std::deque< ClusterPoint > fPoints; // every point in the cluster
                std::deque< std::pair< UInt_t, UInt_t > > fFreqRanges; // first and last frequency bins for each time bin

                UInt_t FirstTimeBin() const {return fPoints.front().fTimeBin;}
                UInt_t LastTimeBin() const {return fPoints.back().fTimeBin;}

                UInt_t EndMinFreqPoint() const {return fFreqRanges.back().first;}
                UInt_t EndMaxFreqPoint() const {return fFreqRanges.back().second;}

                UInt_t fDataComponent;

            };

            typedef std::list< Cluster > ClusterList;

            struct FreqBinCluster
            {
                SetOfDiscriminatedPoints fPoints;
                UInt_t fFirstPoint;
                UInt_t fLastPoint;
                Bool_t fAddedToActiveCluster;
                ClusterList::iterator fActiveCluster;
                UInt_t fACNumber;
            };
            typedef std::list< FreqBinCluster > FreqBinClusters;


            typedef std::list< boost::shared_ptr<KTBundle> > BundleList;


            typedef KTSignal< void (boost::shared_ptr<KTData>) >::signal DataSignal;

        public:
            KTMultiSliceClustering();
            virtual ~KTMultiSliceClustering();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetMaxFrequencySeparation() const;
            void SetMaxFrequencySeparation(Double_t freqSep);

            Double_t GetMaxTimeSeparation() const;
            void SetMaxTimeSeparation(Double_t timeSep);

            UInt_t GetMaxFrequencySeparationBins() const;
            void SetMaxFrequencySeparationBins(UInt_t bins);

            UInt_t GetMaxTimeSeparationBins() const;
            void SetMaxTimeSeparationBins(UInt_t bins);

        private:
            Double_t fMaxFreqSep;
            Double_t fMaxTimeSep;
            UInt_t fMaxFreqSepBins;
            UInt_t fMaxTimeSepBins;
            Bool_t fCalculateMaxFreqSepBins;
            Bool_t fCalculateMaxTimeSepBins;

        public:
            /// Add points from dpData to the active clusters and create candidates
            BundleList* FindClusters(const KTDiscriminatedPoints1DData* dpData, boost::shared_ptr<KTFrequencySpectrumData> fsData);
            /// Add points from dpData to the active clusters and create candidates
            //BundleList* FindClusters(const KTDiscriminatedPoints1DData* dpData, boost::shared_ptr<KTFrequencySpectrumData> fsData);
            /// Add points from dpData to the active clusters and create candidates
            //BundleList* FindClusters(const KTDiscriminatedPoints1DData* dpData, boost::shared_ptr<KTFrequencySpectrumData> corrData);

            /// Add points from dpData to the active clusters
            ClusterList* AddPointsToClusters(const KTDiscriminatedPoints1DData* dpData, boost::shared_ptr<KTFrequencySpectrumData> data);

            /// Add points from a set of points to the active clusters
            ClusterList* AddPointsToClusters(const SetOfDiscriminatedPoints& points, UInt_t component, boost::shared_ptr<KTFrequencySpectrumData> data);

            /// Complete all remaining active clusters
            ClusterList* CompleteAllClusters(UInt_t component);
            //BundleList* CompleteInactiveClusters(UInt_t component);

            void Reset();
            UInt_t GetTimeBin() const;

            Double_t GetTimeBinWidth() const;
            void SetTimeBinWidth(Double_t bw);

            Double_t GetFrequencyBinWidth() const;
            void SetFrequencyBinWidth(Double_t bw);

        private:
            boost::shared_ptr<KTBundle> CreateBundleFromCluster(const Cluster& cluster);

            UInt_t fTimeBin;
            Double_t fTimeBinWidth;
            Double_t fFreqBinWidth;

            std::vector< ClusterList > fActiveClusters;


            //***************
            // Signals
            //***************

         private:
            DataSignal fOneSliceDataSignal;
            DataSignal fClusteredDataSignal;


             //***************
             // Slots
             //***************

         public:
            // QueueData from KTDataQueueProcessorTemplate
            // QueueDataList from KTDataQueueProcessorTemplate

         private:
            /// non-queueing bundle processing
            void ProcessOneSliceData(boost::shared_ptr<KTData> data);

         private:
            void RunDataLoop(BundleList* dataList);

    };


    inline Double_t KTMultiSliceClustering::GetMaxFrequencySeparation() const
    {
        return fMaxFreqSep;
    }

    inline void KTMultiSliceClustering::SetMaxFrequencySeparation(Double_t freqSep)
    {
        fMaxFreqSep = freqSep;
        fCalculateMaxFreqSepBins = true;
        return;
    }

    inline Double_t KTMultiSliceClustering::GetMaxTimeSeparation() const
    {
        return fMaxTimeSep;
    }

    inline void KTMultiSliceClustering::SetMaxTimeSeparation(Double_t timeSep)
    {
        fMaxTimeSep = timeSep;
        fCalculateMaxTimeSepBins = true;
        return;
    }

    inline UInt_t KTMultiSliceClustering::GetMaxFrequencySeparationBins() const
    {
        return fMaxFreqSepBins;
    }

    inline void KTMultiSliceClustering::SetMaxFrequencySeparationBins(UInt_t bins)
    {
        fMaxFreqSepBins = bins;
        fCalculateMaxFreqSepBins = false;
        return;
    }

    inline UInt_t KTMultiSliceClustering::GetMaxTimeSeparationBins() const
    {
        return fMaxTimeSepBins;
    }

    inline void KTMultiSliceClustering::SetMaxTimeSeparationBins(UInt_t bins)
    {
        fMaxTimeSepBins = bins;
        fCalculateMaxTimeSepBins = false;
        return;
    }

    inline UInt_t KTMultiSliceClustering::GetTimeBin() const
    {
        return fTimeBin;
    }

    inline Double_t KTMultiSliceClustering::GetTimeBinWidth() const
    {
        return fTimeBinWidth;
    }

    inline void KTMultiSliceClustering::SetTimeBinWidth(Double_t bw)
    {
        fTimeBinWidth = bw;
        if (fCalculateMaxTimeSepBins)
            fMaxTimeSepBins = KTMath::Nint(fMaxTimeSep / fTimeBinWidth);
        return;
    }

    inline Double_t KTMultiSliceClustering::GetFrequencyBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline void KTMultiSliceClustering::SetFrequencyBinWidth(Double_t bw)
    {
        fFreqBinWidth = bw;
        if (fCalculateMaxFreqSepBins)
            fMaxFreqSepBins = KTMath::Nint(fMaxFreqSep / fFreqBinWidth);
        return;
    }

} /* namespace Katydid */
#endif /* KTMULTISLICECLUSTERING_HH_ */
