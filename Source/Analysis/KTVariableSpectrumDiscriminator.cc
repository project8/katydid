/*
 * KTVariableSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTVariableSpectrumDiscriminator.hh"

#include "KTCorrelationData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTNormalizedFSData.hh"
#include "KTPStoreNode.hh"
#include "KTSpline.hh"
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
    KTLOGGER(sdlog, "KTVariableSpectrumDiscriminator");

    KT_REGISTER_PROCESSOR(KTVariableSpectrumDiscriminator, "variable-spectrum-discriminator");

    KTVariableSpectrumDiscriminator::KTVariableSpectrumDiscriminator(const std::string& name) :
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
            fMagnitudeCache(),
            fDiscrim1DSignal("disc-1d", this),
            fFSPolarSlot("fs-polar", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fFSFFTWSlot("fs-fftw", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fNormFSPolarSlot("norm-fs-polar", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fCorrSlot("corr", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fWVSlot("wv", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal)
    {
    }

    KTVariableSpectrumDiscriminator::~KTVariableSpectrumDiscriminator()
    {
    }

    bool KTVariableSpectrumDiscriminator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("snr-threshold-amplitude"))
        {
            SetSNRAmplitudeThreshold(node->GetData< double >("snr-threshold-amplitude"));
        }
        if (node->HasData("snr-threshold-power"))
        {
            SetSNRPowerThreshold(node->GetData< double >("snr-threshold-power"));
        }
        if (node->HasData("sigma-threshold"))
        {
            SetSigmaThreshold(node->GetData< double >("sigma-threshold"));
        }

        if (node->HasData("min-frequency"))
        {
            SetMinFrequency(node->GetData< double >("min-frequency"));
        }
        if (node->HasData("max-frequency"))
        {
            SetMaxFrequency(node->GetData< double >("max-frequency"));
        }

        if (node->HasData("min-bin"))
        {
            SetMinBin(node->GetData< unsigned >("min-bin"));
        }
        if (node->HasData("max-bin"))
        {
            SetMaxBin(node->GetData< unsigned >("max-bin"));
        }

        return true;
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTFrequencySpectrumDataPolar& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTFrequencySpectrumDataFFTW& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTNormalizedFSDataPolar& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTNormalizedFSDataFFTW& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTCorrelationData& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTWignerVilleData& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    bool KTVariableSpectrumDiscriminator::CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData)
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
        newData.SetBinWidth(data.GetSpectrumFFTW(0)->GetBinWidth());

        for (unsigned iComponent=0; iComponent<nComponents; iComponent++)
        {
            if (! DiscriminateSpectrum(data.GetSpectrumFFTW(iComponent), gvData.GetSpline(iComponent), newData, iComponent))
            {
                KTERROR(sdlog, "Discrimination on spectrum (component " << iComponent << ") failed");
                return false;
            }
            KTDEBUG(sdlog, "Component " << iComponent << " has " << newData.GetSetOfPoints(iComponent).size() << " points above threshold");
        }
        KTINFO(sdlog, "Completed discrimination on " << nComponents << " components");

        return true;
    }

    bool KTVariableSpectrumDiscriminator::CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData)
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
        newData.SetBinWidth(data.GetSpectrumPolar(0)->GetBinWidth());

        for (unsigned iComponent=0; iComponent<nComponents; iComponent++)
        {
            if (! DiscriminateSpectrum(data.GetSpectrumPolar(iComponent), gvData.GetSpline(iComponent), newData, iComponent))
            {
                KTERROR(sdlog, "Discrimination on spectrum (component " << iComponent << ") failed");
                return false;
            }
            KTDEBUG(sdlog, "Component " << iComponent << " has " << newData.GetSetOfPoints(iComponent).size() << " points above threshold");
        }
        KTINFO(sdlog, "Completed discrimination on " << nComponents << " components");

        return true;
    }

    bool KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTFrequencySpectrumPolar* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            KTERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double freqMin = spectrum->GetBinLowEdge(fMinBin);
        double freqMax = spectrum->GetBinLowEdge(fMaxBin) + spectrum->GetBinWidth();
        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        //************
        // SNR mode
        //************
        if (fThresholdMode == eSNR_Amplitude || fThresholdMode == eSNR_Power)
        {
            double thresholdMult = 0.;
            if (fThresholdMode == eSNR_Amplitude)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2, A_noise = mean
                thresholdMult = sqrt(fSNRThreshold);
                KTDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-amplitude mode)");
            }
            else
            {
                // SNR = P_signal / P_noise, P_noise = mean
                thresholdMult = fSNRThreshold;
                KTDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-power mode)");
            }
            newData.SetThreshold(thresholdMult, component);

            // loop over bins, checking against the threshold
            double value;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                value = (*spectrum)(iBin).abs();
                if (value >= thresholdMult * (*splineImp)(iBin - fMinBin))
                    newData.AddPoint(iBin, value, component);
            }
        }
        //**************
        // Sigma mode
        //**************
        else if (fThresholdMode == eSigma)
        {
            double sigmaNorm = 1. / double(nBins - 1);
            double sigma = 0., diff;
#pragma omp parallel for private(diff) reduction(+:sigma)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                diff = (*spectrum)(iBin).abs() - (*splineImp)(iBin - fMinBin);
                sigma += diff * diff;
            }
            sigma = sqrt(sigma * sigmaNorm);

            double thresholdAdd = fSigmaThreshold * sigma;
            KTDEBUG(sdlog, "Discriminator threshold diff for component " << component << " set at <" << thresholdAdd << "> (Sigma mode)");
            newData.SetThreshold(thresholdAdd, component);

            // loop over bins, checking against the threshold
            double value;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                value = (*spectrum)(iBin).abs();
                if (value >= thresholdAdd + (*splineImp)(iBin - fMinBin))
                    newData.AddPoint(iBin, value, component);
            }
        }

        spline->AddToCache(splineImp);

        return true;

    }

    bool KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTFrequencySpectrumFFTW* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            KTERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double freqMin = spectrum->GetBinLowEdge(fMinBin);
        double freqMax = spectrum->GetBinLowEdge(fMaxBin) + spectrum->GetBinWidth();
        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        //************
        // SNR mode
        //************
        if (fThresholdMode == eSNR_Amplitude || fThresholdMode == eSNR_Power)
        {
            double thresholdMult = 0.;
            if (fThresholdMode == eSNR_Amplitude)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2, A_noise = mean
                thresholdMult = sqrt(fSNRThreshold);
                KTDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-amplitude mode)");
            }
            else
            {
                // SNR = P_signal / P_noise, P_noise = mean
                thresholdMult = fSNRThreshold;
                KTDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-power mode)");
            }
            newData.SetThreshold(thresholdMult, component);

            // loop over bins, checking against the threshold
            double value;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                value = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                if (value >= thresholdMult * (*splineImp)(iBin - fMinBin))
                    newData.AddPoint(iBin, value, component);
            }
        }
        //**************
        // Sigma mode
        //**************
        else if (fThresholdMode == eSigma)
        {
            if (spectrum->size() != fMagnitudeCache.size())
            {
                fMagnitudeCache.resize(spectrum->size());
            }
            double sigmaNorm = 1. / double(nBins - 1);
            double sigma = 0., diff;
#pragma omp parallel for private(diff) reduction(+:sigma)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                fMagnitudeCache[iBin] = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                diff = fMagnitudeCache[iBin] - (*splineImp)(iBin - fMinBin);
                sigma += diff * diff;
            }
            sigma = sqrt(sigma * sigmaNorm);

            double thresholdAdd = fSigmaThreshold * sigma;
            KTDEBUG(sdlog, "Discriminator threshold diff for component " << component << " set at <" << thresholdAdd << "> (Sigma mode)");
            newData.SetThreshold(thresholdAdd, component);

            // loop over bins, checking against the threshold
            double value;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                value = fMagnitudeCache[iBin];
                if (value >= thresholdAdd + (*splineImp)(iBin - fMinBin))
                    newData.AddPoint(iBin, value, component);
            }
        }

        spline->AddToCache(splineImp);

        return true;
    }


} /* namespace Katydid */
