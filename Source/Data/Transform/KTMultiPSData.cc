/*
 * KTMultiPSDataCore.cc
 *
 *  Created on: Aug 31, 2017
 *      Author: nsoblath
 */

#include "KTMultiPSData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(datalog, "KTMultiPSData");

    KTMultiPSDataCore::KTMultiPSDataCore() :
            fSpectra()
    {
    }

    KTMultiPSDataCore::~KTMultiPSDataCore()
    {
        while (! fSpectra.empty())
        {
            KTMultiPS* backSpectra = fSpectra.back();
            for (KTMultiPS::iterator iter = backSpectra->begin(); iter != backSpectra->end(); iter++)
            {
                delete *iter;
            }
            delete backSpectra;
            fSpectra.pop_back();
        }
    }

    void KTMultiPSDataCore::SetSpectrum(KTPowerSpectrum* spectrum, unsigned iSpect, unsigned component)
    {
        if (component >= fSpectra.size())
        {
            KTDEBUG(datalog, "Attempting to set spectrum in data which doesn't have component " << component << "; growing the data");
            SetNComponents(component+1);
        }
        if (fSpectra[component] == NULL)
        {
            KTDEBUG(datalog, "Pointer to spectra is NULL; adding new spectra with " << iSpect + 1 << "bins");
            fSpectra[component] = new KTMultiPS(iSpect+1, 0., 1.);
        }
        (*fSpectra[component])(iSpect) = spectrum;
        return;
    }

    void KTMultiPSDataCore::DeleteSpectra(unsigned component)
    {
        if (component >= fSpectra.size() || fSpectra[component] == NULL) return;
        for (KTMultiPS::iterator iter = fSpectra[component]->begin(); iter != fSpectra[component]->end(); ++iter)
        {
            delete *iter;
        }
        delete fSpectra[component];
        fSpectra[component] = NULL;
        return;
    }

#ifdef ROOT_FOUND
    TH2D* KTMultiPSDataCore::CreatePowerHistogram(unsigned component, const std::string& name) const
    {
        if (component >= fSpectra.size()) return NULL;
        if (fSpectra[component]->empty()) return NULL;

        KTPowerSpectrum* firstPS = NULL;
        for (int iBinX=0; iBinX<(int)fSpectra[component]->size() && firstPS==NULL; ++iBinX) firstPS = (*fSpectra[component])(iBinX);
        if (firstPS == NULL) return NULL;

        TH2D* hist = new TH2D(name.c_str(), "Power Spectra",
                fSpectra[component]->size(), fSpectra[component]->GetRangeMin(), fSpectra[component]->GetRangeMax(),
                firstPS->size(), firstPS->GetRangeMin(), firstPS->GetRangeMax());

        KTINFO(datalog, "Frequency axis: " << firstPS->size() << " bins; range: " << hist->GetYaxis()->GetXmin() << " - " << hist->GetYaxis()->GetXmax() << " Hz");
        KTINFO(datalog, "Time axis: " << fSpectra[component]->size() << " bins; range: " << hist->GetXaxis()->GetXmin() << " - " << hist->GetXaxis()->GetXmax() << " s");

        for (int iBinX=0; iBinX<(int)fSpectra[component]->size(); ++iBinX)
        {
            KTPowerSpectrum* ps = (*fSpectra[component])(iBinX);
            if (ps == NULL) continue;
            for (int iBinY=0; iBinY<hist->GetNbinsY(); ++iBinY)
            {
                hist->SetBinContent(iBinX, iBinY, (*ps)(iBinY));
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

 #endif


    const std::string KTMultiPSData::sName("multi-ps");

    KTMultiPSData::KTMultiPSData() :
            KTMultiPSDataCore(),
            KTExtensibleData< KTMultiPSData >()
    {
    }

    KTMultiPSData::~KTMultiPSData()
    {
    }


    KTMultiPSData& KTMultiPSData::SetNComponents(unsigned components)
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

