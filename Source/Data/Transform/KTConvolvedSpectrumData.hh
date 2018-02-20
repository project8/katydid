/*
 * KTConvolvedSpectrumData.hh
 *
 *  Created on: Aug 25, 2017
 *      Author: ezayas
 */

#ifndef KTCONVOLVEDSPECTRUMDATA_HH_
#define KTCONVOLVEDSPECTRUMDATA_HH_

#include "KTData.hh"

#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTMultiFSDataPolar.hh"
#include "KTMultiPSData.hh"

#include <vector>

namespace Katydid
{

    class KTConvolvedPowerSpectrumData : public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTConvolvedPowerSpectrumData >
    {
        public:
            KTConvolvedPowerSpectrumData();
            virtual ~KTConvolvedPowerSpectrumData();

            KTConvolvedPowerSpectrumData& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTConvolvedFrequencySpectrumDataFFTW : public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTConvolvedFrequencySpectrumDataFFTW >
    {
        public:
            KTConvolvedFrequencySpectrumDataFFTW();
            virtual ~KTConvolvedFrequencySpectrumDataFFTW();

            KTConvolvedFrequencySpectrumDataFFTW& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTConvolvedFrequencySpectrumDataPolar : public KTFrequencySpectrumDataPolarCore, public Nymph::KTExtensibleData< KTConvolvedFrequencySpectrumDataPolar >
    {
        public:
            KTConvolvedFrequencySpectrumDataPolar();
            virtual ~KTConvolvedFrequencySpectrumDataPolar();

            KTConvolvedFrequencySpectrumDataPolar& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTConvolvedMultiPSData : public KTMultiPSDataCore, public Nymph::KTExtensibleData< KTConvolvedMultiPSData >
    {
        public:
            KTConvolvedMultiPSData();
            virtual ~KTConvolvedMultiPSData();

            KTConvolvedMultiPSData& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTConvolvedMultiFSDataFFTW : public KTMultiFSDataFFTWCore, public Nymph::KTExtensibleData< KTConvolvedMultiFSDataFFTW >
    {
        public:
            KTConvolvedMultiFSDataFFTW();
            virtual ~KTConvolvedMultiFSDataFFTW();

            KTConvolvedMultiFSDataFFTW& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTConvolvedMultiFSDataPolar : public KTMultiFSDataPolarCore, public Nymph::KTExtensibleData< KTConvolvedMultiFSDataPolar >
    {
        public:
            KTConvolvedMultiFSDataPolar();
            virtual ~KTConvolvedMultiFSDataPolar();

            KTConvolvedMultiFSDataPolar& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };


} /* namespace Katydid */

#endif /* KTCONVOLVEDSPECTRUMDATA_HH_ */
