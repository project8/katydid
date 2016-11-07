/*
 * KTEventTimeCut.hh
 *
 *  Created on: Nov 7, 2016
 *      Author: ezayas
 */

#ifndef KTEVENTTIMECUT_HH_
#define KTEVENTTIMECUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventTimeCut
     @author E. Zayas

     @brief Cuts on the start time of KTMultiTrackEventData

     @details
     KTMultiTrackEventData objects must have the start time in a specified range to pass the cut

     Configuration name: "event-time-cut"

     Available configuration values:
     - "min-time": double -- minimum start time to accept
     - "max-time": double -- maximum start time to accept
    */

    class KTEventTimeCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTEventTimeCut(const std::string& name = "event-time-cut");
        ~KTEventTimeCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinStartTime);
        MEMBERVARIABLE(double, MaxStartTime);

    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEventTimeCut_HH_ */
