/*
 * KTFrequencyCandidateIdentifier.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateIdentifier.hh"

#include "KTCacheDirectory.hh"
#include "KTCorrelationData.hh"
#include "KTNOFactory.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTNormalizedFSData.hh"
#include "KTLogger.hh"

using std::string;


namespace Katydid
{
    KTLOGGER(fcilog, "TestFrequencyCandidateIdentifier");

    static Nymph::KTNORegistrar< Nymph::KTProcessor, KTFrequencyCandidateIdentifier > sFCIRegistrar("frequency-candidate-identifier");

    KTFrequencyCandidateIdentifier::KTFrequencyCandidateIdentifier(const std::string& name) :
            KTProcessor(name),
            fFCSignal("frequency-candidates", this),
            fFSDataPolarSlot("fs-polar", this, &KTFrequencyCandidateIdentifier::IdentifyCandidates, &fFCSignal),
            fFSDataFFTWSlot("fs-fftw", this, &KTFrequencyCandidateIdentifier::IdentifyCandidates, &fFCSignal),
            fFSNormDataPolarSlot("norm-fs-polar", this, &KTFrequencyCandidateIdentifier::IdentifyCandidates, &fFCSignal),
            fFSNormDataFFTWSlot("norm-fs-fftw", this, &KTFrequencyCandidateIdentifier::IdentifyCandidates, &fFCSignal),
            fFSCorrelationDataSlot("corr", this, &KTFrequencyCandidateIdentifier::IdentifyCandidates, &fFCSignal)
    {
    }

    KTFrequencyCandidateIdentifier::~KTFrequencyCandidateIdentifier()
    {
    }

    bool KTFrequencyCandidateIdentifier::Configure(const scarab::param_node*)
    {
        return true;
    }

    bool KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, KTFrequencySpectrumDataPolar& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    bool KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, KTFrequencySpectrumDataFFTW& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    bool KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, KTNormalizedFSDataPolar& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    bool KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, KTNormalizedFSDataFFTW& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    bool KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, KTCorrelationData& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    KTFrequencyCandidateData::Candidates KTFrequencyCandidateIdentifier::IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumPolar* freqSpec)
    {
        KTFrequencyCandidateData::Candidates candidates(clusters.size());

        KTFrequencyCandidate newCandidate;
        unsigned iCandidate = 0;
        for (KTCluster1DData::SetOfClusters::const_iterator it=clusters.begin(); it != clusters.end(); it++)
        {
            unsigned firstBin = it->first;
            unsigned lastBin = it->second;

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

            double weightedMean = 0.;
            double integral = 0.;
            double peakValue = 0.;
            for (unsigned iBin=firstBin; iBin <= lastBin; iBin++)
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
        unsigned iCandidate = 0;
        for (KTCluster1DData::SetOfClusters::const_iterator it=clusters.begin(); it != clusters.end(); it++)
        {
            unsigned firstBin = it->first;
            unsigned lastBin = it->second;

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

            double weightedMean = 0.;
            double integral = 0.;
            double value = 0.;
            double peakValue = 0.;
            for (unsigned iBin=firstBin; iBin <= lastBin; iBin++)
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


    bool KTFrequencyCandidateIdentifier::CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataPolarCore& fsData, KTFrequencyCandidateData& fcData)
    {
        if (clusterData.GetBinWidth() != fsData.GetSpectrumPolar(0)->GetBinWidth())
        {
            KTWARN(fcilog, "There is a mismatch between the bin widths:\n" <<
                    "\tCluster data: " << clusterData.GetBinWidth() << '\n' <<
                    "\tFrequency spectrum: " << fsData.GetSpectrumPolar(0)->GetBinWidth());
        }

        unsigned nComponents = clusterData.GetNComponents();

        fcData.SetBinWidth(clusterData.GetBinWidth());
        fcData.SetNBins(clusterData.GetNBins());

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTCluster1DData::SetOfClusters& clusters = clusterData.GetSetOfClusters(iComponent);
            const KTFrequencySpectrumPolar* freqSpec = fsData.GetSpectrumPolar(iComponent);

            fcData.AddCandidates(IdentifyCandidates(clusters, freqSpec), iComponent);
            fcData.SetThreshold(clusterData.GetThreshold(iComponent), iComponent);
        }

        return true;
    }

    bool KTFrequencyCandidateIdentifier::CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataFFTWCore& fsData, KTFrequencyCandidateData& fcData)
    {
        if (clusterData.GetBinWidth() != fsData.GetSpectrumFFTW(0)->GetBinWidth())
        {
            KTWARN(fcilog, "There is a mismatch between the bin widths:\n" <<
                    "\tCluster data: " << clusterData.GetBinWidth() << '\n' <<
                    "\tFrequency spectrum: " << fsData.GetSpectrumFFTW(0)->GetBinWidth());
        }

        unsigned nComponents = clusterData.GetNComponents();

        fcData.SetBinWidth(clusterData.GetBinWidth());
        fcData.SetNBins(clusterData.GetNBins());

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTCluster1DData::SetOfClusters& clusters = clusterData.GetSetOfClusters(iComponent);
            const KTFrequencySpectrumFFTW* freqSpec = fsData.GetSpectrumFFTW(iComponent);

            fcData.AddCandidates(IdentifyCandidates(clusters, freqSpec), iComponent);
            fcData.SetThreshold(clusterData.GetThreshold(iComponent), iComponent);
        }

        return true;
    }

} /* namespace Katydid */
