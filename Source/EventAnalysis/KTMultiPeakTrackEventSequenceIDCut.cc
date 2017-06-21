/*
 * KTMultiPeakTrackEventSequenceIDCut.cc
 *
 *  Created on: Dec 7, 2016
 *      Author: ezayas
 */

#include "KTMultiPeakTrackEventSequenceIDCut.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(exlog, "KTMultiPeakTrackEventSequenceIDCut");

    const std::string KTMultiPeakTrackEventSequenceIDCut::Result::sName = "mpt-event-sequence-cut";

    KT_REGISTER_CUT(KTMultiPeakTrackEventSequenceIDCut);

    KTMultiPeakTrackEventSequenceIDCut::KTMultiPeakTrackEventSequenceIDCut(const std::string& name) :
         KTCutOneArg(name),
         fMinID(0),
         fMaxID(10)
    {}

    KTMultiPeakTrackEventSequenceIDCut::~KTMultiPeakTrackEventSequenceIDCut()
    {}

    bool KTMultiPeakTrackEventSequenceIDCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinID( node->get_value< int >( "min-id", GetMinID() ) );
        SetMaxID( node->get_value< int >( "max-id", GetMaxID() ) );

        return true;
    }

    bool KTMultiPeakTrackEventSequenceIDCut::Apply( Nymph::KTData& data, KTMultiPeakTrackData& mptData )
    {
        bool isCut = false;

        if( mptData.GetEventSequenceID() < GetMinID() )
        {
            isCut = true;
        }
        if( mptData.GetEventSequenceID() > GetMaxID() )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTMultiPeakTrackEventSequenceIDCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
