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

using std::string;

namespace Katydid
{
    KTLOGGER(sdlog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTSpectrumDiscriminator > sSimpleFFTRegistrar("spectrum-discriminator");

    KTSpectrumDiscriminator::KTSpectrumDiscriminator() :
            KTProcessor(),
            fSNRThreshold(10.),
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

        SetSNRThreshold(node->GetData< Double_t >("snr-threshold", fSNRThreshold));

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

        // Interval: [fMinBin, fMaxBin]
        UInt_t nBins = fMaxBin - fMinBin + 1;

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {

            KTFrequencySpectrum* spectrum = data->GetSpectrum(iChannel);

            Double_t mean = 0.;
            for (UInt_t iBin=fMinBin; iBin<=fMaxBin; iBin++)
            {
                mean += (*spectrum)(iBin).abs();
            }
            mean /= (Double_t)nBins;



        }

        return NULL;
    }



} /* namespace Katydid */
