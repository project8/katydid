/*
 * KTEventTimeInAcqCut.cc
 *
 *  Created on: Jul 10, 2018
 *      Author: enovitski and cclaessens
 */

#include "KTEventTimeInAcqCut.hh"
#include "KTMultiTrackEventData.hh"

#include "logger.hh"

namespace Katydid
{
    LOGGER(exlog, "KTEventTimeInAcqCut");

    const std::string KTEventTimeInAcqCut::Result::sName = "event-time-in-acq-cut";

    KT_REGISTER_CUT(KTEventTimeInAcqCut);

    KTEventTimeInAcqCut::KTEventTimeInAcqCut(const std::string& name) :
         KTCutOneArg(name),
         fMinStartTime(0.9e-3),
         fMaxStartTime(1.2e-3)
    {}

    KTEventTimeInAcqCut::~KTEventTimeInAcqCut()
    {}

    bool KTEventTimeInAcqCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinStartTime( node->get_value< double >( "min-time", GetMinStartTime() ) );
        SetMaxStartTime( node->get_value< double >( "max-time", GetMaxStartTime() ) );

        return true;
    }

    bool KTEventTimeInAcqCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;

        if( eventData.GetStartTimeInAcq() < GetMinStartTime() )
        {
            isCut = true;
        }
        if( eventData.GetStartTimeInAcq() > GetMaxStartTime() )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTEventTimeInAcqCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
