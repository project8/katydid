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


} /* namespace Katydid */

#endif /* KTCONVOLVEDSPECTRUMDATA_HH_ */
