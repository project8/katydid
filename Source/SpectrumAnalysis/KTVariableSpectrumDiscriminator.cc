/*
 * KTVariableSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTVariableSpectrumDiscriminator.hh"

#include "KTCorrelationData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTDiscriminatedPoints2DData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTNormalizedFSData.hh"
#include "KTParam.hh"
#include "KTPowerSpectrumData.hh"
#include "KTSpectrumCollectionData.hh"
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
            fDiscrim2DSignal("disc-2d", this),
            fFSPolarSlot("fs-polar", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fFSFFTWSlot("fs-fftw", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fNormFSPolarSlot("norm-fs-polar", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fCorrSlot("corr", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fWVSlot("wv", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fPSSlot("ps", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fSpecSlot("spec", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim2DSignal),
            fPreCalcSlot("gv", this, &KTVariableSpectrumDiscriminator::SetPreCalcGainVar),
            fPSPreCalcSlot("ps-pre", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal)
    {
    }

    KTVariableSpectrumDiscriminator::~KTVariableSpectrumDiscriminator()
    {
    }

    bool KTVariableSpectrumDiscriminator::Configure(const KTParamNode* node)
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

    bool KTVariableSpectrumDiscriminator::SetPreCalcGainVar(KTGainVariationData& gvData)
    {
        fGVData = gvData;
        return true;
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTPowerSpectrumData& data)
    {
        return Discriminate(data, fGVData);
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

    bool KTVariableSpectrumDiscriminator::Discriminate(KTPowerSpectrumData& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(data.GetNComponents());

        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrum(0)->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrum(0)->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        unsigned nComponents = data.GetNComponents();

        newData.SetNBins(data.GetSpectrum(0)->size());
        newData.SetBinWidth(data.GetSpectrum(0)->GetBinWidth());

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            if (! DiscriminateSpectrum(data.GetSpectrum(iComponent), gvData.GetSpline(iComponent), newData, iComponent))
            {
                KTERROR(sdlog, "Discrimination on spectrum (component " << iComponent << ") failed");
                return false;
            }
            KTDEBUG(sdlog, "Component " << iComponent << " has " << newData.GetSetOfPoints(iComponent).size() << " points above threshold");
        }
        KTINFO(sdlog, "Completed discrimination on " << nComponents << " components");

        return true;
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTPSCollectionData& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints2DData& newData = data.Of< KTDiscriminatedPoints2DData >();
        KTDiscriminatedPoints1DData newDataSlice;

        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectra()[0]->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectra()[0]->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }
        
        newData.SetNBinsX( data.GetSpectra().size() );
        newData.SetNBinsY( data.GetSpectra()[0]->size() );
//        newData.SetBinWidthX( data.GetTimeLength() );
        newData.SetBinWidthY( data.GetSpectra()[0]->GetBinWidth() );

        newDataSlice.SetNComponents( 1 );
        newDataSlice.SetNBins( data.GetSpectra()[0]->size() );
        newDataSlice.SetBinWidth( data.GetSpectra()[0]->GetBinWidth() );

        double XbinWidth = /*data.GetTimeLength();*/ 10e-6;
        double YbinWidth = data.GetSpectra()[0]->GetBinWidth();

        unsigned nSpectra = data.GetSpectra().size();
        unsigned nPoints = 0;

        for( unsigned i = 0; i < nSpectra; ++i )
        {
            if (! DiscriminateSpectrum(data.GetSpectra()[i], gvData.GetSpline(0), newDataSlice, 0))
            {
                KTERROR(sdlog, "Discrimination on spectrogram (slice " << i << ") failed");
                return false;
            }
            nPoints = newDataSlice.GetSetOfPoints(0).size();
            KTDEBUG(sdlog, "Spectrogram slice " << i << " has " << nPoints << " points above threshold");

            for( KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = newDataSlice.GetSetOfPoints(0).begin(); it != newDataSlice.GetSetOfPoints(0).end(); ++it )
            {
                newData.AddPoint( i, it->first, KTDiscriminatedPoints2DData::Point( XbinWidth * ((double)i+0.5), YbinWidth * ((double)it->first+0.5), it->second.fOrdinate, it->second.fThreshold ), 0 );
            }
        }

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

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
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

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
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

    bool KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTFrequencySpectrumPolar* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData&newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            KTERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double binWidth = spectrum->GetBinWidth();
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

            // loop over bins, checking against the threshold
            double threshold, value;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = (*spectrum)(iBin).abs();
                threshold = thresholdMult * (*splineImp)(iBin - fMinBin);
                if (value >= threshold)
                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin + 0.5), value, threshold), component);
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
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                diff = (*spectrum)(iBin).abs() - (*splineImp)(iBin - fMinBin);
                sigma += diff * diff;
            }
            sigma = sqrt(sigma * sigmaNorm);

            double thresholdAdd = fSigmaThreshold * sigma;
            KTDEBUG(sdlog, "Discriminator threshold diff for component " << component << " set at <" << thresholdAdd << "> (Sigma mode)");

            // loop over bins, checking against the threshold
            double value, threshold;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = (*spectrum)(iBin).abs();
                threshold = thresholdAdd + (*splineImp)(iBin - fMinBin);
                if (value >= threshold)
                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin + 0.5), value, threshold), component);
            }
        }

        spline->AddToCache(splineImp);

        return true;

    }

    bool KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTFrequencySpectrumFFTW* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData& newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            KTERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double binWidth = spectrum->GetBinWidth();
        double freqMin = spectrum->GetBinLowEdge(fMinBin);
        double freqMax = spectrum->GetBinLowEdge(fMaxBin) + binWidth;
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

            // loop over bins, checking against the threshold
            double value, threshold;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                threshold = thresholdMult * (*splineImp)(iBin - fMinBin);
                if (value >= threshold)
                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin + 0.5), value, threshold), component);
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
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                fMagnitudeCache[iBin] = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                diff = fMagnitudeCache[iBin] - (*splineImp)(iBin - fMinBin);
                sigma += diff * diff;
            }
            sigma = sqrt(sigma * sigmaNorm);

            double thresholdAdd = fSigmaThreshold * sigma;
            KTDEBUG(sdlog, "Discriminator threshold diff for component " << component << " set at <" << thresholdAdd << "> (Sigma mode)");

            // loop over bins, checking against the threshold
            double value, threshold;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = fMagnitudeCache[iBin];
                threshold = thresholdAdd + (*splineImp)(iBin - fMinBin);
                if (value >= threshold)
                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin + 0.5), value, threshold), component);
            }
        }

        spline->AddToCache(splineImp);

        return true;
    }

    bool KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTPowerSpectrum* spectrum, const KTSpline* spline, KTDiscriminatedPoints1DData& newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            KTERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double binWidth = spectrum->GetBinWidth();
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

            // loop over bins, checking against the threshold
            double threshold, value;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = (*spectrum)(iBin);
                threshold = thresholdMult * (*splineImp)(iBin - fMinBin);
                if (value >= threshold)
                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin + 0.5), value, threshold), component);
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
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                diff = (*spectrum)(iBin) - (*splineImp)(iBin - fMinBin);
                sigma += diff * diff;
            }
            sigma = sqrt(sigma * sigmaNorm);

            double thresholdAdd = fSigmaThreshold * sigma;
            KTDEBUG(sdlog, "Discriminator threshold diff for component " << component << " set at <" << thresholdAdd << "> (Sigma mode)");

            // loop over bins, checking against the threshold
            double value, threshold;
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = (*spectrum)(iBin);
                threshold = thresholdAdd + (*splineImp)(iBin - fMinBin);
                if (value >= threshold)
                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin + 0.5), value, threshold), component);
            }
        }

        spline->AddToCache(splineImp);

        return true;

    }

} /* namespace Katydid */
