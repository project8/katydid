/*
 * KTCreateKDTree.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTCreateKDTree.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTLogger.hh"
#include "KTParam.hh"
#include "KTSliceHeader.hh"

using std::string;


namespace Katydid
{
    KTLOGGER(kdlog, "KTCreateKDTree");

    KT_REGISTER_PROCESSOR(KTCreateKDTree, "create-kd-tree");

    KTCreateKDTree::KTCreateKDTree(const std::string& name) :
            KTProcessor(name),
            fDistanceMethod(KTKDTreeData::kEuclidean),
            fMaxLeafSize(10),
            fDataPtr(new KTData()),
            fTreeData(fDataPtr->Of< KTKDTreeData >()),
            fKDTreeSignal("kd-tree", this),
            fDiscPointsSlot("fs-fftw", this, &KTCreateKDTree::AddPoints)
    {
    }

    KTCreateKDTree::~KTCreateKDTree()
    {
        RegisterSlot("make-tree", this, &KTCreateKDTree::MakeTreeSlot);
    }

    bool KTCreateKDTree::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetMaxLeafSize(node->GetValue("max-leaf-size", GetMaxLeafSize()));

        return true;
    }

    bool KTCreateKDTree::AddPoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints)
    {
        // first check to see if this is a new acquisition; if so, run clustering on the previous acquistion's data
        if (slHeader.GetIsNewAcquisition())
        {
            if (! MakeTree())
            {
                KTERROR(kdlog, "An error occurred while clustering from the previous acquisition");
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

        KTKDTreeData::Point newPoint;
        newPoint.fX = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin();
                    pIt != incomingPts.end(); ++pIt)
            {
                newPoint.fY = pIt->second.fAbscissa;
                newPoint.fAmplitude = pIt->second.fOrdinate;
                fTreeData.AddPoint(newPoint, iComponent);
            }
            KTDEBUG(kdlog, "Tree data (component " << iComponent << ") now has " << fTreeData.GetSetOfPoints(iComponent).size() << " points");
        }
        return true;
    }

    bool KTCreateKDTree::MakeTree()
    {
        unsigned nComponents = fTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            fTreeData.CreateIndex(fDistanceMethod, fMaxLeafSize, iComponent);
        }
        return true;
    }

    void KTCreateKDTree::MakeTreeSlot()
    {
        if (! MakeTree())
        {
            KTERROR(kdlog, "An error occurred while making the k-d tree");
        }
        return;
    }

} /* namespace Katydid */
