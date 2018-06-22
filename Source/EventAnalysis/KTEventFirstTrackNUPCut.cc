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
         fWideOrNarrow("wide" )
    {}

    KTEventFirstTrackNUPCut::~KTEventFirstTrackNUPCut()
    {}

    bool KTEventFirstTrackNUPCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinTotalNUP( node->get_value< double >( "min-total-nup", GetMinTotalNUP() ) );
        SetMinAverageNUP( node->get_value< double >( "min-average-nup", GetMinAverageNUP() ) );
        SetWideOrNarrow( node->get_value("wide-or-narrow", GetWideOrNarrow() ) );

        return true;
    }

    bool KTEventFirstTrackNUPCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;
        if ( fWideOrNarrow == "narrow" )
        {
            if( eventData.GetFirstTrackTotalNUP() < fMinTotalNUP )
            {
                isCut = true;
            }
            if( eventData.GetFirstTrackTotalNUP() / eventData.GetFirstTrackTimeLength() < fMinAverageNUP )
            {
                isCut = true;
            }
        }
        else if ( fWideOrNarrow == "wide" )
        {
            if( eventData.GetFirstTrackTotalWideNUP() < fMinTotalNUP )
            {
                isCut = true;
            }
            if( eventData.GetFirstTrackTotalWideNUP() / eventData.GetFirstTrackTimeLength() < fMinAverageNUP )
            {
                isCut = true;
            }
        }
        else
        {
            KTERROR(ecnuplog, "Invalid string for fWideOrNarrow: "<< fWideOrNarrow);
        }

        data.GetCutStatus().AddCutResult< KTEventFirstTrackNUPCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
