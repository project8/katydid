/*
 * KTEventFirstTrackNPointsCut.hh
 *
 *  Created on: June 14, 2018
 *      Author: C. Claessens
 */

#ifndef KTEVENTFIRSTTRACKNPOINTSCUT_HH_
#define KTEVENTFIRSTTRACKNPOINTSCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventFirstTrackNPointsCut
     @author C. Claessens

     @brief Cut on the number of points in a first track

     @details


     Configuration name: "event-first-track-npoints-cut"

     Available configuration values:
     - "min-points": int -- minimum number of points in the first track
     - "n-tracks-in-event": int -- if event sequence has less than this number of events the cut is applied
    */

    class KTEventFirstTrackNPointsCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };


    public:
        KTEventFirstTrackNPointsCut(const std::string& name = "event-first-track-npoints-cut");
        ~KTEventFirstTrackNPointsCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(unsigned, MinPoints);
        MEMBERVARIABLE(unsigned, MaxTracks);

    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEVENTFIRSTTRACKNPOINTSCUT_HH_ */
