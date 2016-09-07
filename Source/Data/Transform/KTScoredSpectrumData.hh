/*
 * KTScoredSpectrumData.hh
 *
 *  Created on: Mar 10, 2016
 *      Author: Christine
 */

#ifndef SOURCE_DATA_TRANSFORM_KTSCOREDSPECTRUMDATA_HH_
#define SOURCE_DATA_TRANSFORM_KTSCOREDSPECTRUMDATA_HH_


#include "KTData.hh"

#include "KTScoredSpectrum.hh"

#include <vector>

namespace Katydid
{
    using namespace Nymph;

    class KTScoredSpectrumDataCore : public KTFrequencyDomainArrayData
    {
        public:
    	KTScoredSpectrumDataCore();
            virtual ~KTScoredSpectrumDataCore();

            virtual unsigned GetNComponents() const;

            virtual const KTScoredSpectrum* GetSpectrum(unsigned component = 0) const;
            virtual KTScoredSpectrum* GetSpectrum(unsigned component = 0);

            const KTFrequencyDomainArray* GetArray(unsigned component = 0) const;
            KTFrequencyDomainArray* GetArray(unsigned component = 0);

            void SetSpectrum(KTScoredSpectrum* spectrum, unsigned component = 0);

            virtual KTScoredSpectrumDataCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTScoredSpectrum* > fSpectra;
    };

    inline const KTScoredSpectrum* KTScoredSpectrumDataCore::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTScoredSpectrum* KTScoredSpectrumDataCore::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencyDomainArray* KTScoredSpectrumDataCore::GetArray(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencyDomainArray* KTScoredSpectrumDataCore::GetArray(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTScoredSpectrumDataCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTScoredSpectrumDataCore::SetSpectrum(KTScoredSpectrum* spectrum, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        else delete fSpectra[component];
        fSpectra[component] = spectrum;
        return;
    }

    class KTScoredSpectrumData : public KTScoredSpectrumDataCore, public KTExtensibleData< KTScoredSpectrumData >
    {
        public:
            KTScoredSpectrumData();
            virtual ~KTScoredSpectrumData();

            KTScoredSpectrumData& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* SOURCE_DATA_TRANSFORM_KTSCOREDSPECTRUMDATA_HH_ */
