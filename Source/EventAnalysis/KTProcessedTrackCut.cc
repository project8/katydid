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
         KTCut(name)
    {}

    KTProcessedTrackCut::~KTProcessedTrackCut()
    {}

    bool KTProcessedTrackCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        return true;
    }

    bool KTProcessedTrackCut::PerformCut( Nymph::KTData& data, KTProcessedTrackData& trackData )
    {
        bool isCut = trackData.GetIsCut();
        data.GetCutStatus().AddCutResult< KTProcessedTrackCut::Result >(isCut);

        return isCut;
    }

    bool KTProcessedTrackCut::Apply( Nymph::KTDataPtr dataPtr )
    {
        if (! dataPtr->Has< KTProcessedTrackData >())
        {
            KTERROR(exlog, "Data type <KTProcessedTrackData> was not present");
            return false;
        }

        return PerformCut(dataPtr->Of< Nymph::KTData >(), dataPtr->Of< KTProcessedTrackData >());
    }
}
