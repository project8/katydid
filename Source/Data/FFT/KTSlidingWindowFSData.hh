/*
 * KTSlidingWindowFSData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTSLIDINGWINDOWFSDATA_HH_
#define KTSLIDINGWINDOWFSDATA_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrumPolar.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{

    class KTSlidingWindowFSData : public KTWriteableData
    {
        public:
            KTSlidingWindowFSData(unsigned nChannels=1);
            virtual ~KTSlidingWindowFSData();

            const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra(unsigned component = 0) const;
            KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra(unsigned component = 0);
            unsigned GetNComponents() const;

            void SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra, unsigned component = 0);
            void SetNComponents(unsigned channels);

            void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH2D* CreatePhaseHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH2D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPower") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTSlidingWindowFSData::GetSpectra(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTSlidingWindowFSData::GetSpectra(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTSlidingWindowFSData::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTSlidingWindowFSData::SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra, unsigned component)
    {
        if (component >= fSpectra.size()) fSpectra.resize(component+1);
        fSpectra[component] = spectra;
    }

    inline void KTSlidingWindowFSData::SetNComponents(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }

} /* namespace Katydid */

#endif /* KTSLIDINGWINDOWFSDATA_HH_ */
