/*
 * KTProcessedTrackCut.cc
 *
 *  Created on: Nov 2, 2016
 *      Author: ezayas
 */

#include "KTProcessedTrackCut.hh"
#include "KTProcessedTrackData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(exlog, "KTProcessedTrackCut");

    const std::string KTProcessedTrackCut::Result::sName = "track-cut";

    KT_REGISTER_CUT(KTProcessedTrackCut);

    KTProcessedTrackCut::KTProcessedTrackCut(const std::string& name) :
         KTCutOneArg(name)
    {}

    KTProcessedTrackCut::~KTProcessedTrackCut()
    {}

    bool KTProcessedTrackCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        return true;
    }

    bool KTProcessedTrackCut::Apply( Nymph::KTData& data, KTProcessedTrackData& trackData )
    {
        bool isCut = trackData.GetIsCut();
        data.GetCutStatus().AddCutResult< KTProcessedTrackCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
