/*
 * KTNTracksNPointsNUPCut.hh
 *
 *  Created on: February 18, 2019
 *      Author: Yuhao
 */

#ifndef KTNTRACKSNPOINTSNUPCUT_HH_
#define KTNTRACKSNPOINTSNUPCUT_HH_

#include "KTCut.hh"

#include <vector>

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTNTracksNPointsNUPCut

     @author Yu-Hao Sun

     @brief Cut on NUP characteristics based on the number of points in a first track and number of tracks in an event.

     @details
     This cut looks at each event, and removes events that don't meet the minimum total-NUP, minimum average-NUP, or minimum max-NUP.

     The user can select to use the wide or narrow definition of NUP, and they can select to use either time or bin averaging to get the average NUP.

     The parameters for the cut are selected in two ways: a default set of thresholds, and thresholds for specific values of n-tracks and first-track n-points.
     Typically the user will select a range of n-tracks and first-track n-points over which to specify each set of threshold values, e.g. n-tracks = (0, 5) and n-points = (3, 8).
     Outside of those ranges (and for any values in the middle that weren't specified), the default thresholds will be used.

     Configuration name: `ntracks-npoints-nup-cut`

     Available configuration values:
     - `default-parameters`: thresholds (see below) -- default threshold parameters, which get applied whenever there is not a user-specified set of thresholds
     - `parameters`: array of thresholds (see below) -- array of threshold parameters and the positions at which those thresholds apply
     - `wide-or-narrow`: string -- decides whether to use "wide" NUP or "narrow" NUP (default: "wide")
     - `time-or-bin-average`: string -- decides whether to divide total NUP by track "time" length or track number of "bin"s

    Configuring "default-parameters":
    - `min-total-nup`: double -- minimum total NUP in the first track for the event to pass the cut
    - `min-average-nup`: double -- minimum NUP per unit length in the first track for the event to pass the cut
    - `min-max-nup`: double -- minimum max-NUP in the first track fro the even to pass the cut

    Configuring the "parameters" -- an array of:
    - `ntracks`: unsigned int -- number of tracks in the event for this set of thresholds
    - `ft-npoints`: unsigned int -- number of points in the first track for this set of thresholds
    - `min-total-nup`: see above
    - `min-average-nup`: see above
    - `min-max-nup`: see above

    Example configuration of the `default-parameters` and `parameters`:
    ```
         default-parameters:
            min-total-nup: 0
            min-average-nup: 0
            min-max-nup: 0
         parameters:
          - ft-npoints: 3
            ntracks: 1
            min-total-nup: 0
            min-average-nup: 13
            min-max-nup: 0
          - ft-npoints: 3
            ntracks: 2
            min-total-nup: 0
            min-average-nup: 11
            min-max-nup: 0
    ```
    */

    class KTNTracksNPointsNUPCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
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
    	KTNTracksNPointsNUPCut(const std::string& name = "event-ntracks-first-track-npoints-nup-cut-nso");
        virtual ~KTNTracksNPointsNUPCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLEREF(std::vector< std::vector< thresholds > >, Thresholds);
        MEMBERVARIABLE(thresholds, DefaultThresholds);

        MEMBERVARIABLE(WideOrNarrow, WideOrNarrow);
        MEMBERVARIABLE(TimeOrBinAvg, TimeOrBinAverage);
        
    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTNTRACKSNPOINTSNUPCUT_HH_ */
