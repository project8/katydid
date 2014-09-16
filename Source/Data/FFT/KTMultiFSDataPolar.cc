/*
 * KTMultiFSDataPolarCore.cc
 *
 *  Created on: May 20, 2013
 *      Author: nsoblath
 */

#include "KTMultiFSDataPolar.hh"

#include "KTLogger.hh"

using std::vector;

namespace Katydid
{
    KTLOGGER(mfsdlog, "KTMultiFSDataPolar");

    KTMultiFSDataPolarCore::KTMultiFSDataPolarCore() :
            fSpectra(1)
    {
        fSpectra[0] = NULL;

    }

    KTMultiFSDataPolarCore::~KTMultiFSDataPolarCore()
    {
        while (! fSpectra.empty())
        {
            KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* backSpectra = fSpectra.back();
            for (KTPhysicalArray< 1, KTFrequencySpectrumPolar* >::iterator iter = backSpectra->begin(); iter != backSpectra->end(); iter++)
            {
                delete *iter;
            }
            delete backSpectra;
            fSpectra.pop_back();
        }
    }

#ifdef ROOT_FOUND
    TH2D* KTMultiFSDataPolarCore::CreateMagnitudeHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fSpectra.size()) return NULL;
        if (fSpectra[component]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Frequency Spectrum Magnitude",
                fSpectra[component]->size(), fSpectra[component]->GetRangeMin(), fSpectra[component]->GetRangeMax(),
                (*fSpectra[component])(0)->size(), (*fSpectra[component])(0)->GetRangeMin(), (*fSpectra[component])(0)->GetRangeMax());

        KTINFO(mfsdlog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO(mfsdlog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumPolar* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fs)(iBinY-1).abs());
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    TH2D* KTMultiFSDataPolarCore::CreatePhaseHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fSpectra.size()) return NULL;
        if (fSpectra[component]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Frequency Spectrum Phase",
                fSpectra[component]->size(), fSpectra[component]->GetRangeMin(), fSpectra[component]->GetRangeMax(),
                (*fSpectra[component])(0)->size(), (*fSpectra[component])(0)->GetRangeMin(), (*fSpectra[component])(0)->GetRangeMax());

        KTINFO(mfsdlog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO(mfsdlog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumPolar* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fs)(iBinY-1).arg());
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    TH2D* KTMultiFSDataPolarCore::CreatePowerHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fSpectra.size()) return NULL;
        if (fSpectra[component]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Power Spectra",
                fSpectra[component]->size(), fSpectra[component]->GetRangeMin(), fSpectra[component]->GetRangeMax(),
                (*fSpectra[component])(0)->size(), (*fSpectra[component])(0)->GetRangeMin(), (*fSpectra[component])(0)->GetRangeMax());

        KTINFO(mfsdlog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO(mfsdlog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        double value;
        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumPolar* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
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


    const std::string KTMultiFSDataPolar::sName("multi-fs-polar");

    KTMultiFSDataPolar::KTMultiFSDataPolar() :
            KTMultiFSDataPolarCore(),
            KTExtensibleData< KTMultiFSDataPolar >()
    {
    }

    KTMultiFSDataPolar::~KTMultiFSDataPolar()
    {
    }


    KTMultiFSDataPolar& KTMultiFSDataPolar::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            DeleteSpectra(iComponent);
        }
        fSpectra.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }



} /* namespace Katydid */

