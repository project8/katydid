/*
 * KTPowerSpectrumData.hh
 *
 *  Created on: Aug 1, 2014
 *      Author: nsoblath
 */

#ifndef KTPOWERSPECTRUMDATA_HH_
#define KTPOWERSPECTRUMDATA_HH_

#include "Data.hh"

#include "KTFrequencySpectrumVarianceData.hh"
#include "KTPowerSpectrum.hh"

#include <vector>

namespace Katydid
{
    

    class KTPowerSpectrumDataCore : public KTFrequencyDomainArrayData
    {
        public:
	    typedef KTPowerSpectrum spectrum_type;

        public:
            KTPowerSpectrumDataCore();
            virtual ~KTPowerSpectrumDataCore();

            virtual unsigned GetNComponents() const;

            const KTPowerSpectrum* GetSpectrum(unsigned component = 0) const;
            KTPowerSpectrum* GetSpectrum(unsigned component = 0);

            const KTFrequencyDomainArray* GetArray(unsigned component = 0) const;
            KTFrequencyDomainArray* GetArray(unsigned component = 0);

            void SetSpectrum(KTPowerSpectrum* spectrum, unsigned component = 0);

            virtual KTPowerSpectrumDataCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTPowerSpectrum* > fSpectra;
    };


    class KTPowerSpectrumData : public KTPowerSpectrumDataCore, public Nymph::Data
    {
        public:
            KTPowerSpectrumData();
            virtual ~KTPowerSpectrumData();

            KTPowerSpectrumData& SetNComponents(unsigned channels);
    };


    class KTPowerSpectrumVarianceData : public KTFrequencySpectrumVarianceDataCore, public Nymph::Data
    {
        public:
            KTPowerSpectrumVarianceData();
            virtual ~KTPowerSpectrumVarianceData();

            KTPowerSpectrumVarianceData& SetNComponents(unsigned channels);
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


} /* namespace Katydid */

#endif /* KTPOWERSPECTRUMDATA_HH_ */
