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
#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"

#include <cmath>
#include <map>
#include <set>

using std::string;
using std::map;
using std::set;


namespace Katydid
{
    KTLOGGER(sdlog, "katydid.analysis");

    static KTDerivedNORegistrar< KTProcessor, KTDistanceClustering > sDistClustRegistrar("distance-clustering");

    KTDistanceClustering::KTDistanceClustering(const std::string& name) :
            KTProcessor(name),
            fMaxFrequencyDistance(1.),
            fMaxBinDistance(1),
            fCalculateMaxBinDistance(true),
            fCluster1DSignal("cluster-1d", this),
            fDiscPoints1DSlot("disc-1d", this, &KTDistanceClustering::FindClusters, &fCluster1DSignal)
    {
    }

    KTDistanceClustering::~KTDistanceClustering()
    {
    }

    bool KTDistanceClustering::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("max-frequency-distance"))
        {
            SetMaxFrequencyDistance(node->GetData< double >("max-frequency-distance"));
        }

        if (node->HasData("max-bin-distance"))
        {
            SetMaxBinDistance(node->GetData< unsigned >("max-bin-distance"));
        }

        return true;
    }

    bool KTDistanceClustering::FindClusters(KTDiscriminatedPoints1DData& data)
    {
        typedef KTDiscriminatedPoints1DData::SetOfPoints OriginalPoints;
        typedef set< unsigned > Cluster;

        if (fCalculateMaxBinDistance) SetMaxBinDistance(KTMath::Nint(fMaxFrequencyDistance / data.GetBinWidth()));

        unsigned nComponents = data.GetNComponents();

        KTCluster1DData& newData = data.Of< KTCluster1DData >().SetNComponents(nComponents);
        newData.SetNBins(data.GetNBins());
        newData.SetBinWidth(data.GetBinWidth());

        for (unsigned iComponent=0; iComponent<nComponents; iComponent++)
        {
            newData.SetThreshold(data.GetThreshold(iComponent), iComponent);

            const OriginalPoints points = data.GetSetOfPoints(iComponent);

            if (! points.empty())
            {
                OriginalPoints::const_iterator pIt = points.begin();
                unsigned thisPoint = pIt->first;
                Cluster activeCluster;
                activeCluster.insert(thisPoint);
                unsigned lastPointInActiveCluster = thisPoint;

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

            KTINFO(sdlog, newData.GetSetOfClusters(iComponent).size() << " clusters added on channel " << iComponent);
        }

        return true;
    }

    void KTDistanceClustering::Process1DData(KTDataPtr data)
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
