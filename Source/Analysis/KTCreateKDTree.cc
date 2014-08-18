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
using std::vector;

namespace Katydid
{
    KTLOGGER(kdlog, "KTCreateKDTree");

    KT_REGISTER_PROCESSOR(KTCreateKDTree, "create-kd-tree");

    const unsigned KTCreateKDTree::fNDimensions = 2;

    KTCreateKDTree::KTCreateKDTree(const std::string& name) :
            KTProcessor(name),
            fDistanceMethod(KTKDTreeData::kEuclidean),
            fMaxLeafSize(10),
            fTimeRadius(1.),
            fFreqRadius(1.),
            fDataPtr(new KTData()),
            fTreeData(fDataPtr->Of< KTKDTreeData >()),
            fInvScalingX(1.),
            fInvScalingY(1.),
            fKDTreeSignal("kd-tree", this),
            fDiscPointsSlot("disc-1d", this, &KTCreateKDTree::AddPoints)
    {
        RegisterSlot("make-tree", this, &KTCreateKDTree::MakeTreeSlot);
    }

    KTCreateKDTree::~KTCreateKDTree()
    {
    }

    bool KTCreateKDTree::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        if (node->Has("distance-method"))
        {
            string distMethod = node->GetValue("distance-method");
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

        SetMaxLeafSize(node->GetValue("max-leaf-size", GetMaxLeafSize()));

        SetTimeRadius(node->GetValue("time-radius", GetTimeRadius()));
        SetFreqRadius(node->GetValue("freq-radius", GetFreqRadius()));

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
        newPoint.fSliceNumber = slHeader.GetSliceNumber();
        newPoint.fCoords[0] = fInvScalingX * (slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength());
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin();
                    pIt != incomingPts.end(); ++pIt)
            {
                newPoint.fCoords[1] = fInvScalingY * pIt->second.fAbscissa;
                newPoint.fAmplitude = pIt->second.fOrdinate;
                fTreeData.AddPoint(newPoint, iComponent);
            }
            KTDEBUG(kdlog, "Tree data (component " << iComponent << ") now has " << fTreeData.GetSetOfPoints(iComponent).size() << " points");
        }
        return true;
    }

    bool KTCreateKDTree::MakeTree()
    {
        KTINFO(kdlog, "Creating k-d tree");

        fTreeData.SetXScaling(fTimeRadius);
        fTreeData.SetYScaling(fFreqRadius);
        KTINFO(kdlog, "Scalings for k-d tree points: (" << fTimeRadius << ", " << fFreqRadius << ")");

        unsigned nComponents = fTreeData.GetNComponents();
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            fTreeData.BuildIndex(fDistanceMethod, fMaxLeafSize, iComponent);
        }

        return true;
    }

    void KTCreateKDTree::MakeTreeSlot()
    {
        if (! MakeTree())
        {
            KTERROR(kdlog, "An error occurred while making the k-d tree");
            return;
        }
        fKDTreeSignal(fDataPtr);
        return;
    }

} /* namespace Katydid */
