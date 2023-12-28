/*
 * KTTrackMVACut.cc
 *
 *  Created on: Dec 14, 2016
 *      Author: ezayas
 */

#include "KTTrackMVACut.hh"
#include "KTProcessedTrackData.hh"

#include "logger.hh"

namespace Katydid
{
    LOGGER(exlog, "KTTrackMVACut");

    const std::string KTTrackMVACut::Result::sName = "track-mva-cut";

    KT_REGISTER_CUT(KTTrackMVACut);

    KTTrackMVACut::KTTrackMVACut(const std::string& name) :
         KTCutOneArg(name),
         fMinMVAClassifier(-999.),
         fMaxMVAClassifier(999.)
    {}

    KTTrackMVACut::~KTTrackMVACut()
    {}

    bool KTTrackMVACut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinMVAClassifier( node->get_value< double >( "min-mva", GetMinMVAClassifier() ) );
        SetMaxMVAClassifier( node->get_value< double >( "max-mva", GetMaxMVAClassifier() ) );

        return true;
    }

    bool KTTrackMVACut::Apply( Nymph::KTData& data, KTProcessedTrackData& trackData )
    {
        bool isCut = false;

        if( trackData.GetMVAClassifier() < GetMinMVAClassifier() )
        {
            isCut = true;
        }
        if( trackData.GetMVAClassifier() > GetMaxMVAClassifier() )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTTrackMVACut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
