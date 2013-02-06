/*
 * KTSimpleCusteringProcessor.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTSimpleClustering.hh"

#include "KTBundle.hh"
#include "KTCorrelationData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include <set>

using boost::shared_ptr;

using std::deque;
using std::list;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(sclog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTSimpleClustering > sSimpClustRegistrar("simple-clustering");

    KTSimpleClustering::KTSimpleClustering() :
            KTBundleQueueProcessorTemplate< KTSimpleClustering >(),
            fMaxFreqSep(1.),
            fMaxTimeSep(1.),
            fMaxFreqSepBins(1),
            fMaxTimeSepBins(1),
            fCalculateMaxFreqSepBins(false),
            fCalculateMaxTimeSepBins(false),
            fDPInputDataName("disc-points"),
            fFSInputDataName("frequency-spectrum"),
            fOutputDataName("waterfall-candidate"),
            fTimeBin(0),
            fTimeBinWidth(1.),
            fFreqBinWidth(1.)
    {
        fConfigName = "simple-clustering";

        // QueueBundle and QueueBundles are registered in KTBundleQueueProcessorTemplate constructor

        this->SetFuncPtr(&KTSimpleClustering::ProcessOneSliceBundle);
    }

    KTSimpleClustering::~KTSimpleClustering()
    {
    }

    Bool_t KTSimpleClustering::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("max-frequency-sep"))
        {
            SetMaxFrequencySeparation(node->GetData< Double_t >("max-frequency-sep"));
        }
        if (node->HasData("max-time-sep"))
        {
            SetMaxTimeSeparation(node->GetData< Double_t >("max-time-sep"));
        }

        if (node->HasData("max-frequency-sep-bins"))
        {
            SetMaxFrequencySeparationBins(node->GetData< UInt_t >("max-frequency-sep-bins"));
        }
        if (node->HasData("max-time-sep-bins"))
        {
            SetMaxTimeSeparationBins(node->GetData< UInt_t >("max-time-sep-bins"));
        }

        SetDPInputDataName(node->GetData< string >("dp-input-data-name", fDPInputDataName));
        SetFSInputDataName(node->GetData< string >("fs-input-data-name", fFSInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

        return true;
    }

    KTSimpleClustering::BundleList* KTSimpleClustering::FindClusters(const KTDiscriminatedPoints1DData* dpData, const KTFrequencySpectrumData* fsData)
    {
        ClusterList* completedClusters = AddPointsToClusters(dpData, shared_ptr<KTData>(fsData));

        BundleList* newBundles = new BundleList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newBundles->push_back(CreateBundleFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }

        return newBundles;
    }

    KTSimpleClustering::BundleList* KTSimpleClustering::FindClusters(const KTDiscriminatedPoints1DData* dpData, const KTFrequencySpectrumData* fsData)
    {
        ClusterList* completedClusters = AddPointsToClusters(dpData, shared_ptr<KTData>(fsData));

        BundleList* newBundles = new BundleList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newBundles->push_back(CreateBundleFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }

        return newBundles;
    }

    KTSimpleClustering::BundleList* KTSimpleClustering::FindClusters(const KTDiscriminatedPoints1DData* dpData, const KTFrequencySpectrumData* fsData)
    {
        ClusterList* completedClusters = AddPointsToClusters(dpData, shared_ptr<KTData>(fsData));

        BundleList* newBundles = new BundleList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newBundles->push_back(CreateBundleFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }

        return newBundles;
    }

    KTSimpleClustering::ClusterList* KTSimpleClustering::AddPointsToClusters(const KTDiscriminatedPoints1DData* dpData, shared_ptr<KTData> data)
    {
        if (dpData->GetBinWidth() != fFreqBinWidth)
            SetFrequencyBinWidth(dpData->GetBinWidth());
        if (dpData->GetBinWidth() * Double_t(dpData->GetNBins()) != fTimeBinWidth)
            SetTimeBinWidth(dpData->GetBinWidth() * Double_t(dpData->GetNBins()));

        UInt_t nComponents = dpData->GetNChannels();
        if (fActiveClusters.size() < nComponents) fActiveClusters.resize(nComponents);

        ClusterList* newClustersAC = new ClusterList();

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            ClusterList* clustersFromComponent = AddPointsToClusters(dpData->GetSetOfPoints(iComponent), iComponent, data);
            newClustersAC->splice(newClustersAC->end(), *clustersFromComponent);
            delete clustersFromComponent;
        }

        fTimeBin++;

        return newClustersAC;
    }

    KTSimpleClustering::ClusterList* KTSimpleClustering::AddPointsToClusters(const SetOfDiscriminatedPoints& points, UInt_t component, shared_ptr<KTData> data)
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
            UInt_t thisPoint;

            KTDEBUG(sclog, "Clustering frequency points");
            for (pIt++; pIt != points.end(); pIt++)
            {
                thisPoint = pIt->first;
                if (thisPoint - activeFBCluster.fLastPoint > fMaxFreqSepBins)
                {
                    KTDEBUG(sclog, "Adding freq cluster: " << activeFBCluster.fFirstPoint << "  " << activeFBCluster.fLastPoint);
                    freqBinClusters.push_back(activeFBCluster);
                    activeFBCluster.fPoints.clear();
                    activeFBCluster.fFirstPoint = thisPoint;
                }
                activeFBCluster.fPoints.insert(*pIt);
                activeFBCluster.fLastPoint = thisPoint;
            }
            //KTDEBUG(sclog, "Adding cluster: (ch. " << iChannel << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
            KTDEBUG(sclog, "Adding freq cluster: " << activeFBCluster.fFirstPoint << "  " << activeFBCluster.fLastPoint);
            freqBinClusters.push_back(activeFBCluster);
        }


        /*// this stuff is no longer necessary since we're not skipping bins in time

        // loop over all of the active clusters to determine their active range in frequency (i.e. the range over which new frequency bins can be added)
        deque< std::pair< UInt_t, UInt_t > > activeClusterFBRanges;
        for (ActiveClusters::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); acIt++)
        {
            std::pair< UInt_t, UInt_t > activeRange(1, 0);
            std::deque< std::pair< UInt_t, UInt_t > >::const_iterator frIt = (*acIt).fFreqRanges.begin();
            UInt_t nTimeBinsBack = 1;
            for (; frIt != (*acIt).fFreqRanges.end() && nTimeBinsBack <= fMaxTimeSepBins; frIt++, nTimeBinsBack++)
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

            for (; frIt != (*acIt).fFreqRanges.end() && nTimeBinsBack <= fMaxTimeSepBins; frIt++, nTimeBinsBack++)
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
        vector< Bool_t > acHasBeenAddedTo(fActiveClusters[component].size(), false);
        // Make a bool for each active cluster to indicate if it has been merged with another cluster
        vector< Bool_t > acHasBeenMergedElsewhere(fActiveClusters[component].size(), false);
        // Make a vector of pairs to hold the frequency-axis ranges from the current frequency bin
        vector< pair< UInt_t, UInt_t > > acNewFreqRange(fActiveClusters[component].size());

        // Assign frequency bin clusters to active clusters
        ClusterPoint newPoint;
        UInt_t iCluster;
        // loop over all of the frequency-bin clusters
        KTDEBUG(sclog, "assigning FB clusters to active clusters");
        for (FreqBinClusters::iterator fbIt = freqBinClusters.begin(); fbIt != freqBinClusters.end(); fbIt++)
        {
            KTDEBUG(sclog, "next FB cluster");
            // loop over all of the active clusters and look for overlaps with the frequency-bin clusters
            iCluster = 0;
            for (ClusterList::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); acIt++)
            {
                if (acHasBeenMergedElsewhere[iCluster]) continue;

                KTDEBUG(sclog, "    comparing to active cluster " << iCluster);
                // check for overlap
                // y1 <= x2+sep  && x1 <= y2+sep
                // x1 = fbIt->fFirstPoint; x2 = fbIt->fLastPoint
                // y1 = acIt->EndMinFreqPoint(); y2 = acIt->EndMaxFreqPoint()
                if (acIt->EndMinFreqPoint() <= fbIt->fLastPoint + fMaxFreqSepBins &&
                    fbIt->fFirstPoint <= acIt->EndMaxFreqPoint() + fMaxFreqSepBins)
                {
                    if (fbIt->fAddedToActiveCluster)
                    {
                        // second+ active cluster this fb cluster matches; merge active clusters
                        KTDEBUG(sclog, "        found another ac cluster to match the fb cluster; merging ac clusters");
                        acHasBeenMergedElsewhere[iCluster] = true;
                        acHasBeenAddedTo[iCluster] = true;
                        if (acNewFreqRange[iCluster].first < acNewFreqRange[fbIt->fACNumber].first) acNewFreqRange[fbIt->fACNumber].first = acNewFreqRange[iCluster].first;
                        if (acNewFreqRange[iCluster].second > acNewFreqRange[fbIt->fACNumber].second) acNewFreqRange[fbIt->fACNumber].second = acNewFreqRange[iCluster].second;

                        // assign the new ac's (cluster # iCluster) points to the previous active cluster (cluster # fbIt->fACNumber)
                        for (deque< ClusterPoint >::const_iterator acIt_pIt = acIt->fPoints.begin(); acIt_pIt != acIt->fPoints.end(); acIt_pIt++)
                        {
                            fbIt->fActiveCluster->fPoints.push_back(*acIt_pIt);
                        }
                        KTDEBUG(sclog, "        ac cluster updated");
                    }
                    else
                    {
                        // first active cluster this fb cluster matches
                        KTDEBUG(sclog, "        found a new ac cluster to match the fb cluster");
                        if (! acHasBeenAddedTo[iCluster])
                        {
                            acHasBeenAddedTo[iCluster] = true;
                            acNewFreqRange[iCluster] = pair< UInt_t, UInt_t >(fbIt->fFirstPoint, fbIt->fLastPoint);
                            KTDEBUG(sclog, "        this ac has been added to; range is updated");
                        }
                        else
                        {
                            if (fbIt->fFirstPoint < acNewFreqRange[iCluster].first) acNewFreqRange[iCluster].first = fbIt->fFirstPoint;
                            if (fbIt->fLastPoint > acNewFreqRange[iCluster].second) acNewFreqRange[iCluster].second = fbIt->fLastPoint;
                            KTDEBUG(sclog, "        this ac has not been added to; setting new range");
                        }
                        // assign this frequency-bin cluster (at fbIt) to the current active cluster (at acIt)
                        // add points to acIt->fPoints
                        for (SetOfDiscriminatedPoints::const_iterator fbPointsIt = fbIt->fPoints.begin(); fbPointsIt != fbIt->fPoints.end(); fbPointsIt++)
                        {
                            newPoint.fTimeBin = this->fTimeBin;
                            newPoint.fFreqBin = fbPointsIt->first;
                            newPoint.fAmplitude = fbPointsIt->second;
                            acIt->fPoints.push_back(newPoint);
                        }
                        KTDEBUG(sclog, "        ac cluster updated");
                        fbIt->fAddedToActiveCluster = true;
                        fbIt->fActiveCluster = acIt;
                        fbIt->fACNumber = iCluster;
                    }
                }
                iCluster++;
            } // end loop over active clusters
        } // end loop over frequency-bin clusters


        // Update frequency ranges of clusters that were added to this time around
        iCluster = 0;
        KTDEBUG(sclog, "updating frequency ranges of active clusters");
        for (ClusterList::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); acIt++)
        {
            if (acHasBeenAddedTo[iCluster] && ! acHasBeenMergedElsewhere[iCluster])
            {
                acIt->fFreqRanges.push_back(acNewFreqRange[iCluster]);
            }
            iCluster++;
        }

        // Deal with no-longer-active clusters and clusters that were merged with other clusters
        //BundleList* newBundles = new BundleList();
        ClusterList* completeClusters = new ClusterList();
        iCluster = 0;
        KTDEBUG(sclog, "dealing with no-longer-active clusters and clusters that were merged");
        for (ClusterList::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); acIt++)
        {
            KTDEBUG(sclog, "cluster " << iCluster);
            if (acHasBeenMergedElsewhere[iCluster])
            {
                KTDEBUG(sclog, "    merged with another cluster");
                acIt = fActiveClusters[component].erase(acIt); // the iterator returned is the next position in the cluster
                acIt--; // back up the iterator so that when processing hits the beginning of the loop, the iterator is returned to the "next" position
            }
            else if (! acHasBeenAddedTo[iCluster])
            {
                KTDEBUG(sclog, "    no longer active; creating an bundle");
                //newBundles->push_back(CreateBundleFromCluster(*acIt));
                completeClusters->push_back(*acIt);
                acIt = fActiveClusters[component].erase(acIt); // the iterator returned is the next position in the cluster
                acIt--; // back up the iterator so that when processing hits the beginning of the loop, the iterator is returned to the "next" position
            }
            iCluster++;
        }


        // Add unassigned frequency clusters as new clusters
        Cluster newCluster;
        KTDEBUG(sclog, "adding unassigned fb clusters as new clusters");
        for (FreqBinClusters::const_iterator fbIt = freqBinClusters.begin(); fbIt != freqBinClusters.end(); fbIt++)
        {
            KTDEBUG(sclog, "next fb cluster");
            if (! fbIt->fAddedToActiveCluster)
            {
                KTDEBUG(sclog, "    adding new active cluster");
                newCluster.fPoints.clear();
                for (SetOfDiscriminatedPoints::const_iterator fbPointsIt = fbIt->fPoints.begin(); fbPointsIt != fbIt->fPoints.end(); fbPointsIt++)
                {
                    newPoint.fTimeBin = this->fTimeBin;
                    newPoint.fFreqBin = fbPointsIt->first;
                    newPoint.fAmplitude = fbPointsIt->second;
                    newCluster.fPoints.push_back(newPoint);
                }
                newCluster.fFreqRanges.clear();
                newCluster.fFreqRanges.push_back(pair< UInt_t, UInt_t >(fbIt->fFirstPoint, fbIt->fLastPoint));
                fActiveClusters[component].push_back(newCluster);
            }
        }

        return completeClusters; //CompleteInactiveClusters(component);
    }


    KTSimpleClustering::ClusterList* KTSimpleClustering::CompleteAllClusters(UInt_t component)
    {
        ClusterList* newClusters = new ClusterList(fActiveClusters.begin(), fActiveClusters.end());
        /*
        for (ClusterList::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end();)
        {
            newBundles->push_back(CreateBundleFromCluster(*acIt));
            acIt = fActiveClusters[component].erase(acIt);
        }
        */
        return newClusters;
    }
/*
    KTSimpleClustering::BundleList* KTSimpleClustering::CompleteInactiveClusters(UInt_t component)
    {
        BundleList* newBundles = new BundleList();

        for (ActiveClusters::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end();)
        {
            UInt_t lastTimeBinInCluster = (*acIt).fPoints.back().fTimeBin;
            if (fTimeBin - lastTimeBinInCluster > fMaxTimeSepBins)
            {
                newBundles->push_back(CreateBundleFromCluster(*acIt));
                acIt = fActiveClusters[component].erase(acIt);
            }
            else
            {
                acIt++;
            }
        }

        return newBundles;
    }
*/
    void KTSimpleClustering::Reset()
    {
        fActiveClusters.clear();
        fTimeBin = 0;
        return;
    }

    shared_ptr<KTBundle> KTSimpleClustering::CreateBundleFromCluster(const Cluster& cluster)
    {
        shared_ptr<KTBundle> bundle(new KTBundle());

        //KTWaterfallCandidateData* newData = new KTWaterfallCandidateData();

        // fill in the data

        //data->SetBundle(bundle);
        //bundle->AddData(newData);

        return bundle;
    }

    void KTSimpleClustering::ProcessOneSliceBundle(boost::shared_ptr<KTBundle> bundle)
    {
        // signal for any continued use of this bundle
        fOneSliceBundleSignal(bundle);

        KTDiscriminatedPoints1DData* discData = bundle->GetData< KTDiscriminatedPoints1DData >(fDPInputDataName);
        if (discData == NULL)
        {
            KTWARN(sclog, "No discriminated-points data was present in the bundle");
            return;
        }

        BundleList* clusteredBundles = NULL;

        KTFrequencySpectrumData* fsData = bundle->GetData< KTFrequencySpectrumData >(fFSInputDataName);
        if (fsData != NULL)
        {
            clusteredBundles = FindClusters(discData, fsData);
        }
        else
        {
            KTFrequencySpectrumData* fsDataFFTW = bundle->GetData< KTFrequencySpectrumDataFFTW >(fFSInputDataName);
            if (fsDataFFTW != NULL)
            {
                clusteredBundles = FindClusters(discData, fsDataFFTW);
            }
            else
            {
                KTFrequencySpectrumData* fsData = bundle->GetData< KTFrequencySpectrumData >(fFSInputDataName);
                if (fsData != NULL)
                {
                    clusteredBundles = FindClusters(discData, fsData);
                }
                else
                {
                    KTWARN(sclog, "No spectrum data was found with name <" << fFSInputDataName << ">");
                    return;
                }
            }
        }

        if (clusteredBundles != NULL)
        {
            RunBundleLoop(clusteredBundles);
        }
        return;
    }

    void KTSimpleClustering::RunBundleLoop(BundleList* bundles)
    {
        while (! bundles->empty())
        {
            /*
            KTWaterfallCandidateData* wfCandData = bundles->front()->GetData< KTWaterfallCandidateData >(fOutputDataName);
            if (wfCandData == NULL)
            {
                KTWARN(sclog, "Bundle does not contain waterfall-candidate data with name <" << fOutputDataName << ">!");
            }
            else
            {
                fWaterfallCandidateSignal(wfCandData);
            }
            */
            fClusteredBundleSignal(bundles->front());
            bundles->pop_front();
        }

        delete bundles;
        return;
    }


} /* namespace Katydid */
