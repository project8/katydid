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
#include "KTEvent.hh"
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
            fCalculateMaxBinDistance(true),
            fInputDataName("frequency-spectrum"),
            fOutputDataName("peak-list")
    {
        fConfigName = "distance-clustering";

        RegisterSignal("cluster-1d", &fCluster1DSignal, "void (const KTCluster1DData*)");
        //RegisterSignal("cluster-2d", &fCluster2DSignal, "void (const KTCluster2DData*)");

        RegisterSlot("event", this, &KTDistanceClustering::ProcessEvent, "void (shared_ptr<KTEvent>)");
        RegisterSlot("disc-1d-data", this, &KTDistanceClustering::Process1DData, "void (const KTDiscriminatedPoints1DData*)");
        //RegisterSlot("disc-2d-data", this, &KTDistanceClustering::Process2DData, "void (const KTDiscriminatedPoints2DData*)");
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

        SetInputDataName(node->GetData<string>("input-data-name", fInputDataName));
        SetOutputDataName(node->GetData<string>("output-data-name", fOutputDataName));

        return true;
    }

    KTCluster1DData* KTDistanceClustering::FindClusters(const KTDiscriminatedPoints1DData* data)
    {
        typedef KTDiscriminatedPoints1DData::SetOfPoints OriginalPoints;
        typedef set< UInt_t > Cluster;

        if (fCalculateMaxBinDistance) SetMaxBinDistance(KTMath::Nint(fMaxFrequencyDistance / data->GetBinWidth()));

        UInt_t nChannels = data->GetNChannels();

        KTCluster1DData* newData = new KTCluster1DData(nChannels);
        newData->SetNBins(data->GetNBins());
        newData->SetBinWidth(data->GetBinWidth());

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {
            newData->SetThreshold(data->GetThreshold(iChannel), iChannel);

            const OriginalPoints points = data->GetSetOfPoints(iChannel);

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
                        newData->AddCluster(*(activeCluster.begin()), *(activeCluster.rbegin()), iChannel);
                        activeCluster.clear();
                    }
                    activeCluster.insert(thisPoint);
                    lastPointInActiveCluster = thisPoint;
                }
                KTDEBUG(sdlog, "Adding cluster: (ch. " << iChannel << "): " << *(activeCluster.begin()) << "  " << *(activeCluster.rbegin()));
                newData->AddCluster(*(activeCluster.begin()), *(activeCluster.rbegin()), iChannel);
            }

            KTDEBUG(sdlog, newData->GetSetOfClusters(iChannel).size() << " clusters added on channel " << iChannel);
        }

        newData->SetName(fOutputDataName);
        newData->SetEvent(data->GetEvent());

        fCluster1DSignal(newData);

        return newData;
    }

    void KTDistanceClustering::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTDiscriminatedPoints1DData* dp1Data = event->GetData< KTDiscriminatedPoints1DData >(fInputDataName);
        if (dp1Data != NULL)
        {
            KTCluster1DData* newData = FindClusters(dp1Data);
            event->AddData(newData);
            return;
        }
        /*
        const KTDiscriminatedPoints2DData* dp2Data = dynamic_cast< KTDiscriminatedPoints2DData* >(event->GetData(fInputDataName));
        if (dp2Data != NULL)
        {
            KTCluster2DData* newData = FindClusters(dp2Data);
            event->AddData(newData);
            return;
        }
        */
        KTWARN(sdlog, "No discriminated-points data named <" << fInputDataName << "> was available in the event");
        return;
    }

    void KTDistanceClustering::Process1DData(const KTDiscriminatedPoints1DData* data)
    {
        KTCluster1DData* newData = FindClusters(data);
        if (data->GetEvent() != NULL)
            data->GetEvent()->AddData(newData);
        return;
    }
    /*
    void KTDistanceClustering::Process2DData(const KTDiscriminatedPoints2DData* data)
    {
        KTCluster2DData* newData = FindClusters(data);
        if (data->GetEvent() != NULL)
            data->GetEvent()->AddData(newData);
        return;
    }
    */
} /* namespace Katydid */
