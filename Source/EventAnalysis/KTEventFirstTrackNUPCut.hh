/*
 * KTEventFirstTrackNUPCut.hh
 *
 *  Created on: June 14, 2018
 *      Author: C. Claessens
 */

#ifndef KTEVENTFIRSTTRACKNUPCUT_HH_
#define KTEVENTFIRSTTRACKNUPCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventFirstTrackNUPCut
     @author C. Claessens

     @brief Cut on the total NUP and the NUP per unit length of the first track in KTMultiTrackEventData

     @details


     Configuration name: "event-first-track-nup-cut"

     Available configuration values:
     - "min-average-nup": double -- minimum nup per unit length in the first track for the event to pass the cut
     - "min-total-nup": double -- minimum total nup in the first track for the event to pass the cut
     - "wide-or-narrow": string -- decides whether to use wide NUP or narrow NUP (default: "wide")
    */

    class KTEventFirstTrackNUPCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTEventFirstTrackNUPCut(const std::string& name = "event-first-track-nup-cut");
        ~KTEventFirstTrackNUPCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinTotalNUP);
        MEMBERVARIABLE(double, MinAverageNUP);
        MEMBERVARIABLE(std::string, WideOrNarrow);

    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEVENTFIRSTTRACKNUPCUT_HH_ */
