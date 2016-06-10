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
    using namespace Nymph;
    class KTLinearFitResult : public KTExtensibleData< KTLinearFitResult >
    {
        public:
            KTLinearFitResult();
            KTLinearFitResult(const KTLinearFitResult& orig);
            virtual ~KTLinearFitResult();

            KTLinearFitResult& operator=(const KTLinearFitResult& rhs);

            unsigned GetNComponents() const;
            KTLinearFitResult& SetNComponents(unsigned num);

        public:

            double GetSlope(unsigned component = 0) const;
            void SetSlope(double slope, unsigned component = 0);

            double GetSlopeSigma(unsigned component = 0) const;
            void SetSlopeSigma(double sigma, unsigned component = 0);

            double GetIntercept(unsigned component = 0) const;
            void SetIntercept(double intercept, unsigned component = 0);

            double GetIntercept_deviation(unsigned component = 0) const;
            void SetIntercept_deviation(double dev, unsigned component = 0);

            double GetStartingFrequency(unsigned component = 0) const;
            void SetStartingFrequency(double freq, unsigned component = 0);

            double GetTrackDuration(unsigned component = 0) const;
            void SetTrackDuration(double deltaT, unsigned component = 0);

            double GetSidebandSeparation(unsigned component = 0) const;
            void SetSidebandSeparation(double freq, unsigned component = 0);

            double GetFineProbe_sigma_1(unsigned component = 0) const;
            void SetFineProbe_sigma_1(double sigma, unsigned component = 0);

            double GetFineProbe_sigma_2(unsigned component = 0) const;
            void SetFineProbe_sigma_2(double sigma, unsigned component = 0);

            double GetFineProbe_SNR_1(unsigned component = 0) const;
            void SetFineProbe_SNR_1(double snr, unsigned component = 0);

            double GetFineProbe_SNR_2(unsigned component = 0) const;
            void SetFineProbe_SNR_2(double snr, unsigned component = 0);

            double GetFFT_peak(unsigned component = 0) const;
            void SetFFT_peak(double amp, unsigned component = 0);

            double GetFFT_peak_uncertainty(unsigned component = 0) const;
            void SetFFT_peak_uncertainty(double sigma, unsigned component = 0);

            double GetFFT_sigma(unsigned component = 0) const;
            void SetFFT_sigma(double sigma, unsigned component = 0);

            double GetFFT_SNR(unsigned component = 0) const;
            void SetFFT_SNR(double snr, unsigned component = 0);

            double GetFit_width(unsigned component = 0) const;
            void SetFit_width(double freq, unsigned component = 0);

            double GetNPoints(unsigned component = 0) const;
            void SetNPoints(double n, unsigned component = 0);

            double GetProbeWidth(unsigned component = 0) const;
            void SetProbeWidth(double s, unsigned component = 0);

        private:
            struct PerComponentData
            {
                double fSlope;
                double fSlopeSigma;
                double fIntercept;
                double fIntercept_deviation;
                double fStartingFrequency;
                double fTrackDuration;
                double fSidebandSeparation;
                double fFineProbe_sigma_1;
                double fFineProbe_sigma_2;
                double fFineProbe_SNR_1;
                double fFineProbe_SNR_2;
                double fFFT_peak;
                double fFFT_peak_uncertainty;
                double fFFT_sigma;
                double fFFT_SNR;
                double fFit_width;
                uint64_t fNPoints;
                double fProbeWidth;
            };

            std::vector< PerComponentData > fComponentData;

        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTLinearFitResult& hdr);

    inline unsigned KTLinearFitResult::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline KTLinearFitResult& KTLinearFitResult::SetNComponents(unsigned num)
    {
        fComponentData.resize(num);
        return *this;
    }

    inline double KTLinearFitResult::GetSlope(unsigned component) const
    {
        return fComponentData[component].fSlope;
    }

    inline void KTLinearFitResult::SetSlope(double slope, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fSlope = slope;
        return;
    }

    inline double KTLinearFitResult::GetSlopeSigma(unsigned component) const
    {
        return fComponentData[component].fSlopeSigma;
    }

    inline void KTLinearFitResult::SetSlopeSigma(double sigma, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fSlopeSigma = sigma;
        return;
    }

    inline double KTLinearFitResult::GetIntercept(unsigned component) const
    {
        return fComponentData[component].fIntercept;
    }

    inline void KTLinearFitResult::SetIntercept(double intercept, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fIntercept = intercept;
        return;
    }

    inline double KTLinearFitResult::GetIntercept_deviation(unsigned component) const
    {
        return fComponentData[component].fIntercept_deviation;
    }

    inline void KTLinearFitResult::SetIntercept_deviation(double dev, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fIntercept_deviation = dev;
        return;
    }

    inline double KTLinearFitResult::GetStartingFrequency(unsigned component) const
    {
        return fComponentData[component].fStartingFrequency;
    }

    inline void KTLinearFitResult::SetStartingFrequency(double freq, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fStartingFrequency = freq;
        return;
    }

    inline double KTLinearFitResult::GetTrackDuration(unsigned component) const
    {
        return fComponentData[component].fTrackDuration;
    }

    inline void KTLinearFitResult::SetTrackDuration(double deltaT, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fTrackDuration = deltaT;
        return;
    }

    inline double KTLinearFitResult::GetSidebandSeparation(unsigned component) const
    {
        return fComponentData[component].fSidebandSeparation;
    }

    inline void KTLinearFitResult::SetSidebandSeparation(double freq, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fSidebandSeparation = freq;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_sigma_1(unsigned component) const
    {
        return fComponentData[component].fFineProbe_sigma_1;
    }

    inline void KTLinearFitResult::SetFineProbe_sigma_1(double sigma, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFineProbe_sigma_1 = sigma;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_sigma_2(unsigned component) const
    {
        return fComponentData[component].fFineProbe_sigma_2;
    }

    inline void KTLinearFitResult::SetFineProbe_sigma_2(double sigma, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFineProbe_sigma_2 = sigma;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_SNR_1(unsigned component) const
    {
        return fComponentData[component].fFineProbe_SNR_1;
    }

    inline void KTLinearFitResult::SetFineProbe_SNR_1(double snr, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFineProbe_SNR_1 = snr;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_SNR_2(unsigned component) const
    {
        return fComponentData[component].fFineProbe_SNR_2;
    }

    inline void KTLinearFitResult::SetFineProbe_SNR_2(double snr, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFineProbe_SNR_2 = snr;
        return;
    }

    inline double KTLinearFitResult::GetFFT_peak(unsigned component) const
    {
        return fComponentData[component].fFFT_peak;
    }

    inline void KTLinearFitResult::SetFFT_peak(double freq, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFFT_peak = freq;
        return;
    }

    inline double KTLinearFitResult::GetFFT_peak_uncertainty(unsigned component) const
    {
        return fComponentData[component].fFFT_peak_uncertainty;
    }

    inline void KTLinearFitResult::SetFFT_peak_uncertainty(double sigma, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFFT_peak_uncertainty = sigma;
        return;
    }

    inline double KTLinearFitResult::GetFFT_sigma(unsigned component) const
    {
        return fComponentData[component].fFFT_sigma;
    }

    inline void KTLinearFitResult::SetFFT_sigma(double sigma, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFFT_sigma = sigma;
        return;
    }

    inline double KTLinearFitResult::GetFFT_SNR(unsigned component) const
    {
        return fComponentData[component].fFFT_SNR;
    }

    inline void KTLinearFitResult::SetFFT_SNR(double snr, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFFT_SNR = snr;
        return;
    }

    inline double KTLinearFitResult::GetFit_width(unsigned component) const
    {
        return fComponentData[component].fFit_width;
    }

    inline void KTLinearFitResult::SetFit_width(double w, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFit_width = w;
        return;
    }

    inline double KTLinearFitResult::GetNPoints(unsigned component) const
    {
        return fComponentData[component].fNPoints;
    }

    inline void KTLinearFitResult::SetNPoints(double n, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fNPoints = n;
        return;
    }

    inline double KTLinearFitResult::GetProbeWidth(unsigned component) const
    {
        return fComponentData[component].fProbeWidth;
    }

    inline void KTLinearFitResult::SetProbeWidth(double s, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fProbeWidth = s;
        return;
    }
    

} /* namespace Katydid */
#endif /* KTLINEARFITRESULT_HH_ */