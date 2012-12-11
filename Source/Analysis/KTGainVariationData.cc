/*
 * KTGainVariationData.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#include "KTGainVariationData.hh"

#include "KTWriter.hh"

namespace Katydid
{
    KTGainVariationData::KTGainVariationData(unsigned nChannels) :
            KTWriteableData(),
            fChannelData(nChannels)
    {
    }

    KTGainVariationData::~KTGainVariationData()
    {
        while (! fChannelData.empty())
        {
            delete fChannelData.back().fGainVar;
            delete fChannelData.back().fSpline;
            fChannelData.pop_back();
        }
    }

    void KTGainVariationData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

#ifdef ROOT_FOUND
    TH1D* KTGainVariationData::CreateGainVariationHistogram(unsigned channelNum, const std::string& name) const
    {
        GainVariation* gVar = fChannelData[channelNum].fGainVar;
        UInt_t nBins = gVar->size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (Int_t)nBins, gVar->GetRangeMin(), gVar->GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, (*gVar)(iBin));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Gain Variation");
        return hist;
    }
#endif


} /* namespace Katydid */

