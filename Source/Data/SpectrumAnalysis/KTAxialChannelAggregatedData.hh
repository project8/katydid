/**
 @file KTAxialAggregatedFrequencyData
 @brief Contains KTAxialAggregatedDataCore and KTAxialAggregatedFrequencySpectrumDataFFTW
 @details The summed spectra from all the channels in the axial direction.
 @author: P. T. Surukuchi
 @date: Apr 28, 2020
 */

#ifndef KTAXIALCHANNELSUMMEDDDATA_HH_
#define KTAXIALCHANNELSUMMEDDDATA_HH_

#include "KTData.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTPowerSpectrumData.hh"

#include "KTMemberVariable.hh"

#include <vector>

namespace Katydid
{
    class KTAxialAggregatedDataCore
    {
        public:
            KTAxialAggregatedDataCore(){}

            virtual ~KTAxialAggregatedDataCore(){}


    };

    class KTAxialAggregatedFrequencySpectrumDataFFTW : public KTAxialAggregatedDataCore, public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTAxialAggregatedFrequencySpectrumDataFFTW >
    {
        public:
            KTAxialAggregatedFrequencySpectrumDataFFTW();

            virtual ~KTAxialAggregatedFrequencySpectrumDataFFTW();

            virtual KTAxialAggregatedFrequencySpectrumDataFFTW& SetNComponents(unsigned);

            static const std::string sName;
    };

    class KTAxialAggregatedPowerSpectrumData : public KTAxialAggregatedDataCore, public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTAxialAggregatedPowerSpectrumData >
    {
        public:
            KTAxialAggregatedPowerSpectrumData();

            virtual ~KTAxialAggregatedPowerSpectrumData();

            virtual KTAxialAggregatedPowerSpectrumData& SetNComponents(unsigned);

            static const std::string sName;
    };

}/* namespace Katydid */
#endif /* KTAXIALCHANNELSUMMEDDDATA_HH_ */
