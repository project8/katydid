/*
 * KTSimpleCusteringProcessor.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTMultiSliceClustering.hh"

#include "KTCorrelationData.hh"
#include "KTNOFactory.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTSliceHeader.hh"
#include "KTTimeFrequencyPolar.hh"
#include "KTWaterfallCandidateData.hh"
#include "KTWignerVilleData.hh"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <set>
#include <sstream>

using boost::weak_ptr;

using std::list;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace Katydid
{
    KT_REGISTER_PROCESSOR(KTMultiSliceClustering, "multi-slice-clustering");

    KTMultiSliceClustering::KTMultiSliceClustering(const std::string& name) :
            Nymph::KTDataQueueProcessorTemplate< KTMultiSliceClustering >(name),
            fMaxFreqSep(1.),
            fMaxTimeSep(1.),
            fMaxFreqSepBins(1),
            fMaxTimeSepBins(1),
            fCalculateMaxFreqSepBins(false),
            fCalculateMaxTimeSepBins(false),
            fMinTimeBins(2),
            fNFramingTimeBins(1),
            fNFramingFreqBins(1),
            fTimeBin(0),
            fTimeBinWidth(1.),
            fFreqBinWidth(1.),
            fDataCount(0),
            fActiveClusters(),
            fAlmostCompleteClusters(),
            fPreClusterSpectra(),
            fOneSliceDataSignal("one-slice", this),
            fClusteredDataSignal("cluster", this)
    {
        RegisterSlot("complete-remaining-clusters", this, &KTMultiSliceClustering::CompleteRemainingClusters);

        RegisterSlot("fs-polar", this, &KTMultiSliceClustering::ProcessOneSliceFSPolarData);
        RegisterSlot("fs-fftw", this, &KTMultiSliceClustering::ProcessOneSliceFSFFTWData);
        RegisterSlot("corr", this, &KTMultiSliceClustering::ProcessOneSliceCorrelationData);
        RegisterSlot("wv", this, &KTMultiSliceClustering::ProcessOneSliceWVData);

        RegisterSlot("queue-fs-polar", this, &KTMultiSliceClustering::QueueFSPolarData);
        RegisterSlot("queue-fs-fftw", this, &KTMultiSliceClustering::QueueFSFFTWData);
        RegisterSlot("queue-corr", this, &KTMultiSliceClustering::QueueCorrelationData);
        RegisterSlot("queue-wv", this, &KTMultiSliceClustering::QueueWVData);
    }

    KTMultiSliceClustering::~KTMultiSliceClustering()
    {
    }

    bool KTMultiSliceClustering::ConfigureSubClass(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("max-frequency-sep"))
        {
            SetMaxFrequencySeparation(node->get_value< double >("max-frequency-sep"));
        }
        if (node->has("max-time-sep"))
        {
            SetMaxTimeSeparation(node->get_value< double >("max-time-sep"));
        }

        if (node->has("max-frequency-sep-bins"))
        {
            SetMaxFrequencySeparationBins(node->get_value< unsigned >("max-frequency-sep-bins"));
        }
        if (node->has("max-time-sep-bins"))
        {
            SetMaxTimeSeparationBins(node->get_value< unsigned >("max-time-sep-bins"));
        }

        SetMinTimeBins(node->get_value< unsigned >("min-time-bins", fMinTimeBins));

        SetNFramingTimeBins(node->get_value< unsigned >("n-framing-time-bins", fNFramingTimeBins));
        SetNFramingFreqBins(node->get_value< unsigned >("n-framing-freq-bins", fNFramingFreqBins));

        return true;
    }

    KTMultiSliceClustering::DataList* KTMultiSliceClustering::FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataPolar& fsData, const KTSliceHeader& header)
    {
        // Make a copy of the slice header
        boost::shared_ptr< KTSliceHeader > headerPtr(new KTSliceHeader());
        headerPtr->SetIsCopyDisabled(true);
        (*(headerPtr.get())) = header;

        ClusterList* completedClusters = AddPointsToClusters(dpData, fsData, headerPtr);

        DataList* newDataList = new DataList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newDataList->push_back(CreateDataFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }
        delete completedClusters;

        return newDataList;
    }

    KTMultiSliceClustering::DataList* KTMultiSliceClustering::FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataFFTW& fsData, const KTSliceHeader& header)
    {
        // Make a copy of the slice header
        boost::shared_ptr< KTSliceHeader > headerPtr(new KTSliceHeader());
        headerPtr->SetIsCopyDisabled(true);
        (*(headerPtr.get())) = header;

        ClusterList* completedClusters = AddPointsToClusters(dpData, fsData, headerPtr);

        DataList* newDataList = new DataList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newDataList->push_back(CreateDataFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }
        delete completedClusters;

        return newDataList;
    }

    KTMultiSliceClustering::DataList* KTMultiSliceClustering::FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTCorrelationData& corrData, const KTSliceHeader& header)
    {
        // Make a copy of the slice header
        boost::shared_ptr< KTSliceHeader > headerPtr(new KTSliceHeader());
        headerPtr->SetIsCopyDisabled(true);
        (*(headerPtr.get())) = header;

        ClusterList* completedClusters = AddPointsToClusters(dpData, corrData, headerPtr);

        DataList* newDataList = new DataList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newDataList->push_back(CreateDataFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }
        delete completedClusters;

        return newDataList;
    }

    KTMultiSliceClustering::DataList* KTMultiSliceClustering::FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTWignerVilleData& wvData, const KTSliceHeader& header)
    {
        // Make a copy of the slice header
        boost::shared_ptr< KTSliceHeader > headerPtr(new KTSliceHeader());
        headerPtr->SetIsCopyDisabled(true);
        (*(headerPtr.get())) = header;

        ClusterList* completedClusters = AddPointsToClusters(dpData, wvData, headerPtr);

        DataList* newDataList = new DataList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newDataList->push_back(CreateDataFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }
        delete completedClusters;

        return newDataList;
    }



    KTMultiSliceClustering::DataList* KTMultiSliceClustering::CompleteAllClusters()
    {
        DataList* newDataList = new DataList();

        for (unsigned iComponent = 0; iComponent < fAlmostCompleteClusters.size(); ++iComponent)
        {
            for (ClusterList::iterator accIt = fAlmostCompleteClusters[iComponent].begin(); accIt != fAlmostCompleteClusters[iComponent].end();)
            {
                newDataList->push_back(CreateDataFromCluster(*accIt));
                accIt = fActiveClusters[iComponent].erase(accIt);
            }
            fAlmostCompleteClusters[iComponent].clear();
        }

        for (unsigned iComponent = 0; iComponent < fActiveClusters.size(); ++iComponent)
        {
            for (ClusterList::iterator acIt = fActiveClusters[iComponent].begin(); acIt != fActiveClusters[iComponent].end();)
            {
                if (acIt->LastTimeBin() - acIt->FirstTimeBin() + 1 >= fMinTimeBins)
                {
                    newDataList->push_back(CreateDataFromCluster(*acIt));
                }
                acIt = fActiveClusters[iComponent].erase(acIt);
            }
            fActiveClusters[iComponent].clear();
        }

        for (unsigned iComponent = 0; iComponent < fPreClusterSpectra.size(); ++iComponent)
        {
            fPreClusterSpectra[iComponent].clear();
        }

        return newDataList;
    }



    KTMultiSliceClustering::ClusterList* KTMultiSliceClustering::AddPointsToClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataPolarCore& spectrumData, boost::shared_ptr< KTSliceHeader >& headerPtr)
    {
        if (dpData.GetBinWidth() != fFreqBinWidth)
            SetFrequencyBinWidth(dpData.GetBinWidth());
        if (headerPtr->GetSliceLength() != fTimeBinWidth)
            SetTimeBinWidth(headerPtr->GetSliceLength());

        unsigned nComponents = dpData.GetNComponents();
        if (fActiveClusters.size() < nComponents) fActiveClusters.resize(nComponents);
        if (fAlmostCompleteClusters.size() < nComponents) fAlmostCompleteClusters.resize(nComponents);
        if (fPreClusterSpectra.size() < nComponents) fPreClusterSpectra.resize(nComponents);

        ClusterList* newClustersAC = new ClusterList();

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            // Make a copy of the frequency spectrum
            boost::shared_ptr< KTFrequencySpectrumPolar > fsPtr(new KTFrequencySpectrumPolar(*(spectrumData.GetSpectrumPolar(iComponent))));
            // Add these points to the list of clusters and return any that are complete
            ClusterList* clustersFromComponent = AddPointsToClusters(dpData.GetSetOfPoints(iComponent), fsPtr, iComponent, headerPtr);
            // Splice newly completed clusters into the full list of completed clusters
            newClustersAC->splice(newClustersAC->end(), *clustersFromComponent);
            delete clustersFromComponent;
        }

        fTimeBin++;

        return newClustersAC;
    }

    KTMultiSliceClustering::ClusterList* KTMultiSliceClustering::AddPointsToClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataFFTWCore& spectrumData, boost::shared_ptr< KTSliceHeader >& headerPtr)
    {
        if (dpData.GetBinWidth() != fFreqBinWidth)
            SetFrequencyBinWidth(dpData.GetBinWidth());
        if (headerPtr->GetSliceLength() != fTimeBinWidth)
            SetTimeBinWidth(headerPtr->GetSliceLength());

        unsigned nComponents = dpData.GetNComponents();
        if (fActiveClusters.size() < nComponents) fActiveClusters.resize(nComponents);
        if (fAlmostCompleteClusters.size() < nComponents) fAlmostCompleteClusters.resize(nComponents);
        if (fPreClusterSpectra.size() < nComponents) fPreClusterSpectra.resize(nComponents);

        ClusterList* newClustersAC = new ClusterList();

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            // Make a copy of the frequency spectrum
            boost::shared_ptr< KTFrequencySpectrumPolar > fsPtr(spectrumData.GetSpectrumFFTW(iComponent)->CreateFrequencySpectrumPolar());
            // Add these points to the list of clusters and return any that are complete
            ClusterList* clustersFromComponent = AddPointsToClusters(dpData.GetSetOfPoints(iComponent), fsPtr, iComponent, headerPtr);
            // Splice newly completed clusters into the full list of completed clusters
            newClustersAC->splice(newClustersAC->end(), *clustersFromComponent);
            delete clustersFromComponent;
        }

        ++fTimeBin;

        return newClustersAC;
    }

    KTMultiSliceClustering::ClusterList* KTMultiSliceClustering::AddPointsToClusters(const SetOfDiscriminatedPoints& points, boost::shared_ptr< KTFrequencySpectrumPolar >& spectrumPtr, unsigned component, boost::shared_ptr< KTSliceHeader >& headerPtr)
    {
        // Process a single time bin's worth of frequency bins

        FreqBinClusters freqBinClusters;

        // First cluster the frequency bins in this time bin
        if (! points.empty())
        {
            // loop over all of the points
            SetOfDiscriminatedPoints::const_iterator pIt = points.begin();
            FreqBinCluster activeFBCluster;
            activeFBCluster.fPoints.insert(*pIt);
            activeFBCluster.fFirstPoint = pIt->first;
            activeFBCluster.fLastPoint = pIt->first;
            activeFBCluster.fAddedToActiveCluster = false;
            activeFBCluster.fActiveCluster = fActiveClusters[component].end();
            activeFBCluster.fACNumber = fActiveClusters[component].size();
            unsigned thisPoint;

            KTDEBUG(sclog, "Clustering frequency points");
            for (++pIt; pIt != points.end(); ++pIt)
            {
                thisPoint = pIt->first;
                if (thisPoint - activeFBCluster.fLastPoint > fMaxFreqSepBins)
                {
                    //KTDEBUG(sclog, "Adding freq cluster: " << activeFBCluster.fFirstPoint << "  " << activeFBCluster.fLastPoint);
                    freqBinClusters.push_back(activeFBCluster);
                    activeFBCluster.fPoints.clear();
                    activeFBCluster.fFirstPoint = thisPoint;
                }
                activeFBCluster.fPoints.insert(*pIt);
                activeFBCluster.fLastPoint = thisPoint;
            }
            //KTDEBUG(sclog, "Adding cluster: (ch. " << iChannel << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
            //KTDEBUG(sclog, "Adding freq cluster: " << activeFBCluster.fFirstPoint << "  " << activeFBCluster.fLastPoint);
            freqBinClusters.push_back(activeFBCluster);
        }
        KTDEBUG(sclog, "fb clustering complete;  " << freqBinClusters.size() << " fb clusters found");


        /*// this stuff is no longer necessary since we're not skipping bins in time

        // loop over all of the active clusters to determine their active range in frequency (i.e. the range over which new frequency bins can be added)
        deque< std::pair< unsigned, unsigned > > activeClusterFBRanges;
        for (ActiveClusters::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); ++acIt)
        {
            std::pair< unsigned, unsigned > activeRange(1, 0);
            std::deque< std::pair< unsigned, unsigned > >::const_iterator frIt = (*acIt).fFreqRanges.begin();
            unsigned nTimeBinsBack = 1;
            for (; frIt != (*acIt).fFreqRanges.end() && nTimeBinsBack <= fMaxTimeSepBins; ++frIt, ++nTimeBinsBack)
            {
                if (frIt->first < frIt->second)
                {
                    activeRange.first = frIt->first;
                    activeRange.second = frIt->second;
                    break;
                }
            }
            if (activeRange.first > activeRange.second)
            {
                // ERROR! this active cluster should not be active anymore; nothing within the time separation range had active bins
            }

            for (; frIt != (*acIt).fFreqRanges.end() && nTimeBinsBack <= fMaxTimeSepBins; ++frIt, ++nTimeBinsBack)
            {
                if (frIt->first < frIt->second)
                {
                    if (frIt->first < activeRange.first) activeRange.first = frIt->first;
                    if (frIt->second > activeRange.second) activeRange.second = frIt->second;
                }
            }

            // we're guaranteed that activeRange is a valid range
            activeClusterFBRanges.push_back(activeRange);
        }

         */



        // Make a bool for each active cluster to indicate whether or not it had frequency-bin clusters added to it from this frequency bin
        vector< bool > acHasBeenAddedTo(fActiveClusters[component].size(), false);
        // Make a bool for each active cluster to indicate if it has been merged with another cluster
        vector< bool > acHasBeenMergedElsewhere(fActiveClusters[component].size(), false);
        // Make a vector of pairs to hold the frequency-axis ranges from the current frequency bin
        vector< pair< unsigned, unsigned > > acNewFreqRange(fActiveClusters[component].size());

        // Assign frequency bin clusters to active clusters
        ClusterPoint newPoint;
        newPoint.fSpectrumPtr = spectrumPtr; // adds another smart pointer pointing to this spectrum
        newPoint.fHeaderPtr = headerPtr; // adds another smart pointer pointing to this slice header
        unsigned iCluster;
        // loop over all of the frequency-bin clusters
        KTDEBUG(sclog, "assigning FB clusters to active clusters");
        for (FreqBinClusters::iterator fbIt = freqBinClusters.begin(); fbIt != freqBinClusters.end(); ++fbIt)
        {
            //KTDEBUG(sclog, "next FB cluster");
            // loop over all of the active clusters and look for overlaps with the frequency-bin clusters
            iCluster = 0;
            for (ClusterList::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); ++acIt)
            {
                if (acHasBeenMergedElsewhere[iCluster])
                {
                    //KTDEBUG(sclog, "    continuing ac loop due to merged-elsewhere");
                    ++iCluster;
                    continue;
                }

                //KTDEBUG(sclog, "    comparing to active cluster " << iCluster);

                // check for overlap
                // if sep = 1, neighboring bins are allowed; if sep = 2, there can be a space of 1 bin between them
                // y1 <= x2+sep  && x1 <= y2+sep
                // x1 = fbIt->fFirstPoint; x2 = fbIt->fLastPoint
                // y1 = acIt->EndMinFreqPoint(); y2 = acIt->EndMaxFreqPoint()
                if (acIt->fEndMinFreqPoint <= fbIt->fLastPoint + fMaxFreqSepBins &&
                    fbIt->fFirstPoint <= acIt->fEndMaxFreqPoint + fMaxFreqSepBins)
                {
                    // Yes, this fb cluster (fbIt) overlaps with this active cluster (acIt and iCluster)
                    if (fbIt->fAddedToActiveCluster)
                    {
                        // second+ active cluster this fb cluster matches; merge active clusters
                        //KTDEBUG(sclog, "        found another ac cluster to match the fb cluster; merging ac clusters");
                        acHasBeenMergedElsewhere[iCluster] = true;
                        acHasBeenAddedTo[iCluster] = true;
                        //KTDEBUG(sclog, "        new range: " << acNewFreqRange[fbIt->fACNumber].first << " - " << acNewFreqRange[fbIt->fACNumber].second);
                        // assign the new ac's (cluster # iCluster) points to the previous active cluster (cluster # fbIt->fACNumber)
                        for (SetOfPoints::const_iterator acIt_pIt = acIt->fPoints.begin(); acIt_pIt != acIt->fPoints.end(); ++acIt_pIt)
                        {
                            fbIt->fActiveCluster->fPoints.insert(*acIt_pIt);
                        }
                        //KTDEBUG(sclog, "        ac cluster updated; now contains " << fbIt->fActiveCluster->fPoints.size() << " points");
                    }
                    else
                    {
                        // first active cluster this fb cluster matches
                        //KTDEBUG(sclog, "        found a new ac cluster to match the fb cluster");
                        if (! acHasBeenAddedTo[iCluster])
                        {
                            acHasBeenAddedTo[iCluster] = true;
                            acNewFreqRange[iCluster] = pair< unsigned, unsigned >(fbIt->fFirstPoint, fbIt->fLastPoint);
                            //KTDEBUG(sclog, "        this ac has not already been added to; setting new range: " << acNewFreqRange[iCluster].first << " - " << acNewFreqRange[iCluster].second);
                        }
                        else
                        {
                            if (fbIt->fFirstPoint < acNewFreqRange[iCluster].first) acNewFreqRange[iCluster].first = fbIt->fFirstPoint;
                            if (fbIt->fLastPoint > acNewFreqRange[iCluster].second) acNewFreqRange[iCluster].second = fbIt->fLastPoint;
                            //KTDEBUG(sclog, "        this ac has not been added to; updating range: " << acNewFreqRange[iCluster].first << " - " << acNewFreqRange[iCluster].second);
                        }
                        // assign this frequency-bin cluster (at fbIt) to the current active cluster (at acIt)
                        // add points to acIt->fPoints
                        for (SetOfDiscriminatedPoints::const_iterator fbPointsIt = fbIt->fPoints.begin(); fbPointsIt != fbIt->fPoints.end(); ++fbPointsIt)
                        {
                            newPoint.fTimeBin = this->fTimeBin;
                            newPoint.fFreqBin = fbPointsIt->first;
                            newPoint.fAmplitude = fbPointsIt->second.fOrdinate;
                            acIt->fPoints.insert(newPoint);
                        }
                        //KTDEBUG(sclog, "        ac cluster updated; now contains " << acIt->fPoints.size() << " points");
                        fbIt->fAddedToActiveCluster = true;
                        fbIt->fActiveCluster = acIt;
                        fbIt->fACNumber = iCluster;
                    }
                }
                ++iCluster;
            } // end loop over active clusters
        } // end loop over frequency-bin clusters

        // Update frequency ranges of clusters that were added to this time around
        iCluster = 0;
        KTDEBUG(sclog, "updating frequency ranges of active clusters");
        for (ClusterList::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); ++acIt)
        {
            if (acHasBeenAddedTo[iCluster] && ! acHasBeenMergedElsewhere[iCluster])
            {
                acIt->fEndMinFreqPoint = acNewFreqRange[iCluster].first;
                acIt->fEndMaxFreqPoint = acNewFreqRange[iCluster].second;
                //KTDEBUG(sclog, "range for cluster " << iCluster << " is " << acNewFreqRange[iCluster].first << " - " << acNewFreqRange[iCluster].second);
            }
            ++iCluster;
        }

        //KTDEBUG(sclog, "almost complete: " << fAlmostCompleteClusters[0].size() << "   active: " << fActiveClusters[0].size());

        // For the almost-complete clusters, add this spectrum as a post-cluster spectrum, and complete any clusters that are now done
        ClusterList* completeClusters = new ClusterList();
        KTDEBUG(sclog, "dealing with almost-complete clusters");
        for (ClusterList::iterator accIt = fAlmostCompleteClusters[component].begin(); accIt != fAlmostCompleteClusters[component].end(); ++accIt)
        {
            // this first if statement is used in case there are clusters that already have the right number of post-cluster spectra.
            // e.g. if 0 framing time bins are being used.
            if (accIt->fPostClusterSpectra.size() != fNFramingTimeBins)
            {
                accIt->fPostClusterSpectra.push_back(spectrumPtr);
            }
            if (accIt->fPostClusterSpectra.size() == fNFramingTimeBins)
            {
                completeClusters->push_back(*accIt);
                accIt = fAlmostCompleteClusters[component].erase(accIt);
                --accIt;
            }
        }
        //KTDEBUG(sclog, "almost complete: " << fAlmostCompleteClusters[0].size() << "   active: " << fActiveClusters[0].size());

        // Deal with no-longer-active clusters and clusters that were merged with other clusters
        iCluster = 0;
        KTDEBUG(sclog, "dealing with no-longer-active clusters and clusters that were merged");
        for (ClusterList::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); ++acIt)
        {
            //KTDEBUG(sclog, "active cluster " << iCluster);
            if (acHasBeenMergedElsewhere[iCluster])
            {
                //KTDEBUG(sclog, "    merged with another cluster");
                acIt = fActiveClusters[component].erase(acIt); // the iterator returned is the next position in the cluster
                --acIt; // back up the iterator so that when processing hits the beginning of the loop, the iterator is returned to the "next" position
            }
            else if (acHasBeenAddedTo[iCluster]) // cluster is still active because it was added to
            {
                acIt->fTimeBinSkipCounter = 0;
            }
            else
            {
                ++(acIt->fTimeBinSkipCounter);
                if (acIt->fTimeBinSkipCounter >= fMaxTimeSepBins) // cluster is no longer active
                {
                    if (acIt->LastTimeBin() - acIt->FirstTimeBin() + 1 >= fMinTimeBins)
                    {
                        //KTDEBUG(sclog, "    no longer active and long enough; creating a slice");
                        fAlmostCompleteClusters[component].push_back(*acIt);
                    }
                    //else
                    //{
                        //KTDEBUG(sclog, "    no longer active but not long enough; removing cluster");
                    //}
                    acIt = fActiveClusters[component].erase(acIt); // the iterator returned is the next position in the cluster
                    --acIt; // back up the iterator so that when processing hits the beginning of the loop, the iterator is returned to the "next" position
                }
            }
            ++iCluster;
        }
        //KTDEBUG(sclog, "almost complete: " << fAlmostCompleteClusters[0].size() << "   active: " << fActiveClusters[0].size());


        // Add unassigned frequency clusters as new clusters
        Cluster newCluster;
        newCluster.fDataComponent = component;
        newCluster.fPreClusterSpectra.insert(newCluster.fPreClusterSpectra.end(), fPreClusterSpectra[component].begin(), fPreClusterSpectra[component].end());
        KTDEBUG(sclog, "adding unassigned fb clusters as new clusters");
        for (FreqBinClusters::const_iterator fbIt = freqBinClusters.begin(); fbIt != freqBinClusters.end(); ++fbIt)
        {
            //KTDEBUG(sclog, "next fb cluster");
            if (! fbIt->fAddedToActiveCluster)
            {
                //KTDEBUG(sclog, "    adding new active cluster");
                newCluster.fPoints.clear();
                for (SetOfDiscriminatedPoints::const_iterator fbPointsIt = fbIt->fPoints.begin(); fbPointsIt != fbIt->fPoints.end(); ++fbPointsIt)
                {
                    newPoint.fTimeBin = this->fTimeBin;
                    newPoint.fFreqBin = fbPointsIt->first;
                    newPoint.fAmplitude = fbPointsIt->second.fOrdinate;
                    newCluster.fPoints.insert(newPoint);
                }
                newCluster.fEndMinFreqPoint = fbIt->fFirstPoint;
                newCluster.fEndMaxFreqPoint = fbIt->fLastPoint;
                //KTDEBUG(sclog, "    range is " << newCluster.fEndMinFreqPoint << " - " << newCluster.fEndMaxFreqPoint);
                //KTDEBUG(sclog, "    contains " << newCluster.fPoints.size() << " points");
                fActiveClusters[component].push_back(newCluster);
            }
        }
        //KTDEBUG(sclog, "almost complete: " << fAlmostCompleteClusters[0].size() << "   active: " << fActiveClusters[0].size());

        // Last step: add this frequency spectrum into the pre-cluster spectra list; remove the oldest item in the list if necessary
        fPreClusterSpectra[component].push_back(spectrumPtr);
        while (fPreClusterSpectra[component].size() > fNFramingTimeBins)
        {
            fPreClusterSpectra[component].pop_front();
        }

        return completeClusters; //CompleteInactiveClusters(component);
    }


    void KTMultiSliceClustering::Reset()
    {
        fActiveClusters.clear();
        fAlmostCompleteClusters.clear();
        fPreClusterSpectra.clear();
        fTimeBin = 0;
        return;
    }

    Nymph::KTDataPtr KTMultiSliceClustering::CreateDataFromCluster(const Cluster& cluster)
    {
#ifndef NDEBUG
        // if in debug mode, put together a string to print
        std::stringstream printStream;
        printStream << "Creating data object # " << fDataCount << '\n';
#endif

        ++fDataCount;

        Nymph::KTDataPtr data(new Nymph::KTData());

        KTWaterfallCandidateData& wfcData = data->Of< KTWaterfallCandidateData >();
        wfcData.SetComponent(cluster.fDataComponent);

        wfcData.SetStartRecordNumber(cluster.fPoints.begin()->fHeaderPtr->GetStartRecordNumber());
        wfcData.SetStartSampleNumber(cluster.fPoints.begin()->fHeaderPtr->GetStartSampleNumber());
        wfcData.SetEndRecordNumber(cluster.fPoints.rbegin()->fHeaderPtr->GetEndRecordNumber());
        wfcData.SetEndSampleNumber(cluster.fPoints.rbegin()->fHeaderPtr->GetEndSampleNumber());

        unsigned firstTimeBin = cluster.FirstTimeBin();
        unsigned lastTimeBin = cluster.LastTimeBin();

        int firstTimeBinWithFrame = (int)firstTimeBin - (int)fNFramingTimeBins;
        int lastTimeBinWithFrame = (int)lastTimeBin + (int)fNFramingTimeBins;

#ifndef NDEBUG
        printStream << "\tTime bin range: " << firstTimeBin << " - " << lastTimeBin << "; with frame: " << firstTimeBinWithFrame << " - " << lastTimeBinWithFrame << '\n';
#endif

        double ftbSumOfWeights = 0., ftbWeightedSum = 0.;
        double ltbSumOfWeights = 0., ltbWeightedSum = 0.;

        SetOfPoints::const_iterator it = cluster.fPoints.begin();
        unsigned firstFreqBin = it->fFreqBin;
        unsigned lastFreqBin = firstFreqBin;

        if (it->fTimeBin == firstTimeBin)
        {
            ftbSumOfWeights += it->fAmplitude;
            ftbWeightedSum += it->fSpectrumPtr->GetBinCenter(it->fFreqBin) * it->fAmplitude;
        }
        if (it->fTimeBin == lastTimeBin)
        {
            ltbSumOfWeights += it->fAmplitude;
            ltbWeightedSum += it->fSpectrumPtr->GetBinCenter(it->fFreqBin) * it->fAmplitude;
        }

        for (++it; it != cluster.fPoints.end(); ++it)
        {
            if (it->fFreqBin < firstFreqBin) firstFreqBin = it->fFreqBin;
            if (it->fFreqBin > lastFreqBin) lastFreqBin = it->fFreqBin;

            if (it->fTimeBin == firstTimeBin)
            {
                ftbSumOfWeights += it->fAmplitude;
                ftbWeightedSum += it->fSpectrumPtr->GetBinCenter(it->fFreqBin) * it->fAmplitude;
            }
            if (it->fTimeBin == lastTimeBin)
            {
                ltbSumOfWeights += it->fAmplitude;
                ltbWeightedSum += it->fSpectrumPtr->GetBinCenter(it->fFreqBin) * it->fAmplitude;
            }
        }

        int firstFreqBinWithFrame = (int)firstFreqBin - (int)fNFramingFreqBins;
        int lastFreqBinWithFrame =(int) lastFreqBin + (int)fNFramingFreqBins;

#ifndef NDEBUG
        printStream << "\tFreq bin range: " << firstFreqBin << " - " << lastFreqBin << "; with frame: " << firstFreqBinWithFrame << " - " << lastFreqBinWithFrame << '\n';
#endif

        unsigned firstFreqBinToUse = firstFreqBinWithFrame >= 0 ? (unsigned)firstFreqBinWithFrame : 0;
        unsigned lastFreqBinToUse = lastFreqBinWithFrame < (int)(cluster.fPoints.begin()->fSpectrumPtr->size()) ? (unsigned)lastFreqBinWithFrame : cluster.fPoints.begin()->fSpectrumPtr->size() - 1;
        unsigned freqBinOffset = unsigned((int)firstFreqBinToUse - firstFreqBinWithFrame);

        double timeOverlapFrac = cluster.fPoints.begin()->fHeaderPtr->GetNonOverlapFrac();
        double timeBinWidthWithOverlap = cluster.fPoints.begin()->fHeaderPtr->GetSliceLength();
        double timeBinWidth = timeBinWidthWithOverlap * timeOverlapFrac;
        double freqBinWidth = cluster.fPoints.begin()->fSpectrumPtr->GetBinWidth();
        unsigned nTimeBins = lastTimeBin - firstTimeBin + 1;
        unsigned nFreqBins = lastFreqBin - firstFreqBin + 1;

        unsigned nTimeBinsWithFrame = nTimeBins + 2 * fNFramingTimeBins;
        unsigned nFreqBinsWithFrame = nFreqBins + 2 * fNFramingFreqBins;

#ifndef NDEBUG
        printStream << "\tDimensions with frame: " << nTimeBinsWithFrame << " time bins and " << nFreqBinsWithFrame << " freq bins;  cluster dimensions are " << nTimeBins << " by " << nFreqBins << '\n';
        printStream << "\tTime bin width: " << timeBinWidthWithOverlap << " s (raw) --- " << timeBinWidth << " s (accounting for overlap)\n";
        printStream << "\tFreq bin width: " << freqBinWidth << " Hz\n";
#endif

        //KTWARN(sclog, "loading spectrum pointers; expecting " << nTimeBinsWithFrame << " spectra");
        // Create a vector of pointers to spectra, where each component is for a single time bin
        vector< boost::shared_ptr< KTFrequencySpectrumPolar > > spectra(nTimeBinsWithFrame);
        // Pre-cluster frame bins
        unsigned frameBin = fNFramingTimeBins - cluster.fPreClusterSpectra.size();
        for (ListOfSpectra::const_iterator losIt = cluster.fPreClusterSpectra.begin(); losIt != cluster.fPreClusterSpectra.end(); ++losIt)
        {
            spectra[frameBin] = *losIt;
            //KTDEBUG(sclog, "put spectrum in frame bin " << frameBin << ": " << bool(spectra[frameBin]));
            ++frameBin;
        }
        // Cluster bins
        for (SetOfPoints::const_iterator it = cluster.fPoints.begin(); it != cluster.fPoints.end(); ++it)
        {
            if (! it->fSpectrumPtr)
            {
                KTWARN(sclog, "Spectrum pointer is NULL! " << it->fSpectrumPtr.get());
            }
            spectra[it->fTimeBin - firstTimeBin + fNFramingTimeBins] = it->fSpectrumPtr;
            //KTDEBUG(sclog, "put spectrum in frame bin " << it->fTimeBin - firstTimeBin + fNFramingTimeBins << ": " << bool(spectra[it->fTimeBin - firstTimeBin + fNFramingTimeBins]));
        }
        // Post-cluster frame bins
        frameBin = lastTimeBin - firstTimeBin + 1 + fNFramingTimeBins;
        for (ListOfSpectra::const_iterator losIt = cluster.fPostClusterSpectra.begin(); losIt != cluster.fPostClusterSpectra.end(); ++losIt)
        {
            spectra[frameBin] = *losIt;
            //KTDEBUG(sclog, "put spectrum in frame bin " << frameBin << ": " << bool(spectra[frameBin]));
            ++frameBin;
        }

        // this is the amount of time missing on each end of a cluster if the length of the cluster is calculated with the timeBinWidth, which is the non-overlap fraction of the slice length
        double endTimeBinShift = 0.5 * (1. - timeOverlapFrac) * timeBinWidthWithOverlap;

        wfcData.SetTimeInRun(cluster.fPoints.begin()->fHeaderPtr->GetTimeInRun());
        wfcData.SetFirstSliceNumber(cluster.fPoints.begin()->fHeaderPtr->GetSliceNumber());
        wfcData.SetLastSliceNumber(cluster.fPoints.rbegin()->fHeaderPtr->GetSliceNumber());
        wfcData.SetTimeLength(timeBinWidth * double(nTimeBins) + 2. * endTimeBinShift);
        wfcData.SetMeanStartFrequency(ftbWeightedSum / ftbSumOfWeights);
        wfcData.SetMeanEndFrequency(ltbWeightedSum / ltbSumOfWeights);
        wfcData.SetFrequencyWidth(freqBinWidth * double(nFreqBins));

        unsigned i = 0;
        for (; ! spectra[i]; ++i); // get the first spectrum that is definitely there (since sometimes spectra are missing)
        if (i != spectra.size())
        {
            wfcData.SetMinimumFrequency(spectra[i]->GetBinLowEdge(firstFreqBin));
            wfcData.SetMaximumFrequency(spectra[i]->GetBinLowEdge(lastFreqBin) + freqBinWidth);
        }
#ifndef NDEBUG
        printStream << "\tTime in run: " << wfcData.GetTimeInRun() << " s\n";
        printStream << "\tTime length: " << wfcData.GetTimeLength() << " s\n";
        printStream << "\tFirst slice number: " << wfcData.GetFirstSliceNumber() << '\n';
        printStream << "\tLast slice number: " << wfcData.GetLastSliceNumber() << '\n';
        printStream << "\tMean start freq: " << wfcData.GetMeanStartFrequency() << " Hz\n";
        printStream << "\tMean end freq: " << wfcData.GetMeanEndFrequency() << " Hz\n";
        printStream << "\tFreq width: " << wfcData.GetFrequencyWidth() << " Hz\n";
        printStream << "\tMinimum frequency: " << wfcData.GetMinimumFrequency() << " Hz\n";
        printStream << "\tMaximum frequency: " << wfcData.GetMaximumFrequency() << " Hz\n";
#endif


        double tfStartTime = wfcData.GetTimeInRun() + endTimeBinShift - double(fNFramingTimeBins) * timeBinWidth;
        double tfStartFreq = wfcData.GetMinimumFrequency() - double(fNFramingTimeBins) * freqBinWidth;
        KTTimeFrequency* tf = new KTTimeFrequencyPolar(nTimeBinsWithFrame, tfStartTime, tfStartTime + double(nTimeBinsWithFrame)*timeBinWidth, nFreqBinsWithFrame, tfStartFreq, tfStartFreq + double(nFreqBinsWithFrame)*freqBinWidth);
        //KTDEBUG(sclog, "tf freq dims: " << tfStartFreq << " - " << tfStartFreq + double(nFreqBinsWithFrame)*freqBinWidth);
        for (int iTBin=firstTimeBinWithFrame; iTBin <= lastTimeBinWithFrame; ++iTBin)
        {
            int spectrumNum = iTBin - firstTimeBinWithFrame;
            if (spectra[spectrumNum])
            {
                //KTDEBUG(sclog, "    have spectrum for spectrumNum = " << spectrumNum);
                for (unsigned iFBin=firstFreqBinToUse; iFBin <= lastFreqBinToUse; ++iFBin)
                {
                    //KTDEBUG(sclog, "    setting point at (" << spectrumNum << ", " << iFBin-firstFreqBinToUse << "), aka (" << iTBin << ", " << iFBin << "); length of spectrum: " << spectra[spectrumNum]->size());
                    tf->SetPolar(spectrumNum, iFBin - firstFreqBinToUse + freqBinOffset, spectra[spectrumNum]->GetAbs(iFBin), spectra[spectrumNum]->GetArg(iFBin));
                }
            }
            /*
            else
            {
                KTDEBUG(sclog, "    NO SPECTRUM for spectrumNum = " << spectrumNum);
            }
            */
        }
        wfcData.SetCandidate(tf);

#ifndef NDEBUG
        KTDEBUG(sclog, printStream.str());
#endif

        return data;
    }

    void KTMultiSliceClustering::RunDataLoop(DataList* dataList)
    {
        while (! dataList->empty())
        {
            fClusteredDataSignal(dataList->front());
            dataList->pop_front();
        }

        delete dataList;
        return;
    }

    void KTMultiSliceClustering::CompleteRemainingClusters()
    {
        KTINFO(sclog, "Signal received to clean up remaining active clusters");
        DataList* lastData = CompleteAllClusters();
        if (lastData != NULL)
        {
            RunDataLoop(lastData);
        }
        return;
    }


} /* namespace Katydid */
