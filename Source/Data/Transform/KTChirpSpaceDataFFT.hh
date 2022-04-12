/**
 @file KTChirpSpaceDataFFT.hh
 @brief Contains KTChirpSpaceDataFFT
 @details 
 @author: N. S. Oblath:
 @date: Aug 24, 2012
 */

#ifndef KTCHIRPSPACEDATAFFT_HH_
#define KTCHIRPSPACEDATAFFT_HH_

#include "KTData.hh"

#include "KTChirpSpaceFFT.hh"
#include "KTChirpSpaceVarianceData.hh"

#include <vector>

namespace Katydid
{
    
    class KTChirpSpaceDataFFTCore : public KTChirpSpaceData
    {
        public:
            typedef KTChirpSpaceFFT spectrum_type;

        public:
            KTChirpSpaceDataFFTCore();
            virtual ~KTChirpSpaceDataFFTCore();

            unsigned GetNComponents() const;

            const KTChirpSpaceFFT* GetSpectrumFFTW(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) const;
            KTChirpSpaceFFT* GetSpectrumFFTW(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0);

            const KTChirpSpace* GetSpectrum(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) const;
            KTChirpSpace* GetSpectrum(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0);

            const KTChirpDomainArray* GetArray(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) const;
            KTChirpDomainArray* GetArray(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0);

            void SetSpectrum(KTChirpSpaceFFT* record, unsigned SlopeComponent = 0, unsigned InterceptComponent = 0);

            virtual KTChirpSpaceDataFFTCore& SetNComponents(unsigned SlopeComponent, unsigned InterceptComponent) = 0;

        protected:
            std::vector< KTChirpSpaceFFT* > fSpectra;

    };

//    class KTChirpSpaceDataFFT : public Nymph::KTExtensibleData< KTChirpSpaceDataFFT >
    class KTChirpSpaceDataFFT : public KTChirpSpaceDataFFTCore, public Nymph::KTExtensibleData< KTChirpSpaceDataFFT >
    {
        public:
            KTChirpSpaceDataFFT();
            virtual ~KTChirpSpaceDataFFT();

            virtual KTChirpSpaceDataFFT& SetNComponents(unsigned SlopeComponent, unsigned InterceptComponent);

        public:
            static const std::string sName;

    };


    class KTChirpSpaceVarianceDataFFT : public KTChirpSpaceVarianceDataCore, public Nymph::KTExtensibleData< KTChirpSpaceVarianceDataFFT >
    {
        public:
            KTChirpSpaceVarianceDataFFT();
            virtual ~KTChirpSpaceVarianceDataFFT();

            KTChirpSpaceVarianceDataFFT& SetNComponents(unsigned SlopeComponent, unsigned InterceptComponent);

        public:
            static const std::string sName;

    };


    inline const KTChirpSpaceFFT* KTChirpSpaceDataFFTCore::GetSpectrumFFTW(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return fSpectra[SlopeComponent, InterceptComponent];
    }

    inline KTChirpSpaceFFT* KTChirpSpaceDataFFTCore::GetSpectrumFFTW(unsigned SlopeComponent, unsigned InterceptComponent)
    {
        return fSpectra[SlopeComponent, InterceptComponent];
    }

    inline const KTChirpSpace* KTChirpSpaceDataFFTCore::GetSpectrum(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return fSpectra[SlopeComponent, InterceptComponent];
    }

    inline KTChirpSpace* KTChirpSpaceDataFFTCore::GetSpectrum(unsigned SlopeComponent, unsigned InterceptComponent)
    {
        return fSpectra[SlopeComponent, InterceptComponent];
    }

    inline const KTChirpDomainArray* KTChirpSpaceDataFFTCore::GetArray(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return fSpectra[SlopeComponent, InterceptComponent];
    }

    inline KTChirpDomainArray* KTChirpSpaceDataFFTCore::GetArray(unsigned SlopeComponent, unsigned InterceptComponent)
    {
        return fSpectra[SlopeComponent, InterceptComponent];
    }

    inline unsigned KTChirpSpaceDataFFTCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTChirpSpaceDataFFTCore::SetSpectrum(KTChirpSpaceFFT* record, unsigned SlopeComponent, unsigned InterceptComponent)
    {
        if (SlopeComponent >= fSpectra.size()) SetNComponents(SlopeComponent+1, InterceptComponent+1);
        else delete fSpectra[SlopeComponent, InterceptComponent];
        fSpectra[SlopeComponent, InterceptComponent] = record;
        return;
    }


} /* namespace Katydid */

#endif /* KTCHIRPSPACEDATAFFT_HH_ */
