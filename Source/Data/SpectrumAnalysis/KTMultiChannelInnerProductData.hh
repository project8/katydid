/*
 * KTMultiChannelInnerProductData.hh
 *
 *  Created on: Apr 28, 2021
 *      Author: F. Thomas
 */

#ifndef KTMULTICHANNELINNERPRODUCTDATA_HH_
#define KTMULTICHANNELINNERPRODUCTDATA_HH_

#include "KTData.hh"

#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"

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

    class KTConvolvedPowerSpectrumVarianceData : public KTFrequencySpectrumVarianceDataCore, public Nymph::KTExtensibleData< KTConvolvedPowerSpectrumVarianceData >
    {
        public:
            KTConvolvedPowerSpectrumVarianceData();
            virtual ~KTConvolvedPowerSpectrumVarianceData();

            KTConvolvedPowerSpectrumVarianceData& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTConvolvedFrequencySpectrumVarianceDataFFTW : public KTFrequencySpectrumVarianceDataCore, public Nymph::KTExtensibleData< KTConvolvedFrequencySpectrumVarianceDataFFTW >
    {
        public:
            KTConvolvedFrequencySpectrumVarianceDataFFTW();
            virtual ~KTConvolvedFrequencySpectrumVarianceDataFFTW();

            KTConvolvedFrequencySpectrumVarianceDataFFTW& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTConvolvedFrequencySpectrumVarianceDataPolar : public KTFrequencySpectrumVarianceDataCore, public Nymph::KTExtensibleData< KTConvolvedFrequencySpectrumVarianceDataPolar >
    {
        public:
            KTConvolvedFrequencySpectrumVarianceDataPolar();
            virtual ~KTConvolvedFrequencySpectrumVarianceDataPolar();

            KTConvolvedFrequencySpectrumVarianceDataPolar& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };


} /* namespace Katydid */

#endif /* KTMULTICHANNELINNERPRODUCTDATA_HH_ */
