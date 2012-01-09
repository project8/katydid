/*
 * KTPowerSpectrum.hh
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#ifndef KTPOWERSPECTRUM_HH_
#define KTPOWERSPECTRUM_HH_

#include "KTComplexVector.hh"

#include <string>

namespace Katydid
{

    class KTPowerSpectrum : public KTComplexVector
    {
        public:
            KTPowerSpectrum();
            KTPowerSpectrum(const KTPowerSpectrum& original);
            virtual ~KTPowerSpectrum();

            /// Input should be the direct FFT, not a power spectrum
            virtual void TakeFrequencySpectrum(const KTComplexVector& freqSpect);
            virtual void TakeFrequencySpectrum(unsigned int nBins, const Double_t* real, const Double_t* imag);

            Double_t GetPowerAtFrequency(Double_t freq);
            Double_t GetPhaseAtFrequency(Double_t freq);

            virtual TH1D* CreateMagnitudeHistogram() const;
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name) const;
            virtual TH1D* CreatePhaseHistogram() const;
            virtual TH1D* CreatePhaseHistogram(const std::string& name) const;

            virtual TH1D* CreatePowerDistributionHistogram() const;
            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name) const;

            Double_t GetBinWidth() const;

            void SetBinWidth(Double_t bw);

        protected:
            Double_t fBinWidth;

            ClassDef(KTPowerSpectrum, 2);
    };

    inline Double_t KTPowerSpectrum::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTPowerSpectrum::SetBinWidth(Double_t bw)
    {
        fBinWidth = bw;
        return;
    }

} /* namespace Katydid */
#endif /* KTPOWERSPECTRUM_HH_ */
