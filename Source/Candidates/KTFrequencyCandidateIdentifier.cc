/*
 * KTFrequencyCandidateIdentifier.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateIdentifier.hh"

#include "KTCacheDirectory.hh"
#include "KTCorrelationData.hh"
#include "KTEggHeader.hh"
#include "KTBundle.hh"
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
        RegisterSlot("bundle", this, &KTFrequencyCandidateIdentifier::ProcessEvent, "void (shared_ptr<KTBundle>)");
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
            KTWARN(fcilog, "There is a mismatch between the bin widths:\n" <<
                    "\tCluster data: " << clusterData->GetBinWidth() << '\n' <<
                    "\tFrequency spectrum: " << fsData->GetSpectrum(0)->GetBinWidth());
        }

        KTFrequencyCandidateData* fcData = new KTFrequencyCandidateData(clusterData->GetNGroups());
        fcData->SetBinWidth(clusterData->GetBinWidth());
        fcData->SetNBins(clusterData->GetNBins());
        fcData->SetTimeInRun(fsData->GetTimeInRun());
        fcData->SetSliceNumber(fsData->GetSliceNumber());

        for (UInt_t iComponent = 0; iComponent < clusterData->GetNGroups(); iComponent++)
        {
            const KTCluster1DData::SetOfClusters& clusters = clusterData->GetSetOfClusters(iComponent);
            const KTFrequencySpectrum* freqSpec = fsData->GetSpectrum(iComponent);

            fcData->AddCandidates(IdentifyCandidates(clusters, freqSpec), iComponent);
            fcData->SetThreshold(clusterData->GetThreshold(iComponent), iComponent);
        }

        fcData->SetEvent(clusterData->GetEvent());
        fcData->SetName(fOutputDataName);

        fFCSignal(fcData);

        return fcData;
    }

    KTFrequencyCandidateData* KTFrequencyCandidateIdentifier::IdentifyCandidates(const KTCluster1DData* clusterData, const KTFrequencySpectrumDataFFTW* fsData)
    {
        if (clusterData->GetBinWidth() != fsData->GetSpectrum(0)->GetBinWidth())
        {
            KTWARN(fcilog, "There is a mismatch between the bin widths:\n" <<
                    "\tCluster data: " << clusterData->GetBinWidth() << '\n' <<
                    "\tFrequency spectrum: " << fsData->GetSpectrum(0)->GetBinWidth());
        }

        KTFrequencyCandidateData* fcData = new KTFrequencyCandidateData(clusterData->GetNGroups());
        fcData->SetBinWidth(clusterData->GetBinWidth());
        fcData->SetNBins(clusterData->GetNBins());
        fcData->SetTimeInRun(fsData->GetTimeInRun());
        fcData->SetSliceNumber(fsData->GetSliceNumber());

        for (UInt_t iComponent = 0; iComponent < clusterData->GetNGroups(); iComponent++)
        {
            const KTCluster1DData::SetOfClusters& clusters = clusterData->GetSetOfClusters(iComponent);
            const KTFrequencySpectrumFFTW* freqSpec = fsData->GetSpectrum(iComponent);

            fcData->AddCandidates(IdentifyCandidates(clusters, freqSpec), iComponent);
            fcData->SetThreshold(clusterData->GetThreshold(iComponent), iComponent);
        }

        fcData->SetEvent(clusterData->GetEvent());
        fcData->SetName(fOutputDataName);

        fFCSignal(fcData);

        return fcData;
    }

    KTFrequencyCandidateData* KTFrequencyCandidateIdentifier::IdentifyCandidates(const KTCluster1DData* clusterData, const KTCorrelationData* fsData)
    {
        if (clusterData->GetBinWidth() != fsData->GetCorrelation(0)->GetBinWidth())
        {
            KTWARN(fcilog, "There is a mismatch between the bin widths:\n" <<
                    "\tCluster data: " << clusterData->GetBinWidth() << '\n' <<
                    "\tFrequency spectrum: " << fsData->GetCorrelation(0)->GetBinWidth());
        }

        KTFrequencyCandidateData* fcData = new KTFrequencyCandidateData(clusterData->GetNGroups());
        fcData->SetBinWidth(clusterData->GetBinWidth());
        fcData->SetNBins(clusterData->GetNBins());
        fcData->SetTimeInRun(fsData->GetTimeInRun());
        fcData->SetSliceNumber(fsData->GetSliceNumber());

        for (UInt_t iComponent = 0; iComponent < clusterData->GetNGroups(); iComponent++)
        {
            const KTCluster1DData::SetOfClusters& clusters = clusterData->GetSetOfClusters(iComponent);
            const KTFrequencySpectrum* freqSpec = fsData->GetCorrelation(iComponent);

            fcData->AddCandidates(IdentifyCandidates(clusters, freqSpec), iComponent);
            fcData->SetThreshold(clusterData->GetThreshold(iComponent), iComponent);
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
            Double_t peakValue = 0.;
            for (UInt_t iBin=firstBin; iBin <= lastBin; iBin++)
            {
                weightedMean += freqSpec->GetBinCenter(iBin) * (*freqSpec)(iBin).abs();
                integral += (*freqSpec)(iBin).abs();
                if ((*freqSpec)(iBin).abs() > peakValue) peakValue = (*freqSpec)(iBin).abs();
            }
            weightedMean /= integral;
            newCandidate.SetMeanFrequency(weightedMean);
            newCandidate.SetPeakAmplitude(peakValue);
            newCandidate.SetAmplitudeSum(integral);

            candidates[iCandidate] = newCandidate;
            iCandidate++;
        }

        return candidates;
    }


    KTFrequencyCandidateData::Candidates KTFrequencyCandidateIdentifier::IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumFFTW* freqSpec)
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
            Double_t value = 0.;
            Double_t peakValue = 0.;
            for (UInt_t iBin=firstBin; iBin <= lastBin; iBin++)
            {
                value = sqrt((*freqSpec)(iBin)[0] * (*freqSpec)(iBin)[0] + (*freqSpec)(iBin)[1] * (*freqSpec)(iBin)[1]);
                weightedMean += freqSpec->GetBinCenter(iBin) * value;
                integral += value;
                if (value > peakValue) peakValue = value;
            }
            weightedMean /= integral;
            newCandidate.SetMeanFrequency(weightedMean);
            newCandidate.SetPeakAmplitude(peakValue);
            newCandidate.SetAmplitudeSum(integral);

            candidates[iCandidate] = newCandidate;
            iCandidate++;
        }

        return candidates;
    }


    void KTFrequencyCandidateIdentifier::ProcessClusterData(const KTCluster1DData* clusterData)
    {
        KTBundle* bundle = clusterData->GetEvent();
        if (bundle == NULL)
        {
            KTERROR(fcilog, "Cluster data must be associated with an bundle.");
            return;
        }

        const KTFrequencySpectrumData* fsData = bundle->GetData< KTFrequencySpectrumData >(fFSInputDataName);
        if (fsData != NULL)
        {
            KTFrequencyCandidateData* newData = newData = this->IdentifyCandidates(clusterData, fsData);
            bundle->AddData(newData);
            return;
        }

        const KTFrequencySpectrumDataFFTW* fsDataFFTW = bundle->GetData< KTFrequencySpectrumDataFFTW >(fFSInputDataName);
        if (fsDataFFTW != NULL)
        {
            KTFrequencyCandidateData* newData = newData = this->IdentifyCandidates(clusterData, fsDataFFTW);
            bundle->AddData(newData);
            return;
        }

        const KTCorrelationData* corrData = bundle->GetData< KTCorrelationData >(fFSInputDataName);
        if (corrData != NULL)
        {
            KTFrequencyCandidateData* newData = newData = this->IdentifyCandidates(clusterData, corrData);
            bundle->AddData(newData);
            return;
        }

        KTERROR(fcilog, "The bundle associated with the cluster data must have a frequency spectrum by the name <" << fFSInputDataName << ">");

        return;
    }

    void KTFrequencyCandidateIdentifier::ProcessEvent(shared_ptr<KTBundle> bundle)
    {
        const KTCluster1DData* clusterData = bundle->GetData< KTCluster1DData >(fClusterInputDataName);
        if (clusterData == NULL)
        {
            KTWARN(fcilog, "No cluster data named <" << fClusterInputDataName << "> was available in the bundle");
            return;
        }

        ProcessClusterData(clusterData);
        return;
    }

} /* namespace Katydid */
