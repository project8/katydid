/*
 * KTCreateKDTree.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTCreateKDTree.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTDiscriminatedPoint.hh"

using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(kdlog, "KTCreateKDTree");

    KT_REGISTER_PROCESSOR(KTCreateKDTree, "create-kd-tree");

    const unsigned KTCreateKDTree::fNDimensions = 2;

    KTCreateKDTree::KTCreateKDTree(const std::string& name) :
            KTProcessor(name),
            fWindowSize(0),
            fWindowOverlap(0),
            fDistanceMethod(KTKDTreeData::kEuclidean),
            fMaxLeafSize(10),
            fTimeRadius(1.),
            fFreqRadius(1.),
            fTimeRadiusAlreadySet(false),
            fFreqRadiusAlreadySet(false),
            fDataPtr(new Nymph::KTData()),
            fTreeData(fDataPtr->Of< KTKDTreeData >()),
            fSliceInWindowCount(0),
            fInvScalingX(1.),
            fInvScalingY(1.),
            fHaveNewData(false),
            fKDTreeSignal("kd-tree", this),
            fDoneSignal("done", this),
            fHeaderSlot("header", this, &KTCreateKDTree::ReconfigureWithHeader),
            fDiscPointsSlot("disc-1d", this, &KTCreateKDTree::AddPoints),
            fSWFCAndPTSlot("swfc-and-track", this, &KTCreateKDTree::AddPoints),
            fSWFCSlot("swfc", this, &KTCreateKDTree::AddPoints),
            fDoneSlot("done", this, &KTCreateKDTree::MakeTreeSlot, &fDoneSignal)
    {
        RegisterSlot("make-tree", this, &KTCreateKDTree::MakeTreeSlot);
    }

    KTCreateKDTree::~KTCreateKDTree()
    {
    }

    bool KTCreateKDTree::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetWindowSize(node->get_value("window-size", GetWindowSize()));
        SetWindowOverlap(node->get_value("window-overlap", GetWindowOverlap()));

        if (node->has("distance-method"))
        {
            string distMethod = node->get_value("distance-method");
            if (distMethod == string("euclidean"))
            {
                SetDistanceMethod(KTKDTreeData::kEuclidean);
            }
            else if (distMethod == string("manhattan"))
            {
                SetDistanceMethod(KTKDTreeData::kManhattan);
            }
            else
            {
                KTERROR(kdlog, "Unknown distance method requested: " << distMethod);
            }
        }

        SetMaxLeafSize(node->get_value("max-leaf-size", GetMaxLeafSize()));

        if (node->has("time-radius")) SetTimeRadius(node->get_value< double >("time-radius"), true);
        if (node->has("freq-radius")) SetFreqRadius(node->get_value< double >("freq-radius"), true);

        return true;
    }

    bool KTCreateKDTree::ReconfigureWithHeader(KTEggHeader& header)
    {
        KTChannelHeader* chanHeader = header.GetChannelHeader(0);
        SetTimeRadius(chanHeader->GetRawSliceSize() / header.GetAcquisitionRate());
        SetFreqRadius(1. / GetTimeRadius());
        KTDEBUG(kdlog, "Reconfiguring with header: time radius = " << GetTimeRadius() << "; freq radius = " << GetFreqRadius());
        return true;
    }

    bool KTCreateKDTree::AddPoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints)
    {

        KTDEBUG(kdlog, "Is this a new acquisition? fHaveNewData=" << fHaveNewData << " and GetIsNewAcquisition=" << slHeader.GetIsNewAcquisition());
        // first check to see if this is a new acquisition; if so, run clustering on the previous acquistion's data
        if (fHaveNewData && slHeader.GetIsNewAcquisition())
        {
            KTDEBUG(kdlog, "New Acquisition - Run clustering on previous acquistion, clear out old points from tree");
            if (! MakeTree(false))
            {
                KTERROR(kdlog, "An error occurred while clustering from the previous acquisition");
                return false;
            }
            if (! ClearTree(false))
            {
                KTERROR(kdlog, "An error occurred while clearing the tree");
                return false;
            }
        }

        // update time and frequency bin widths, though they probably don't actually need to be updated
        //fTimeBinWidth = slHeader.GetSliceLength();
        //fFreqBinWidth = discPoints.GetBinWidth();

        // verify that we have the right number of components
        unsigned nComponents = slHeader.GetNComponents();
        if (nComponents > fTreeData.GetNComponents())
        {
            fTreeData.SetNComponents(nComponents);
        }
        fTreeData.SetAcquisitionID(slHeader.GetAcquisitionID());

        KTKDTreeData::Point newPoint;
        newPoint.fSliceNumber = slHeader.GetSliceNumber();
        if (newPoint.fSliceNumber > fTreeData.GetLastSlice()) fTreeData.SetLastSlice(newPoint.fSliceNumber);
        newPoint.fCoords[0] = fInvScalingX * (slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength());
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin();
                    pIt != incomingPts.end(); ++pIt)
            {
                newPoint.fCoords[1] = fInvScalingY * pIt->second.fAbscissa;
                newPoint.fAmplitude = pIt->second.fOrdinate;
                newPoint.fTimeInAcq = fInvScalingX * (slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength());
                newPoint.fMean = pIt->second.fMean;
                newPoint.fVariance = pIt->second.fVariance;
                newPoint.fNeighborhoodAmplitude = pIt->second.fNeighborhoodAmplitude;
                fTreeData.AddPoint(newPoint, iComponent);
            }
            KTDEBUG(kdlog, "Tree data (component " << iComponent << ") now has " << fTreeData.GetSetOfPoints(iComponent).size() << " points (Slice Number: " << newPoint.fSliceNumber << ")");
        }

        ++fSliceInWindowCount;
        KTDEBUG(kdlog, "Slice-count-in-window is now " << fSliceInWindowCount << " (window size = " << fWindowSize << "; window overlap = " << fWindowOverlap << ")");
        // if fSliceInWindowCount == fWindowSize, then we need to move the window before taking the next slice
        if (fWindowSize != 0 && fSliceInWindowCount == fWindowSize)
        {
            if (! MakeTree(true) || ! ClearTree(true, slHeader.GetSliceNumber() - fWindowOverlap + 1))
            {
                KTERROR(kdlog, "An error occurred while clustering or clearing the tree");
                return false;
            }
            fSliceInWindowCount = 0;
        }

        fHaveNewData = true;
        return true;
    }

    bool KTCreateKDTree::AddPoints(KTSparseWaterfallCandidateData& swfcData, KTProcessedTrackData& ptData)
    {
        if (ptData.GetIsCut())
        {
            return true;
        }
        return AddPoints(swfcData);
    }

    bool KTCreateKDTree::AddPoints(KTSparseWaterfallCandidateData& swfcData)
    {
        // verify that we have the right number of components
        unsigned component = swfcData.GetComponent();
        if (component >= fTreeData.GetNComponents())
        {
            fTreeData.SetNComponents(component + 1);
        }

        KTKDTreeData::Point newPoint;
        newPoint.fSliceNumber = 0; // slice number isn't available in KTSparseWaterfallCandidateData
        if (newPoint.fSliceNumber > fTreeData.GetLastSlice()) fTreeData.SetLastSlice(newPoint.fSliceNumber);
        const KTDiscriminatedPoints& incomingPts = swfcData.GetPoints();
        for (KTDiscriminatedPoints::const_iterator pIt = incomingPts.begin();
                pIt != incomingPts.end(); ++pIt)
        {
            newPoint.fCoords[0] = fInvScalingX * pIt->fTimeInRunC;
            newPoint.fCoords[1] = fInvScalingY * pIt->fFrequency;
            newPoint.fAmplitude = pIt->fAmplitude;
            newPoint.fTimeInAcq = fInvScalingX * pIt->fTimeInAcq;
            newPoint.fMean = pIt->fMean;
            newPoint.fVariance = pIt->fVariance;
            newPoint.fNeighborhoodAmplitude = pIt->fNeighborhoodAmplitude;
            fTreeData.AddPoint(newPoint, component);
        }
        KTDEBUG(kdlog, "Tree data (component " << component << ") now has " << fTreeData.GetSetOfPoints(component).size() << " points");

        fHaveNewData = true;
        return true;
    }

    bool KTCreateKDTree::MakeTree(bool willContinue)
    {
        if (! fHaveNewData) return true;

        KTINFO(kdlog, "Creating k-d tree; last slice is " << fTreeData.GetLastSlice());
        KTDEBUG(kdlog, "Tree will continue: " << willContinue);

        fTreeData.SetDataWillContinue(willContinue);
        fTreeData.SetXScaling(fTimeRadius);
        fTreeData.SetYScaling(fFreqRadius);
        KTINFO(kdlog, "Scalings for k-d tree points: (" << fTimeRadius << ", " << fFreqRadius << ")");

        unsigned nComponents = fTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            fTreeData.BuildIndex(fDistanceMethod, fMaxLeafSize, iComponent);
        }

        // yet another exception to the separation of normal function and signals/slots; sorry
        fKDTreeSignal(fDataPtr);

        // LdV 2015-01-14 - Commented the line below
        //                - fHaveNewData has to remain unchanged until ClearTree; it shouldn't be changed here
        //                - This probably wasn't noticed before because we weren't using multi-record files with the digitizer data
        //fHaveNewData = false;
        return true;
    }

    bool KTCreateKDTree::ClearTree(bool willContinue, uint64_t firstSliceKept)
    {
        KTDEBUG(kdlog, "ClearTree");
        if (! fHaveNewData) return true;

        // firstSliceKept is only used if willContinue == true
        if (willContinue)
        {
            KTDEBUG(kdlog, "ClearTree(true)");

            // clear data up to the
            unsigned nComponents = fTreeData.GetNComponents();
            for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
            {
                fTreeData.ClearIndex(iComponent);
                KTKDTreeData::SetOfPoints& points = fTreeData.GetSetOfPoints(iComponent);
                KTKDTreeData::SetOfPoints::iterator startErase = points.begin();
                KTKDTreeData::SetOfPoints::iterator endErase = startErase;
                while (endErase != points.end() && endErase->fSliceNumber < firstSliceKept)
                {
                    ++endErase;
                }
                points.erase(startErase, endErase);
            }
        }
        else
        {
            KTDEBUG(kdlog, "ClearTree(false)");
            // clear all data from the tree, but leave the memory intact
            unsigned nComponents = fTreeData.GetNComponents();
            for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
            {
                fTreeData.ClearPoints(iComponent);
            }
        }

        fHaveNewData = false;
        return true;
    }

    void KTCreateKDTree::MakeTreeSlot()
    {
        if (! MakeTree(false))
        {
            KTERROR(kdlog, "An error occurred while making the k-d tree");
            return;
        }
        if (! ClearTree(false))
        {
            KTERROR(kdlog, "An error occurred while clearing the tree");
        }
        return;
    }

} /* namespace Katydid */
