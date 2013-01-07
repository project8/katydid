/*
 * KTFrequencyCandidateIdentifier.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateIdentifier.hh"

#include "KTCacheDirectory.hh"
#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

using std::string;

using boost::shared_ptr;


namespace Katydid
{
    KTLOGGER(fcilog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTFrequencyCandidateIdentifier > sFCIRegistrar("frequency-candidate-identifier");

    KTFrequencyCandidateIdentifier::KTFrequencyCandidateIdentifier() :
            KTProcessor(),
            fFSInputDataName("frequency-spectrum"),
            fClusterInputDataName("peak-list"),
            fOutputDataName("frequency-candidates")

    {
        fConfigName = "frequency-candidate-identifier";

        RegisterSignal("frequency-candidates", &fFCSignal, "void (const KTFrequencyCandidateData*)");

        RegisterSlot("clusters", this, &KTFrequencyCandidateIdentifier::ProcessClusterData, "void (const KTCluster1DData*)");
        RegisterSlot("event", this, &KTFrequencyCandidateIdentifier::ProcessEvent, "void (shared_ptr<KTEvent>)");
    }

    KTFrequencyCandidateIdentifier::~KTFrequencyCandidateIdentifier()
    {
    }

    Bool_t KTFrequencyCandidateIdentifier::Configure(const KTPStoreNode* node)
    {
        SetFSInputDataName(node->GetData< string >("fs-input-data-name", fFSInputDataName));
        SetClusterInputDataName(node->GetData< string >("cluster-input-data-name", fClusterInputDataName));
        SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));

        return true;
    }

    KTFrequencyCandidateData* KTFrequencyCandidateIdentifier::IdentifyCandidates(const KTCluster1DData* clusterData, const KTFrequencySpectrumData* fsData)
    {
        if (clusterData->GetBinWidth() != fsData->GetSpectrum(0)->GetBinWidth())
        {

        }

        KTFrequencyCandidateData* fcData = new KTFrequencyCandidateData(clusterData->GetNGroups());
        fcData->SetBinWidth(clusterData->GetBinWidth());
        fcData->SetNBins(clusterData->GetNBins());

        for (UInt_t iGroup = 0; iGroup < clusterData->GetNGroups(); iGroup++)
        {
            const KTCluster1DData::SetOfClusters& clusters = clusterData->GetSetOfClusters(iGroup);
            const KTFrequencySpectrum* freqSpec = fsData->GetSpectrum(iGroup);

            fcData->AddCandidates(IdentifyCandidates(clusters, freqSpec), iGroup);
            fcData->SetThreshold(clusterData->GetThreshold(iGroup), iGroup);
        }

        fcData->SetEvent(clusterData->GetEvent());
        fcData->SetName(fOutputDataName);

        fFCSignal(fcData);

        return fcData;
    }

    KTFrequencyCandidateData::Candidates KTFrequencyCandidateIdentifier::IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrum* freqSpec)
    {
        KTFrequencyCandidateData::Candidates candidates(clusters.size());

        KTFrequencyCandidate newCandidate;
        UInt_t iCandidate = 0;
        for (KTCluster1DData::SetOfClusters::const_iterator it=clusters.begin(); it != clusters.end(); it++)
        {
            UInt_t firstBin = it->first;
            UInt_t lastBin = it->second;

            if (firstBin >= freqSpec->size() || lastBin >= freqSpec->size())
            {
                KTWARN(fcilog, "First bin and/or last bin it outside the range of the given frequency spectrum:\n" <<
                        "\tFirst bin: " << firstBin << '\n' <<
                        "\tLast bin: " << lastBin << '\n' <<
                        "\tFS size: " << freqSpec->size());
                continue;
            }

            newCandidate.SetFirstBin(firstBin);
            newCandidate.SetLastBin(lastBin);

            Double_t weightedMean = 0.;
            Double_t integral = 0.;
            for (UInt_t iBin=firstBin; iBin <= lastBin; iBin++)
            {
                weightedMean += freqSpec->GetBinCenter(iBin) * (*freqSpec)(iBin).abs();
                integral += (*freqSpec)(iBin).abs();
            }
            weightedMean /= integral;
            newCandidate.SetMeanFrequency(weightedMean);

            candidates[iCandidate] = newCandidate;
            iCandidate++;
        }

        return candidates;
    }


    void KTFrequencyCandidateIdentifier::ProcessClusterData(const KTCluster1DData* clusterData)
    {
        KTEvent* event = clusterData->GetEvent();
        if (event == NULL)
        {
            KTERROR(fcilog, "Cluster data must be associated with an event.");
            return;
        }

        const KTFrequencySpectrumData* fsData = event->GetData< KTFrequencySpectrumData >(fFSInputDataName);
        if (fsData == NULL)
        {
            KTERROR(fcilog, "The event associated with the cluster data must have a frequency spectrum by the name <" << fFSInputDataName << ">");
            return;
        }

        KTFrequencyCandidateData* newData = this->IdentifyCandidates(clusterData, fsData);

        if (newData == NULL)
        {
            KTERROR(fcilog, "Unable to extract candidates");
            return;
        }

        event->AddData(newData);

        return;
    }

    void KTFrequencyCandidateIdentifier::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTCluster1DData* clusterData = event->GetData< KTCluster1DData >(fClusterInputDataName);
        if (clusterData == NULL)
        {
            KTWARN(fcilog, "No cluster data named <" << fClusterInputDataName << "> was available in the event");
            return;
        }

        ProcessClusterData(clusterData);
        return;
    }

} /* namespace Katydid */
