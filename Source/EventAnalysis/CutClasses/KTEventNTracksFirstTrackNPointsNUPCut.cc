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