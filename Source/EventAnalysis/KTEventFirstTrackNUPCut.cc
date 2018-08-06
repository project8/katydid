/*
 * KTEventFirstTrackNUPCut.cc
 *
 *  Created on: June 14, 2018
 *      Author: C. Claessens
 */

#include "KTEventFirstTrackNUPCut.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(ecnuplog, "KTEventFirstTrackNUPCut");

    const std::string KTEventFirstTrackNUPCut::Result::sName = "event-first-track-nup-cut";

    KT_REGISTER_CUT(KTEventFirstTrackNUPCut);

    KTEventFirstTrackNUPCut::KTEventFirstTrackNUPCut(const std::string& name) :
         KTCutOneArg(name),
         fMinTotalNUP(0.),
         fMinAverageNUP(0.),
         fWideOrNarrow( wide_or_narrow::wide ),
         fTimeOrBinAverage( time_or_bin_average::time )
    {}

    KTEventFirstTrackNUPCut::~KTEventFirstTrackNUPCut()
    {}

    bool KTEventFirstTrackNUPCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinTotalNUP( node->get_value< double >( "min-total-nup", GetMinTotalNUP() ) );
        SetMinAverageNUP( node->get_value< double >( "min-average-nup", GetMinAverageNUP() ) );

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

    bool KTEventFirstTrackNUPCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;
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

        data.GetCutStatus().AddCutResult< KTEventFirstTrackNUPCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
