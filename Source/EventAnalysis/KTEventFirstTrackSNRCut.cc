/*
 * KTEventFirstTrackSNRCut.cc
 *
 *  Created on: June 14, 2018
 *      Author: C. Claessens
 */

#include "KTEventFirstTrackSNRCut.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(ecsnrlog, "KTEventFirstTrackSNRCut");

    const std::string KTEventFirstTrackSNRCut::Result::sName = "event-first-track-snr-cut";

    KT_REGISTER_CUT(KTEventFirstTrackSNRCut);

    KTEventFirstTrackSNRCut::KTEventFirstTrackSNRCut(const std::string& name) :
         KTCutOneArg(name),
         fMinTotalSNR(0.),
         fMinAverageSNR(0.),
         fWideOrNarrow("wide" )
    {}

    KTEventFirstTrackSNRCut::~KTEventFirstTrackSNRCut()
    {}

    bool KTEventFirstTrackSNRCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinTotalSNR( node->get_value< double >( "min-total-snr", GetMinTotalSNR() ) );
        SetMinAverageSNR( node->get_value< double >( "min-average-snr", GetMinAverageSNR() ) );
        SetWideOrNarrow( node->get_value("wide-or-narrow", GetWideOrNarrow() ) );

        return true;
    }

    bool KTEventFirstTrackSNRCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;
        if ( fWideOrNarrow == "narrow" )
        {
            if( eventData.GetFirstTrackTotalSNR() < fMinTotalSNR )
            {
                isCut = true;
            }
            if( eventData.GetFirstTrackTotalSNR() / eventData.GetFirstTrackTimeLength() < fMinAverageSNR )
            {
                isCut = true;
            }
        }
        else if ( fWideOrNarrow == "wide" )
        {
            if( eventData.GetFirstTrackTotalWideSNR() < fMinTotalSNR )
            {
                isCut = true;
            }
            if( eventData.GetFirstTrackTotalWideSNR() / eventData.GetFirstTrackTimeLength() < fMinAverageSNR )
            {
                isCut = true;
            }
        }
        else
        {
            KTERROR(ecsnrlog, "Invalid string for fWideOrNarrow: "<< fWideOrNarrow);
        }

        data.GetCutStatus().AddCutResult< KTEventFirstTrackSNRCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
