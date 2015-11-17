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
            double GetIntercept(unsigned component = 0) const;
            void SetIntercept(double intercept, unsigned component = 0);

            double GetIntercept_deviation(unsigned component = 0) const;
            void SetIntercept_deviation(double dev, unsigned component = 0);

            double GetFineProbe_sigma(unsigned component = 0) const;
            void SetFineProbe_sigma(double sigma, unsigned component = 0);

            double GetFineProbe_SNR(unsigned component = 0) const;
            void SetFineProbe_SNR(double snr, unsigned component = 0);

        private:
            struct PerComponentData
            {
                double Intercept;
                double Intercept_deviation;
                double FineProbe_sigma;
                double FineProbe_SNR;
                double FFT_peak;
                double FFT_peak_uncertainty;
                double FFT_sigma;
                double FFT_SNR;
                double Fit_width;
                uint64_t NPoints;
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

    inline double KTLinearFitResult::GetFineProbe_sigma(unsigned component) const
    {
        return fComponentData[component].fFineProbe_sigma_1;
    }

    inline void KTLinearFitResult::SetFineProbe_sigma(double sigma, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFineProbe_sigma_1 = sigma;
        return;
    }

    inline double KTLinearFitResult::GetFineProbe_SNR(unsigned component) const
    {
        return fComponentData[component].fFineProbe_SNR_1;
    }

    inline void KTLinearFitResult::SetFineProbe_SNR(double snr, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fFineProbe_SNR_1 = snr;
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
    

} /* namespace Katydid */
#endif /* KTLINEARFITRESULT_HH_ */