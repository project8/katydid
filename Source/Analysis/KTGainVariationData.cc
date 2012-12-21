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
    KTGainVariationData::KTGainVariationData(UInt_t nChannels) :
            KTWriteableData(),
            fChannelData(nChannels)
    {
    }

    KTGainVariationData::~KTGainVariationData()
    {
        while (! fChannelData.empty())
        {
            //delete fChannelData.back().fGainVar;
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
    TH1D* KTGainVariationData::CreateGainVariationHistogram(UInt_t nBins, UInt_t channelNum, const std::string& name) const
    {
        KTSpline* spline = fChannelData[channelNum].fSpline;
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", nBins, spline->GetXMin(), spline->GetXMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, spline->Evaluate(hist->GetBinCenter(iBin+1)));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Gain Variation");
        return hist;
    }
#endif


} /* namespace Katydid */

