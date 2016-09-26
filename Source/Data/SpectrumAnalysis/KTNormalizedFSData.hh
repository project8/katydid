/**
 @file KTNormalizedFSData.hh
 @brief Contains KTNormalizedFSDataPolar and KTNormalizedFSDataFFTW
 @details
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTNORMALIZEDFSDATA_HH_
#define KTNORMALIZEDFSDATA_HH_

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTPowerSpectrumData.hh"

namespace Katydid
{
    

    class KTNormalizedFSDataPolar : public KTFrequencySpectrumDataPolarCore, public Nymph::KTExtensibleData< KTNormalizedFSDataPolar >
    {
        public:
            KTNormalizedFSDataPolar();
            virtual ~KTNormalizedFSDataPolar();

            KTNormalizedFSDataPolar& SetNComponents(unsigned components);

        public:
            static const std::string sName;

    };

    class KTNormalizedFSDataFFTW : public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTNormalizedFSDataFFTW >
    {
        public:
            KTNormalizedFSDataFFTW();
            virtual ~KTNormalizedFSDataFFTW();

            KTNormalizedFSDataFFTW& SetNComponents(unsigned components);

        public:
            static const std::string sName;

    };

    class KTNormalizedPSData : public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTNormalizedPSData >
    {
        public:
            KTNormalizedPSData();
            virtual ~KTNormalizedPSData();

            KTNormalizedPSData& SetNComponents(unsigned components);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */
#endif /* KTNORMALIZEDFSDATA_HH_ */
