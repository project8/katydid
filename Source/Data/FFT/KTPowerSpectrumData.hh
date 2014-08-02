/*
 * KTPowerSpectrumData.hh
 *
 *  Created on: Aug 1, 2014
 *      Author: nsoblath
 */

#ifndef KTPOWERSPECTRUMDATA_HH_
#define KTPOWERSPECTRUMDATA_HH_

#include "KTData.hh"

#include "KTPowerSpectrum.hh"

#include <vector>

namespace Katydid
{

    class KTPowerSpectrumData : public KTExtensibleData< KTPowerSpectrumData >
    {
        public:
            KTPowerSpectrumData();
            virtual ~KTPowerSpectrumData();

            virtual unsigned GetNComponents() const;

            virtual const KTPowerSpectrum* GetSpectrum(unsigned component = 0) const;
            virtual KTPowerSpectrum* GetSpectrum(unsigned component = 0);

            void SetSpectrum(KTPowerSpectrum* spectrum, unsigned component = 0);

            KTPowerSpectrumData& SetNComponents(unsigned channels);

        private:
            std::vector< KTPowerSpectrum* > fSpectra;
    };

    inline const KTPowerSpectrum* KTPowerSpectrumData::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTPowerSpectrum* KTPowerSpectrumData::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTPowerSpectrumData::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTPowerSpectrumData::SetSpectrum(KTPowerSpectrum* spectrum, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        else delete fSpectra[component];
        fSpectra[component] = spectrum;
        return;
    }


} /* namespace Katydid */

#endif /* KTPOWERSPECTRUMDATA_HH_ */
