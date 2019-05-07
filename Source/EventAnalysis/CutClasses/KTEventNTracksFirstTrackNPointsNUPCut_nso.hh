/*
 * KTEventNTracksFirstTrackNPointsNUPCut_nso.hh
 *
 *  Created on: February 18, 2019
 *      Author: Yuhao
 */

#ifndef KTEVENTNTRACKSFIRSTTRACKNPOINTSNUPCUTNSO_HH_
#define KTEVENTNTRACKSFIRSTTRACKNPOINTSNUPCUTNSO_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventNTracksFirstTrackNPointsNUPCut
     @author Yuhao
     @brief Cut based on the number of points in a first track and number of tracks in an event. 
     Made by combining features of EventFirstTrackNUPCut and EventFirstTrackNPointsCut.
     @details
     Configuration name: "event-ntracks-first-track-npoints-nup-cut"
     Available configuration values:
     - "n-points-in-event-first-track": int -- number of points in the first track
     - "n-tracks-in-event": int -- if event sequence has this number of events the thresholds specified in the cut is applied
     - "min-average-nup": double -- minimum nup per unit length in the first track for the event to pass the cut
     - "min-total-nup": double -- minimum total nup in the first track for the event to pass the cut
     - "min-max-track-nup": double -- minimum maximum nup in the first track for the event to pass the cut
     - "wide-or-narrow": string -- decides whether to use "wide" NUP or "narrow" NUP (default: "wide")
     - "time-or-bin-average": string -- decides whether to divide total NUP by track time length or track NTrackBins
    */

    class KTEventNTracksFirstTrackNPointsNUPCut_nso : public Nymph::KTCutOneArg< KTMultiTrackEventData >
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
	
	private:    
        struct thresholds
		{
			double min_total_nup;
			double min_average_nup;
			double min_max_track_nup;
		};
    	
    public:
        KTEventNTracksFirstTrackNPointsNUPCut_nso(const std::string& name = "event-ntracks-first-track-npoints-nup-cut-nso");
        ~KTEventNTracksFirstTrackNPointsNUPCut_nso();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(unsigned, DimensionFTNPoints);
        MEMBERVARIABLE(unsigned, DimensionNTracks);
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