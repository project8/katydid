/*
 * KTEventFirstTrackPowerCut.hh
 *
 *  Created on: Apr 12, 2017
 *      Author: N.S. Oblath
 */

#ifndef KTEVENTFIRSTTRACKPOWERCUT_HH_
#define KTEVENTFIRSTTRACKPOWERCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventFirstTrackPowerCut
     @author N.S. Oblath

     @brief Cut on the power per unit length of the first track in KTMultiTrackEventData

     @details


     Configuration name: "event-first-track-power-cut"

     Available configuration values:
     - "min-power": double -- minimum power per unit length in the first track for the event to pass the cut
    */

    class KTEventFirstTrackPowerCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTEventFirstTrackPowerCut(const std::string& name = "event-first-track-power-cut");
        ~KTEventFirstTrackPowerCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinPower);

    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEVENTFIRSTTRACKPOWERCUT_HH_ */
