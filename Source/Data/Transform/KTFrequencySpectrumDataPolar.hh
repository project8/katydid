/*
 * KTFrequencySpectrumDataPolar.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATAPOLAR_HH_
#define KTFREQUENCYSPECTRUMDATAPOLAR_HH_

#include "KTData.hh"

#include "KTFrequencySpectrumPolar.hh"

#include <vector>

namespace Katydid
{
    

    class KTFrequencySpectrumDataPolarCore : public KTFrequencySpectrumData
    {
        public:
            typedef KTFrequencySpectrumPolar spectrum_type;

        public:
            KTFrequencySpectrumDataPolarCore();
            virtual ~KTFrequencySpectrumDataPolarCore();

            unsigned GetNComponents() const;

            const KTFrequencySpectrumPolar* GetSpectrumPolar(unsigned component = 0) const;
            KTFrequencySpectrumPolar* GetSpectrumPolar(unsigned component = 0);

            const KTFrequencySpectrum* GetSpectrum(unsigned component = 0) const;
            KTFrequencySpectrum* GetSpectrum(unsigned component = 0);

            const KTFrequencyDomainArray* GetArray(unsigned component = 0) const;
            KTFrequencyDomainArray* GetArray(unsigned component = 0);

            void SetSpectrum(KTFrequencySpectrumPolar* record, unsigned component = 0);

            virtual KTFrequencySpectrumDataPolarCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTFrequencySpectrumPolar* > fSpectra;

    };

    inline const KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolarCore::GetSpectrumPolar(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolarCore::GetSpectrumPolar(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataPolarCore::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataPolarCore::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencyDomainArray* KTFrequencySpectrumDataPolarCore::GetArray(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencyDomainArray* KTFrequencySpectrumDataPolarCore::GetArray(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTFrequencySpectrumDataPolarCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTFrequencySpectrumDataPolarCore::SetSpectrum(KTFrequencySpectrumPolar* record, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        else delete fSpectra[component];
        fSpectra[component] = record;
        return;
    }


    class KTFrequencySpectrumDataPolar : public KTFrequencySpectrumDataPolarCore, public Nymph::KTExtensibleData< KTFrequencySpectrumDataPolar >
    {
        public:
            KTFrequencySpectrumDataPolar();
            virtual ~KTFrequencySpectrumDataPolar();

            KTFrequencySpectrumDataPolar& SetNComponents(unsigned component);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAPOLAR_HH_ */
