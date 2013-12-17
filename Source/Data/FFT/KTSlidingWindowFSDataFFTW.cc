/*
 * KTSlidingWindowFSDataFFTW.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTSlidingWindowFSDataFFTW.hh"

#include "KTDataMap.hh"
#include "KTTIFactory.hh"
#include "KTWriter.hh"

#include <cmath>

using std::vector;

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTSlidingWindowFSDataFFTW > > sSWFSFFTWRegistrar;

    KTSlidingWindowFSDataFFTW::KTSlidingWindowFSDataFFTW(unsigned nChannels) :
            KTWriteableData(),
            fSpectra(nChannels)
    {
    }

    KTSlidingWindowFSDataFFTW::~KTSlidingWindowFSDataFFTW()
    {
        while (! fSpectra.empty())
        {
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* backSpectra = fSpectra.back();
            for (KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >::iterator iter = backSpectra->begin(); iter != backSpectra->end(); iter++)
            {
                delete *iter;
            }
            delete backSpectra;
            fSpectra.pop_back();
        }
    }

    void KTSlidingWindowFSDataFFTW::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

#ifdef ROOT_FOUND
    TH2D* KTSlidingWindowFSDataFFTW::CreateMagnitudeHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fSpectra.size()) return NULL;
        if (fSpectra[component]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Frequency Spectrum Magnitude",
                fSpectra[component]->size(), fSpectra[component]->GetRangeMin(), fSpectra[component]->GetRangeMax(),
                (*fSpectra[component])(0)->size(), (*fSpectra[component])(0)->GetRangeMin(), (*fSpectra[component])(0)->GetRangeMax());

        KTINFO("Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz; bin width: " << hist->GetYaxis()->GetBinWidth(1) << " Hz");
        KTINFO("Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s; bin width: " << hist->GetXaxis()->GetBinWidth(1) << " s");

        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, sqrt((*fs)(iBinY-1)[0] * (*fs)(iBinY-1)[0] + (*fs)(iBinY-1)[1] * (*fs)(iBinY-1)[1]));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    TH2D* KTSlidingWindowFSDataFFTW::CreatePhaseHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fSpectra.size()) return NULL;
        if (fSpectra[component]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Frequency Spectrum Phase",
                fSpectra[component]->size(), fSpectra[component]->GetRangeMin(), fSpectra[component]->GetRangeMax(),
                (*fSpectra[component])(0)->size(), (*fSpectra[component])(0)->GetRangeMin(), (*fSpectra[component])(0)->GetRangeMax());

        KTINFO("Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO("Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, atan2((*fs)(iBinY-1)[1], (*fs)(iBinY-1)[0]));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    TH2D* KTSlidingWindowFSDataFFTW::CreatePowerHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fSpectra.size()) return NULL;
        if (fSpectra[component]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Power Spectra",
                fSpectra[component]->size(), fSpectra[component]->GetRangeMin(), fSpectra[component]->GetRangeMax(),
                (*fSpectra[component])(0)->size(), (*fSpectra[component])(0)->GetRangeMin(), (*fSpectra[component])(0)->GetRangeMax());

        KTINFO("Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO("Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        double value;
        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                value = (*fs)(iBinY-1)[0] * (*fs)(iBinY-1)[0] + (*fs)(iBinY-1)[1] * (*fs)(iBinY-1)[1];
                hist->SetBinContent(iBinX, iBinY, value);
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

 #endif


} /* namespace Katydid */

