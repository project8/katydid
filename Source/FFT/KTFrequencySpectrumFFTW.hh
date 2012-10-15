/*
 * KTFrequencySpectrumFFTW.hh
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMFFTW_HH_
#define KTFREQUENCYSPECTRUMFFTW_HH_

#include "KTPhysicalArrayFFTW.hh"

#include "Rtypes.h"

#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTPowerSpectrum;

    class KTFrequencySpectrumFFTW : public KTPhysicalArray< 1, fftw_complex >
    {
        public:
            KTFrequencySpectrumFFTW();
            KTFrequencySpectrumFFTW(size_t totalBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTFrequencySpectrumFFTW(const KTFrequencySpectrumFFTW& orig);
            virtual ~KTFrequencySpectrumFFTW();

            // replace some of the KTPhysicalArray interface

            /// Returns the size of the positive-frequency part of the array
            //size_t size() const;
            /// Returns the isze of the positive-frequency part of the array
            //size_t GetNBins() const;

            /// Returns the actual size of the storage array
            //size_t size_total() const;
            /// Returns the actual size of the storage array
            //size_t GetNBinsTotal() const;

            // normal KTFrequencySpectrum functions

            virtual KTFrequencySpectrumFFTW& operator=(const KTFrequencySpectrumFFTW& rhs);

            virtual KTFrequencySpectrumFFTW& CConjugate();

            virtual KTPowerSpectrum* CreatePowerSpectrum() const;

            void Print(unsigned startPrint, unsigned nToPrint) const;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMFFTW_HH_ */
