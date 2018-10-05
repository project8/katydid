/*
 * KTTrackTimeCut.hh
 *
 *  Created on: Nov 7, 2016
 *      Author: ezayas
 */

#ifndef KTTRACKTIMECUT_HH_
#define KTTRACKTIMECUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTProcessedTrackData;

    /*
     @class KTTrackTimeCut
     @author E. Zayas

     @brief Cuts on the start time of KTProcessedTrackData

     @details
     KTProcessedTrackData objects must have the start time in a specified range to pass the cut

     Configuration name: "track-time-cut"

     Available configuration values:
     - "min-time": double -- minimum start time to accept
     - "max-time": double -- maximum start time to accept
    */

    class KTTrackTimeCut : public Nymph::KTCutOneArg< KTProcessedTrackData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTTrackTimeCut(const std::string& name = "track-time-cut");
        ~KTTrackTimeCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinStartTime);
        MEMBERVARIABLE(double, MaxStartTime);

    public:
        bool Apply(Nymph::KTData& data, KTProcessedTrackData& trackData);

    };
} // namespace Katydid

#endif /* KTTRACKTIMECUT_HH_ */
