/*
 * KTTrackFreqCut.cc
 *
 *  Created on: Nov 10, 2016
 *      Author: ezayas
 */

#include "KTTrackFreqCut.hh"
#include "KTProcessedTrackData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(exlog, "KTTrackFreqCut");

    const std::string KTTrackFreqCut::Result::sName = "track-freq-cut";

    KT_REGISTER_CUT(KTTrackFreqCut);

    KTTrackFreqCut::KTTrackFreqCut(const std::string& name) :
         KTCutOneArg(name),
         fMinStartFreq(0.9e-3),
         fMaxStartFreq(1.2e-3)
    {}

    KTTrackFreqCut::~KTTrackFreqCut()
    {}

    bool KTTrackFreqCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinStartFreq( node->get_value< double >( "min-freq", GetMinStartFreq() ) );
        SetMaxStartFreq( node->get_value< double >( "max-freq", GetMaxStartFreq() ) );

        return true;
    }

    bool KTTrackFreqCut::Apply( Nymph::KTData& data, KTProcessedTrackData& trackData )
    {
        bool isCut = false;

        if( trackData.GetStartFrequency() < GetMinStartFreq() )
        {
            isCut = true;
        }
        if( trackData.GetStartFrequency() > GetMaxStartFreq() )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTTrackFreqCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
