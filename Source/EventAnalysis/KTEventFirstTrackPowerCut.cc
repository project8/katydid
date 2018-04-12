/*
 * KTEventFirstTrackPowerCut.cc
 *
 *  Created on: Apr 12, 2017
 *      Author: N.S. Oblath
 */

#include "KTEventFirstTrackPowerCut.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(exlog, "KTEventFitTrackPowerCut");

    const std::string KTEventFirstTrackPowerCut::Result::sName = "event-time-cut";

    KT_REGISTER_CUT(KTEventFirstTrackPowerCut);

    KTEventFirstTrackPowerCut::KTEventFirstTrackPowerCut(const std::string& name) :
         KTCutOneArg(name),
         fMinPower(0.9e-3)
    {}

    KTEventFirstTrackPowerCut::~KTEventFirstTrackPowerCut()
    {}

    bool KTEventFirstTrackPowerCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinPower( node->get_value< double >( "min-power", GetMinPower() ) );

        return true;
    }

    bool KTEventFirstTrackPowerCut::Apply( Nymph::KTData& data, KTMultiTrackEventData& eventData )
    {
        bool isCut = false;

        if( eventData.GetFirstTrackTotalPower() / eventData.GetFirstTrackTimeLength() < fMinPower )
        {
            isCut = true;
        }
        
        data.GetCutStatus().AddCutResult< KTEventFirstTrackPowerCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
