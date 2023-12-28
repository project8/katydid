/*
 * KTDistanceClustering.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTDistanceClustering.hh"

#include "KTCluster1DData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTMath.hh"

#include <cmath>
#include <map>
#include <set>

using std::string;
using std::map;
using std::set;


namespace Katydid
{
    LOGGER(sdlog, "KTDistanceClustering");

    KT_REGISTER_PROCESSOR(KTDistanceClustering, "distance-clustering");

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

    bool KTDistanceClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("max-frequency-distance"))
        {
            SetMaxFrequencyDistance(node->get_value< double >("max-frequency-distance"));
        }

        if (node->has("max-bin-distance"))
        {
            SetMaxBinDistance(node->get_value< unsigned >("max-bin-distance"));
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

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const OriginalPoints points = data.GetSetOfPoints(iComponent);

            double threshold = 0.;
            if (! points.empty())
            {
                OriginalPoints::const_iterator pIt = points.begin();
                unsigned thisPoint = pIt->first;
                threshold = pIt->second.fThreshold;
                Cluster activeCluster;
                activeCluster.insert(thisPoint);
                unsigned lastPointInActiveCluster = thisPoint;

                for (pIt++; pIt != points.end(); pIt++)
                {
                    thisPoint = pIt->first;
                    if (thisPoint - lastPointInActiveCluster > fMaxBinDistance)
                    {
                        //LDEBUG(sdlog, "Adding cluster (ch. " << iChannel << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
                        newData.AddCluster(*(activeCluster.begin()), *(activeCluster.rbegin()), iComponent);
                        activeCluster.clear();
                    }
                    activeCluster.insert(thisPoint);
                    lastPointInActiveCluster = thisPoint;
                }
                LDEBUG(sdlog, "Adding cluster: (ch. " << iComponent << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
                newData.AddCluster(*(activeCluster.begin()), *(activeCluster.rbegin()), iComponent);
            }
            newData.SetThreshold(threshold, iComponent);

            LINFO(sdlog, newData.GetSetOfClusters(iComponent).size() << " clusters added on channel " << iComponent);
        }

        return true;
    }

    void KTDistanceClustering::Process1DData(Nymph::KTDataPtr data)
    {
        if (! data->Has< KTDiscriminatedPoints1DData >())
        {
            LERROR(sdlog, "No discriminated-points data was present");
            return;
        }
        if (! FindClusters(data->Of< KTDiscriminatedPoints1DData >()))
        {
            LERROR(sdlog, "Something went wrong while performing the cluster-finding");
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
