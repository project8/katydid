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

#include <list>
//#include <map>
#include <set>
#include <utility>

namespace Katydid
{
    using namespace Nymph;
    KTLOGGER(sclog, "KTMultiSliceClustering");

    //class KTCorrelationData;
    class KTFrequencySpectrum;
    //class KTFrequencySpectrumDataFFTW;
    //class KTFrequencySpectrumDataFFTWCore;
    //class KTFrequencySpectrumDataPolar;
    //class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumPolar;
    class KTSliceHeader;
    //class KTWignerVilleData;

    /*!
     @class KTMultiSliceClustering
     @author N. S. Oblath

     @brief Clustering algorithm for sequential time slices

     @details
     For separations used in clustering: if the bin separation required for clustering is 1, neighboring bins are allowed.
     If the bin separation required for clustering is 2, high-power bins with 1 low-power bin in between are allowed, etc.

     Configuration name: "multi-slice-clustering"

     Available configuration values:
     - "max-frequency-sep": double -- Maximum frequency separation for clustering in a single time slice (Hz)
     - "max-time-sep": double -- Maximum time separation for clustering between time slices (s) -- NOT CURRENTLY USED
     - "max-frequency-sep-bins": -- Maximum frequency separation for clustering in a single time slice, specified in # of bins
     - "max-time-sep-bins": Maximum time separation for clustering between time slices, specified in # of bins -- NOT CURRENTLY USED
     - "min-time-bins": Minimum number of time slices needed to be counted as a cluster
     - "n-framing-time-bins": Number of time bins to include on either side of a cluster
     - "n-framing-freq-bins": Number of frequency bins to include on the top and bottom of a cluster

     Slots:
     - "fs-polar": void (KTDataPtr) -- Processes a data object for clustering based on polar FS data; Requires KTFrequencySpectrumDataPolar; May create new data objects with KTWaterfallCandidateData
     - "fs-fftw": void (KTDataPtr) -- Processes a data object for clustering based on fftw FS data; Requires KTFrequencySpectrumDataFFTW; May create new data objects with KTWaterfallCandidateData
     - "corr": void (KTDataPtr) -- Processes a data object for clustering based on correlation data; Requires KTCorrelationData; May create new data objects with KTWaterfallCandidateData
     - "wv": void (KTDataPtr) -- Processes a data object for clustering based on wigner-ville data; Requires KTWignerVilleData; May create new data objects with KTWaterfallCandidateData
     - "queue-fs-polar": void (KTDataPtr) -- Queues a data object for clustering based on polar FS data; Requires KTFrequencySpectrumDataPolar; May create new data objects with KTWaterfallCandidateData
     - "queue-fs-fftw": void (KTDataPtr) -- Queues a data object for clustering based on fftw FS data; Requires KTFrequencySpectrumDataFFTW; May create new data objects with KTWaterfallCandidateData
     - "queue-corr": void (KTDataPtr) -- Queues a data object for clustering based on correlation data; Requires KTCorrelationData; May create new data objects with KTWaterfallCandidateData
     - "queue-wv": void (KTDataPtr) -- Queues a data object for clustering based on wigner-ville data; Requires KTWignerVilleData; May create new data objects with KTWaterfallCandidateData
     - "complete-remaining-clusters": void () -- Completes any remaining partial clusters; May create new data objects with KTWaterfallCandidateData

     Signals:
     - "one-slice": void (KTDataPtr) -- Emitted upon receipt of a one-slice data object, without modification
     - "cluster": void (KTDataPtr) -- Emitted upon creation of a cluster; guarantees KTWaterfallCandidateData
     - "queue-done": void () -- Emitted when queue is emptied (inherited from KTDataQueueProcessorTemplate)
    */
    class KTMultiSliceClustering : public KTDataQueueProcessorTemplate< KTMultiSliceClustering >
    {
        public:
            typedef KTDiscriminatedPoints1DData::SetOfPoints SetOfDiscriminatedPoints;

            struct ClusterPoint
            {
                 unsigned fTimeBin;
                 unsigned fFreqBin;
                 double fAmplitude;
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
            typedef std::list< boost::shared_ptr< KTFrequencySpectrumPolar > > ListOfSpectra;

            struct Cluster
            {
                SetOfPoints fPoints; // every point in the cluster

                unsigned FirstTimeBin() const {return fPoints.begin()->fTimeBin;}
                unsigned LastTimeBin() const {return fPoints.rbegin()->fTimeBin;}

                unsigned fEndMinFreqPoint;
                unsigned fEndMaxFreqPoint;

                unsigned fTimeBinSkipCounter;

                unsigned fDataComponent;

                ListOfSpectra fPreClusterSpectra;
                ListOfSpectra fPostClusterSpectra;
            };

            typedef std::list< Cluster > ClusterList;

            struct FreqBinCluster
            {
                SetOfDiscriminatedPoints fPoints;
                unsigned fFirstPoint;
                unsigned fLastPoint;
                bool fAddedToActiveCluster;
                ClusterList::iterator fActiveCluster;
                unsigned fACNumber;
            };
            typedef std::list< FreqBinCluster > FreqBinClusters;


            typedef std::list< KTDataPtr > DataList;

        public:
            KTMultiSliceClustering(const std::string& name = "multi-slice-clustering");
            virtual ~KTMultiSliceClustering();

            bool ConfigureSubClass(const KTParamNode* node);

            double GetMaxFrequencySeparation() const;
            void SetMaxFrequencySeparation(double freqSep);

            double GetMaxTimeSeparation() const;
            void SetMaxTimeSeparation(double timeSep);

            unsigned GetMaxFrequencySeparationBins() const;
            void SetMaxFrequencySeparationBins(unsigned bins);

            unsigned GetMaxTimeSeparationBins() const;
            void SetMaxTimeSeparationBins(unsigned bins);

            unsigned GetMinTimeBins() const;
            void SetMinTimeBins(unsigned bins);

            unsigned GetNFramingTimeBins() const;
            void SetNFramingTimeBins(unsigned bins);

            unsigned GetNFramingFreqBins() const;
            void SetNFramingFreqBins(unsigned bins);

        private:
            double fMaxFreqSep;
            double fMaxTimeSep;
            unsigned fMaxFreqSepBins;
            unsigned fMaxTimeSepBins;
            bool fCalculateMaxFreqSepBins;
            bool fCalculateMaxTimeSepBins;
            unsigned fMinTimeBins;
            unsigned fNFramingTimeBins;
            unsigned fNFramingFreqBins;

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
            ClusterList* AddPointsToClusters(const SetOfDiscriminatedPoints& points, boost::shared_ptr<KTFrequencySpectrumPolar>& spectrumPtr, unsigned component, boost::shared_ptr< KTSliceHeader >& header);

            void Reset();
            unsigned GetTimeBin() const;

            double GetTimeBinWidth() const;
            void SetTimeBinWidth(double bw);

            double GetFrequencyBinWidth() const;
            void SetFrequencyBinWidth(double bw);

            unsigned GetDataCount() const;

        private:
            KTDataPtr CreateDataFromCluster(const Cluster& cluster);

            unsigned fTimeBin;
            double fTimeBinWidth;
            double fFreqBinWidth;

            unsigned fDataCount;

            // the vectors are over the components, so there is one list per component
            std::vector< ClusterList > fActiveClusters;
            std::vector< ClusterList > fAlmostCompleteClusters; // these still need their post-cluster spectra

            std::vector< ListOfSpectra > fPreClusterSpectra;

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
            // Queueing slot functions
            void QueueFSPolarData(KTDataPtr& data);
            void QueueFSFFTWData(KTDataPtr& data);
            void QueueCorrelationData(KTDataPtr& data);
            void QueueWVData(KTDataPtr& data);

         private:
            // Non-queueing slot functions
            // These slot functions differ slightly from the KTSlotData implementation, so these custom functions are used
            void ProcessOneSliceFSPolarData(KTDataPtr data);
            void ProcessOneSliceFSFFTWData(KTDataPtr data);
            void ProcessOneSliceCorrelationData(KTDataPtr data);
            void ProcessOneSliceWVData(KTDataPtr data);

            void CompleteRemainingClusters();

         private:
            template< class XDataType >
            void ProcessOneSliceData(KTDataPtr data);

            void RunDataLoop(DataList* dataList);

    };


    inline double KTMultiSliceClustering::GetMaxFrequencySeparation() const
    {
        return fMaxFreqSep;
    }

    inline void KTMultiSliceClustering::SetMaxFrequencySeparation(double freqSep)
    {
        fMaxFreqSep = freqSep;
        fCalculateMaxFreqSepBins = true;
        return;
    }

    inline double KTMultiSliceClustering::GetMaxTimeSeparation() const
    {
        return fMaxTimeSep;
    }

    inline void KTMultiSliceClustering::SetMaxTimeSeparation(double timeSep)
    {
        /* variable time seaparation is not currently in use
        fMaxTimeSep = timeSep;
        fCalculateMaxTimeSepBins = true;
        */
        return;
    }

    inline unsigned KTMultiSliceClustering::GetMaxFrequencySeparationBins() const
    {
        return fMaxFreqSepBins;
    }

    inline void KTMultiSliceClustering::SetMaxFrequencySeparationBins(unsigned bins)
    {
        fMaxFreqSepBins = bins;
        fCalculateMaxFreqSepBins = false;
        return;
    }

    inline unsigned KTMultiSliceClustering::GetMaxTimeSeparationBins() const
    {
        return fMaxTimeSepBins;
    }

    inline void KTMultiSliceClustering::SetMaxTimeSeparationBins(unsigned bins)
    {
        /* variable time separation is not currently in use
        fMaxTimeSepBins = bins;
        fCalculateMaxTimeSepBins = false;
        */
        return;
    }

    inline unsigned KTMultiSliceClustering::GetMinTimeBins() const
    {
        return fMinTimeBins;
    }

    inline void KTMultiSliceClustering::SetMinTimeBins(unsigned bins)
    {
        fMinTimeBins = bins;
        return;
    }

    inline unsigned KTMultiSliceClustering::GetNFramingTimeBins() const
    {
        return fNFramingTimeBins;
    }

    inline void KTMultiSliceClustering::SetNFramingTimeBins(unsigned bins)
    {
        fNFramingTimeBins = bins;
        return;
    }

    inline unsigned KTMultiSliceClustering::GetNFramingFreqBins() const
    {
        return fNFramingFreqBins;
    }

    inline void KTMultiSliceClustering::SetNFramingFreqBins(unsigned bins)
    {
        fNFramingFreqBins = bins;
        return;
    }

    inline unsigned KTMultiSliceClustering::GetTimeBin() const
    {
        return fTimeBin;
    }

    inline double KTMultiSliceClustering::GetTimeBinWidth() const
    {
        return fTimeBinWidth;
    }

    inline void KTMultiSliceClustering::SetTimeBinWidth(double bw)
    {
        fTimeBinWidth = bw;
        if (fCalculateMaxTimeSepBins)
            fMaxTimeSepBins = KTMath::Nint(fMaxTimeSep / fTimeBinWidth);
        return;
    }

    inline double KTMultiSliceClustering::GetFrequencyBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline void KTMultiSliceClustering::SetFrequencyBinWidth(double bw)
    {
        fFreqBinWidth = bw;
        if (fCalculateMaxFreqSepBins)
            fMaxFreqSepBins = KTMath::Nint(fMaxFreqSep / fFreqBinWidth);
        return;
    }

    inline unsigned KTMultiSliceClustering::GetDataCount() const
    {
        return fDataCount;
    }

    inline void KTMultiSliceClustering::QueueFSPolarData(KTDataPtr& data)
    {
        return DoQueueData(data, &KTMultiSliceClustering::ProcessOneSliceData< KTFrequencySpectrumDataPolar >);
    }

    inline void KTMultiSliceClustering::QueueFSFFTWData(KTDataPtr& data)
    {
        return DoQueueData(data, &KTMultiSliceClustering::ProcessOneSliceData< KTFrequencySpectrumDataFFTW >);
    }

    inline void KTMultiSliceClustering::QueueCorrelationData(KTDataPtr& data)
    {
        return DoQueueData(data, &KTMultiSliceClustering::ProcessOneSliceData< KTCorrelationData >);
    }

    inline void KTMultiSliceClustering::QueueWVData(KTDataPtr& data)
    {
        return DoQueueData(data, &KTMultiSliceClustering::ProcessOneSliceData< KTWignerVilleData >);
    }

    inline void KTMultiSliceClustering::ProcessOneSliceFSPolarData(KTDataPtr data)
    {
        ProcessOneSliceData< KTFrequencySpectrumDataPolar >(data);
        return;
    }

    inline void KTMultiSliceClustering::ProcessOneSliceFSFFTWData(KTDataPtr data)
    {
        ProcessOneSliceData< KTFrequencySpectrumDataFFTW >(data);
        return;
    }

    inline void KTMultiSliceClustering::ProcessOneSliceCorrelationData(KTDataPtr data)
    {
        ProcessOneSliceData< KTCorrelationData >(data);
        return;
    }

    inline void KTMultiSliceClustering::ProcessOneSliceWVData(KTDataPtr data)
    {
        ProcessOneSliceData< KTWignerVilleData >(data);
        return;
    }

    template< class XDataType >
    void KTMultiSliceClustering::ProcessOneSliceData(KTDataPtr data)
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

        if (data->GetLastData())
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
