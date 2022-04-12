/*
 * KTChirpSpace.hh
 *
 *  Created on: Apr 5, 2022
 *      Author: jkgaison
 */

#ifndef KTCHIRPSPACE_HH_
#define KTCHIRPSPACE_HH_

#include "KTChirpDomainArray.hh"

namespace Katydid
{
    class KTPowerSpectrum;

    class KTChirpSpace : public KTChirpDomainArray
    {
        public:
            KTChirpSpace();
            virtual ~KTChirpSpace();

            /// Get the size of the time-domain array from which this spectrum was created
            virtual unsigned GetNTimeBins() const = 0;
            /// Set the size of the time-domain array from which this spectrum was created
            virtual void SetNTimeBins(unsigned bins) = 0;

            virtual double GetReal(unsigned SlopeComponent, unsigned InterceptComponent) const = 0;
            virtual double GetImag(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) const = 0;

            virtual void SetRect(unsigned SlopeComponent, unsigned InterceptComponent, double real, double imag) = 0;

            virtual double GetAbs(unsigned SlopeComponent, unsigned InterceptComponent) const = 0;
            virtual double GetArg(unsigned SlopeComponent, unsigned InterceptComponent) const = 0;
            virtual double GetNorm(unsigned SlopeComponent, unsigned InterceptComponent) const = 0;

            //virtual void SetPolar(unsigned SlopeComponent, unsigned InterceptComponent, double abs, double arg) = 0;

            virtual KTChirpSpace& CConjugate() = 0;

            virtual KTChirpSpace& Scale(double scale) = 0;

            virtual KTPowerSpectrum* CreatePowerSpectrum() const= 0;
    };

    class KTChirpSpaceData : public KTChirpDomainArrayData
    {
        public:
            KTChirpSpaceData();
            virtual ~KTChirpSpaceData();

            virtual const KTChirpSpace* GetSpectrum(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) const = 0;
            virtual KTChirpSpace* GetSpectrum(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) = 0;

    };

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUM_HH_ */
