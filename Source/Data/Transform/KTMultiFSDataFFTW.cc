/*
 * KTMultiFSDataFFTWCore.cc
 *
 *  Created on: May 20, 2013
 *      Author: nsoblath
 */

#include "KTMultiFSDataFFTW.hh"

#include "KTLogger.hh"

#include <cmath>

using std::vector;

namespace Katydid
{
    KTLOGGER(datalog, "KTMultiFSDataFFTW");

    KTMultiFSDataFFTWCore::KTMultiFSDataFFTWCore() :
            fSpectra(1)
    {
        fSpectra[0] = new KTMultiFSFFTW(1, 0., 1.);

        (*fSpectra[0])(0) = NULL;
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
            KTDEBUG(datalog, "Attempting to set spectrum in data which doesn't have component " << component << "; growing the data");
            SetNComponents(component+1);
        }
        if (fSpectra[component] == NULL)
        {
            KTDEBUG(datalog, "Pointer to spectra is NULL; adding new spectra with " << iSpect + 1 << "bins");
            fSpectra[component] = new KTMultiFSFFTW(iSpect+1, 0., 1.);
        }
        (*fSpectra[component])(iSpect) = spectrum;
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

        KTINFO(datalog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO(datalog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            if (fs == NULL) continue;
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, sqrt((*fs)(iBinY-1)[0] * (*fs)(iBinY-1)[0] + (*fs)(iBinY-1)[1] * (*fs)(iBinY-1)[1]));
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

        KTINFO(datalog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO(datalog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            if (fs == NULL) continue;
            for (int iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, atan2((*fs)(iBinY-1)[1], (*fs)(iBinY-1)[0]));
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

        KTINFO(datalog, "Frequency axis: " << (*fSpectra[component])(0)->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO(datalog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        double value;
        for (int iBinX=1; iBinX<=(int)fSpectra[component]->size(); iBinX++)
        {
            KTFrequencySpectrumFFTW* fs = (*fSpectra[component])(iBinX-1);
            if (fs == NULL) continue;
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

