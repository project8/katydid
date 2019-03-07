/*
 * KTEventNTracksFirstTrackNPointsNUPCut.cc
 *
 *  Created on: February 18, 2019
 *      Author: Yuhao
 */

#include "KTEventNTracksFirstTrackNPointsNUPCut.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(ecnuplog, "KTEventNTracksFirstTrackNPointsNUPCut");

    const std::string KTEventNTracksFirstTrackNPointsNUPCut::Result::sName = "event-ntracks-first-track-npoints-nup-cut";

    KT_REGISTER_CUT(KTEventNTracksFirstTrackNPointsNUPCut);

    KTEventNTracksFirstTrackNPointsNUPCut::KTEventNTracksFirstTrackNPointsNUPCut(const std::string& name) :
         KTCutOneArg(name),
         fEventFirstTrackNPoints(0),
         fEventNTracks(1),
         fMinTotalNUP(0.),
         fMinAverageNUP(0.),
         fMinMaxNUP(0.),
         fWideOrNarrow( wide_or_narrow::wide ),
         fTimeOrBinAverage( time_or_bin_average::time )
    {}

    KTEventNTracksFirstTrackNPointsNUPCut::~KTEventNTracksFirstTrackNPointsNUPCut()
    {}

    bool KTEventNTracksFirstTrackNPointsNUPCut::Configure(const scarab::param_node* node)
    {
        /*
    parameters:
      - ft-npoints: 3
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 13
        min-max-track-nup: 0
      - ft-npoints: 3
        ntracks: 2
        min-total-nup: 0
        min-average-nup: 11
        min-max-track-nup: 0
      - ft-npoints: 3
        ntracks: 3
        min-total-nup: 0
        min-average-nup: 7.8
        min-max-track-nup: 0
      - ft-npoints: 4
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 10
        min-max-track-nup: 0
      - ft-npoints: 4
        ntracks: 2
        min-total-nup: 0
        min-average-nup: 8.5
        min-max-track-nup: 0
      - ft-npoints: 5
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 7.8
        min-max-track-nup: 0
      - ft-npoints: 5
        ntracks: 2
        min-total-nup: 0
        min-average-nup: 7.8
        min-max-track-nup: 0
      - ft-npoints: 6
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 8.5
        min-max-track-nup: 0
      - ft-npoints: 7
        ntracks: 1
        min-total-nup: 0
        min-average-nup: 7.3
        min-max-track-nup: 0

        To fill in the cut parameters:
        1. scan array to get dimensions
        2. create 2D array with those dimensions
        3. store offset and maximum of both dimensions
        3. fill 2D array with -1
        4. fill in values given
        5. for each row:
            1. scan horizontally from left to find first >=0 value, then fill back in to the left side
            2. scan horizontally from the right to find the first >=0 value, then fill back in to the right side
            3. check for holes in between and fill from the left

        Need to work out function to retrieve the cut parameters with appropriate bounding

         */
        if (node == NULL) return true;

        SetEventFirstTrackNPoints( node->get_value< unsigned >( "n-points-in-event-first-track", GetEventFirstTrackNPoints() ) );
        SetEventNTracks( node->get_value< unsigned >( "n-tracks-in-event", GetEventNTracks() ) );
        SetMinTotalNUP( node->get_value< double >( "min-total-nup", GetMinTotalNUP() ) );
        SetMinAverageNUP( node->get_value< double >( "min-average-nup", GetMinAverageNUP() ) );
        SetMinMaxNUP( node->get_value< double >("min-max-track-nup", GetMinMaxNUP() ) );

        if (node->has("wide-or-narrow"))
        {
            if (node->get_value("wide-or-narrow") == "wide")
            {
                SetWideOrNarrow(wide_or_narrow::wide);
            }
            else if (node->get_value("wide-or-narrow") == "narrow")
            {
                SetWideOrNarrow(wide_or_narrow::narrow);
            }
            else
            {
                KTERROR(ecnuplog, "Invalid string for fWideOrNarrow");
                return false;
            }
        }
        if (node->has("time-or-bin-average"))
        {
            if (node->get_value("time-or-bin-average") == "time")
            {
                SetTimeOrBinAverage(time_or_bin_average:: time);
            }
            else if (node->get_value("time-or-bin-average") == "bin")
            {
                SetTimeOrBinAverage(time_or_bin_average::bin);
            }
            else
            {
                KTERROR(ecnuplog, "Invalid string for fTimeOrBinAverage");
                return false;
            }
        }
        return true;
    }

    bool KTEventNTracksFirstTrackNPointsNUPCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;
		if( eventData.GetTotalEventSequences() == fEventNTracks and eventData.GetFirstTrackNTrackBins() == fEventFirstTrackNPoints )
		{	
			if ( fWideOrNarrow == wide_or_narrow::narrow )
			{
				if( eventData.GetFirstTrackTotalNUP() < fMinTotalNUP )
				{
					isCut = true;
				}
				if ( fTimeOrBinAverage == time_or_bin_average::time )
				{
					if( eventData.GetFirstTrackTotalNUP() / eventData.GetFirstTrackTimeLength() < fMinAverageNUP )
					{
						isCut = true;
					}
				}
				else
				{
					if( eventData.GetFirstTrackTotalNUP() / eventData.GetFirstTrackNTrackBins() < fMinAverageNUP )
					{
						isCut = true;
					}
				}
			}
			else
			{
				if( eventData.GetFirstTrackTotalWideNUP() < fMinTotalNUP )
				{
					isCut = true;
				}
				if ( fTimeOrBinAverage == time_or_bin_average::time )
				{
					if( eventData.GetFirstTrackTotalWideNUP() / eventData.GetFirstTrackTimeLength() < fMinAverageNUP )
					{
						isCut = true;
					}
				}
				else
				{
					if( eventData.GetFirstTrackTotalWideNUP() / eventData.GetFirstTrackNTrackBins() < fMinAverageNUP )
					{
						isCut = true;
					}
				}
			}
			if( eventData.GetFirstTrackMaxNUP() < fMinMaxNUP )
			{
				isCut = true;
			}
		}
		data.GetCutStatus().AddCutResult< KTEventNTracksFirstTrackNPointsNUPCut::Result >(isCut);

		return isCut;
    }

} // namespace Katydid
