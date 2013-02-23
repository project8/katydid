/*
 * KTSimpleCusteringProcessor.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTMultiSliceClustering.hh"

#include "KTCorrelator.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTSliceHeader.hh"
#include "KTTimeFrequencyPolar.hh"
#include "KTWaterfallCandidateData.hh"
#include "KTWignerVille.hh"

#include <boost/weak_ptr.hpp>

#include <set>

using boost::shared_ptr;
using boost::weak_ptr;

using std::deque;
using std::list;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(sclog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTMultiSliceClustering > sMSClustRegistrar("multi-slice-clustering");

    KTMultiSliceClustering::KTMultiSliceClustering() :
            KTDataQueueProcessorTemplate< KTMultiSliceClustering >(),
            fMaxFreqSep(1.),
            fMaxTimeSep(1.),
            fMaxFreqSepBins(1),
            fMaxTimeSepBins(1),
            fCalculateMaxFreqSepBins(false),
            fCalculateMaxTimeSepBins(false),
            fTimeBin(0),
            fTimeBinWidth(1.),
            fFreqBinWidth(1.)
    {
        fConfigName = "multi-slice-clustering";

        RegisterSignal("one-slice", &fOneSliceDataSignal, "void (shared_ptr< KTData >)");
        RegisterSignal("cluster", &fClusteredDataSignal, "void (shared_ptr< KTData >)");

        RegisterSlot("fs-polar", this, &KTMultiSliceClustering::QueueFSPolarData, "void (shared_ptr< KTData >)");
        RegisterSlot("fs-fftw", this, &KTMultiSliceClustering::QueueFSFFTWData, "void (shared_ptr< KTData >)");
        RegisterSlot("correlation", this, &KTMultiSliceClustering::QueueCorrelationData, "void (shared_ptr< KTData >)");
        RegisterSlot("wigner-ville", this, &KTMultiSliceClustering::QueueWVData, "void (shared_ptr< KTData >)");
    }

    KTMultiSliceClustering::~KTMultiSliceClustering()
    {
    }

    Bool_t KTMultiSliceClustering::Configure(const KTPStoreNode* node)
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

        return true;
    }

    KTMultiSliceClustering::DataList* KTMultiSliceClustering::FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataPolar& fsData, const KTSliceHeader& header)
    {
        // Make a copy of the spectrum data
        shared_ptr< KTFrequencySpectrumDataPolar > cachedSpectrumData(new KTFrequencySpectrumDataPolar());
        UInt_t nComponents = fsData.GetNComponents();
        cachedSpectrumData->SetNComponents(nComponents);
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            cachedSpectrumData->SetSpectrum(new KTFrequencySpectrumPolar(*(fsData.GetSpectrumPolar(iComponent))), iComponent);
        }

        shared_ptr< KTSliceHeader > headerPtr(new KTSliceHeader(header));

        ClusterList* completedClusters = AddPointsToClusters(dpData, cachedSpectrumData, headerPtr);

        DataList* newDataList = new DataList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newDataList->push_back(CreateDataFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }

        return newDataList;
    }

    KTMultiSliceClustering::DataList* KTMultiSliceClustering::FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTFrequencySpectrumDataFFTW& fsData, const KTSliceHeader& header)
    {
        // Make a copy of the spectrum data
        shared_ptr< KTFrequencySpectrumDataPolar > cachedSpectrumData(new KTFrequencySpectrumDataPolar());
        UInt_t nComponents = fsData.GetNComponents();
        cachedSpectrumData->SetNComponents(nComponents);
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            cachedSpectrumData->SetSpectrum(fsData.GetSpectrumFFTW(iComponent)->CreateFrequencySpectrum(), iComponent);
        }

        shared_ptr< KTSliceHeader > headerPtr(new KTSliceHeader(header));

        ClusterList* completedClusters = AddPointsToClusters(dpData, cachedSpectrumData, headerPtr);

        DataList* newDataList = new DataList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newDataList->push_back(CreateDataFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }

        return newDataList;
    }

    KTMultiSliceClustering::DataList* KTMultiSliceClustering::FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTCorrelationData& corrData, const KTSliceHeader& header)
    {
        // Make a copy of the spectrum data
        shared_ptr< KTFrequencySpectrumDataPolar > cachedSpectrumData(new KTFrequencySpectrumDataPolar());
        UInt_t nComponents = corrData.GetNComponents();
        cachedSpectrumData->SetNComponents(nComponents);
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            cachedSpectrumData->SetSpectrum(new KTFrequencySpectrumPolar(*(corrData.GetSpectrumPolar(iComponent))), iComponent);
        }

        shared_ptr< KTSliceHeader > headerPtr(new KTSliceHeader(header));

        ClusterList* completedClusters = AddPointsToClusters(dpData, cachedSpectrumData, headerPtr);

        DataList* newDataList = new DataList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newDataList->push_back(CreateDataFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }

        return newDataList;
    }

    KTMultiSliceClustering::DataList* KTMultiSliceClustering::FindClusters(const KTDiscriminatedPoints1DData& dpData, const KTWignerVilleData& wvData, const KTSliceHeader& header)
    {
        // Make a copy of the spectrum data
        shared_ptr< KTFrequencySpectrumDataPolar > cachedSpectrumData(new KTFrequencySpectrumDataPolar());
        UInt_t nComponents = wvData.GetNComponents();
        cachedSpectrumData->SetNComponents(nComponents);
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            cachedSpectrumData->SetSpectrum(wvData.GetSpectrumFFTW(iComponent)->CreateFrequencySpectrum(), iComponent);
        }

        shared_ptr< KTSliceHeader > headerPtr(new KTSliceHeader(header));

        ClusterList* completedClusters = AddPointsToClusters(dpData, cachedSpectrumData, headerPtr);

        DataList* newDataList = new DataList();

        for (ClusterList::iterator acIt = completedClusters->begin(); acIt != completedClusters->end();)
        {
            newDataList->push_back(CreateDataFromCluster(*acIt));
            acIt = completedClusters->erase(acIt);
        }

        return newDataList;
    }



    KTMultiSliceClustering::ClusterList* KTMultiSliceClustering::AddPointsToClusters(const KTDiscriminatedPoints1DData& dpData, shared_ptr< KTFrequencySpectrumDataPolar >& spectrumDataPtr, shared_ptr< KTSliceHeader >& headerPtr)
    {
        if (dpData.GetBinWidth() != fFreqBinWidth)
            SetFrequencyBinWidth(dpData.GetBinWidth());
        if (headerPtr->GetSliceSize() != fTimeBinWidth)
            SetTimeBinWidth(headerPtr->GetSliceSize());

        UInt_t nComponents = dpData.GetNComponents();
        if (fActiveClusters.size() < nComponents) fActiveClusters.resize(nComponents);

        ClusterList* newClustersAC = new ClusterList();

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            ClusterList* clustersFromComponent = AddPointsToClusters(dpData.GetSetOfPoints(iComponent), iComponent, spectrumDataPtr, headerPtr);
            newClustersAC->splice(newClustersAC->end(), *clustersFromComponent);
            delete clustersFromComponent;
        }

        fTimeBin++;

        return newClustersAC;
    }

    KTMultiSliceClustering::ClusterList* KTMultiSliceClustering::AddPointsToClusters(const SetOfDiscriminatedPoints& points, UInt_t component, shared_ptr< KTFrequencySpectrumDataPolar >& spectrumDataPtr, shared_ptr< KTSliceHeader >& headerPtr)
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
        newPoint.fSpectrumPtr = spectrumDataPtr;
        newPoint.fHeaderPtr = headerPtr;
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
                KTDEBUG(sclog, "    no longer active; creating a slice");
                completeClusters->push_back(*acIt);
                acIt = fActiveClusters[component].erase(acIt); // the iterator returned is the next position in the cluster
                acIt--; // back up the iterator so that when processing hits the beginning of the loop, the iterator is returned to the "next" position
            }
            iCluster++;
        }


        // Add unassigned frequency clusters as new clusters
        Cluster newCluster;
        newCluster.fDataComponent = component;
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


    KTMultiSliceClustering::ClusterList* KTMultiSliceClustering::CompleteAllClusters(UInt_t component)
    {
        ClusterList* newClusters = new ClusterList(fActiveClusters[component].begin(), fActiveClusters[component].end());
        return newClusters;
    }

    void KTMultiSliceClustering::Reset()
    {
        fActiveClusters.clear();
        fTimeBin = 0;
        return;
    }

    shared_ptr<KTData> KTMultiSliceClustering::CreateDataFromCluster(const Cluster& cluster)
    {
        shared_ptr< KTData > data(new KTData());

        KTWaterfallCandidateData& wfcData = data->Of< KTWaterfallCandidateData >();
        wfcData.SetComponent(cluster.fDataComponent);

        UInt_t firstTimeBin = cluster.FirstTimeBin();
        UInt_t lastTimeBin = cluster.LastTimeBin();

        deque< pair< UInt_t, UInt_t > >::const_iterator frIt = cluster.fFreqRanges.begin();
        UInt_t firstFreqBin = frIt->first;
        UInt_t lastFreqBin = frIt->second;
        for (frIt++; frIt != cluster.fFreqRanges.end(); frIt++)
        {
            if (frIt->first < firstFreqBin) firstFreqBin = frIt->first;
            if (frIt->second > lastFreqBin) lastFreqBin = frIt->second;
        }

        Double_t timeBinWidth = cluster.fPoints.front().fHeaderPtr->GetSliceLength();
        Double_t freqBinWidth = cluster.fPoints.front().fSpectrumPtr->GetSpectrumPolar(0)->GetBinWidth();
        UInt_t nTimeBins = lastTimeBin - firstTimeBin + 1;
        UInt_t nFreqBins = lastFreqBin - firstFreqBin + 1;

        vector< boost::weak_ptr< KTFrequencySpectrumDataPolar > > spectra(nFreqBins);
        for (deque< ClusterPoint >::const_iterator it = cluster.fPoints.begin(); it != cluster.fPoints.end(); it++)
        {
            spectra[it->fTimeBin - nTimeBins] = boost::weak_ptr<KTFrequencySpectrumDataPolar>(it->fSpectrumPtr);
        }

        wfcData.SetTimeInRun(cluster.fPoints.front().fHeaderPtr->GetTimeInRun());
        wfcData.SetFirstSliceNumber(cluster.fPoints.front().fHeaderPtr->GetSliceNumber());
        wfcData.SetLastSliceNumber(cluster.fPoints.back().fHeaderPtr->GetSliceNumber());
        wfcData.SetTimeLength(timeBinWidth * Double_t(nTimeBins));
        wfcData.SetFrequencyWidth(freqBinWidth * Double_t(nFreqBins));

        KTTimeFrequency* tf = new KTTimeFrequencyPolar(nTimeBins, timeBinWidth * Double_t(firstTimeBin), timeBinWidth * Double_t(firstTimeBin + nTimeBins), nFreqBins, freqBinWidth * Double_t(firstFreqBin), freqBinWidth * Double_t(firstFreqBin + nFreqBins));
        for (UInt_t iTBin=firstTimeBin; iTBin <= lastTimeBin; iTBin++)
        {
            const KTFrequencySpectrum* spectrum = spectra[iTBin].lock()->GetSpectrum(cluster.fDataComponent);
            for (UInt_t iFBin=firstFreqBin; iFBin <= lastFreqBin; iFBin++)
            {
                tf->SetPolar(iTBin - firstTimeBin, iFBin - firstFreqBin, spectrum->GetAbs(iFBin), spectrum->GetArg(iFBin));
            }
        }
        wfcData.SetCandidate(tf);

        return data;
    }



    void KTMultiSliceClustering::ProcessOneSliceFSPolarData(shared_ptr<KTData> data)
    {
        if (! data->Has< KTDiscriminatedPoints1DData >())
        {
            KTWARN(sclog, "No discriminated-points data was present");
            return;
        }
        if (! data->Has< KTFrequencySpectrumDataPolar >())
        {
            KTWARN(sclog, "No frequency spectrum (polar) data was present");
            return;
        }
        // signal for any continued use of the input data
        fOneSliceDataSignal(data);
        DataList* clusteredData = FindClusters(data->Of< KTDiscriminatedPoints1DData >(), data->Of< KTFrequencySpectrumDataPolar >(), data->Of< KTSliceHeader >());
        if (clusteredData != NULL)
        {
            RunDataLoop(clusteredData);
        }
        return;
    }

    void KTMultiSliceClustering::ProcessOneSliceFSFFTWData(shared_ptr<KTData> data)
    {
        if (! data->Has< KTDiscriminatedPoints1DData >())
        {
            KTWARN(sclog, "No discriminated-points data was present");
            return;
        }
        if (! data->Has< KTFrequencySpectrumDataFFTW >())
        {
            KTWARN(sclog, "No frequency spectrum (FFTW) data was present");
            return;
        }
        // signal for any continued use of the input data
        fOneSliceDataSignal(data);
        DataList* clusteredData = FindClusters(data->Of< KTDiscriminatedPoints1DData >(), data->Of< KTFrequencySpectrumDataFFTW >(), data->Of< KTSliceHeader >());
        if (clusteredData != NULL)
        {
            RunDataLoop(clusteredData);
        }
        return;
    }

    void KTMultiSliceClustering::ProcessOneSliceCorrelationData(shared_ptr<KTData> data)
    {
        if (! data->Has< KTDiscriminatedPoints1DData >())
        {
            KTWARN(sclog, "No discriminated-points data was present");
            return;
        }
        if (! data->Has< KTCorrelationData >())
        {
            KTWARN(sclog, "No Correlation data was present");
            return;
        }
        // signal for any continued use of the input data
        fOneSliceDataSignal(data);
        DataList* clusteredData = FindClusters(data->Of< KTDiscriminatedPoints1DData >(), data->Of< KTCorrelationData >(), data->Of< KTSliceHeader >());
        if (clusteredData != NULL)
        {
            RunDataLoop(clusteredData);
        }
        return;
    }

    void KTMultiSliceClustering::ProcessOneSliceWVData(shared_ptr<KTData> data)
    {
        if (! data->Has< KTDiscriminatedPoints1DData >())
        {
            KTWARN(sclog, "No discriminated-points data was present");
            return;
        }
        if (! data->Has< KTWignerVilleData >())
        {
            KTWARN(sclog, "No Wigner-Ville data was present");
            return;
        }
        // signal for any continued use of the input data
        fOneSliceDataSignal(data);
        DataList* clusteredData = FindClusters(data->Of< KTDiscriminatedPoints1DData >(), data->Of< KTWignerVilleData >(), data->Of< KTSliceHeader >());
        if (clusteredData != NULL)
        {
            RunDataLoop(clusteredData);
        }
        return;
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


} /* namespace Katydid */
