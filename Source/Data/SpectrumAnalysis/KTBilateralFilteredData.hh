/**
 @file KTBilateralFilteredData.hh
 @brief Contains KTBilateralFilteredFSDataPolar and KTBilateralFilteredFSDataFFTW
 @details
 @author: N. Buzinsky
 @date: Mar 7, 2018
 */

#ifndef KTBILATERALFILTEREDDATA_HH_
#define KTBILATERALFILTEREDDATA_HH_

#include "KTFrequencySpectrumDataFFTW.hh"

namespace Katydid
{
    class KTBilateralFilteredFSDataFFTW : public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTBilateralFilteredFSDataFFTW >
    {
        public:
            KTBilateralFilteredFSDataFFTW();
            virtual ~KTBilateralFilteredFSDataFFTW();

            KTBilateralFilteredFSDataFFTW& SetNComponents(unsigned components);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */
#endif /* KTBILATERALFILTEREDDATA_HH_ */
