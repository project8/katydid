/*
 * KTEventFirstTrackNPointsCut.hh
 *
 *  Created on: February 18, 2019
 *      Author: Yuhao
 */

#ifndef KTEVENTFIRSTTRACKNPOINTSCUT_HH_
#define KTEVENTFIRSTTRACKNPOINTSCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventFirstTrackNPointsCut
     @author Yuhao
     @brief Cut on the number of points in a first track
     @details
     Configuration name: "event-ntracks-first-track-npoints-nup-cut"
     Available configuration values:
     - "n-points-in-event-first-track": int -- minimum number of points in the first track
     - "n-tracks-in-event": int -- if event sequence has less than this number of events the cut is applied
     - "min-average-nup": double -- minimum nup per unit length in the first track for the event to pass the cut
     - "min-total-nup": double -- minimum total nup in the first track for the event to pass the cut
     - "min-max-track-nup": double -- minimum maximum nup in the first track for the event to pass the cut
     - "wide-or-narrow": string -- decides whether to use "wide" NUP or "narrow" NUP (default: "wide")
     - "time-or-bin-average": string -- decides whether to divide total NUP by track time length or track NTrackBins
    */

    class KTEventNTracksFirstTrackNPointsNUPCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

	private:
        enum class wide_or_narrow
        {
            wide,
            narrow
        };
        enum class time_or_bin_average
        {
            time,
            bin
        };
    public:
        KTEventNTracksFirstTrackNPointsNUPCut(const std::string& name = "event-ntracks-first-track-npoints-nup-cut");
        ~KTEventNTracksFirstTrackNPointsNUPCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(unsigned, EventFirstTrackNPoints);
        MEMBERVARIABLE(unsigned, EventNTracks);
        MEMBERVARIABLE(double, MinTotalNUP);
        MEMBERVARIABLE(double, MinAverageNUP);
        MEMBERVARIABLE(double, MinMaxNUP);
        MEMBERVARIABLE(wide_or_narrow, WideOrNarrow);
        MEMBERVARIABLE(time_or_bin_average, TimeOrBinAverage);

    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEVENTFIRSTTRACKNPOINTSCUT_HH_ */