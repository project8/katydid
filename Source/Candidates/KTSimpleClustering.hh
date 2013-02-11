/**
 @file KTSimpleClustering.hh
 @brief Contains KTSimpleClustering
 @details Simple cluster-finding algorithm that works by looking for lines of high-peaked bins increasing in frequency
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTSIMPLECLUSTERING_HH_
#define KTSIMPLECLUSTERING_HH_

#include "KTBundleQueueProcessor.hh"

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
    class KTBundle;
    class KTCorrelationData;
    class KTData;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;

    class KTSimpleClustering : public KTBundleQueueProcessorTemplate< KTSimpleClustering >
    {
        public:
            typedef KTDiscriminatedPoints1DData::SetOfPoints SetOfDiscriminatedPoints;

            struct ClusterPoint
            {
                 UInt_t fTimeBin;
                 UInt_t fFreqBin;
                 Double_t fAmplitude;
                 boost::shared_ptr<KTData> fDataPtr;
            };
            //typedef std::deque< ClusterPoint > Cluster;
            struct Cluster
            {
                std::deque< ClusterPoint > fPoints; // every point in the cluster
                std::deque< std::pair< UInt_t, UInt_t > > fFreqRanges; // first and last frequency bins for each time bin

                UInt_t EndMinFreqPoint() {return fFreqRanges.back().first;}
                UInt_t EndMaxFreqPoint() {return fFreqRanges.back().second;}

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


            typedef KTSignal< void (boost::shared_ptr<KTBundle>) >::signal BundleSignal;
            //typedef KTSignal< void (const KTWaterfallCandidateData*) >::signal CandidateSignal;

        public:
            KTSimpleClustering();
            virtual ~KTSimpleClustering();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetMaxFrequencySeparation() const;
            void SetMaxFrequencySeparation(Double_t freqSep);

            Double_t GetMaxTimeSeparation() const;
            void SetMaxTimeSeparation(Double_t timeSep);

            UInt_t GetMaxFrequencySeparationBins() const;
            void SetMaxFrequencySeparationBins(UInt_t bins);

            UInt_t GetMaxTimeSeparationBins() const;
            void SetMaxTimeSeparationBins(UInt_t bins);

            const std::string& GetDPInputDataName() const;
            void SetDPInputDataName(const std::string& name);

            const std::string& GetFSInputDataName() const;
            void SetFSInputDataName(const std::string& name);

           const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        private:
            Double_t fMaxFreqSep;
            Double_t fMaxTimeSep;
            UInt_t fMaxFreqSepBins;
            UInt_t fMaxTimeSepBins;
            Bool_t fCalculateMaxFreqSepBins;
            Bool_t fCalculateMaxTimeSepBins;

            std::string fDPInputDataName;
            std::string fFSInputDataName;
            std::string fOutputDataName;

        public:
            /// Add points from dpData to the active clusters and create candidates
            BundleList* FindClusters(const KTDiscriminatedPoints1DData* dpData, boost::shared_ptr<KTData> fsData);
            /// Add points from dpData to the active clusters and create candidates
            //BundleList* FindClusters(const KTDiscriminatedPoints1DData* dpData, boost::shared_ptr<KTData> fsData);
            /// Add points from dpData to the active clusters and create candidates
            //BundleList* FindClusters(const KTDiscriminatedPoints1DData* dpData, boost::shared_ptr<KTData> corrData);

            /// Add points from dpData to the active clusters
            ClusterList* AddPointsToClusters(const KTDiscriminatedPoints1DData* dpData, boost::shared_ptr<KTData> data);

            /// Add points from a set of points to the active clusters
            ClusterList* AddPointsToClusters(const SetOfDiscriminatedPoints& points, UInt_t component, boost::shared_ptr<KTData> data);

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
            BundleSignal fOneSliceBundleSignal;
            BundleSignal fClusteredBundleSignal;
            //CandidateSignal fWaterfallCandidateSignal;


             //***************
             // Slots
             //***************

         public:
            // QueueBundle from KTBundleQueueProcessorTemplate
            // QueueBundles from KTBundleQueueProcessorTemplate

         private:
            /// non-queueing bundle processing
            void ProcessOneSliceBundle(boost::shared_ptr<KTBundle> bundle);

         private:
            void RunBundleLoop(BundleList* bundles);

    };


    inline Double_t KTSimpleClustering::GetMaxFrequencySeparation() const
    {
        return fMaxFreqSep;
    }

    inline void KTSimpleClustering::SetMaxFrequencySeparation(Double_t freqSep)
    {
        fMaxFreqSep = freqSep;
        fCalculateMaxFreqSepBins = true;
        return;
    }

    inline Double_t KTSimpleClustering::GetMaxTimeSeparation() const
    {
        return fMaxTimeSep;
    }

    inline void KTSimpleClustering::SetMaxTimeSeparation(Double_t timeSep)
    {
        fMaxTimeSep = timeSep;
        fCalculateMaxTimeSepBins = true;
        return;
    }

    inline UInt_t KTSimpleClustering::GetMaxFrequencySeparationBins() const
    {
        return fMaxFreqSepBins;
    }

    inline void KTSimpleClustering::SetMaxFrequencySeparationBins(UInt_t bins)
    {
        fMaxFreqSepBins = bins;
        fCalculateMaxFreqSepBins = false;
        return;
    }

    inline UInt_t KTSimpleClustering::GetMaxTimeSeparationBins() const
    {
        return fMaxTimeSepBins;
    }

    inline void KTSimpleClustering::SetMaxTimeSeparationBins(UInt_t bins)
    {
        fMaxTimeSepBins = bins;
        fCalculateMaxTimeSepBins = false;
        return;
    }

    inline const std::string& KTSimpleClustering::GetDPInputDataName() const
    {
        return fDPInputDataName;
    }

    inline void KTSimpleClustering::SetDPInputDataName(const std::string& name)
    {
        fDPInputDataName = name;
        return;
    }

    inline const std::string& KTSimpleClustering::GetFSInputDataName() const
    {
        return fFSInputDataName;
    }

    inline void KTSimpleClustering::SetFSInputDataName(const std::string& name)
    {
        fFSInputDataName = name;
        return;
    }

    inline const std::string& KTSimpleClustering::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTSimpleClustering::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

    inline UInt_t KTSimpleClustering::GetTimeBin() const
    {
        return fTimeBin;
    }

    inline Double_t KTSimpleClustering::GetTimeBinWidth() const
    {
        return fTimeBinWidth;
    }

    inline void KTSimpleClustering::SetTimeBinWidth(Double_t bw)
    {
        fTimeBinWidth = bw;
        if (fCalculateMaxTimeSepBins)
            fMaxTimeSepBins = KTMath::Nint(fMaxTimeSep / fTimeBinWidth);
        return;
    }

    inline Double_t KTSimpleClustering::GetFrequencyBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline void KTSimpleClustering::SetFrequencyBinWidth(Double_t bw)
    {
        fFreqBinWidth = bw;
        if (fCalculateMaxFreqSepBins)
            fMaxFreqSepBins = KTMath::Nint(fMaxFreqSep / fFreqBinWidth);
        return;
    }




    /*
    class KTSlidingWindowFSData;
    class KTPStoreNode;
    class KTSlidingWindowFFT;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTSimpleClustering : public KTProcessor
    {
        private:
            typedef std::list< std::multimap< Int_t, Int_t >* > epbList;

        public:
            KTSimpleClustering();
            virtual ~KTSimpleClustering();

            Bool_t Configure(const KTPStoreNode* node);

            void ProcessSlidingWindowFFT(KTSlidingWindowFSData* swFSData);
            void ProcessFrequencySpectrum(UInt_t psNum, KTFrequencySpectrumPolar* powerSpectrum);

            void SetBundlePeakBinsList(epbList* bundlePeakBinsList); /// does NOT take ownership of bundlePeakBinsList
            void SetBinCuts(KTMaskedArray< KTFrequencySpectrumPolar::array_type, complexpolar<Double_t> >* binCuts); /// takes ownership of binCuts
            void SetMinimumGroupSize(UInt_t size);

        private:
            epbList* fBundlePeakBins;
            Double_t fThresholdMult;

            KTMaskedArray< KTFrequencySpectrumPolar::array_type, complexpolar<Double_t> >* fBinCuts;

            UInt_t fMinimumGroupSize;

            Int_t fGroupBinsMarginLow;
            Int_t fGroupBinsMarginHigh;
            Int_t fGroupBinsMarginSameTime;

            UInt_t fFirstBinToUse;

            Bool_t fDrawFlag;

    };

    inline void KTSimpleClustering::SetBundlePeakBinsList(epbList* list)
    {
        fBundlePeakBins = list;
        return;
    }

    inline void KTSimpleClustering::SetBinCuts(KTMaskedArray< KTFrequencySpectrumPolar::array_type, complexpolar<Double_t> >* binCuts)
    {
        delete fBinCuts;
        fBinCuts = binCuts;
        return;
    }

    inline void KTSimpleClustering::SetMinimumGroupSize(UInt_t size)
    {
        fMinimumGroupSize = size;
        return;
    }
    */

} /* namespace Katydid */
#endif /* KTSIMPLECLUSTERING_HH_ */
