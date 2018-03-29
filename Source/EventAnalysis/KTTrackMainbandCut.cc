/*
 * KTTrackMainbandCut.cc
 *
 *  Created on: Dec 14, 2016
 *      Author: ezayas
 */

#include "KTTrackMainbandCut.hh"
#include "KTProcessedTrackData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(exlog, "KTTrackMainbandCut");

    const std::string KTTrackMainbandCut::Result::sName = "track-mainband-cut";

    KT_REGISTER_CUT(KTTrackMainbandCut);

    KTTrackMainbandCut::KTTrackMainbandCut(const std::string& name) :
         KTCutOneArg(name)
    {}

    KTTrackMainbandCut::~KTTrackMainbandCut()
    {}

    bool KTTrackMainbandCut::Configure(const scarab::param_node* node)
    {
        return true;
    }

    bool KTTrackMainbandCut::Apply( Nymph::KTData& data, KTProcessedTrackData& trackData )
    {
        bool isCut = ! trackData.GetMainband();

        data.GetCutStatus().AddCutResult< KTTrackMainbandCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
