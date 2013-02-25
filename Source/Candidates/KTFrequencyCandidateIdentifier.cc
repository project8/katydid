/*
 * KTFrequencyCandidateIdentifier.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateIdentifier.hh"

#include "KTCacheDirectory.hh"
#include "KTCorrelator.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTGainNormalization.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

using std::string;

using boost::shared_ptr;


namespace Katydid
{
    KTLOGGER(fcilog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTFrequencyCandidateIdentifier > sFCIRegistrar("frequency-candidate-identifier");

    KTFrequencyCandidateIdentifier::KTFrequencyCandidateIdentifier() :
            KTProcessor()
    {
        fConfigName = "frequency-candidate-identifier";

        RegisterSignal("frequency-candidates", &fFCSignal, "void (const KTFrequencyCandidateData*)");

        RegisterSlot("fs-polar", this, &KTFrequencyCandidateIdentifier::ProcessClusterAndFSPolarData, "void (shared_ptr< KTData >)");
        RegisterSlot("fs-fftw", this, &KTFrequencyCandidateIdentifier::ProcessClusterAndFSFFTWData, "void (shared_ptr< KTData >)");
        RegisterSlot("norm-fs-polar", this, &KTFrequencyCandidateIdentifier::ProcessClusterAndNormFSPolarData, "void (shared_ptr< KTData >)");
        RegisterSlot("norm-fs-fftw", this, &KTFrequencyCandidateIdentifier::ProcessClusterAndNormFSFFTWData, "void (shared_ptr< KTData >)");
        RegisterSlot("corr", this, &KTFrequencyCandidateIdentifier::ProcessClusterAndCorrelationData, "void (shared_ptr< KTData >)");
    }

    KTFrequencyCandidateIdentifier::~KTFrequencyCandidateIdentifier()
    {
    }

    Bool_t KTFrequencyCandidateIdentifier::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    Bool_t KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataPolar& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    Bool_t KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataFFTW& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    Bool_t KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, const KTNormalizedFSDataPolar& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    Bool_t KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, const KTNormalizedFSDataFFTW& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    Bool_t KTFrequencyCandidateIdentifier::IdentifyCandidates(KTCluster1DData& clusterData, const KTCorrelationData& fsData)
    {
        KTFrequencyCandidateData& fcData = clusterData.Of< KTFrequencyCandidateData >().SetNComponents(clusterData.GetNComponents());
        return CoreIdentifyCandidates(clusterData, fsData, fcData);
    }

    KTFrequencyCandidateData::Candidates KTFrequencyCandidateIdentifier::IdentifyCandidates(const KTCluster1DData::SetOfClusters& clusters, const KTFrequencySpectrumPolar* freqSpec)
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


    Bool_t KTFrequencyCandidateIdentifier::CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataPolarCore& fsData, KTFrequencyCandidateData& fcData)
    {
        if (clusterData.GetBinWidth() != fsData.GetSpectrumPolar(0)->GetBinWidth())
        {
            KTWARN(fcilog, "There is a mismatch between the bin widths:\n" <<
                    "\tCluster data: " << clusterData.GetBinWidth() << '\n' <<
                    "\tFrequency spectrum: " << fsData.GetSpectrumPolar(0)->GetBinWidth());
        }

        UInt_t nComponents = clusterData.GetNComponents();

        fcData.SetBinWidth(clusterData.GetBinWidth());
        fcData.SetNBins(clusterData.GetNBins());

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            const KTCluster1DData::SetOfClusters& clusters = clusterData.GetSetOfClusters(iComponent);
            const KTFrequencySpectrumPolar* freqSpec = fsData.GetSpectrumPolar(iComponent);

            fcData.AddCandidates(IdentifyCandidates(clusters, freqSpec), iComponent);
            fcData.SetThreshold(clusterData.GetThreshold(iComponent), iComponent);
        }

        return true;
    }

    Bool_t KTFrequencyCandidateIdentifier::CoreIdentifyCandidates(KTCluster1DData& clusterData, const KTFrequencySpectrumDataFFTWCore& fsData, KTFrequencyCandidateData& fcData)
    {
        if (clusterData.GetBinWidth() != fsData.GetSpectrumFFTW(0)->GetBinWidth())
        {
            KTWARN(fcilog, "There is a mismatch between the bin widths:\n" <<
                    "\tCluster data: " << clusterData.GetBinWidth() << '\n' <<
                    "\tFrequency spectrum: " << fsData.GetSpectrumFFTW(0)->GetBinWidth());
        }

        UInt_t nComponents = clusterData.GetNComponents();

        fcData.SetBinWidth(clusterData.GetBinWidth());
        fcData.SetNBins(clusterData.GetNBins());

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            const KTCluster1DData::SetOfClusters& clusters = clusterData.GetSetOfClusters(iComponent);
            const KTFrequencySpectrumFFTW* freqSpec = fsData.GetSpectrumFFTW(iComponent);

            fcData.AddCandidates(IdentifyCandidates(clusters, freqSpec), iComponent);
            fcData.SetThreshold(clusterData.GetThreshold(iComponent), iComponent);
        }

        return true;
    }

    void KTFrequencyCandidateIdentifier::ProcessClusterAndFSPolarData(boost::shared_ptr< KTData > data)
    {
        if (! data->Has< KTFrequencySpectrumDataPolar >())
        {
            KTERROR(fcilog, "No frequency spectrum data was present");
            return;
        }
        if (! data->Has< KTCluster1DData >())
        {
            KTERROR(fcilog, "No cluster data was present");
        }
        if (! IdentifyCandidates(data->Of< KTCluster1DData >(), data->Of< KTFrequencySpectrumDataPolar >()))
        {
            KTERROR(fcilog, "Something went wrong while identifying candidates");
            return;
        }
        fFCSignal(data);
        return;
    }
    void KTFrequencyCandidateIdentifier::ProcessClusterAndFSFFTWData(boost::shared_ptr< KTData > data)
    {
        if (! data->Has< KTFrequencySpectrumDataFFTW >())
        {
            KTERROR(fcilog, "No frequency spectrum data was present");
            return;
        }
        if (! data->Has< KTCluster1DData >())
        {
            KTERROR(fcilog, "No cluster data was present");
        }
        if (! IdentifyCandidates(data->Of< KTCluster1DData >(), data->Of< KTFrequencySpectrumDataFFTW >()))
        {
            KTERROR(fcilog, "Something went wrong while identifying candidates");
            return;
        }
        fFCSignal(data);
        return;
    }
    void KTFrequencyCandidateIdentifier::ProcessClusterAndNormFSPolarData(boost::shared_ptr< KTData > data)
    {
        if (! data->Has< KTNormalizedFSDataPolar >())
        {
            KTERROR(fcilog, "No frequency spectrum data was present");
            return;
        }
        if (! data->Has< KTCluster1DData >())
        {
            KTERROR(fcilog, "No cluster data was present");
        }
        if (! IdentifyCandidates(data->Of< KTCluster1DData >(), data->Of< KTNormalizedFSDataPolar >()))
        {
            KTERROR(fcilog, "Something went wrong while identifying candidates");
            return;
        }
        fFCSignal(data);
        return;
    }
    void KTFrequencyCandidateIdentifier::ProcessClusterAndNormFSFFTWData(boost::shared_ptr< KTData > data)
    {
        if (! data->Has< KTNormalizedFSDataFFTW >())
        {
            KTERROR(fcilog, "No frequency spectrum data was present");
            return;
        }
        if (! data->Has< KTCluster1DData >())
        {
            KTERROR(fcilog, "No cluster data was present");
        }
        if (! IdentifyCandidates(data->Of< KTCluster1DData >(), data->Of< KTNormalizedFSDataFFTW >()))
        {
            KTERROR(fcilog, "Something went wrong while identifying candidates");
            return;
        }
        fFCSignal(data);
        return;
    }
    void KTFrequencyCandidateIdentifier::ProcessClusterAndCorrelationData(boost::shared_ptr< KTData > data)
    {
        if (! data->Has< KTCorrelationData >())
        {
            KTERROR(fcilog, "No correlation data was present");
            return;
        }
        if (! data->Has< KTCluster1DData >())
        {
            KTERROR(fcilog, "No cluster data was present");
        }
        if (! IdentifyCandidates(data->Of< KTCluster1DData >(), data->Of< KTCorrelationData >()))
        {
            KTERROR(fcilog, "Something went wrong while identifying candidates");
            return;
        }
        fFCSignal(data);
        return;
    }

} /* namespace Katydid */
