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

#include "KTCorrelationData.hh"
#include "KTData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTSignal.hh"
#include "KTWignerVilleData.hh"

#include <boost/shared_ptr.hpp>

#include <list>
//#include <map>
#include <set>
#include <utility>

namespace Katydid
{
    KTLOGGER(sclog, "katydid.analysis");

    //class KTCorrelationData;
    class KTFrequencySpectrum;
    //class KTFrequencySpectrumDataFFTW;
    //class KTFrequencySpectrumDataFFTWCore;
    //class KTFrequencySpectrumDataPolar;
    //class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumPolar;
    class KTSliceHeader;
    //class KTWignerVilleData;

    class KTMultiSliceClustering : public KTDataQueueProcessorTemplate< KTMultiSliceClustering >
    {
        public:
            typedef KTDiscriminatedPoints1DData::SetOfPoints SetOfDiscriminatedPoints;

            struct ClusterPoint
            {
                 UInt_t fTimeBin;
                 UInt_t fFreqBin;
                 Double_t fAmplitude;
                 boost::shared_ptr< KTSliceHeader > fHeaderPtr;
                 boost::shared_ptr< KTFrequencySpectrumPolar > fSpectrumPtr;
            };

            struct PointCompare
            {
                bool operator() (const ClusterPoint& lhs, const ClusterPoint& rhs)
                {
                    return lhs.fTimeBin < rhs.fTimeBin || (lhs.fTimeBin == rhs.fTimeBin && lhs.fFreqBin < rhs.fFreqBin);
                }
            };

            typedef std::set< ClusterPoint, PointCompare > SetOfPoints;

            struct Cluster
            {
                SetOfPoints fPoints; // every point in the cluster

                UInt_t FirstTimeBin() const {return fPoints.begin()->fTimeBin;}
                UInt_t LastTimeBin() const {return fPoints.rbegin()->fTimeBin;}

                UInt_t fEndMinFreqPoint;
                UInt_t fEndMaxFreqPoint;

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


            typedef std::list< boost::shared_ptr<KTData> > DataList;

        public:
            KTMultiSliceClustering(const std::string& name = "multi-slice-processing");
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

            UInt_t GetMinTimeBins() const;
            void SetMinTimeBins(UInt_t bins);

        private:
            Double_t fMaxFreqSep;
            Double_t fMaxTimeSep;
            UInt_t fMaxFreqSepBins;
            UInt_t fMaxTimeSepBins;
            Bool_t fCalculateMaxFreqSepBins;
            Bool_t fCalculateMaxTimeSepBins;
            UInt_t fMinTimeBins;

        public:
            /// Add points from dpData to the active clusters and create candidates
            DataList* FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataPolar& fsData, const KTSliceHeader& header);
            /// Add points from dpData to the active clusters and create candidates
            DataList* FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataFFTW& fsData, const KTSliceHeader& header);
            /// Add points from dpData to the active clusters and create candidates
            DataList* FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTCorrelationData& corrData, const KTSliceHeader& header);
            /// Add points from dpData to the active clusters and create candidates
            DataList* FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTWignerVilleData& wvData, const KTSliceHeader& header);

            /// Complete all remaining active clusters
            DataList* CompleteAllClusters();

            /// Add points from dpData to the active clusters
            ClusterList* AddPointsToClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataPolarCore& spectrumData, boost::shared_ptr< KTSliceHeader >& header);
            /// Add points from dpData to the active clusters
            ClusterList* AddPointsToClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataFFTWCore& spectrumData, boost::shared_ptr< KTSliceHeader >& header);

            /// Add points from a set of points to the active clusters
            ClusterList* AddPointsToClusters(const SetOfDiscriminatedPoints& points, boost::shared_ptr<KTFrequencySpectrumPolar>& spectrumPtr, UInt_t component, boost::shared_ptr< KTSliceHeader >& header);

            void Reset();
            UInt_t GetTimeBin() const;

            Double_t GetTimeBinWidth() const;
            void SetTimeBinWidth(Double_t bw);

            Double_t GetFrequencyBinWidth() const;
            void SetFrequencyBinWidth(Double_t bw);

        private:
            boost::shared_ptr<KTData> CreateDataFromCluster(const Cluster& cluster);

            UInt_t fTimeBin;
            Double_t fTimeBinWidth;
            Double_t fFreqBinWidth;

            std::vector< ClusterList > fActiveClusters;


            //***************
            // Signals
            //***************

         private:
            KTSignalData fOneSliceDataSignal;
            KTSignalData fClusteredDataSignal;


             //***************
             // Slots
             //***************

         public:
            // These are the actual slot functions
            void QueueFSPolarData(boost::shared_ptr< KTData >& data);
            void QueueFSFFTWData(boost::shared_ptr< KTData >& data);
            void QueueCorrelationData(boost::shared_ptr< KTData >& data);
            void QueueWVData(boost::shared_ptr< KTData >& data);
/*
         private:
            // These slot functions differ slightly from the KTSlotData implementation, so these custom functions are used
            void ProcessOneSliceFSPolarData(boost::shared_ptr<KTData> data);
            void ProcessOneSliceFSFFTWData(boost::shared_ptr<KTData> data);
            void ProcessOneSliceCorrelationData(boost::shared_ptr<KTData> data);
            void ProcessOneSliceWVData(boost::shared_ptr<KTData> data);
*/
         private:
            template< class XDataType >
            void ProcessOneSliceData(boost::shared_ptr< KTData > data);

            void RunDataLoop(DataList* dataList);

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

    inline UInt_t KTMultiSliceClustering::GetMinTimeBins() const
    {
        return fMinTimeBins;
    }

    inline void KTMultiSliceClustering::SetMinTimeBins(UInt_t bins)
    {
        fMinTimeBins = bins;
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

    inline void KTMultiSliceClustering::QueueFSPolarData(boost::shared_ptr< KTData >& data)
    {
        return DoQueueData(data, &KTMultiSliceClustering::ProcessOneSliceData< KTFrequencySpectrumDataPolar >);
    }

    inline void KTMultiSliceClustering::QueueFSFFTWData(boost::shared_ptr< KTData >& data)
    {
        return DoQueueData(data, &KTMultiSliceClustering::ProcessOneSliceData< KTFrequencySpectrumDataFFTW >);
    }

    inline void KTMultiSliceClustering::QueueCorrelationData(boost::shared_ptr< KTData >& data)
    {
        return DoQueueData(data, &KTMultiSliceClustering::ProcessOneSliceData< KTCorrelationData >);
    }

    inline void KTMultiSliceClustering::QueueWVData(boost::shared_ptr< KTData >& data)
    {
        return DoQueueData(data, &KTMultiSliceClustering::ProcessOneSliceData< KTWignerVilleData >);
    }
/*
    inline void KTMultiSliceClustering::ProcessOneSliceFSPolarData(shared_ptr<KTData> data)
    {
        ProcessOneSliceData< KTFrequencySpectrumDataPolar >(data);
        return;
    }

    inline void KTMultiSliceClustering::ProcessOneSliceFSFFTWData(shared_ptr<KTData> data)
    {
        ProcessOneSliceData< KTFrequencySpectrumDataFFTW >(data);
        return;
    }

    inline void KTMultiSliceClustering::ProcessOneSliceCorrelationData(shared_ptr<KTData> data)
    {
        ProcessOneSliceData< KTCorrelationData >(data);
        return;
    }

    inline void KTMultiSliceClustering::ProcessOneSliceWVData(shared_ptr<KTData> data)
    {
        ProcessOneSliceData< KTWignerVilleData >(data);
        return;
    }
*/
    template< class XDataType >
    void KTMultiSliceClustering::ProcessOneSliceData(boost::shared_ptr<KTData> data)
    {
        if (! data->Has< KTDiscriminatedPoints1DData >())
        {
            KTWARN(sclog, "No discriminated-points data was present");
            return;
        }
        if (! data->Has< XDataType >())
        {
            KTWARN(sclog, "No frequency spectrum (polar) data was present");
            return;
        }
        // signal for any continued use of the input data
        fOneSliceDataSignal(data);
        DataList* clusteredData = FindClusters(data->Of< KTDiscriminatedPoints1DData >(), data->Of< XDataType >(), data->Of< KTSliceHeader >());

        if (data->fLastData)
        {
            KTINFO(sclog, "Last input data processed; Cleaning up remaining active clusters");
            KTMultiSliceClustering::DataList* lastData = CompleteAllClusters();
            clusteredData->splice(clusteredData->end(), *lastData);
            delete lastData;
        }

        if (clusteredData != NULL)
        {
            RunDataLoop(clusteredData);
        }
        return;
    }



} /* namespace Katydid */
#endif /* KTMULTISLICECLUSTERING_HH_ */
