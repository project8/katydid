/**
 @file KTLowPassFilteredData.hh
 @brief Contains KTLowPassFilteredFSDataPolar and KTLowPassFilteredFSDataFFTW
 @details
 @author: N. S. Oblath
 @date: Nov 3, 2014
 */

#ifndef KTLOWPASSFILTEREDDATA_HH_
#define KTLOWPASSFILTEREDDATA_HH_

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTPowerSpectrumData.hh"

namespace Katydid
{

    class KTLowPassFilteredFSDataPolar : public KTFrequencySpectrumDataPolarCore, public Nymph::KTExtensibleData< KTLowPassFilteredFSDataPolar >
    {
        public:
            KTLowPassFilteredFSDataPolar();
            virtual ~KTLowPassFilteredFSDataPolar();

            KTLowPassFilteredFSDataPolar& SetNComponents(unsigned components);

        public:
            static const std::string sName;

    };

    class KTLowPassFilteredFSDataFFTW : public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTLowPassFilteredFSDataFFTW >
    {
        public:
            KTLowPassFilteredFSDataFFTW();
            virtual ~KTLowPassFilteredFSDataFFTW();

            KTLowPassFilteredFSDataFFTW& SetNComponents(unsigned components);

        public:
            static const std::string sName;

    };

    class KTLowPassFilteredPSData : public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTLowPassFilteredPSData >
    {
        public:
            KTLowPassFilteredPSData();
            virtual ~KTLowPassFilteredPSData();

            KTLowPassFilteredPSData& SetNComponents(unsigned components);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */
#endif /* KTLOWPASSFILTEREDDATA_HH_ */
