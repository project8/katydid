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

    class KTSimpleClustering : public KTBundleQueueProcessorTemplate< KTSimpleClustering >
    {
        public:
            typedef KTDiscriminatedPoints1DData::SetOfPoints SetOfDiscriminatedPoints;

            struct ClusterPoint
            {
                 UInt_t fTimeBin;
                 UInt_t fFreqBin;
                 Double_t fAmplitude;
            };
            //typedef std::deque< ClusterPoint > Cluster;
            struct Cluster
            {
                std::deque< ClusterPoint > fPoints; // every point in the cluster
                std::deque< std::pair< UInt_t, UInt_t > > fFreqRanges; // first and last frequency bins for each time bin

                UInt_t EndMinFreqPoint() {return fFreqRanges.back().first;}
                UInt_t EndMaxFreqPoint() {return fFreqRanges.back().second;}
            };

            typedef std::list< Cluster > ActiveClusters;

            struct FreqBinCluster
            {
                SetOfDiscriminatedPoints fPoints;
                UInt_t fFirstPoint;
                UInt_t fLastPoint;
                Bool_t fAddedToActiveCluster;
                ActiveClusters::iterator fActiveCluster;
                UInt_t fACNumber;
            };
            typedef std::list< FreqBinCluster > FreqBinClusters;

            typedef std::list< boost::shared_ptr<KTBundle> > NewBundleList;

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

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        private:
            Double_t fMaxFreqSep;
            Double_t fMaxTimeSep;
            UInt_t fMaxFreqSepBins;
            UInt_t fMaxTimeSepBins;
            Bool_t fCalculateMaxFreqSepBins;
            Bool_t fCalculateMaxTimeSepBins;

            std::string fInputDataName;
            std::string fOutputDataName;

        public:
            NewBundleList* AddPointsToClusters(const KTDiscriminatedPoints1DData* dpData);

            NewBundleList* AddPointsToClusters(const SetOfDiscriminatedPoints& points, UInt_t component);

            NewBundleList* CompleteAllClusters(UInt_t component);
            //NewBundleList* CompleteInactiveClusters(UInt_t component);

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

            std::vector< ActiveClusters > fActiveClusters;


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
            void RunBundleLoop(NewBundleList* bundles);

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

    inline const std::string& KTSimpleClustering::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTSimpleClustering::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
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
            void ProcessFrequencySpectrum(UInt_t psNum, KTFrequencySpectrum* powerSpectrum);

            void SetBundlePeakBinsList(epbList* bundlePeakBinsList); /// does NOT take ownership of bundlePeakBinsList
            void SetBinCuts(KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* binCuts); /// takes ownership of binCuts
            void SetMinimumGroupSize(UInt_t size);

        private:
            epbList* fBundlePeakBins;
            Double_t fThresholdMult;

            KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* fBinCuts;

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

    inline void KTSimpleClustering::SetBinCuts(KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* binCuts)
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
