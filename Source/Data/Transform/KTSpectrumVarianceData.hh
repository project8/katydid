/*
 * KTSpectrumVarianceData.hh
 *
 *  Created on: Feb 9, 2018
 *      Author: ezayas
 */

#ifndef KTSPECTRUMVARIANCEDATA_HH_
#define KTSPECTRUMVARIANCEDATA_HH_

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

    class KTPowerSpectrumVarianceData : public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTPowerSpectrumVarianceData >
    {
        public:
            KTPowerSpectrumVarianceData();
            virtual ~KTPowerSpectrumVarianceData();

            KTPowerSpectrumVarianceData& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTFrequencySpectrumVarianceDataFFTW : public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTFrequencySpectrumVarianceDataFFTW >
    {
        public:
            KTFrequencySpectrumVarianceDataFFTW();
            virtual ~KTFrequencySpectrumVarianceDataFFTW();

            KTFrequencySpectrumVarianceDataFFTW& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };

    class KTFrequencySpectrumVarianceDataPolar : public KTFrequencySpectrumDataPolarCore, public Nymph::KTExtensibleData< KTFrequencySpectrumVarianceDataPolar >
    {
        public:
            KTFrequencySpectrumVarianceDataPolar();
            virtual ~KTFrequencySpectrumVarianceDataPolar();

            KTFrequencySpectrumVarianceDataPolar& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };


} /* namespace Katydid */

#endif /* KTSPECTRUMVARIANCEDATA_HH_ */
