/*
 * KTSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTSpectrumDiscriminator.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include <cmath>

using std::string;

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
            fCalculateMaxBin(true),
            fInputDataName("frequency-spectrum"),
            fOutputDataName("peak-list")
    {
        fConfigName = "spectrum-discriminator";

        //RegisterSlot(...);
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

        SetInputDataName(node->GetData<string>("input-data-name", fInputDataName));
        SetOutputDataName(node->GetData<string>("output-data-name", fOutputDataName));

        return true;
    }

    KTDiscriminatedPoints1DData* KTSpectrumDiscriminator::Discriminate(const KTFrequencySpectrumData* data)
    {
        if (fCalculateMinBin) SetMinBin(data->GetSpectrum(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(data->GetSpectrum(0)->FindBin(fMaxFrequency));

        UInt_t nChannels = data->GetNChannels();

        KTDiscriminatedPoints1DData* newData = new KTDiscriminatedPoints1DData(nChannels);

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t sigmaNorm = 1. / Double_t(nBins - 1);

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {

            const KTFrequencySpectrum* spectrum = data->GetSpectrum(iChannel);

            Double_t threshold = 0.;

            Double_t mean = 0.;
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                mean += (*spectrum)(iBin).abs();
            }
            mean /= (Double_t)nBins;

            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrum), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (SNR mode)");
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
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (Sigma mode)");
            }

            newData->SetThreshold(threshold, iChannel);

            // loop over bins, checking against the threshold
            Double_t value;
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                value = (*spectrum)(iBin).abs();
                if (value >= threshold) newData->AddPoint(iBin, value, iChannel);
            }

        }

        newData->SetName(fOutputDataName);
        newData->SetEvent(data->GetEvent());

        // emit signal

        return newData;
    }



} /* namespace Katydid */
