/*
 * KTMultiFSDataPolarCore.cc
 *
 *  Created on: May 20, 2013
 *      Author: nsoblath
 */

#include "KTMultiFSDataPolar.hh"

#include "logger.hh"

using std::vector;

namespace Katydid
{
    LOGGER(datalog, "KTMultiFSDataPolar");

    KTMultiFSDataPolarCore::KTMultiFSDataPolarCore() :
            fSpectra()
    {
    }

    KTMultiFSDataPolarCore::~KTMultiFSDataPolarCore()
    {
        while (! fSpectra.empty())
        {
            KTMultiFSPolar* backSpectra = fSpectra.back();
            for (KTMultiFSPolar::iterator iter = backSpectra->begin(); iter != backSpectra->end(); iter++)
            {
                delete *iter;
            }
            delete backSpectra;
            fSpectra.pop_back();
        }
    }

    void KTMultiFSDataPolarCore::SetSpectrum(KTFrequencySpectrumPolar* spectrum, unsigned iSpect, unsigned component)
    {
        if (component >= fSpectra.size())
        {
            LDEBUG(datalog, "Attempting to set spectrum in data which doesn't have component " << component << "; growing the data");
            SetNComponents(component+1);
        }
        if (fSpectra[component] == NULL)
        {
            LDEBUG(datalog, "Pointer to spectra is NULL; adding new spectra with " << iSpect + 1 << "bins");
            fSpectra[component] = new KTMultiFSPolar(iSpect+1, 0., 1.);
        }
        (*fSpectra[component])(iSpect) = spectrum;
        return;
    }

    void KTMultiFSDataPolarCore::DeleteSpectra(unsigned component)
    {
        if (component >= fSpectra.size() || fSpectra[component] == NULL) return;
        for (KTMultiFSPolar::iterator iter = fSpectra[component]->begin(); iter != fSpectra[component]->end(); ++iter)
        {
            delete *iter;
        }
        delete fSpectra[component];
        fSpectra[component] = NULL;
        return;
    }

#ifdef ROOT_FOUND
    TH2D* KTMultiFSDataPolarCore::CreateMagnitudeHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fSpectra.size()) return NULL;
        if (fSpectra[component]->empty()) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Frequency Spectrum Magnitude",
                fSpectra[component]->size(), fSpectra[component]->GetRangeMin(), fSpectra[component]->GetRangeMax(),
                (*fSpectra[component])(0)->size(), (*fSpectra[component])(0)->GetRangeMin(), (*fSpectra[component])(0)->GetRangeMax());

        LINFO(datalog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        LINFO(datalog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); ++iBinX)
        {
            KTFrequencySpectrumPolar* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); ++iBinY)
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

        LINFO(datalog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        LINFO(datalog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); ++iBinX)
        {
            KTFrequencySpectrumPolar* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); ++iBinY)
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

        LINFO(datalog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        LINFO(datalog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        double value;
        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); ++iBinX)
        {
            KTFrequencySpectrumPolar* fs = (*fSpectra[component])(iBinX-1);
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); ++iBinY)
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

