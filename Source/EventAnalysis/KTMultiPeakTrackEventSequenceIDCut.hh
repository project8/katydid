/*
 * KTMultiPeakTrackEventSequenceIDCut.hh
 *
 *  Created on: Dec 7, 2016
 *      Author: ezayas
 */

#ifndef KTMULTIPEAKTRACKEVENTSEQUENCEIDCUT_HH_
#define KTMULTIPEAKTRACKEVENTSEQUENCEIDCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiPeakTrackData;

    /*
     @class KTMultiPeakTrackEventSequenceIDCut
     @author E. Zayas
     @brief Cuts on the event sequence ID of a MPT structure
     @details
     Specify and min and max event sequence ID
     Configuration name: "mpt-event-sequence-cut"
     Available configuration values:
     - "min-id": int -- minimum event sequence ID to accept
     - "max-id": int -- maximum event sequence ID to accept
    */

    class KTMultiPeakTrackEventSequenceIDCut : public Nymph::KTCutOneArg< KTMultiPeakTrackData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTMultiPeakTrackEventSequenceIDCut(const std::string& name = "mpt-event-sequence-cut");
        ~KTMultiPeakTrackEventSequenceIDCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinID);
        MEMBERVARIABLE(double, MaxID);

    public:
        bool Apply(Nymph::KTData& data, KTMultiPeakTrackData& mptData);

    };
} // namespace Katydid

#endif /* KTMULTIPEAKTRACKEVENTSEQUENCEIDCUT_HH_ */
