/*
 * KTFrequencySpectrumVarianceData.hh
 *
 *  Created on: Feb 23, 2018
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMVARIANCEDATA_HH_
#define KTFREQUENCYSPECTRUMVARIANCEDATA_HH_

#include "KTFrequencyDomainArray.hh"

#include "KTFrequencySpectrumVariance.hh"

namespace Katydid
{
    class KTFrequencySpectrumVarianceDataCore : public KTFrequencyDomainArrayData
    {
        public:
            typedef KTFrequencySpectrumVariance spectrum_type;

        public:
            KTFrequencySpectrumVarianceDataCore();
            virtual ~KTFrequencySpectrumVarianceDataCore();

            virtual unsigned GetNComponents() const;

            const KTFrequencySpectrumVariance* GetSpectrum(unsigned component = 0) const;
            KTFrequencySpectrumVariance* GetSpectrum(unsigned component = 0);

            const KTFrequencyDomainArray* GetArray(unsigned component = 0) const;
            KTFrequencyDomainArray* GetArray(unsigned component = 0);

            void SetSpectrum(KTFrequencySpectrumVariance* spectrum, unsigned component = 0);

            virtual KTFrequencySpectrumVarianceDataCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTFrequencySpectrumVariance* > fSpectra;
    };

    inline const KTFrequencySpectrumVariance* KTFrequencySpectrumVarianceDataCore::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumVariance* KTFrequencySpectrumVarianceDataCore::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencyDomainArray* KTFrequencySpectrumVarianceDataCore::GetArray(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencyDomainArray* KTFrequencySpectrumVarianceDataCore::GetArray(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTFrequencySpectrumVarianceDataCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTFrequencySpectrumVarianceDataCore::SetSpectrum(KTFrequencySpectrumVariance* spectrum, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        else delete fSpectra[component];
        fSpectra[component] = spectrum;
        return;
    }

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMVARIANCEDATA_HH_ */
