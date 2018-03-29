/*
 * KTLinearFitResult.hh
 *
 *  Created on: Nov 16, 2015
 *      Author: ezayas
 */

#ifndef KTLINEARFITRESULT_HH
#define KTLINEARFITRESULT_HH

#include "KTData.hh"

#include <inttypes.h>
#include <utility>
#include <vector>

namespace Katydid
{
    
    class KTLinearFitResult : public Nymph::KTExtensibleData< KTLinearFitResult >
    {
        public:
            KTLinearFitResult();
            KTLinearFitResult(const KTLinearFitResult& orig);
            virtual ~KTLinearFitResult();

            KTLinearFitResult& operator=(const KTLinearFitResult& rhs);

            unsigned GetComponent() const;
            void SetComponent(unsigned iComponent);

            unsigned GetNFits() const;
            void SetNFits(unsigned num);

        public:

            double GetSlope(unsigned fit = 0) const;
            void SetSlope(double slope, unsigned fit = 0);

            double GetIntercept(unsigned fit = 0) const;
            void SetIntercept(double intercept, unsigned fit = 0);

            double GetStartingFrequency(unsigned fit = 0) const;
            void SetStartingFrequency(double freq, unsigned fit = 0);

            double GetTrackDuration(unsigned fit = 0) const;
            void SetTrackDuration(double deltaT, unsigned fit = 0);

            double GetSidebandSeparation(unsigned fit = 0) const;
            void SetSidebandSeparation(double freq, unsigned fit = 0);

            double GetFineProbe_sigma_1(unsigned fit = 0) const;
            void SetFineProbe_sigma_1(double sigma, unsigned fit = 0);

            double GetFineProbe_sigma_2(unsigned fit = 0) const;
            void SetFineProbe_sigma_2(double sigma, unsigned fit = 0);

            double GetFineProbe_SNR_1(unsigned fit = 0) const;
            void SetFineProbe_SNR_1(double snr, unsigned fit = 0);

            double GetFineProbe_SNR_2(unsigned fit = 0) const;
            void SetFineProbe_SNR_2(double snr, unsigned fit = 0);

            double GetFFT_peak(unsigned fit = 0) const;
            void SetFFT_peak(double amp, unsigned fit = 0);

            double GetFFT_SNR(unsigned fit = 0) const;
            void SetFFT_SNR(double snr, unsigned fit = 0);

            double GetFit_width(unsigned fit = 0) const;
            void SetFit_width(double freq, unsigned fit = 0);

            double GetNPoints(unsigned fit = 0) const;
            void SetNPoints(double n, unsigned fit = 0);

            double GetProbeWidth(unsigned fit = 0) const;
            void SetProbeWidth(double s, unsigned fit = 0);

        private:

            unsigned fComponent;

            struct PerFitData
            {
                double fSlope;
                double fIntercept;
                double fStartingFrequency;
                double fTrackDuration;
                double fSidebandSeparation;
                double fFineProbe_sigma_1;
                double fFineProbe_sigma_2;
                double fFineProbe_SNR_1;
                double fFineProbe_SNR_2;
                double fFFT_peak;
                double fFFT_SNR;
                double fFit_width;
                uint64_t fNPoints;
                double fProbeWidth;
            };

            std::vector< PerFitData > fFitData;

        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTLinearFitResult& hdr);

    inline unsigned KTLinearFitResult::GetComponent() const
    {
        return fComponent;
    }

    inline void KTLinearFitResult::SetComponent(unsigned iComponent)
    {
        fComponent = iComponent;
        return;
    }

    inline unsigned KTLinearFitResult::GetNFits() const
    {
        return unsigned(fFitData.size());
    }

    inline void KTLinearFitResult::SetNFits(unsigned num)
    {
        fFitData.resize(num);
        return;
    }

    inline double KTLinearFitResult::GetSlope(unsigned fit) const
    {
        return fFitData[fit].fSlope;
    }

    inline void KTLinearFitResult::SetSlope(double slope, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fSlope = slope;
        return;
    }

    inline double KTLinearFitResult::GetIntercept(unsigned fit) const
    {
        return fFitData[fit].fIntercept;
    }

    inline void KTLinearFitResult::SetIntercept(double intercept, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fIntercept = intercept;
        return;
    }

    inline double KTLinearFitResult::GetStartingFrequency(unsigned fit) const
    {
        return fFitData[fit].fStartingFrequency;
    }

    inline void KTLinearFitResult::SetStartingFrequency(double freq, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fStartingFrequency = freq;
        return;
    }

    inline double KTLinearFitResult::GetTrackDuration(unsigned fit) const
    {
        return fFitData[fit].fTrackDuration;
    }

    inline void KTLinearFitResult::SetTrackDuration(double deltaT, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fTrackDuration = deltaT;
        return;
    }

    inline double KTLinearFitResult::GetSidebandSeparation(unsigned fit) const
    {
        return fFitData[fit].fSidebandSeparation;
    }

    inline void KTLinearFitResult::SetSidebandSeparation(double freq, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fSidebandSeparation = freq;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_sigma_1(unsigned fit) const
    {
        return fFitData[fit].fFineProbe_sigma_1;
    }

    inline void KTLinearFitResult::SetFineProbe_sigma_1(double sigma, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fFineProbe_sigma_1 = sigma;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_sigma_2(unsigned fit) const
    {
        return fFitData[fit].fFineProbe_sigma_2;
    }

    inline void KTLinearFitResult::SetFineProbe_sigma_2(double sigma, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fFineProbe_sigma_2 = sigma;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_SNR_1(unsigned fit) const
    {
        return fFitData[fit].fFineProbe_SNR_1;
    }

    inline void KTLinearFitResult::SetFineProbe_SNR_1(double snr, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fFineProbe_SNR_1 = snr;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_SNR_2(unsigned fit) const
    {
        return fFitData[fit].fFineProbe_SNR_2;
    }

    inline void KTLinearFitResult::SetFineProbe_SNR_2(double snr, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fFineProbe_SNR_2 = snr;
        return;
    }

    inline double KTLinearFitResult::GetFFT_peak(unsigned fit) const
    {
        return fFitData[fit].fFFT_peak;
    }

    inline void KTLinearFitResult::SetFFT_peak(double freq, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fFFT_peak = freq;
        return;
    }

    inline double KTLinearFitResult::GetFFT_SNR(unsigned fit) const
    {
        return fFitData[fit].fFFT_SNR;
    }

    inline void KTLinearFitResult::SetFFT_SNR(double snr, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fFFT_SNR = snr;
        return;
    }

    inline double KTLinearFitResult::GetFit_width(unsigned fit) const
    {
        return fFitData[fit].fFit_width;
    }

    inline void KTLinearFitResult::SetFit_width(double w, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fFit_width = w;
        return;
    }

    inline double KTLinearFitResult::GetNPoints(unsigned fit) const
    {
        return fFitData[fit].fNPoints;
    }

    inline void KTLinearFitResult::SetNPoints(double n, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fNPoints = n;
        return;
    }

    inline double KTLinearFitResult::GetProbeWidth(unsigned fit) const
    {
        return fFitData[fit].fProbeWidth;
    }

    inline void KTLinearFitResult::SetProbeWidth(double s, unsigned fit)
    {
        if (fit >= fFitData.size()) fFitData.resize(fit+1);
        fFitData[fit].fProbeWidth = s;
        return;
    }
    

} /* namespace Katydid */
#endif /* KTLINEARFITRESULT_HH_ */
