/*
 * KTTrackMainbandCut.hh
 *
 *  Created on: Dec 14, 2016
 *      Author: ezayas
 */

#ifndef KTTRACKMAINBANDCUT_HH_
#define KTTRACKMAINBANDCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTProcessedTrackData;

    /*
     @class KTTrackMainbandCut
     @author E. Zayas

     @brief Isolates mainband tracks

     @details
     KTProcessedTrackData objects must have fMainband==true to pass
     Tracks identified by KTTrackClassifier as signals will pass, and sidebands will fail

     Configuration name: "track-mainband-cut"

     Available configuration values:
     (none)
    */

    class KTTrackMainbandCut : public Nymph::KTCutOneArg< KTProcessedTrackData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTTrackMainbandCut(const std::string& name = "track-mainband-cut");
        ~KTTrackMainbandCut();

        bool Configure(const scarab::param_node* node);

    public:
        bool Apply(Nymph::KTData& data, KTProcessedTrackData& trackData);

    };
} // namespace Katydid

#endif /* KTTRACKMAINBANDCUT_HH_ */
