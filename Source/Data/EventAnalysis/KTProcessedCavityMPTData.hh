/*
 * KTProcessedCavityMPTData.hh
 *
 *  Created on: Aug 19, 2025
 *      Author: juniorpe
 */

#ifndef KTPROCESSEDCAVITYMPTDATA_HH_
#define KTPROCESSEDCAVITYMPTDATA_HH_

#include "KTData.hh"
#include "KTProcessedTrackData.hh"
#include "KTMemberVariable.hh"

namespace Katydid
{

    class KTProcessedCavityMPTData : public Nymph::KTExtensibleData< KTProcessedCavityMPTData >
    {
        public:
            KTProcessedCavityMPTData();
            KTProcessedCavityMPTData(const KTProcessedCavityMPTData& orig);
            virtual ~KTProcessedCavityMPTData();

            KTProcessedCavityMPTData& operator=(const KTProcessedCavityMPTData& rhs);

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(double, AxialFrequency);

        public:
            static const std::string sName;
    };

}
#endif

