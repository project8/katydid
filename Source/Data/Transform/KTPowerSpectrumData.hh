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

    class KTPowerSpectrumDataCore : public KTFrequencyDomainArrayData
    {
        public:
            KTPowerSpectrumDataCore();
            virtual ~KTPowerSpectrumDataCore();

            virtual unsigned GetNComponents() const;

            virtual const KTPowerSpectrum* GetSpectrum(unsigned component = 0) const;
            virtual KTPowerSpectrum* GetSpectrum(unsigned component = 0);

            const KTFrequencyDomainArray* GetArray(unsigned component = 0) const;
            KTFrequencyDomainArray* GetArray(unsigned component = 0);

            void SetSpectrum(KTPowerSpectrum* spectrum, unsigned component = 0);

            virtual KTPowerSpectrumDataCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTPowerSpectrum* > fSpectra;
    };

    inline const KTPowerSpectrum* KTPowerSpectrumDataCore::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTPowerSpectrum* KTPowerSpectrumDataCore::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencyDomainArray* KTPowerSpectrumDataCore::GetArray(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencyDomainArray* KTPowerSpectrumDataCore::GetArray(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTPowerSpectrumDataCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTPowerSpectrumDataCore::SetSpectrum(KTPowerSpectrum* spectrum, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        else delete fSpectra[component];
        fSpectra[component] = spectrum;
        return;
    }

    class KTPowerSpectrumData : public KTPowerSpectrumDataCore, public KTExtensibleData< KTPowerSpectrumData >
    {
        public:
            KTPowerSpectrumData();
            virtual ~KTPowerSpectrumData();

            KTPowerSpectrumData& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };



} /* namespace Katydid */

#endif /* KTPOWERSPECTRUMDATA_HH_ */
