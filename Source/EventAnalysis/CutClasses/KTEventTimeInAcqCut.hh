/*
 * KTEventTimeInAcqCut.hh
 *
 *  Created on: Jul 10, 2018
 *      Author: enovitski and cclaessens
 */

#ifndef KTEVENTTIMEINACQCUT_HH_
#define KTEVENTTIMEINACQCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventTimeInAcqCut
     @author E. Novitski and C. Claessens

     @brief Cuts on the start time in acquisition of KTMultiTrackEventData

     @details
     KTMultiTrackEventData objects must have the start time in a specified range to pass the cut

     Configuration name: "event-time-in-acq-cut"

     Available configuration values:
     - "min-time": double -- minimum start time to accept
     - "max-time": double -- maximum start time to accept
    */

    class KTEventTimeInAcqCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTEventTimeInAcqCut(const std::string& name = "event-time-in-acq-cut");
        ~KTEventTimeInAcqCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinStartTime);
        MEMBERVARIABLE(double, MaxStartTime);

    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEventTimeInAcqCut_HH_ */
