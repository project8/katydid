/*
 * KTCreateKDTree.cc
 *
 *  Created on: Aug 7, 2014
 *      Author: nsoblath
 */

#include "KTCreateKDTree.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
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
            fScalings(fNDimensions),
            fDataPtr(new KTData()),
            fTreeData(fDataPtr->Of< KTKDTreeData >()),
            fKDTreeSignal("kd-tree", this),
            fDiscPointsSlot("disc-1d", this, &KTCreateKDTree::AddPoints)
    {
    }

    KTCreateKDTree::~KTCreateKDTree()
    {
        RegisterSlot("make-tree", this, &KTCreateKDTree::MakeTreeSlot);
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

        if (node->Has("coord-scalings"))
        {
            const KTParamArray* scalings = node->ArrayAt("coord-scalings");
            if (scalings->Size() != fNDimensions)
            {
                KTERROR(kdlog, "Scalings array does not have the right number of dimensions: <" << scalings->Size() << "> instead of <" << fNDimensions << ">");
                return false;
            }
            fScalings[0] = scalings->GetValue< double >(0);
            fScalings[1] = scalings->GetValue< double >(1);
        }


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

        // invert the scalings to use multiplication later instead of division
        double xInvScaling = 1. / (fScalings[0] * KTMath::Sqrt2());
        double yInvScaling = 1. / (fScalings[1] * KTMath::Sqrt2());

        // verify that we have the right number of components
        unsigned nComponents = slHeader.GetNComponents();
        if (nComponents > fTreeData.GetNComponents())
        {
            fTreeData.SetNComponents(nComponents);
        }

        KTKDTreeData::Point newPoint;
        newPoint.fCoords[0] = xInvScaling * (slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength());
        for (unsigned iComponent = 0; iComponent != nComponents; ++iComponent)
        {
            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin();
                    pIt != incomingPts.end(); ++pIt)
            {
                newPoint.fCoords[1] = yInvScaling * pIt->second.fAbscissa;
                newPoint.fAmplitude = pIt->second.fOrdinate;
                fTreeData.AddPoint(newPoint, iComponent);
            }
            KTDEBUG(kdlog, "Tree data (component " << iComponent << ") now has " << fTreeData.GetSetOfPoints(iComponent).size() << " points");
        }
        return true;
    }

    bool KTCreateKDTree::MakeTree()
    {
        fTreeData.SetXScaling(fScalings[0]);
        fTreeData.SetYScaling(fScalings[1]);

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
