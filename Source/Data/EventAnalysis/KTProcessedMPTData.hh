/*
 * KTProcessedMPTData.hh
 *
 *  Created on: Mar 14, 2018
 *      Author: ezayas
 */

#ifndef KTPROCESSEDMPTDATA_HH_
#define KTPROCESSEDMPTDATA_HH_

#include "KTData.hh"
#include "KTProcessedTrackData.hh"
#include "KTMemberVariable.hh"

namespace Katydid
{

    class KTProcessedMPTData : public Nymph::KTExtensibleData< KTProcessedMPTData >
    {
        public:
            KTProcessedMPTData();
            KTProcessedMPTData(const KTProcessedMPTData& orig);
            virtual ~KTProcessedMPTData();

            KTProcessedMPTData& operator=(const KTProcessedMPTData& rhs);

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLEREF(KTProcessedTrackData, MainTrack);
            MEMBERVARIABLE(double, AxialFrequency);

        public:
            static const std::string sName;
    };

}
#endif

