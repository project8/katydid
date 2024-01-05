/**
 @file KTFrequencySpectrumDataFFTW.hh
 @brief Contains KTFrequencySpectrumDataFFTW
 @details .
 @author: N. S. Oblath
 @date: Aug 24, 2012
 */

#ifndef KTFREQUENCYSPECTRUMDATAFFTW_HH_
#define KTFREQUENCYSPECTRUMDATAFFTW_HH_

#include "Data.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumVarianceData.hh"

#include <vector>

namespace Katydid
{
    

    class KTFrequencySpectrumDataFFTWCore : public KTFrequencySpectrumData
    {
        public:
            typedef KTFrequencySpectrumFFTW spectrum_type;

        public:
            KTFrequencySpectrumDataFFTWCore();
            virtual ~KTFrequencySpectrumDataFFTWCore();

            unsigned GetNComponents() const;

            const KTFrequencySpectrumFFTW* GetSpectrumFFTW(unsigned component = 0) const;
            KTFrequencySpectrumFFTW* GetSpectrumFFTW(unsigned component = 0);

            const KTFrequencySpectrum* GetSpectrum(unsigned component = 0) const;
            KTFrequencySpectrum* GetSpectrum(unsigned component = 0);

            const KTFrequencyDomainArray* GetArray(unsigned component = 0) const;
            KTFrequencyDomainArray* GetArray(unsigned component = 0);

            void SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned component = 0);

            virtual KTFrequencySpectrumDataFFTWCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTFrequencySpectrumFFTW* > fSpectra;

    };


    class KTFrequencySpectrumDataFFTW : public KTFrequencySpectrumDataFFTWCore, public Nymph::Data
    {
        public:
            KTFrequencySpectrumDataFFTW();
            virtual ~KTFrequencySpectrumDataFFTW();

            virtual KTFrequencySpectrumDataFFTW& SetNComponents(unsigned components);
    };


    class KTFrequencySpectrumVarianceDataFFTW : public KTFrequencySpectrumVarianceDataCore, public Nymph::Data
    {
        public:
            KTFrequencySpectrumVarianceDataFFTW();
            virtual ~KTFrequencySpectrumVarianceDataFFTW();

            KTFrequencySpectrumVarianceDataFFTW& SetNComponents(unsigned channels);
    };


    inline const KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTWCore::GetSpectrumFFTW(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTWCore::GetSpectrumFFTW(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataFFTWCore::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataFFTWCore::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencyDomainArray* KTFrequencySpectrumDataFFTWCore::GetArray(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencyDomainArray* KTFrequencySpectrumDataFFTWCore::GetArray(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTFrequencySpectrumDataFFTWCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTFrequencySpectrumDataFFTWCore::SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        else delete fSpectra[component];
        fSpectra[component] = record;
        return;
    }


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAFFTW_HH_ */
