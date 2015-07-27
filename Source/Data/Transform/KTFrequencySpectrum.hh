/*
 * KTFrequencySpectrum.hh
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUM_HH_
#define KTFREQUENCYSPECTRUM_HH_

#include "KTFrequencyDomainArray.hh"

namespace Katydid
{
    class KTPowerSpectrum;

    class KTFrequencySpectrum : public KTFrequencyDomainArray
    {
        public:
            KTFrequencySpectrum();
            virtual ~KTFrequencySpectrum();

            /// Get the size of the time-domain array from which this spectrum was created
            virtual unsigned GetNTimeBins() const = 0;
            /// Set the size of the time-domain array from which this spectrum was created
            virtual void SetNTimeBins(unsigned bins) = 0;

            virtual double GetReal(unsigned bin) const = 0;
            virtual double GetImag(unsigned bin) const = 0;

            virtual void SetRect(unsigned bin, double real, double imag) = 0;

            virtual double GetAbs(unsigned bin) const = 0;
            virtual double GetArg(unsigned bin) const = 0;

            virtual void SetPolar(unsigned bin, double abs, double arg) = 0;

            virtual KTFrequencySpectrum& CConjugate() = 0;

            virtual KTFrequencySpectrum& Scale(double scale) = 0;

            virtual KTPowerSpectrum* CreatePowerSpectrum() const= 0;
    };

    class KTFrequencySpectrumData : public KTFrequencyDomainArrayData
    {
        public:
            KTFrequencySpectrumData();
            virtual ~KTFrequencySpectrumData();

            virtual const KTFrequencySpectrum* GetSpectrum(unsigned component = 0) const = 0;
            virtual KTFrequencySpectrum* GetSpectrum(unsigned component = 0) = 0;

    };

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUM_HH_ */
