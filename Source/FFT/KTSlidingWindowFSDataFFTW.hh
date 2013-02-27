/**
 @file KTSlidingWindowFSDataFFTW.hh
 @brief Contains KTSlidingWindowFSDataFFTW
 @details 
 @author: N. S. Oblath
 @date: Aug 24, 2012
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

            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra(unsigned component = 0) const;
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra(unsigned component = 0);
            unsigned GetNComponents() const;

            void SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra, unsigned component = 0);
            void SetNComponents(unsigned channels);

            void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH2D* CreatePhaseHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH2D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPower") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTSlidingWindowFSDataFFTW::GetSpectra(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTSlidingWindowFSDataFFTW::GetSpectra(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTSlidingWindowFSDataFFTW::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTSlidingWindowFSDataFFTW::SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra, unsigned component)
    {
        if (component >= fSpectra.size()) fSpectra.resize(component+1);
        fSpectra[component] = spectra;
    }

    inline void KTSlidingWindowFSDataFFTW::SetNComponents(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }


} /* namespace Katydid */

#endif /* KTSLIDINGWINDOWFSDATAFFTW_HH_ */
