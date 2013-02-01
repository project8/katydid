/*
 * KTSimpleCusteringProcessor.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTSimpleClustering.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTPStoreNode.hh"

#include <set>

using boost::shared_ptr;

using std::deque;
using std::list;
using std::set;
using std::string;
using std::vector;

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTSimpleClustering > sSimpClustRegistrar("simple-clustering");

    KTSimpleClustering::KTSimpleClustering() :
            KTProcessor(),
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
        fConfigName = "simple-clustering";

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

        SetInputDataName(node->GetData< string >("fs-input-data-name", fInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

        return true;
    }

    KTSimpleClustering::NewEventList* KTSimpleClustering::AddPointsToClusters(const KTDiscriminatedPoints1DData* dpData)
    {
        if (dpData->GetBinWidth() != fFreqBinWidth)
            SetFrequencyBinWidth(dpData->GetBinWidth());
        if (dpData->GetBinWidth() * Double_t(dpData->GetNBins()) != fTimeBinWidth)
            SetTimeBinWidth(dpData->GetBinWidth() * Double_t(dpData->GetNBins()));

        UInt_t nComponents = dpData->GetNChannels();

        NewEventList* newEventsAC = new NewEventList();

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            NewEventList* eventsFromComponent = AddPointsToClusters(dpData->GetSetOfPoints(iComponent), iComponent);
            newEventsAC->splice(newEventsAC->end(), *eventsFromComponent);
            delete eventsFromComponent;
        }

        fTimeBin++;

        return newEventsAC;
    }

    KTSimpleClustering::NewEventList* KTSimpleClustering::AddPointsToClusters(const SetOfDiscriminatedPoints& points, UInt_t component)
    {
        // Process a single time bin's worth of frequency bins

        typedef list< SetOfDiscriminatedPoints > FreqBinClusters;
        FreqBinClusters freqBinClusters;

        // First cluster the frequency bins in this time bin
        if (! points.empty())
        {
            // loop over all of the points
            SetOfDiscriminatedPoints::const_iterator pIt = points.begin();
            SetOfDiscriminatedPoints activeFBCluster;
            activeFBCluster.insert(*pIt);
            UInt_t thisPoint;
            UInt_t lastPointInActiveCluster = pIt->first;

            for (pIt++; pIt != points.end(); pIt++)
            {
                thisPoint = pIt->first;
                if (thisPoint - lastPointInActiveCluster > fMaxFreqSepBins)
                {
                    //KTDEBUG(sdlog, "Adding cluster (ch. " << iChannel << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
                    freqBinClusters.push_back(activeFBCluster);
                    activeFBCluster.clear();
                }
                activeFBCluster.insert(*pIt);
                lastPointInActiveCluster = thisPoint;
            }
            //KTDEBUG(sdlog, "Adding cluster: (ch. " << iChannel << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
            freqBinClusters.push_back(activeFBCluster);
        }


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



        // loop over all of the active clusters
        deque< std::pair< UInt_t, UInt_t > >::const_iterator arIt = activeClusterFBRanges.begin();
        for (ActiveClusters::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end(); acIt++, arIt++)
        {
            // loop over all of the frequency-bin clusters
            for (FreqBinClusters::const_iterator fbIt = freqBinClusters.begin(); fbIt != freqBinClusters.end(); fbIt++)
            {
                // check for overlap
                // y1 <= x2+sep  && x1 <= y2+sep
                // x1 = (fbIt->begin())->first; x2 = (fbIt->rbegin())->first
                // y1 = arIt->first; y2 = arIt->second
                if (arIt->first <= (fbIt->rbegin())->first + fMaxFreqSepBins &&
                    (fbIt->begin())->first <= arIt->second + fMaxFreqSepBins)
                {
                    // assign this frequency-bin cluster (at fbIt) to the current active cluster (at acIt)
                    // add points to acIt->fPoints
                    for (SetOfDiscriminatedPoints::const_iterator fbPointsIt = fbIt->begin(); fbPointsIt != fbIt->end(); fbPointsIt++)
                    {
                        ClusterPoint newPoint;
                        newPoint.fTimeBin = this->fTimeBin;
                        newPoint.fFreqBin = fbPointsIt->first;
                        newPoint.fAmplitude = fbPointsIt->second;
                        acIt->fPoints.push_back(newPoint);
                    }
                    // add range to acIt->fFreqRanges

                }
            }
        }



        return CompleteInactiveClusters(component);
    }


    KTSimpleClustering::NewEventList* KTSimpleClustering::CompleteAllClusters(UInt_t component)
    {
        NewEventList* newEvents = new NewEventList();

        for (ActiveClusters::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end();)
        {
            newEvents->push_back(CreateEventFromCluster(*acIt));
            acIt = fActiveClusters[component].erase(acIt);
        }

        return newEvents;
    }

    KTSimpleClustering::NewEventList* KTSimpleClustering::CompleteInactiveClusters(UInt_t component)
    {
        NewEventList* newEvents = new NewEventList();

        for (ActiveClusters::iterator acIt = fActiveClusters[component].begin(); acIt != fActiveClusters[component].end();)
        {
            UInt_t lastTimeBinInCluster = (*acIt).fPoints.back().fTimeBin;
            if (fTimeBin - lastTimeBinInCluster > fMaxTimeSepBins)
            {
                newEvents->push_back(CreateEventFromCluster(*acIt));
                acIt = fActiveClusters[component].erase(acIt);
            }
            else
            {
                acIt++;
            }
        }

        return newEvents;
    }

    void KTSimpleClustering::Reset()
    {
        fActiveClusters.clear();
        fTimeBin = 0;
        return;
    }

    shared_ptr<KTEvent> KTSimpleClustering::CreateEventFromCluster(const Cluster& cluster)
    {
        shared_ptr<KTEvent> event(new KTEvent());

        //KTSingleCluster2DData* newData = new KTSingleCluster2DData();

        // fill in the data

        //data->SetEvent(event);
        //event->AddData(newData);

        return event;
    }



} /* namespace Katydid */









#ifdef BLAH

#include "KTFactory.hh"
#include "KTMaskedArray.hh"
#include "KTPhysicalArray.hh"
#include "KTFrequencySpectrum.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFFT.hh"
#include "KTSlidingWindowFSData.hh"

#ifdef ROOT_FOUND
#include "TCanvas.h"
#include "TH1.h"
#endif

#include <iostream>
#include <set>
#include <utility>

using std::list;
using std::multimap;
using std::set;
using std::pair;

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTSimpleClustering > sSimpClustRegistrar("simple-clustering");

    KTSimpleClustering::KTSimpleClustering() :
            KTProcessor(),
            fEventPeakBins(NULL),
            fThresholdMult(8.),
            fBinCuts(NULL),
            fMinimumGroupSize(2),
            fGroupBinsMarginLow(1),
            fGroupBinsMarginHigh(3),
            fGroupBinsMarginSameTime(1),
            fFirstBinToUse(0),
            fDrawFlag(kFALSE)
    {
        fConfigName = "sliding-window-fft";

        RegisterSlot("freq_spect", this, &KTSimpleClustering::ProcessFrequencySpectrum, "void (UInt_t, KTFrequencySpectrum*)");
    }

    KTSimpleClustering::~KTSimpleClustering()
    {
        delete fBinCuts;
    }

    Bool_t KTSimpleClustering::Configure(const KTPStoreNode* node)
    {
        if (node != NULL)
        {
            fThresholdMult = node->GetData< Double_t >("threshold-multiplier", 10.0);
            fGroupBinsMarginHigh = node->GetData< Int_t >("group-bins-margin-high", 1);
            fGroupBinsMarginLow = node->GetData< Int_t >("group-bins-margin-low", 3);
            fGroupBinsMarginSameTime = node->GetData< Int_t >("group-bins-margin-same-time", 1);
            fFirstBinToUse = node->GetData< UInt_t >("first-bin-to-use", 1);
            fDrawFlag = node->GetData< Bool_t >("draw-flag", false);
        }

        // No CL options

        return true;
    }


    void KTSimpleClustering::ProcessSlidingWindowFFT(KTSlidingWindowFSData* swFSData)
    {
        KTPhysicalArray< 1, KTFrequencySpectrum* >* spectra = swFSData->GetSpectra(0);
        UInt_t nPowerSpectra = spectra->size();
        for (UInt_t iPS=0; iPS<nPowerSpectra; iPS++)
        {
            ProcessFrequencySpectrum(iPS, (*spectra)(iPS));
        }

        return;
    }

    void KTSimpleClustering::ProcessFrequencySpectrum(UInt_t psNum, KTFrequencySpectrum* freqSpectrum)
    {
        // Look for the highest-peaked bins in this power spectrum

        // this will hold the bin numbers that are above the threshold
        set< Int_t > peakBins;

        KTFrequencySpectrum::array_type dataArray = freqSpectrum->GetData().data();
        UInt_t nBins = (UInt_t)freqSpectrum->size();

        KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* localBinCuts = fBinCuts;
        if (nBins != fBinCuts->GetArraySize())
        {
            std::cout << "Warning from KTSimpleClustering::ProcessPowerSpectrum: size from power spectrum does not match bin cut array size" << std::endl;
            localBinCuts = new KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >(dataArray, nBins);
        }
        else
        {
            fBinCuts->ChangeArray(dataArray);
        }
        UInt_t nCutBins = localBinCuts->size();

        //Double_t mean = TMath::Mean(nBins-fFirstBinToUse, dataArray+fFirstBinToUse);
        Double_t mean = 0., value = 0.;
        const UInt_t firstCutBinToUse = localBinCuts->FindCutPositionOrNext(fFirstBinToUse);
        for (UInt_t iCutBin=firstCutBinToUse; iCutBin<nCutBins; iCutBin++)
        {
            value = (*localBinCuts)[iCutBin].abs();
            mean += value * value;
        }
        //cout << "   Mean: " << mean << endl;

        Double_t threshold = fThresholdMult * mean;

        /*
        std::cout << "mean: " << mean << "  threshold: " << threshold << std::endl;

        for (unsigned int iBin=0; iBin<fFirstBinToUse; iBin++)
        {
            std::cout << "  " << powerSpectrum->GetMagnitudeAt((Int_t)iBin) << "  " << dataArray[iBin] << std::endl;
        }
        std::cout << std::endl;
        for (unsigned int iBin=fFirstBinToUse; iBin<100; iBin++)
        {
            std::cout << "  " << powerSpectrum->GetMagnitudeAt((Int_t)iBin) << "  " << dataArray[iBin] << std::endl;
        }
        */

        // search for bins above the threshold
        //UInt_t firstCutBinToUse = localBinCuts->FindCutPositionOrNext(fFirstBinToUse);
        for (UInt_t iCutBin=firstCutBinToUse; iCutBin<nCutBins; iCutBin++)
        {
            /*
            if (dataArray[iBin] > threshold)
            {
                peakBins.insert(iBin);
            }
            */
            value = (*localBinCuts)[iCutBin].abs();
            if (value*value > threshold)
            {
                peakBins.insert(localBinCuts->GetArrayPosition(iCutBin));
            }
        }
        //std::cout << "FFT " << psNum << " -- Peak bins: " << peakBins.size() << std::endl;

#ifdef ROOT_FOUND
        /**/// DEBUG
        if (fDrawFlag && psNum < 5)
        //if (psNum < 5)
        {
            TCanvas *cSCP = new TCanvas("cSCP", "cSCP");
            TH1D* histPS = freqSpectrum->CreatePowerHistogram("ps_scp");
            cSCP->SetLogy(1);
            char projnum[30];
            sprintf(projnum, "%s%i", "fft #", psNum);
            histPS->SetTitle(projnum);
            histPS->Draw();
            cSCP->WaitPrimitive();
            //cSCP->Print(outputFileNamePS.c_str());
            delete cSCP;
        }
        /**/
#endif

        // Look for groups
        for (set< Int_t >::iterator iPB=peakBins.begin(); iPB!=peakBins.end(); iPB++)
        {
            Int_t pbVal = *iPB;
            Bool_t foundGroup = kFALSE;
            for (list< multimap< Int_t, Int_t >* >::iterator iEPB=fEventPeakBins->begin(); iEPB!=fEventPeakBins->end(); iEPB++)
            {
                multimap< Int_t, Int_t >* groupMap = *iEPB;
                multimap< Int_t, Int_t >::iterator lastGroup = groupMap->end();
                lastGroup--;
                UInt_t lastFFT = (UInt_t)lastGroup->first;
                // check if we've passed this group, and if so, if the group is too small, remove it
                if (lastFFT < psNum - 1 && (UInt_t)groupMap->size() <= fMinimumGroupSize)
                {
                    delete groupMap;
                    iEPB = fEventPeakBins->erase(iEPB);
                    iEPB--; // move the iterator back one so we don't skip anything when the for loop advances the iterator
                    continue;
                }
                pair< multimap< Int_t, Int_t >::iterator, multimap< Int_t, Int_t >::iterator > lastFFTRange =
                        groupMap->equal_range(lastFFT);
                multimap< Int_t, Int_t >::iterator firstGroupInRange = lastFFTRange.first;
                Int_t firstGroupFreqBin = firstGroupInRange->second;
                Int_t lastGroupFreqBin = firstGroupFreqBin;
                firstGroupInRange++;
                for (multimap< Int_t, Int_t >::iterator grIt=firstGroupInRange; grIt!=lastFFTRange.second; grIt++)
                {
                    if (grIt->second > lastGroupFreqBin) lastGroupFreqBin = grIt->second;
                    else if (grIt->second < firstGroupFreqBin) firstGroupFreqBin = grIt->second;
                }

                if ((UInt_t)lastFFT == psNum)
                {
                    if (pbVal >= firstGroupFreqBin - fGroupBinsMarginSameTime && pbVal <= lastGroupFreqBin + fGroupBinsMarginSameTime)
                    {
                        groupMap->insert( pair< Int_t, Int_t >((Int_t)psNum, pbVal) );
                        foundGroup = kTRUE;
                        break;
                    }
                }
                else if ((UInt_t)lastFFT == psNum - 1)
                {
                    if (pbVal >= firstGroupFreqBin - fGroupBinsMarginLow && pbVal <= lastGroupFreqBin + fGroupBinsMarginHigh)
                    {
                        groupMap->insert( pair< Int_t, Int_t >((Int_t)psNum, pbVal) );
                        foundGroup = kTRUE;
                        break;
                    }
                }
            }
            if (foundGroup) continue;

            // no match to existing groups, so add a new one
            multimap< Int_t, Int_t >* newGroupMap = new multimap< Int_t, Int_t >();
            newGroupMap->insert( pair< Int_t, Int_t >((Int_t)psNum, pbVal) );
            fEventPeakBins->push_back(newGroupMap);
        }

        return;
    }
} /* namespace Katydid */
#endif
