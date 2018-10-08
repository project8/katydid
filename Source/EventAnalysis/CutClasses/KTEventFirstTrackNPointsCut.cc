/*
 * KTEventFirstTrackNPointsCut.cc
 *
 *  Created on: October 8, 2018
 *      Author: C. Claessens
 */

#include "KTEventFirstTrackNPointsCut.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(ecnuplog, "KTEventFirstTrackNPointsCut");

    const std::string KTEventFirstTrackNPointsCut::Result::sName = "event-first-track-npoints-cut";

    KT_REGISTER_CUT(KTEventFirstTrackNPointsCut);

    KTEventFirstTrackNPointsCut::KTEventFirstTrackNPointsCut(const std::string& name) :
         KTCutOneArg(name),
         fMinPoints(0),
         fMaxTracks(1)
    {}

    KTEventFirstTrackNPointsCut::~KTEventFirstTrackNPointsCut()
    {}

    bool KTEventFirstTrackNPointsCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinPoints( node->get_value< unsigned >( "min-points", GetMinPoints() ) );
        SetMaxTracks( node->get_value< unsigned >( "n-tracks-in-event", GetMaxTracks() ) );

        return true;
    }

    bool KTEventFirstTrackNPointsCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;
        if( eventData.GetTotalEventSequences() <= fMaxTracks and eventData.GetFirstTrackNTrackBins() < fMinPoints )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTEventFirstTrackNPointsCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
