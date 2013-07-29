/*
 * KTVariableSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTVariableSpectrumDiscriminator.hh"

#include "KTCorrelationData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTNOFactory.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTLogger.hh"
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
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(sdlog, "katydid.analysis");

    static KTDerivedNORegistrar< KTProcessor, KTVariableSpectrumDiscriminator > sSpectDiscRegistrar("variable-spectrum-discriminator");

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

    Bool_t KTVariableSpectrumDiscriminator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("snr-threshold-amplitude"))
        {
            SetSNRAmplitudeThreshold(node->GetData< Double_t >("snr-threshold-amplitude"));
        }
        if (node->HasData("snr-threshold-power"))
        {
            SetSNRPowerThreshold(node->GetData< Double_t >("snr-threshold-power"));
        }
        if (node->HasData("sigma-threshold"))
        {
            SetSigmaThreshold(node->GetData< Double_t >("sigma-threshold"));
        }

        if (node->HasData("min-frequency"))
        {
            SetMinFrequency(node->GetData< Double_t >("min-frequency"));
        }
        if (node->HasData("max-frequency"))
        {
            SetMaxFrequency(node->GetData< Double_t >("max-frequency"));
        }

        if (node->HasData("min-bin"))
        {
            SetMinBin(node->GetData< UInt_t >("min-bin"));
        }
        if (node->HasData("max-bin"))
        {
            SetMaxBin(node->GetData< UInt_t >("max-bin"));
        }

        return true;
    }

    Bool_t KTVariableSpectrumDiscriminator::Discriminate(KTFrequencySpectrumDataPolar& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    Bool_t KTVariableSpectrumDiscriminator::Discriminate(KTFrequencySpectrumDataFFTW& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    Bool_t KTVariableSpectrumDiscriminator::Discriminate(KTNormalizedFSDataPolar& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    Bool_t KTVariableSpectrumDiscriminator::Discriminate(KTNormalizedFSDataFFTW& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    Bool_t KTVariableSpectrumDiscriminator::Discriminate(KTCorrelationData& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    Bool_t KTVariableSpectrumDiscriminator::Discriminate(KTWignerVilleData& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, gvData, newData);
    }

    Bool_t KTVariableSpectrumDiscriminator::CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData)
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

        UInt_t nComponents = data.GetNComponents();

        newData.SetNBins(data.GetSpectrumFFTW(0)->size());
        newData.SetBinWidth(data.GetSpectrumFFTW(0)->GetBinWidth());

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
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

    Bool_t KTVariableSpectrumDiscriminator::CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData)
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

        UInt_t nComponents = data.GetNComponents();

        newData.SetNBins(data.GetSpectrumPolar(0)->size());
        newData.SetBinWidth(data.GetSpectrumPolar(0)->GetBinWidth());

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
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

    Bool_t KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTFrequencySpectrumPolar* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, UInt_t component)
    {
        if (spectrum == NULL)
        {
            KTERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t freqMin = spectrum->GetBinLowEdge(fMinBin);
        Double_t freqMax = spectrum->GetBinLowEdge(fMaxBin) + spectrum->GetBinWidth();
        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        //************
        // SNR mode
        //************
        if (fThresholdMode == eSNR_Amplitude || fThresholdMode == eSNR_Power)
        {
            Double_t thresholdMult = 0.;
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
            Double_t value;
#pragma omp parallel for private(value)
            for (UInt_t iBin=fMinBin; iBin<=fMaxBin; iBin++)
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
            Double_t sigmaNorm = 1. / Double_t(nBins - 1);
            Double_t sigma = 0., diff;
#pragma omp parallel for private(diff) reduction(+:sigma)
            for (UInt_t iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                diff = (*spectrum)(iBin).abs() - (*splineImp)(iBin - fMinBin);
                sigma += diff * diff;
            }
            sigma = sqrt(sigma * sigmaNorm);

            Double_t thresholdAdd = fSigmaThreshold * sigma;
            KTDEBUG(sdlog, "Discriminator threshold diff for component " << component << " set at <" << thresholdAdd << "> (Sigma mode)");
            newData.SetThreshold(thresholdAdd, component);

            // loop over bins, checking against the threshold
            Double_t value;
#pragma omp parallel for private(value)
            for (UInt_t iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                value = (*spectrum)(iBin).abs();
                if (value >= thresholdAdd + (*splineImp)(iBin - fMinBin))
                    newData.AddPoint(iBin, value, component);
            }
        }

        return true;

    }

    Bool_t KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTFrequencySpectrumFFTW* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, UInt_t component)
    {
        if (spectrum == NULL)
        {
            KTERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t freqMin = spectrum->GetBinLowEdge(fMinBin);
        Double_t freqMax = spectrum->GetBinLowEdge(fMaxBin) + spectrum->GetBinWidth();
        KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);

        //************
        // SNR mode
        //************
        if (fThresholdMode == eSNR_Amplitude || fThresholdMode == eSNR_Power)
        {
            Double_t thresholdMult = 0.;
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
            Double_t value;
#pragma omp parallel for private(value)
            for (UInt_t iBin=fMinBin; iBin<=fMaxBin; iBin++)
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
            Double_t sigmaNorm = 1. / Double_t(nBins - 1);
            Double_t sigma = 0., diff;
#pragma omp parallel for private(diff) reduction(+:sigma)
            for (UInt_t iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                fMagnitudeCache[iBin] = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                diff = fMagnitudeCache[iBin] - (*splineImp)(iBin - fMinBin);
                sigma += diff * diff;
            }
            sigma = sqrt(sigma * sigmaNorm);

            Double_t thresholdAdd = fSigmaThreshold * sigma;
            KTDEBUG(sdlog, "Discriminator threshold diff for component " << component << " set at <" << thresholdAdd << "> (Sigma mode)");
            newData.SetThreshold(thresholdAdd, component);

            // loop over bins, checking against the threshold
            Double_t value;
#pragma omp parallel for private(value)
            for (UInt_t iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                value = fMagnitudeCache[iBin];
                if (value >= thresholdAdd + (*splineImp)(iBin - fMinBin))
                    newData.AddPoint(iBin, value, component);
            }
        }

        return true;
    }


} /* namespace Katydid */
