/*
 * KTSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTSpectrumDiscriminator.hh"

#include "KTCorrelationData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTNormalizedFSData.hh"
#include "KTParam.hh"
#include "KTWignerVilleData.hh"

#include <cmath>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::string;
using std::vector;


namespace Katydid
{
    KTLOGGER(sdlog, "KTSpectrumDiscriminator");

    KT_REGISTER_PROCESSOR(KTSpectrumDiscriminator, "spectrum-discriminator");

    KTSpectrumDiscriminator::KTSpectrumDiscriminator(const std::string& name) :
            KTProcessor(name),
            fSNRThreshold(10.),
            fSigmaThreshold(5.),
            fThresholdMode(eSigma),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fDiscrim1DSignal("disc-1d", this),
            fFSPolarSlot("fs-polar", this, &KTSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fFSFFTWSlot("fs-fftw", this, &KTSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fNormFSPolarSlot("norm-fs-polar", this, &KTSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fCorrSlot("corr", this, &KTSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fWVSlot("wv", this, &KTSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal)
    {
    }

    KTSpectrumDiscriminator::~KTSpectrumDiscriminator()
    {
    }

    bool KTSpectrumDiscriminator::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        if (node->Has("snr-threshold-amplitude"))
        {
            SetSNRAmplitudeThreshold(node->GetValue< double >("snr-threshold-amplitude"));
        }
        if (node->Has("snr-threshold-power"))
        {
            SetSNRPowerThreshold(node->GetValue< double >("snr-threshold-power"));
        }
        if (node->Has("sigma-threshold"))
        {
            SetSigmaThreshold(node->GetValue< double >("sigma-threshold"));
        }

        if (node->Has("min-frequency"))
        {
            SetMinFrequency(node->GetValue< double >("min-frequency"));
        }
        if (node->Has("max-frequency"))
        {
            SetMaxFrequency(node->GetValue< double >("max-frequency"));
        }

        if (node->Has("min-bin"))
        {
            SetMinBin(node->GetValue< unsigned >("min-bin"));
        }
        if (node->Has("max-bin"))
        {
            SetMaxBin(node->GetValue< unsigned >("max-bin"));
        }

        return true;
    }

    bool KTSpectrumDiscriminator::Discriminate(KTFrequencySpectrumDataPolar& data)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, newData);
    }

    bool KTSpectrumDiscriminator::Discriminate(KTFrequencySpectrumDataFFTW& data)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, newData);
    }

    bool KTSpectrumDiscriminator::Discriminate(KTNormalizedFSDataPolar& data)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, newData);
    }

    bool KTSpectrumDiscriminator::Discriminate(KTNormalizedFSDataFFTW& data)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, newData);
    }

    bool KTSpectrumDiscriminator::Discriminate(KTCorrelationData& data)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, newData);
    }

    bool KTSpectrumDiscriminator::Discriminate(KTWignerVilleData& data)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, newData);
    }

    bool KTSpectrumDiscriminator::CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTDiscriminatedPoints1DData& newData)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumFFTW(0)->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumFFTW(0)->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        unsigned nComponents = data.GetNComponents();

        newData.SetNBins(data.GetSpectrumFFTW(0)->size());
        double binWidth = data.GetSpectrumFFTW(0)->GetBinWidth();
        newData.SetBinWidth(binWidth);

        // Interval: [fMinBin, fMaxBin)
        unsigned nBins = fMaxBin - fMinBin + 1;
        double sigmaNorm = 1. / double(nBins - 1);

        // Temporary storage for magnitude values
        vector< double > magnitude(data.GetSpectrumFFTW(0)->size());

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* spectrum = data.GetSpectrumFFTW(iComponent);
            if (spectrum == NULL)
            {
                KTERROR(sdlog, "Frequency spectrum pointer (component " << iComponent << ") is NULL!");
                return false;
            }
            if (spectrum->size() != magnitude.size())
            {
                magnitude.resize(spectrum->size());
            }

            double mean = 0.;
#pragma omp parallel for reduction(+:mean)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                magnitude[iBin] = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                mean += magnitude[iBin];
            }
            mean /= (double)nBins;

            double threshold = 0.;
            if (fThresholdMode == eSNR_Amplitude)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2, A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSNR_Power)
            {
                // SNR = P_signal / P_noise, P_noise = mean
                threshold = fSNRThreshold * mean;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                double sigma = 0., diff;
#pragma omp parallel for private(diff) reduction(+:sigma)
                for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
                {
                    diff = magnitude[iBin] - mean;
                    sigma += diff * diff;
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (Sigma mode)");
            }

            // loop over bins, checking against the threshold
            double value;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                value = magnitude[iBin];
                if (value >= threshold) newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin + 0.5), value, threshold), iComponent);
            }
            KTDEBUG(sdlog, "Component " << iComponent << " has " << newData.GetSetOfPoints(iComponent).size() << " points above threshold");

        }
        KTINFO(sdlog, "Completed discrimination on " << nComponents << " components");

        return true;
    }

    bool KTSpectrumDiscriminator::CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTDiscriminatedPoints1DData& newData)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumPolar(0)->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin << " (frequency: " << fMinFrequency << ")");
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumPolar(0)->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin << " (frequency: " << fMaxFrequency << ")");
        }

        unsigned nComponents = data.GetNComponents();

        newData.SetNBins(data.GetSpectrumPolar(0)->size());
        double binWidth = data.GetSpectrumPolar(0)->GetBinWidth();
        newData.SetBinWidth(binWidth);

        // Interval: [fMinBin, fMaxBin)
        unsigned nBins = fMaxBin - fMinBin + 1;
        double sigmaNorm = 1. / double(nBins - 1);

        // Temporary storage for magnitude values
        vector< double > magnitude(data.GetSpectrumPolar(0)->size());

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);
            if (spectrum == NULL)
            {
                KTERROR(sdlog, "Frequency spectrum pointer (component " << iComponent << ") is NULL!");
                return false;
            }
            if (spectrum->size() != magnitude.size())
            {
                magnitude.resize(spectrum->size());
            }

            double mean = 0.;
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                mean += (*spectrum)(iBin).abs();
            }
            mean /= (double)nBins;

            double threshold = 0.;
            if (fThresholdMode == eSNR_Amplitude)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2, A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSNR_Power)
            {
                // SNR = P_signal / P_noise, P_noise = mean
                threshold = fSNRThreshold * mean;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                double sigma = 0., diff;
                for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
                {
                    diff = (*spectrum)(iBin).abs() - mean;
                    sigma += diff * diff;
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (Sigma mode)");
            }

            // loop over bins, checking against the threshold
            double value;
            //std::stringstream printer;
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                value = (*spectrum)(iBin).abs();
                if (value >= threshold)
                {
                    //printer << "   " << iBin << " -- " << value;
                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin + 0.5), value, threshold), iComponent);
                }
            }

            KTDEBUG(sdlog, "Component " << iComponent << " has " << newData.GetSetOfPoints(iComponent).size() << " points above threshold");
            //KTDEBUG(sdlog, printer.str());
        }
        KTINFO(sdlog, "Completed discrimination on " << nComponents << " components");

        return true;
    }

} /* namespace Katydid */
