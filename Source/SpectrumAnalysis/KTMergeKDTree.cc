/*
 * KTMergeKDTree.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTMergeKDTree.hh"

#include "KTKDTreeData.hh"
#include "logger.hh"

using std::string;


namespace Katydid
{
    LOGGER(mkdlog, "KTMergeKDTree");

    KT_REGISTER_PROCESSOR(KTMergeKDTree, "merge-kd-tree");

    KTMergeKDTree::KTMergeKDTree(const std::string& name) :
            KTProcessor(name),
            fDataPtr(new Nymph::KTData()),
            fMergedTreeData(fDataPtr->Of< KTKDTreeData >()),
            fHaveNewData(false),
            fKDTreeSignal("kd-tree-out", this),
            fDoneSignal("done", this),
            fKDTreeSlot("kd-tree-in", this, &KTMergeKDTree::MergeTree),
            fDoneSlot("done", this, &KTMergeKDTree::FinishTreeSlot, &fDoneSignal)
    {
        RegisterSlot("finish-tree", this, &KTMergeKDTree::FinishTreeSlot);
    }

    KTMergeKDTree::~KTMergeKDTree()
    {
    }

    bool KTMergeKDTree::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        // no values

        return true;
    }

    bool KTMergeKDTree::MergeTree(KTKDTreeData& kdTreeData)
    {
        fMergedTreeData.SetDataWillContinue(false);
        fMergedTreeData.SetXScaling(kdTreeData.GetXScaling());
        fMergedTreeData.SetYScaling(kdTreeData.GetYScaling());

        unsigned nComponents = kdTreeData.GetNComponents();
        if (nComponents > fMergedTreeData.GetNComponents())
        {
            fMergedTreeData.SetNComponents(nComponents);
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            MergeTree(kdTreeData.GetSetOfPoints(iComponent), kdTreeData.GetDistanceMethod(iComponent), kdTreeData.GetMaxLeafSize(iComponent), iComponent);
        }

        // change the last slice number for the merged tree _after_ merging points in all components
        if (kdTreeData.GetLastSlice() > fMergedTreeData.GetLastSlice()) fMergedTreeData.SetLastSlice(kdTreeData.GetLastSlice());

        fHaveNewData = true;

        if (! kdTreeData.GetDataWillContinue())
        {
            return FinishTree() && ClearTree();
        }
        return true;
    }

    bool KTMergeKDTree::MergeTree(const KTKDTreeData::SetOfPoints& setOfPoints, KTKDTreeData::DistanceMethod dist, unsigned maxLeafSize, unsigned component)
    {
        fMergedTreeData.SetDistanceMethod(dist, component);
        fMergedTreeData.SetMaxLeafSize(maxLeafSize);

        KTKDTreeData::SetOfPoints& destination = fMergedTreeData.GetSetOfPoints(component);
        for (KTKDTreeData::SetOfPoints::const_iterator pIt = setOfPoints.begin(); pIt != setOfPoints.end(); ++pIt)
        {
            if (pIt->fSliceNumber > fMergedTreeData.GetLastSlice() && ! pIt->fNoiseFlag) destination.push_back(*pIt);

        }
        return true;
    }

    bool KTMergeKDTree::FinishTree()
    {
        if (! fHaveNewData) return true;

        LINFO(mkdlog, "Creating merged k-d tree");

        unsigned nComponents = fMergedTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            fMergedTreeData.BuildIndex(iComponent);
        }

        // yet another exception to the separation of regular functions and signals/slots; sorry
        fKDTreeSignal(fDataPtr);

        fHaveNewData = false;
        return true;
    }

    bool KTMergeKDTree::ClearTree()
    {
        // clear all data from the tree, but leave the memory intact
        unsigned nComponents = fMergedTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            fMergedTreeData.ClearPoints(iComponent);
        }
        fHaveNewData = false;
        return true;
    }

    void KTMergeKDTree::FinishTreeSlot()
    {
        if (! FinishTree())
        {
            LERROR(mkdlog, "An error occurred while finishing the merged k-d tree");
            return;
        }
        ClearTree();
        return;
    }


} /* namespace Katydid */
