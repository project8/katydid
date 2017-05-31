/*
 * KTEventTimeCut.cc
 *
 *  Created on: Nov 7, 2016
 *      Author: ezayas
 */

#include "KTEventTimeCut.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(exlog, "KTEventTimeCut");

    const std::string KTEventTimeCut::Result::sName = "event-time-cut";

    KT_REGISTER_CUT(KTEventTimeCut);

    KTEventTimeCut::KTEventTimeCut(const std::string& name) :
         KTCutOneArg(name),
         fMinStartTime(0.9e-3),
         fMaxStartTime(1.2e-3)
    {}

    KTEventTimeCut::~KTEventTimeCut()
    {}

    bool KTEventTimeCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinStartTime( node->get_value< double >( "min-time", GetMinStartTime() ) );
        SetMaxStartTime( node->get_value< double >( "max-time", GetMaxStartTime() ) );

        return true;
    }

    bool KTEventTimeCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;

        if( eventData.GetStartTimeInRunC() < GetMinStartTime() )
        {
            isCut = true;
        }
        if( eventData.GetStartTimeInRunC() > GetMaxStartTime() )
        {
            isCut = true;
        }
        
        data.CutStatus().template AddCutResult< KTEventTimeCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
