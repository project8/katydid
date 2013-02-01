/**
 @file KTSimpleClustering.hh
 @brief Contains KTSimpleClustering
 @details Simple cluster-finding algorithm that works by looking for lines of high-peaked bins increasing in frequency
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTSIMPLECLUSTERING_HH_
#define KTSIMPLECLUSTERING_HH_

#include "KTProcessor.hh"

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
    class KTEvent;

    class KTSimpleClustering : public KTProcessor
    {
        public:
            typedef KTDiscriminatedPoints1DData::SetOfPoints SetOfDiscriminatedPoints;

            typedef std::list< boost::shared_ptr<KTEvent> > NewEventList;

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
            };

            typedef std::list< Cluster > ActiveClusters;

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
            NewEventList* AddPointsToClusters(const KTDiscriminatedPoints1DData* dpData);

            NewEventList*  AddPointsToClusters(const SetOfDiscriminatedPoints& points, UInt_t component);

            NewEventList* CompleteAllClusters(UInt_t component);
            NewEventList* CompleteInactiveClusters(UInt_t component);

            void Reset();
            UInt_t GetTimeBin() const;

            Double_t GetTimeBinWidth() const;
            void SetTimeBinWidth(Double_t bw);

            Double_t GetFrequencyBinWidth() const;
            void SetFrequencyBinWidth(Double_t bw);

        private:
            boost::shared_ptr<KTEvent> CreateEventFromCluster(const Cluster& cluster);

            UInt_t fTimeBin;
            Double_t fTimeBinWidth;
            Double_t fFreqBinWidth;

            std::vector< ActiveClusters > fActiveClusters;
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

            void SetEventPeakBinsList(epbList* eventPeakBinsList); /// does NOT take ownership of eventPeakBinsList
            void SetBinCuts(KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* binCuts); /// takes ownership of binCuts
            void SetMinimumGroupSize(UInt_t size);

        private:
            epbList* fEventPeakBins;
            Double_t fThresholdMult;

            KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* fBinCuts;

            UInt_t fMinimumGroupSize;

            Int_t fGroupBinsMarginLow;
            Int_t fGroupBinsMarginHigh;
            Int_t fGroupBinsMarginSameTime;

            UInt_t fFirstBinToUse;

            Bool_t fDrawFlag;

    };

    inline void KTSimpleClustering::SetEventPeakBinsList(epbList* list)
    {
        fEventPeakBins = list;
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
