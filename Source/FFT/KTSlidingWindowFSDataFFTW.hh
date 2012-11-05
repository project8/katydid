/*
 * KTSlidingWindowFSDataFFTW.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTSLIDINGWINDOWFSDATAFFTW_HH_
#define KTSLIDINGWINDOWFSDATAFFTW_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrumFFTW.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{

    class KTSlidingWindowFSDataFFTW : public KTWriteableData
    {
        public:
            KTSlidingWindowFSDataFFTW(unsigned nChannels=1);
            virtual ~KTSlidingWindowFSDataFFTW();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra(unsigned channelNum = 0) const;
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra(unsigned channelNum = 0);
            unsigned GetNChannels() const;

            void SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra, unsigned channelNum = 0);
            void SetNChannels(unsigned channels);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fName;

            std::vector< KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH2D* CreatePhaseHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH2D* CreatePowerHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const;
#endif


    };

    inline const std::string& KTSlidingWindowFSDataFFTW::GetName() const
    {
        return fName;
    }

    inline const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTSlidingWindowFSDataFFTW::GetSpectra(unsigned channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTSlidingWindowFSDataFFTW::GetSpectra(unsigned channelNum)
    {
        return fSpectra[channelNum];
    }

    inline unsigned KTSlidingWindowFSDataFFTW::GetNChannels() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTSlidingWindowFSDataFFTW::SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra, unsigned channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = spectra;
    }

    inline void KTSlidingWindowFSDataFFTW::SetNChannels(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }

#ifdef ROOT_FOUND
    inline TH2D* KTSlidingWindowFSDataFFTW::CreateMagnitudeHistogram(unsigned channelNum, const std::string& name) const
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
            KTFrequencySpectrumFFTW* fs = (*fSpectra[channelNum])(iBinX-1);
            for (Int_t iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fs)(iBinY-1)[0] * (*fs)(iBinY-1)[0] + (*fs)(iBinY-1)[1] * (*fs)(iBinY-1)[1]);
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }
    inline TH2D* KTSlidingWindowFSDataFFTW::CreatePhaseHistogram(unsigned channelNum, const std::string& name) const
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
            KTFrequencySpectrumFFTW* fs = (*fSpectra[channelNum])(iBinX-1);
            for (Int_t iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                hist->SetBinContent(iBinX, iBinY, (*fs)(iBinY-1)[0] * (*fs)(iBinY-1)[0] + (*fs)(iBinY-1)[1] * (*fs)(iBinY-1)[1]);
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

    inline TH2D* KTSlidingWindowFSDataFFTW::CreatePowerHistogram(unsigned channelNum, const std::string& name) const
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
            KTFrequencySpectrumFFTW* fs = (*fSpectra[channelNum])(iBinX-1);
            for (Int_t iBinY=1; iBinY<=hist->GetNbinsY(); iBinY++)
            {
                value = (*fs)(iBinY-1)[0] * (*fs)(iBinY-1)[0] + (*fs)(iBinY-1)[1] * (*fs)(iBinY-1)[1];
                hist->SetBinContent(iBinX, iBinY, value*value);
            }
        }

        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        return hist;
    }

 #endif


} /* namespace Katydid */

#endif /* KTSLIDINGWINDOWFSDATAFFTW_HH_ */
