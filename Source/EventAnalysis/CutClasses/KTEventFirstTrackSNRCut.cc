/*
 * KTEventFirstTrackSNRCut.cc
 *
 *  Created on: June 14, 2018
 *      Author: C. Claessens
 */

#include "KTEventFirstTrackSNRCut.hh"
#include "KTMultiTrackEventData.hh"

#include "logger.hh"

namespace Katydid
{
    LOGGER(ecsnrlog, "KTEventFirstTrackSNRCut");

    const std::string KTEventFirstTrackSNRCut::Result::sName = "event-first-track-snr-cut";

    KT_REGISTER_CUT(KTEventFirstTrackSNRCut);

    KTEventFirstTrackSNRCut::KTEventFirstTrackSNRCut(const std::string& name) :
         KTCutOneArg(name),
         fMinTotalSNR(0.),
         fMinAverageSNR(0.),
         fMinMaxSNR(0.),
         fWideOrNarrow( wide_or_narrow::wide ),
         fTimeOrBinAverage( time_or_bin_average::time )
    {}

    KTEventFirstTrackSNRCut::~KTEventFirstTrackSNRCut()
    {}

    bool KTEventFirstTrackSNRCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinTotalSNR( node->get_value< double >( "min-total-snr", GetMinTotalSNR() ) );
        SetMinAverageSNR( node->get_value< double >( "min-average-snr", GetMinAverageSNR() ) );
        SetMinMaxSNR( node->get_value< double >("min-max-track-snr", GetMinMaxSNR() ) );

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
                LERROR(ecsnrlog, "Invalid string for fWideOrNarrow");
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
                LERROR(ecsnrlog, "Invalid string for fTimeOrBinAverage");
                return false;
            }
        }

        return true;
    }

    bool KTEventFirstTrackSNRCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;
        if ( fWideOrNarrow == wide_or_narrow::narrow )
        {
            if( eventData.GetFirstTrackTotalSNR() < fMinTotalSNR )
            {
                isCut = true;
            }
            if ( fTimeOrBinAverage == time_or_bin_average::time )
            {
                if( eventData.GetFirstTrackTotalSNR() / eventData.GetFirstTrackTimeLength() < fMinAverageSNR )
                {
                    isCut = true;
                }
            }
            else
            {
                if( eventData.GetFirstTrackTotalSNR() / eventData.GetFirstTrackNTrackBins() < fMinAverageSNR )
                {
                    isCut = true;
                }
            }
        }
        else
        {
            if( eventData.GetFirstTrackTotalWideSNR() < fMinTotalSNR )
            {
                isCut = true;
            }
            if ( fTimeOrBinAverage == time_or_bin_average::time )
            {
                if( eventData.GetFirstTrackTotalWideSNR() / eventData.GetFirstTrackTimeLength() < fMinAverageSNR )
                {
                    isCut = true;
                }
            }
            else
            {
                if( eventData.GetFirstTrackTotalWideSNR() / eventData.GetFirstTrackNTrackBins() < fMinAverageSNR )
                {
                    isCut = true;
                }
            }
        }
        if( eventData.GetFirstTrackMaxSNR() < fMinMaxSNR )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTEventFirstTrackSNRCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
