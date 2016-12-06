/*
 * KTMultiPeakTrackMultiplicityCut.cc
 *
 *  Created on: Nov 10, 2016
 *      Author: ezayas
 */

#include "KTMultiPeakTrackMultiplicityCut.hh"
#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(exlog, "KTMultiPeakTrackMultiplicityCut");

    const std::string KTMultiPeakTrackMultiplicityCut::Result::sName = "mpt-multiplicity-cut";

    KT_REGISTER_CUT(KTMultiPeakTrackMultiplicityCut);

    KTMultiPeakTrackMultiplicityCut::KTMultiPeakTrackMultiplicityCut(const std::string& name) :
         KTCutOneArg(name),
         fMinMPT(0),
         fMaxMPT(10)
    {}

    KTMultiPeakTrackMultiplicityCut::~KTMultiPeakTrackMultiplicityCut()
    {}

    bool KTMultiPeakTrackMultiplicityCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinMPT( node->get_value< int >( "min-mpt", GetMinMPT() ) );
        SetMaxMPT( node->get_value< int >( "max-mpt", GetMaxMPT() ) );

        return true;
    }

    bool KTMultiPeakTrackMultiplicityCut::Apply( Nymph::KTData& data, KTMultiPeakTrackData& mptData )
    {
        bool isCut = false;

        if( mptData.GetMultiplicity() < GetMinMPT() )
        {
            isCut = true;
        }
        if( mptData.GetMultiplicity() > GetMaxMPT() )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTMultiPeakTrackMultiplicityCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
