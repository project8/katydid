/*
 * KTChirpSpaceVarianceData.hh
 *
 *  Created on: Apr 5, 2022
 *      Author: jkgaison
 */

#ifndef KTCHIRPSPACEVARIANCEDATA_HH_
#define KTCHIRPSPACEVARIANCEDATA_HH_

#include "KTChirpDomainArray.hh"

#include "KTChirpSpaceVariance.hh"

namespace Katydid
{
    class KTChirpSpaceVarianceDataCore : public KTChirpDomainArrayData
    {
        public:
            typedef KTChirpSpaceVariance spectrum_type;

        public:
            KTChirpSpaceVarianceDataCore();
            virtual ~KTChirpSpaceVarianceDataCore();

            virtual unsigned GetNComponents() const;

            const KTChirpSpaceVariance* GetSpectrum(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) const;
            KTChirpSpaceVariance* GetSpectrum(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0);

            const KTChirpDomainArray* GetArray(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) const;
            KTChirpDomainArray* GetArray(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0);

            void SetSpectrum(KTChirpSpaceVariance* spectrum, unsigned SlopeComponent = 0, unsigned InterceptComponent = 0);

            virtual KTChirpSpaceVarianceDataCore& SetNComponents(unsigned SlopeComponent, unsigned InterceptComponent) = 0;

        protected:
            std::vector< KTChirpSpaceVariance* > fSpectra;
    };

    inline const KTChirpSpaceVariance* KTChirpSpaceVarianceDataCore::GetSpectrum(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return fSpectra[SlopeComponent,InterceptComponent];
    }

    inline KTChirpSpaceVariance* KTChirpSpaceVarianceDataCore::GetSpectrum(unsigned SlopeComponent, unsigned InterceptComponent)
    {
        return fSpectra[SlopeComponent,InterceptComponent];
    }

    inline const KTChirpDomainArray* KTChirpSpaceVarianceDataCore::GetArray(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return fSpectra[SlopeComponent,InterceptComponent];
    }

    inline KTChirpDomainArray* KTChirpSpaceVarianceDataCore::GetArray(unsigned SlopeComponent, unsigned InterceptComponent)
    {
        return fSpectra[SlopeComponent,InterceptComponent];
    }

    inline unsigned KTChirpSpaceVarianceDataCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTChirpSpaceVarianceDataCore::SetSpectrum(KTChirpSpaceVariance* spectrum, unsigned SlopeComponent, unsigned InterceptComponent)
    {
        if (SlopeComponent >= fSpectra.size()) SetNComponents(SlopeComponent+1,InterceptComponent+1);
        else delete fSpectra[SlopeComponent,InterceptComponent];
        fSpectra[SlopeComponent,InterceptComponent] = spectrum;
        return;
    }

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMVARIANCEDATA_HH_ */
