/*
 * KTSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTSpectrumDiscriminator.hh"

#include "KTCorrelator.hh"
#include "KTDiscriminatedPoints1DData.hh"
//#include "KTDiscriminatedPoints2DData.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
//#include "KTSlidingWindowFSData.hh"
//#include "KTSlidingWindowFSDataFFTW.hh"

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

    static KTDerivedRegistrar< KTProcessor, KTSpectrumDiscriminator > sSimpleFFTRegistrar("spectrum-discriminator");

    KTSpectrumDiscriminator::KTSpectrumDiscriminator() :
            KTProcessor(),
            fSNRThreshold(10.),
            fSigmaThreshold(5.),
            fThresholdMode(eSigma),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true)
    {
        fConfigName = "spectrum-discriminator";

        RegisterSignal("disc-1d", &fDiscrim1DSignal, "void (const KTDiscriminatedPoints1DData*)");
        //RegisterSignal("disc-2d", &fDiscrim2DSignal, "void (const KTDiscriminatedPoints2DData*)");

        RegisterSlot("fs-polar", this, &KTSpectrumDiscriminator::ProcessFrequencySpectrumData, "void (shared_ptr< KTData >)");
        RegisterSlot("fs-fftw", this, &KTSpectrumDiscriminator::ProcessFrequencySpectrumDataFFTW, "void (shared_ptr< KTData >)");
        RegisterSlot("corr", this, &KTSpectrumDiscriminator::ProcessCorrelationData, "void (shared_ptr< KTData >)");
        //RegisterSlot("swfsdata", this, &KTSpectrumDiscriminator::ProcessSlidingWindowFSData, "void (const KTSlidingWindowFSData*)");
        //RegisterSlot("swfsdata-fftw", this, &KTSpectrumDiscriminator::ProcessSlidingWindowFSDataFFTW, "void (const KTSlidingWindowFSDataFFTW*)");
    }

    KTSpectrumDiscriminator::~KTSpectrumDiscriminator()
    {
    }

    Bool_t KTSpectrumDiscriminator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("snr-threshold"))
        {
            SetSNRThreshold(node->GetData< Double_t >("snr-threshold"));
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

    Bool_t KTSpectrumDiscriminator::Discriminate(KTFrequencySpectrumDataPolar& data)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumPolar(0)->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumPolar(0)->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        UInt_t nComponents = data.GetNComponents();

        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(nComponents);

        newData.SetNBins(data.GetSpectrumPolar(0)->size());
        newData.SetBinWidth(data.GetSpectrumPolar(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t sigmaNorm = 1. / Double_t(nBins - 1);

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {

            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);

            Double_t mean = 0.;
#pragma omp parallel for reduction(+:mean)
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                mean += (*spectrum)(iBin).abs();
            }
            mean /= (Double_t)nBins;

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrumPolar), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
#pragma omp parallel for private(diff) reduction(+:sigma)
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    diff = (*spectrum)(iBin).abs() - mean;
                    sigma += diff * diff;
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (Sigma mode)");
            }

            newData.SetThreshold(threshold, iComponent);

            // loop over bins, checking against the threshold
            Double_t value;
#pragma omp parallel for private(value)
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                value = (*spectrum)(iBin).abs();
                if (value >= threshold) newData.AddPoint(iBin, value, iComponent);
            }

        }

        return true;
    }

    Bool_t KTSpectrumDiscriminator::Discriminate(KTFrequencySpectrumDataFFTW& data)
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

        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(nComponents);

        newData.SetNBins(data.GetSpectrumFFTW(0)->size());
        newData.SetBinWidth(data.GetSpectrumFFTW(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t sigmaNorm = 1. / Double_t(nBins - 1);

        // Temporary storage for magnitude values
        vector< Double_t > magnitude(data.GetSpectrumFFTW(0)->size());

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTFrequencySpectrumFFTW* spectrum = data.GetSpectrumFFTW(iComponent);
            if (spectrum->size() != magnitude.size())
            {
                magnitude.resize(spectrum->size());
            }

            Double_t mean = 0.;
#pragma omp parallel for reduction(+:mean)
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                magnitude[iBin] = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                mean += magnitude[iBin];
            }
            mean /= (Double_t)nBins;

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrumPolar), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
#pragma omp parallel for private(diff) reduction(+:sigma)
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    diff = magnitude[iBin] - mean;
                    sigma += diff * diff;
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (Sigma mode)");
            }

            newData.SetThreshold(threshold, iComponent);

            // loop over bins, checking against the threshold
            Double_t value;
#pragma omp parallel for private(value)
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                value = magnitude[iBin];
                if (value >= threshold) newData.AddPoint(iBin, value, iComponent);
            }

        }

        return true;
    }

    Bool_t KTSpectrumDiscriminator::Discriminate(KTCorrelationData& data)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumPolar(0)->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumPolar(0)->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        UInt_t nComponents = data.GetNComponents();

        KTDiscriminatedPoints1DData& newData = data.Of< KTDiscriminatedPoints1DData >().SetNComponents(nComponents);

        newData.SetNBins(data.GetSpectrumPolar(0)->size());
        newData.SetBinWidth(data.GetSpectrumPolar(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t sigmaNorm = 1. / Double_t(nBins - 1);

        // Temporary storage for magnitude values
        vector< Double_t > magnitude(data.GetSpectrumPolar(0)->size());

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);

            Double_t mean = 0.;
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                mean += (*spectrum)(iBin).abs();
            }
            mean /= (Double_t)nBins;

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrumPolar), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    diff = (*spectrum)(iBin).abs() - mean;
                    sigma += diff * diff;
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold for channel " << iComponent << " set at <" << threshold << "> (Sigma mode)");
            }

            newData.SetThreshold(threshold, iComponent);

            // loop over bins, checking against the threshold
            Double_t value;
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                value = (*spectrum)(iBin).abs();
                if (value >= threshold) newData.AddPoint(iBin, value, iComponent);
            }
        }

        return true;
    }
/*
    KTDiscriminatedPoints2DData* KTSpectrumDiscriminator::Discriminate(const KTSlidingWindowFSData* data)
    {
        if (fCalculateMinBin) SetMinBin((*(data->GetSpectra(0)))(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin((*(data->GetSpectra(0)))(0)->FindBin(fMaxFrequency));

        UInt_t nComponents = data->GetNComponents();

        KTDiscriminatedPoints2DData* newData = new KTDiscriminatedPoints2DData(nComponents);

        newData->SetNBinsX(data->GetSpectra(0)->size());
        newData->SetNBinsY((*(data->GetSpectra(0)))(0)->size());
        newData->SetBinWidthX(data->GetSpectra(0)->GetBinWidth());
        newData->SetBinWidthY((*(data->GetSpectra(0)))(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {

            const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra = data->GetSpectra(iComponent);

            Double_t sigmaNorm = 1. / Double_t((nBins * spectra->size()) - 1);

            Double_t mean = 0.;
            for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
            {
                KTFrequencySpectrumPolar* spectrum = (*spectra)(iSpectrum);

                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    mean += (*spectrum)(iBin).abs();
                }
            }
            mean /= (Double_t)(nBins * spectra->size());

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrumPolar), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
                for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
                {
                    KTFrequencySpectrumPolar* spectrum = (*spectra)(iSpectrum);
                    for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                    {
                        diff = (*spectrum)(iBin).abs() - mean;
                        sigma += diff * diff;
                    }
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (Sigma mode)");
            }

            newData->SetThreshold(threshold, iComponent);

            // loop over bins, checking against the threshold
            Double_t value;
            for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
            {
                KTFrequencySpectrumPolar* spectrum = (*spectra)(iSpectrum);
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    value = (*spectrum)(iBin).abs();
                    if (value >= threshold) newData->AddPoint(iSpectrum, iBin, value, iComponent);
                }
            }

        }

        //newData->SetTimeInRun(-1.);
        //newData->SetSliceNumber(0);

        newData->SetName(fOutputDataName);

        return newData;
    }

    KTDiscriminatedPoints2DData* KTSpectrumDiscriminator::Discriminate(const KTSlidingWindowFSDataFFTW* data)
    {
        if (fCalculateMinBin) SetMinBin((*(data->GetSpectra(0)))(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin((*(data->GetSpectra(0)))(0)->FindBin(fMaxFrequency));

        UInt_t nComponents = data->GetNComponents();

        KTDiscriminatedPoints2DData* newData = new KTDiscriminatedPoints2DData(nComponents);

        newData->SetNBinsX(data->GetSpectra(0)->size());
        newData->SetNBinsY((*(data->GetSpectra(0)))(0)->size());
        newData->SetBinWidthX(data->GetSpectra(0)->GetBinWidth());
        newData->SetBinWidthY((*(data->GetSpectra(0)))(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;

        // Temporary storage for magnitude values
        vector< vector< Double_t > > magnitude((*(data->GetSpectra(0)))(0)->size());

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra = data->GetSpectra(iComponent);

            Double_t sigmaNorm = 1. / Double_t((nBins * spectra->size()) - 1);

            if (spectra->size() != magnitude.size())
            {
                magnitude.resize(spectra->size());
            }

            Double_t mean = 0.;
            for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
            {
                KTFrequencySpectrumFFTW* spectrum = (*spectra)(iSpectrum);
                if (magnitude[iSpectrum].size() != spectrum->size()) magnitude[iSpectrum].resize(spectrum->size());
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    magnitude[iSpectrum][iBin] = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                    mean += magnitude[iSpectrum][iBin];
                }
            }
            mean /= (Double_t)(nBins * spectra->size());

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrumPolar), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
                for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
                {
                    KTFrequencySpectrumFFTW* spectrum = (*spectra)(iSpectrum);
                    for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                    {
                        diff = magnitude[iSpectrum][iBin] - mean;
                        sigma += diff * diff;
                    }
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (Sigma mode)");
            }

            newData->SetThreshold(threshold, iComponent);

            // loop over bins, checking against the threshold
            Double_t value;
            for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
            {
                KTFrequencySpectrumFFTW* spectrum = (*spectra)(iSpectrum);
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    value = magnitude[iSpectrum][iBin];
                    if (value >= threshold) newData->AddPoint(iSpectrum, iBin, value, iComponent);
                }
            }
        }

        //newData->SetTimeInRun(-1.);
        //newData->SetSliceNumber(0);

        newData->SetName(fOutputDataName);

        return newData;
    }
*/
    void KTSpectrumDiscriminator::ProcessFrequencySpectrumData(shared_ptr< KTData > data)
    {
        if (! data->Has< KTFrequencySpectrumDataPolar >())
        {
            KTERROR(sdlog, "No frequency spectrum (Polar) data was present");
            return;
        }
        if (! Discriminate(data->Of< KTFrequencySpectrumDataPolar >()))
        {
            KTERROR(sdlog, "Something went wrong while performing discrimination");
            return;
        }
        fDiscrim1DSignal(data);
        return;
    }

    void KTSpectrumDiscriminator::ProcessFrequencySpectrumDataFFTW(shared_ptr< KTData > data)
    {
        if (! data->Has< KTFrequencySpectrumDataFFTW >())
        {
            KTERROR(sdlog, "No frequency spectrum (FFTW) data was present");
            return;
        }
        if (! Discriminate(data->Of< KTFrequencySpectrumDataFFTW >()))
        {
            KTERROR(sdlog, "Something went wrong while performing discrimination");
            return;
        }
        fDiscrim1DSignal(data);
        return;
    }

    void KTSpectrumDiscriminator::ProcessCorrelationData(shared_ptr< KTData > data)
    {
        if (! data->Has< KTCorrelationData >())
        {
            KTERROR(sdlog, "No frequency spectrum (FFTW) data was present");
            return;
        }
        if (! Discriminate(data->Of< KTCorrelationData >()))
        {
            KTERROR(sdlog, "Something went wrong while performing discrimination");
            return;
        }
        fDiscrim1DSignal(data);
        return;
    }
/*
    void KTSpectrumDiscriminator::ProcessSlidingWindowFSData(const KTSlidingWindowFSData* data)
    {
    }

    void KTSpectrumDiscriminator::ProcessSlidingWindowFSDataFFTW(const KTSlidingWindowFSDataFFTW* data)
    {
    }
*/
} /* namespace Katydid */
