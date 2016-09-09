/*
 * KTPowerSpectrumData.hh
 *
 *  Created on: Oct 2, 2015
 *      Author: ezayas
 */

#ifndef KTPOWERSPECTRUMUNCERTAINTYDATA_HH_
#define KTPOWERSPECTRUMUNCERTAINTYDATA_HH_

#include "KTData.hh"

#include "KTPowerSpectrumData.hh"

#include <vector>

namespace Katydid
{
    

    class KTPowerSpectrumUncertaintyData : public KTPowerSpectrumDataCore, public Nymph::KTExtensibleData< KTPowerSpectrumUncertaintyData >
    {
        public:
            KTPowerSpectrumUncertaintyData();
            virtual ~KTPowerSpectrumUncertaintyData();

            KTPowerSpectrumUncertaintyData& SetNComponents(unsigned channels);

        public:
            static const std::string sName;

    };



} /* namespace Katydid */

#endif /* KTPOWERSPECTRUMUncertaintyDATA_HH_ */