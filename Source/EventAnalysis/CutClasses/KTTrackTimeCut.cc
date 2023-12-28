/*
 * KTTrackTimeCut.cc
 *
 *  Created on: Nov 7, 2016
 *      Author: ezayas
 */

#include "KTTrackTimeCut.hh"
#include "KTProcessedTrackData.hh"

#include "logger.hh"

namespace Katydid
{
    LOGGER(exlog, "KTTrackTimeCut");

    const std::string KTTrackTimeCut::Result::sName = "track-time-cut";

    KT_REGISTER_CUT(KTTrackTimeCut);

    KTTrackTimeCut::KTTrackTimeCut(const std::string& name) :
         KTCutOneArg(name),
         fMinStartTime(0.9e-3),
         fMaxStartTime(1.2e-3)
    {}

    KTTrackTimeCut::~KTTrackTimeCut()
    {}

    bool KTTrackTimeCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinStartTime( node->get_value< double >( "min-time", GetMinStartTime() ) );
        SetMaxStartTime( node->get_value< double >( "max-time", GetMaxStartTime() ) );

        return true;
    }

    bool KTTrackTimeCut::Apply( Nymph::KTData& data, KTProcessedTrackData& trackData )
    {
        bool isCut = false;

        if( trackData.GetStartTimeInRunC() < GetMinStartTime() )
        {
            isCut = true;
        }
        if( trackData.GetStartTimeInRunC() > GetMaxStartTime() )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTTrackTimeCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
