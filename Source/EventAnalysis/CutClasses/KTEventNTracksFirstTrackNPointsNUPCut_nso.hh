/*
 * KTEventNTracksFirstTrackNPointsNUPCut_nso.hh
 *
 *  Created on: February 18, 2019
 *      Author: Yuhao
 */

#ifndef KTEVENTNTRACKSFIRSTTRACKNPOINTSNUPCUTNSO_HH_
#define KTEVENTNTRACKSFIRSTTRACKNPOINTSNUPCUTNSO_HH_

#include "KTCut.hh"

#include <vector>

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
        enum class WideOrNarrow
        {
            wide,
            narrow
        };
        enum class TimeOrBinAvg
        {
            time,
            bin
        };
	
	private:    
        struct thresholds
		{
			double fMinTotalNUP;
			double fMinAverageNUP;
			double fMinMaxNUP;
			bool fFilled;
			thresholds() : fMinTotalNUP(-1.), fMinAverageNUP(-1.), fMinMaxNUP(-1.), fFilled(false) {}
		};
    	unsigned maxFTNPointsConfig = 0;
    	unsigned maxNTracksConfig = 0;
    	
    public:
        KTEventNTracksFirstTrackNPointsNUPCut_nso(const std::string& name = "event-ntracks-first-track-npoints-nup-cut-nso");
        ~KTEventNTracksFirstTrackNPointsNUPCut_nso();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLEREF(std::vector< std::vector< thresholds > >, Thresholds);

        MEMBERVARIABLE(WideOrNarrow, WideOrNarrow);
        MEMBERVARIABLE(TimeOrBinAvg, TimeOrBinAverage);
        
    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEVENTFIRSTTRACKNPOINTSCUT_HH_ */
