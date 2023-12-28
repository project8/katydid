/*
 * KTVariableSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTVariableSpectrumDiscriminator.hh"

#include "KTConvolvedSpectrumData.hh"
#include "KTCorrelationData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTDiscriminatedPoints2DData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTNormalizedFSData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTSpline.hh"
#include "KTWignerVilleData.hh"

#include <cmath>
#include <vector>
#include <algorithm>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::string;
using std::vector;


namespace Katydid
{
    LOGGER(sdlog, "KTVariableSpectrumDiscriminator");

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
            fNeighborhoodRadius(0),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fNormalize(false),
            fMagnitudeCache(),
            fDiscrim1DSignal("disc-1d", this),
            fDiscrim2DSignal("disc-2d", this),
            fFSPolarSlot("fs-polar", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fFSFFTWSlot("fs-fftw", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fConvPSSlot("conv-ps", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fNormFSPolarSlot("norm-fs-polar", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fCorrSlot("corr", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fWVSlot("wv", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fPSSlot("ps", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fSpecSlot("spec", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim2DSignal),
            fPreCalcSlot("gv", this, &KTVariableSpectrumDiscriminator::SetPreCalcGainVar),
            fConvPSPreCalcSlot("conv-ps-pre", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fPSPreCalcSlot("ps-pre", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim1DSignal),
            fSpecPreCalcSlot("spec-pre", this, &KTVariableSpectrumDiscriminator::Discriminate, &fDiscrim2DSignal)
    {
    }

    KTVariableSpectrumDiscriminator::~KTVariableSpectrumDiscriminator()
    {
    }

    bool KTVariableSpectrumDiscriminator::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        // The if(has) pattern is used here so that Set[whatever] is only called if the particular parameter is present.

        if (node->has("neighborhood-Radius"))
        {
            SetNeighborhoodRadius(node->get_value< int >("neighborhood-radius"));
        }

        // These Set[whatever] functions also change the threshold mode, 
        // so we only want to call them if we are setting the value, and not just keeping the existing value.
        if (node->has("snr-threshold-amplitude"))
        {
            SetSNRAmplitudeThreshold(node->get_value< double >("snr-threshold-amplitude"));
        }
        if (node->has("snr-threshold-power"))
        {
            SetSNRPowerThreshold(node->get_value< double >("snr-threshold-power"));
        }
        if (node->has("sigma-threshold"))
        {
            SetSigmaThreshold(node->get_value< double >("sigma-threshold"));
        }

        // These Set[whatever] functions also set the flags to calculate the min/max bin, 
        // so we only want to call them if we are setting the value, and not just keeping the existing value.
        if (node->has("min-frequency"))
        {
            SetMinFrequency(node->get_value< double >("min-frequency"));
        }
        if (node->has("max-frequency"))
        {
            SetMaxFrequency(node->get_value< double >("max-frequency"));
        }

        // These Set[whatever] functions also set the flags to calculate the min/max bin, 
        // so we only want to call them if we are setting the value, and not just keeping the existing value.
        if (node->has("min-bin"))
        {
            SetMinBin(node->get_value< unsigned >("min-bin"));
        }
        if (node->has("max-bin"))
        {
            SetMaxBin(node->get_value< unsigned >("max-bin"));
        }

        SetNormalize(node->get_value< bool >("normalize", fNormalize));

        return true;
    }

    bool KTVariableSpectrumDiscriminator::CheckGVData()
    {
        if( fGVData.GetSpline() == nullptr )
        {
            LERROR( sdlog, "I don't have any gain variation data! Did you forget to send me some?" );
            return false;
        }
        if( fGVData.GetVarianceSpline() == nullptr && fThresholdMode == eSigma )
        {
            LERROR( sdlog, "I don't have any gain variation variance data! Did you forget to send me some?" );
            return false;
        }

        return true;
    }

    bool KTVariableSpectrumDiscriminator::SetPreCalcGainVar(KTGainVariationData& gvData)
    {
        fGVData = gvData;
        return true;
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTConvolvedPowerSpectrumData& data)
    {
        
        if( ! CheckGVData() ){ return false; }

        return Discriminate(data, fGVData);
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTPowerSpectrumData& data)
    {
        if( ! CheckGVData() ){ return false; }
        
        return Discriminate(data, fGVData);
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTPSCollectionData& data)
    {
        if( ! CheckGVData() ){ return false; }
        
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

    bool KTVariableSpectrumDiscriminator::Discriminate(KTConvolvedPowerSpectrumData& data, KTGainVariationData& gvData)
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
        return CoreDiscriminate(data, gvData, newData);
    }

    bool KTVariableSpectrumDiscriminator::Discriminate(KTPSCollectionData& data, KTGainVariationData& gvData)
    {
        KTDiscriminatedPoints2DData& newData = data.Of< KTDiscriminatedPoints2DData >();
        KTDiscriminatedPoints1DData newDataSlice; // The 1DData will be used with the discrimination methods already in place to iteratively achieve a 2D discrimination

        // Min and Max frequency must be set according to the PSCollectionData
        // We use temporary variables to avoid overwriting the global fMin(Max)Frequency specified in the config

        double tempMinFrequency = std::max( data.GetMinFreq(), fMinFrequency );
        double tempMaxFrequency = std::min( data.GetMaxFreq(), fMaxFrequency );

        // Min and Max bins must be recalculated with these temp frequencies
        SetMinBin((*data.GetSpectra())(0)->FindBin(tempMinFrequency));
        LDEBUG(sdlog, "Minimum bin set to " << fMinBin);
    
        SetMaxBin((*data.GetSpectra())(0)->FindBin(tempMaxFrequency) - 1); // -1 to avoid out-of-range error
        LDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        
        // Parametrize 2D and 1D point objects

        newData.SetNBinsX( data.GetSpectra()->GetNBins() );
        newData.SetNBinsY( (*data.GetSpectra())(0)->GetNFrequencyBins() );
        newData.SetBinWidthX( data.GetDeltaT() );
        newData.SetBinWidthY( (*data.GetSpectra())(0)->GetFrequencyBinWidth() );

        newDataSlice.SetNComponents( 1 );
        newDataSlice.SetNBins( (*data.GetSpectra())(0)->GetNFrequencyBins() );
        newDataSlice.SetBinWidth( (*data.GetSpectra())(0)->GetFrequencyBinWidth() );

        // X and Y bin width for the 2D points
        double XbinWidth = data.GetDeltaT();
        double YbinWidth = (*data.GetSpectra())(0)->GetFrequencyBinWidth();

        // X and Y of first bin
        double Xmin = data.GetStartTime();
        double Ymin = (*data.GetSpectra())(0)->GetRangeMin();

        LDEBUG(sdlog, "Set XbinWidth to " << XbinWidth << " and YbinWidth to " << YbinWidth);

        unsigned nSpectra = data.GetSpectra()->GetNBins();     // Number of time slices in the spectrogram collection
        unsigned nPoints = 0;                           // Number of points above threshold in one slice
        unsigned sliceNumber = 0;                       // Slice counter

        // Iterate through the power spectra
        for( KTPhysicalArray< 1, KTPowerSpectrum* >::const_iterator it = data.GetSpectra()->begin(); it != data.GetSpectra()->end(); ++it )
        {
            // To avoid confusion using newDataSlice in a loop, each time slice with be associated to a new component
            newDataSlice.SetNComponents( sliceNumber + 1 );

            // Discriminate the 1D spectrum
            if (! DiscriminateSpectrum(*it, gvData.GetSpline(0), gvData.GetVarianceSpline(0), newDataSlice, sliceNumber))
            {
                LERROR(sdlog, "Discrimination on spectrogram (slice " << sliceNumber << ") failed");
                return false;
            }

            nPoints = newDataSlice.GetSetOfPoints( sliceNumber ).size();
            LDEBUG(sdlog, "Spectrogram slice " << sliceNumber << " has " << nPoints << " points above threshold");

            // Iterate through the 1D points and add them to the 2D points
            for( KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pointsIt = newDataSlice.GetSetOfPoints( sliceNumber ).begin(); pointsIt != newDataSlice.GetSetOfPoints( sliceNumber ).end(); ++pointsIt )
            {
                newData.AddPoint( sliceNumber, pointsIt->first, KTDiscriminatedPoints2DData::Point( XbinWidth * ((double)sliceNumber+0.5) + Xmin, YbinWidth * ((double)pointsIt->first+0.5) + Ymin, pointsIt->second.fOrdinate, pointsIt->second.fThreshold, pointsIt->second.fMean, pointsIt->second.fVariance, pointsIt->second.fNeighborhoodAmplitude ), 0 );
            }

            sliceNumber++;
        }

        return true;
    }

    bool KTVariableSpectrumDiscriminator::CoreDiscriminate(KTFrequencySpectrumDataPolarCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumPolar(0)->FindBin(fMinFrequency));
            LDEBUG(sdlog, "Minimum bin set to " << fMinBin << " (frequency: " << fMinFrequency << ")");
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumPolar(0)->FindBin(fMaxFrequency));
            LDEBUG(sdlog, "Maximum bin set to " << fMaxBin << " (frequency: " << fMaxFrequency << ")");
        }

        unsigned nComponents = data.GetNComponents();

        newData.SetNBins(data.GetSpectrumPolar(0)->size());
        newData.SetBinWidth(data.GetSpectrumPolar(0)->GetBinWidth());

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            if (! DiscriminateSpectrum(data.GetSpectrumPolar(iComponent), gvData.GetSpline(iComponent), gvData.GetVarianceSpline(iComponent), newData, iComponent))
            {
                LERROR(sdlog, "Discrimination on spectrum (component " << iComponent << ") failed");
                return false;
            }
            LDEBUG(sdlog, "Component " << iComponent << " has " << newData.GetSetOfPoints(iComponent).size() << " points above threshold");
        }
        LINFO(sdlog, "Completed discrimination on " << nComponents << " components");

        return true;


    }

    bool KTVariableSpectrumDiscriminator::CoreDiscriminate(KTFrequencySpectrumDataFFTWCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumFFTW(0)->FindBin(fMinFrequency));
            LDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumFFTW(0)->FindBin(fMaxFrequency));
            LDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        unsigned nComponents = data.GetNComponents();

        newData.SetNBins(data.GetSpectrumFFTW(0)->size());
        newData.SetBinWidth(data.GetSpectrumFFTW(0)->GetBinWidth());

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            if (! DiscriminateSpectrum(data.GetSpectrumFFTW(iComponent), gvData.GetSpline(iComponent), gvData.GetVarianceSpline(iComponent), newData, iComponent))
            {
                LERROR(sdlog, "Discrimination on spectrum (component " << iComponent << ") failed");
                return false;
            }
            LDEBUG(sdlog, "Component " << iComponent << " has " << newData.GetSetOfPoints(iComponent).size() << " points above threshold");
        }
        LINFO(sdlog, "Completed discrimination on " << nComponents << " components");

        return true;
    }

    bool KTVariableSpectrumDiscriminator::CoreDiscriminate(KTPowerSpectrumDataCore& data, KTGainVariationData& gvData, KTDiscriminatedPoints1DData& newData)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrum(0)->FindBin(fMinFrequency));
            LDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrum(0)->FindBin(fMaxFrequency));
            LDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        unsigned nComponents = data.GetNComponents();

        newData.SetNBins(data.GetSpectrum(0)->size());
        newData.SetBinWidth(data.GetSpectrum(0)->GetBinWidth());

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            if (! DiscriminateSpectrum(data.GetSpectrum(iComponent), gvData.GetSpline(iComponent), gvData.GetVarianceSpline(iComponent), newData, iComponent))
            {
                LERROR(sdlog, "Discrimination on spectrum (component " << iComponent << ") failed");
                return false;
            }
            LDEBUG(sdlog, "Component " << iComponent << " has " << newData.GetSetOfPoints(iComponent).size() << " points above threshold");
        }
        LINFO(sdlog, "Completed discrimination on " << nComponents << " components");

        return true;
    }

    bool KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTFrequencySpectrumPolar* spectrum, const KTSpline* spline, const KTSpline* varSpline, KTDiscriminatedPoints1DData& newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            LERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double binWidth = spectrum->GetBinWidth();
        double freqMin = spectrum->GetBinLowEdge(fMinBin);
        double freqMax = spectrum->GetBinLowEdge(fMaxBin) + spectrum->GetBinWidth();
        std::shared_ptr< KTSpline::Implementation > splineImp = spline->Implement(nBins, freqMin, freqMax);
        std::shared_ptr< KTSpline::Implementation > varSplineImp = varSpline->Implement(nBins, freqMin, freqMax);

        // Average of each spline
        double normalizedValue = splineImp->GetMean();
        double normalizedVariance = varSplineImp->GetMean();

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
                LDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-amplitude mode)");
            }
            else
            {
                // SNR = P_signal / P_noise, P_noise = mean
                thresholdMult = fSNRThreshold;
                LDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-power mode)");
            }

            // loop over bins, checking against the threshold
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                double value = (*spectrum)(iBin).abs();
                double threshold = thresholdMult * (*splineImp)(iBin - fMinBin);
                double mean = (*splineImp)(iBin - fMinBin);
                double variance = (*varSplineImp)(iBin - fMinBin);
                if (value >= threshold)
                {
                    double neighborhoodAmplitude = 0.;
                    this->SumAdjacentBinAmplitude(spectrum, neighborhoodAmplitude, iBin);

                    if( fNormalize )
                    {
                        value = normalizedValue + (value - mean) * sqrt( normalizedVariance / variance );
                        neighborhoodAmplitude = normalizedValue + ( neighborhoodAmplitude - ( 2 * fNeighborhoodRadius+1 ) * mean ) * sqrt( normalizedVariance / variance );
                        mean = normalizedValue;
                        variance = normalizedVariance;
                    }
                    else
                    {
                        neighborhoodAmplitude = neighborhoodAmplitude - ( 2 * fNeighborhoodRadius ) * mean;
                    }

                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin), value, threshold, mean, variance, neighborhoodAmplitude), component);
                }
            }
        }
        //**************
        // Sigma mode
        //**************
        else if (fThresholdMode == eSigma)
        {
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                double mean = (*splineImp)(iBin - fMinBin);
                double variance = (*varSplineImp)(iBin - fMinBin);
                double threshold = mean + fSigmaThreshold * sqrt( variance );
                double value = (*spectrum)(iBin).abs();

                if (value >= threshold)
                {
                    double neighborhoodAmplitude = 0.;
                    this->SumAdjacentBinAmplitude(spectrum, neighborhoodAmplitude, iBin);
                    if( fNormalize )
                    {
                        value = normalizedValue + (value - mean) * sqrt( normalizedVariance / variance );
                        neighborhoodAmplitude = normalizedValue + ( neighborhoodAmplitude - ( 2 * fNeighborhoodRadius+1 ) * mean ) * sqrt( normalizedVariance / variance );
                        mean = normalizedValue;
                        variance = normalizedVariance;
                    }
                    else
                    {
                        neighborhoodAmplitude = neighborhoodAmplitude - ( 2 * fNeighborhoodRadius ) * mean;
                    }

                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin), value, threshold, mean, variance, neighborhoodAmplitude), component);
                }
            }
        }

        return true;
    }

    bool KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTFrequencySpectrumFFTW* spectrum, const KTSpline* spline, const KTSpline* varSpline, KTDiscriminatedPoints1DData& newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            LERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double binWidth = spectrum->GetBinWidth();
        double freqMin = spectrum->GetBinLowEdge(fMinBin);
        double freqMax = spectrum->GetBinLowEdge(fMaxBin) + binWidth;
        std::shared_ptr< KTSpline::Implementation > splineImp = spline->Implement(nBins, freqMin, freqMax);
        std::shared_ptr< KTSpline::Implementation > varSplineImp = varSpline->Implement(nBins, freqMin, freqMax);

        // Average of each spline
        double normalizedValue = splineImp->GetMean();
        double normalizedVariance = varSplineImp->GetMean();

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
                LDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-amplitude mode)");
            }
            else
            {
                // SNR = P_signal / P_noise, P_noise = mean
                thresholdMult = fSNRThreshold;
                LDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-power mode)");
            }

            // loop over bins, checking against the threshold
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                double value = spectrum->GetAbs(iBin);
                double threshold = thresholdMult * (*splineImp)(iBin - fMinBin);
                double mean = (*splineImp)(iBin - fMinBin);
                double variance = (*varSplineImp)(iBin - fMinBin);

                if (value >= threshold)
                {
                    double neighborhoodAmplitude = 0.;
                    this->SumAdjacentBinAmplitude(spectrum, neighborhoodAmplitude, iBin);
                    if( fNormalize )
                    {
                        value = normalizedValue + (value - mean) * sqrt( normalizedVariance / variance );
                        neighborhoodAmplitude = normalizedValue + ( neighborhoodAmplitude - ( 2 * fNeighborhoodRadius+1 ) * mean ) * sqrt( normalizedVariance / variance );
                        mean = normalizedValue;
                        variance = normalizedVariance;
                    }
                    else
                    {
                        neighborhoodAmplitude = neighborhoodAmplitude - ( 2 * fNeighborhoodRadius ) * mean;
                    }

                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin), value, threshold, mean, variance, neighborhoodAmplitude), component);
                }
            }
        }
        //**************
        // Sigma mode
        //**************
        else if (fThresholdMode == eSigma)
        {
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                double mean = (*splineImp)(iBin - fMinBin);
                double variance = (*varSplineImp)(iBin - fMinBin);
                double threshold = mean + fSigmaThreshold * sqrt( variance );
                double value = spectrum->GetAbs(iBin);
                if (value >= threshold)
                {
                    double neighborhoodAmplitude = 0.;
                    this->SumAdjacentBinAmplitude(spectrum, neighborhoodAmplitude, iBin);
                    if( fNormalize )
                    {
                        value = normalizedValue + (value - mean) * sqrt( normalizedVariance / variance );
                        neighborhoodAmplitude = normalizedValue + ( neighborhoodAmplitude - ( 2 * fNeighborhoodRadius+1 ) * mean ) * sqrt( normalizedVariance / variance );
                        mean = normalizedValue;
                        variance = normalizedVariance;
                    }
                    else
                    {
                        neighborhoodAmplitude = neighborhoodAmplitude - ( 2 * fNeighborhoodRadius ) * mean;
                    }

                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin), value, threshold, mean, variance, neighborhoodAmplitude), component);
                }
            }
        }

        return true;
    }

    bool KTVariableSpectrumDiscriminator::DiscriminateSpectrum(const KTPowerSpectrum* spectrum, const KTSpline* spline, const KTSpline* varSpline, KTDiscriminatedPoints1DData& newData, unsigned component)
    {
        if (spectrum == NULL)
        {
            LERROR(sdlog, "Frequency spectrum pointer (component " << component << ") is NULL!");
            return false;
        }

        unsigned nBins = fMaxBin - fMinBin + 1;
        double binWidth = spectrum->GetBinWidth();
        double freqMin = spectrum->GetBinLowEdge(fMinBin);
        double freqMax = spectrum->GetBinLowEdge(fMaxBin) + spectrum->GetBinWidth();
        std::shared_ptr< KTSpline::Implementation > splineImp = spline->Implement(nBins, freqMin, freqMax);
        std::shared_ptr< KTSpline::Implementation > varSplineImp = varSpline->Implement(nBins, freqMin, freqMax);

        // Average of each spline
        double normalizedValue = splineImp->GetMean();
        double normalizedVariance = varSplineImp->GetMean();

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
                LDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-amplitude mode)");
            }
            else
            {
                // SNR = P_signal / P_noise, P_noise = mean
                thresholdMult = fSNRThreshold;
                LDEBUG(sdlog, "Discriminator threshold multiplier for component " << component << " set at <" << thresholdMult << "> (SNR-power mode)");
            }

            // loop over bins, checking against the threshold
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                double value = (*spectrum)(iBin);
                double mean = (*splineImp)(iBin - fMinBin);
                double threshold = thresholdMult * mean;
                double variance = (*varSplineImp)(iBin - fMinBin);
                if (value >= threshold)
                {
                    double neighborhoodAmplitude = 0.;
                    this->SumAdjacentBinAmplitude(spectrum, neighborhoodAmplitude, iBin);
                    if( fNormalize )
                    {
                        value = normalizedValue + (value - mean) * sqrt( normalizedVariance / variance );
                        neighborhoodAmplitude = normalizedValue + ( neighborhoodAmplitude - ( 2 * fNeighborhoodRadius+1 ) * mean ) * sqrt( normalizedVariance / variance );
                        mean = normalizedValue;
                        variance = normalizedVariance;
                    }
                    else
                    {
                        neighborhoodAmplitude = neighborhoodAmplitude - ( 2 * fNeighborhoodRadius ) * mean;
                    }

                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin), value, threshold, mean, variance, neighborhoodAmplitude), component);
                }
            }
        }

        //**************
        // Sigma mode
        //**************
        else if (fThresholdMode == eSigma)
        {
#pragma omp parallel for private(value)
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                double mean = (*splineImp)(iBin - fMinBin);
                double variance = (*varSplineImp)(iBin - fMinBin);
                double threshold = mean + fSigmaThreshold * sqrt( variance );
                double value = (*spectrum)(iBin);
                if (value >= threshold)
                {
                    double neighborhoodAmplitude = 0.;
                    this->SumAdjacentBinAmplitude(spectrum, neighborhoodAmplitude, iBin);
                    if( fNormalize )
                    {
                        value = normalizedValue + (value - mean) * sqrt( normalizedVariance / variance );
                        neighborhoodAmplitude = normalizedValue + ( neighborhoodAmplitude - ( 2 * fNeighborhoodRadius+1 ) * mean ) * sqrt( normalizedVariance / variance );
                        mean = normalizedValue;
                        variance = normalizedVariance;
                    }
                    else
                    {
                        neighborhoodAmplitude = neighborhoodAmplitude - ( 2 * fNeighborhoodRadius ) * mean;
                    }

                    newData.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(binWidth * ((double)iBin), value, threshold, mean, variance, neighborhoodAmplitude), component);
                }
            }
        }

        return true;

    }

    void KTVariableSpectrumDiscriminator::SumAdjacentBinAmplitude(const KTPowerSpectrum* spectrum, double& neighborhoodAmplitude, const unsigned& iBin)
    {
        neighborhoodAmplitude = 0;
        for (unsigned jBin = iBin-fNeighborhoodRadius; jBin <= iBin+fNeighborhoodRadius; ++jBin)
        {
            neighborhoodAmplitude += (*spectrum)(jBin);
        }
    }
    void KTVariableSpectrumDiscriminator::SumAdjacentBinAmplitude(const KTFrequencySpectrumFFTW* spectrum, double& neighborhoodAmplitude, const unsigned& iBin)
    {
        neighborhoodAmplitude = 0;
        for (unsigned jBin = iBin-fNeighborhoodRadius; jBin <= iBin+fNeighborhoodRadius; ++jBin)
        {
            neighborhoodAmplitude += spectrum->GetAbs(jBin);
        }
    }
    void KTVariableSpectrumDiscriminator::SumAdjacentBinAmplitude(const KTFrequencySpectrumPolar* spectrum, double& neighborhoodAmplitude, const unsigned& iBin)
    {
        neighborhoodAmplitude = 0;
        for (unsigned jBin = iBin-fNeighborhoodRadius; jBin <= iBin+fNeighborhoodRadius; ++jBin)
        {
            neighborhoodAmplitude += (*spectrum)(jBin).abs();
        }
    }


} /* namespace Katydid */
