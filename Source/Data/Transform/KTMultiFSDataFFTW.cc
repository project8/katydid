/*
 * KTMultiFSDataFFTWCore.cc
 *
 *  Created on: May 20, 2013
 *      Author: nsoblath
 */

#include "KTMultiFSDataFFTW.hh"

#include "logger.hh"

#include <cmath>

using std::vector;

namespace Katydid
{
    LOGGER(datalog, "KTMultiFSDataFFTW");

    KTMultiFSDataFFTWCore::KTMultiFSDataFFTWCore() :
            fSpectra()
    {
    }

    KTMultiFSDataFFTWCore::~KTMultiFSDataFFTWCore()
    {
        while (! fSpectra.empty())
        {
            KTMultiFSFFTW* backSpectra = fSpectra.back();
            for (KTMultiFSFFTW::iterator iter = backSpectra->begin(); iter != backSpectra->end(); iter++)
            {
                delete *iter;
            }
            delete backSpectra;
            fSpectra.pop_back();
        }
    }

    void KTMultiFSDataFFTWCore::SetSpectrum(KTFrequencySpectrumFFTW* spectrum, unsigned iSpect, unsigned component)
    {
        if (component >= fSpectra.size())
        {
            LDEBUG(datalog, "Attempting to set spectrum in data which doesn't have component " << component << "; growing the data");
            SetNComponents(component+1);
        }
        if (fSpectra[component] == NULL)
        {
            LDEBUG(datalog, "Pointer to spectra is NULL; adding new spectra with " << iSpect + 1 << "bins");
            fSpectra[component] = new KTMultiFSFFTW(iSpect+1, 0., 1.);
        }
        (*fSpectra[component])(iSpect) = spectrum;
        return;
    }

    void KTMultiFSDataFFTWCore::DeleteSpectra(unsigned component)
    {
        if (component >= fSpectra.size() || fSpectra[component] == NULL) return;
        for (KTMultiFSFFTW::iterator iter = fSpectra[component]->begin(); iter != fSpectra[component]->end(); ++iter)
        {
            delete *iter;
        }
        delete fSpectra[component];
        fSpectra[component] = NULL;
        return;
    }

#ifdef ROOT_FOUND
    TH2D* KTMultiFSDataFFTWCore::CreateMagnitudeHistogram(unsigned component, const std::string& name) const
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
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            if (fs == NULL) continue;
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); ++iBinY)
            {
                hist->SetBinContent(iBinX, iBinY, std::abs((*fs)(iBinY-1)));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    TH2D* KTMultiFSDataFFTWCore::CreatePhaseHistogram(unsigned component, const std::string& name) const
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
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            if (fs == NULL) continue;
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); ++iBinY)
            {
                hist->SetBinContent(iBinX, iBinY, std::arg((*fs)(iBinY-1)));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    TH2D* KTMultiFSDataFFTWCore::CreatePowerHistogram(unsigned component, const std::string& name) const
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
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            if (fs == NULL) continue;
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); ++iBinY)
            {
                value = std::norm((*fs)(iBinY-1));
                hist->SetBinContent(iBinX, iBinY, value);
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

 #endif


    const std::string KTMultiFSDataFFTW::sName("multi-fs-fftw");

    KTMultiFSDataFFTW::KTMultiFSDataFFTW() :
            KTMultiFSDataFFTWCore(),
            KTExtensibleData< KTMultiFSDataFFTW >()
    {
    }

    KTMultiFSDataFFTW::~KTMultiFSDataFFTW()
    {
    }


    KTMultiFSDataFFTW& KTMultiFSDataFFTW::SetNComponents(unsigned components)
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

