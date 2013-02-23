/*
 * KTDistanceClustering.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTDistanceClustering.hh"

#include "KTCluster1DData.hh"
//#include "KTCluster2DData.hh"
#include "KTDiscriminatedPoints1DData.hh"
//#include "KTDiscriminatedPoints2DData.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"

#include <cmath>
#include <map>
#include <set>

using std::string;
using std::map;
using std::set;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(sdlog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTDistanceClustering > sSimpleFFTRegistrar("distance-clustering");

    KTDistanceClustering::KTDistanceClustering() :
            KTProcessor(),
            fMaxFrequencyDistance(1.),
            fMaxBinDistance(1),
            fCalculateMaxBinDistance(true)
    {
        fConfigName = "distance-clustering";

        RegisterSignal("cluster-1d", &fCluster1DSignal, "void (const KTCluster1DData*)");
        //RegisterSignal("cluster-2d", &fCluster2DSignal, "void (const KTCluster2DData*)");

        RegisterSlot("disc-1d", this, &KTDistanceClustering::Process1DData, "void (shared_ptr<KTData>)");
        //RegisterSlot("disc-2d", this, &KTDistanceClustering::Process2DData, "void (const KTDiscriminatedPoints2DData*)");
    }

    KTDistanceClustering::~KTDistanceClustering()
    {
    }

    Bool_t KTDistanceClustering::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("max-frequency-distance"))
        {
            SetMaxFrequencyDistance(node->GetData< Double_t >("max-frequency-distance"));
        }

        if (node->HasData("max-bin-distance"))
        {
            SetMaxBinDistance(node->GetData< UInt_t >("max-bin-distance"));
        }

        return true;
    }

    Bool_t KTDistanceClustering::FindClusters(KTDiscriminatedPoints1DData& data)
    {
        typedef KTDiscriminatedPoints1DData::SetOfPoints OriginalPoints;
        typedef set< UInt_t > Cluster;

        if (fCalculateMaxBinDistance) SetMaxBinDistance(KTMath::Nint(fMaxFrequencyDistance / data.GetBinWidth()));

        UInt_t nComponents = data.GetNComponents();

        KTCluster1DData& newData = data.Of< KTCluster1DData >().SetNComponents(nComponents);
        newData.SetNBins(data.GetNBins());
        newData.SetBinWidth(data.GetBinWidth());

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            newData.SetThreshold(data.GetThreshold(iComponent), iComponent);

            const OriginalPoints points = data.GetSetOfPoints(iComponent);

            if (! points.empty())
            {
                OriginalPoints::const_iterator pIt = points.begin();
                UInt_t thisPoint = pIt->first;
                Cluster activeCluster;
                activeCluster.insert(thisPoint);
                UInt_t lastPointInActiveCluster = thisPoint;

                for (pIt++; pIt != points.end(); pIt++)
                {
                    thisPoint = pIt->first;
                    if (thisPoint - lastPointInActiveCluster > fMaxBinDistance)
                    {
                        //KTDEBUG(sdlog, "Adding cluster (ch. " << iChannel << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
                        newData.AddCluster(*(activeCluster.begin()), *(activeCluster.rbegin()), iComponent);
                        activeCluster.clear();
                    }
                    activeCluster.insert(thisPoint);
                    lastPointInActiveCluster = thisPoint;
                }
                KTDEBUG(sdlog, "Adding cluster: (ch. " << iComponent << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
                newData.AddCluster(*(activeCluster.begin()), *(activeCluster.rbegin()), iComponent);
            }

            KTDEBUG(sdlog, newData.GetSetOfClusters(iComponent).size() << " clusters added on channel " << iComponent);
        }

        return true;
    }

    void KTDistanceClustering::Process1DData(shared_ptr< KTData > data)
    {
        if (! data->Has< KTDiscriminatedPoints1DData >())
        {
            KTERROR(sdlog, "No discriminated-points data was present");
            return;
        }
        if (! FindClusters(data->Of< KTDiscriminatedPoints1DData >()))
        {
            KTERROR(sdlog, "Something went wrong while performing the cluster-finding");
            return;
        }
        fCluster1DSignal(data);
        return;
    }
    /*
    void KTDistanceClustering::Process2DData(const KTDiscriminatedPoints2DData* data)
    {

    }
    */
} /* namespace Katydid */
