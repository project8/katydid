/*
 * KTSlidingWindowFSData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTSlidingWindowFSData.hh"

#include "KTFrequencySpectrum.hh"
#include "KTWriter.hh"

using std::vector;

namespace Katydid
{
    KTSlidingWindowFSData::KTSlidingWindowFSData(unsigned nChannels) :
            KTWriteableData(),
            fSpectra(nChannels)
    {
    }

    KTSlidingWindowFSData::~KTSlidingWindowFSData()
    {
        while (! fSpectra.empty())
        {
            KTPhysicalArray< 1, KTFrequencySpectrum* >* backSpectra = fSpectra.back();
            for (KTPhysicalArray< 1, KTFrequencySpectrum* >::iterator iter = backSpectra->begin(); iter != backSpectra->end(); iter++)
            {
                delete *iter;
            }
            delete backSpectra;
            fSpectra.pop_back();
        }
    }

    void KTSlidingWindowFSData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

#ifdef ROOT_FOUND
    TH2D* KTSlidingWindowFSData::CreateMagnitudeHistogram(unsigned channelNum, const std::string& name) const
    {
        if (channelNum >= fSpectra.size()) return NULL;
        if (fSpectra[channelNum]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Frequency Spectrum Magnitude",
                fSpectra[channelNum]->size(), fSpectra[channelNum]->GetRangeMin(), fSpectra[channelNum]->GetRangeMax(),
                (*fSpectra[channelNum])(0)->size(), (*fSpectra[channelNum])(0)->GetRangeMin(), (*fSpectra[channelNum])(0)->GetRangeMax());

        KTINFO("Frequency axis: " << (*fSpectra[channelNum])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO("Time axis: " << fSpectra[channelNum]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (Int_t iBinX=1; iBinX<=(Int_t)fSpectra[channelNum]->size(); iBinX++)
        {
            KTFrequencySpectrum* fs = (*fSpectra[channelNum])(iBinX-1);
            for (Int_t iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fs)(iBinY-1).abs());
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    TH2D* KTSlidingWindowFSData::CreatePhaseHistogram(unsigned channelNum, const std::string& name) const
    {
        if (channelNum >= fSpectra.size()) return NULL;
        if (fSpectra[channelNum]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Frequency Spectrum Phase",
                fSpectra[channelNum]->size(), fSpectra[channelNum]->GetRangeMin(), fSpectra[channelNum]->GetRangeMax(),
                (*fSpectra[channelNum])(0)->size(), (*fSpectra[channelNum])(0)->GetRangeMin(), (*fSpectra[channelNum])(0)->GetRangeMax());

        KTINFO("Frequency axis: " << (*fSpectra[channelNum])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO("Time axis: " << fSpectra[channelNum]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (Int_t iBinX=1; iBinX<=(Int_t)fSpectra[channelNum]->size(); iBinX++)
        {
            KTFrequencySpectrum* fs = (*fSpectra[channelNum])(iBinX-1);
            for (Int_t iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fs)(iBinY-1).arg());
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    TH2D* KTSlidingWindowFSData::CreatePowerHistogram(unsigned channelNum, const std::string& name) const
    {
        if (channelNum >= fSpectra.size()) return NULL;
        if (fSpectra[channelNum]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Power Spectra",
                fSpectra[channelNum]->size(), fSpectra[channelNum]->GetRangeMin(), fSpectra[channelNum]->GetRangeMax(),
                (*fSpectra[channelNum])(0)->size(), (*fSpectra[channelNum])(0)->GetRangeMin(), (*fSpectra[channelNum])(0)->GetRangeMax());

        KTINFO("Frequency axis: " << (*fSpectra[channelNum])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO("Time axis: " << fSpectra[channelNum]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        Double_t value;
        for (Int_t iBinX=1; iBinX<=(Int_t)fSpectra[channelNum]->size(); iBinX++)
        {
            KTFrequencySpectrum* fs = (*fSpectra[channelNum])(iBinX-1);
            for (Int_t iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                value =(*fs)(iBinY-1).abs();
                hist->SetBinContent(iBinX, iBinY, value*value);
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

 #endif


} /* namespace Katydid */

