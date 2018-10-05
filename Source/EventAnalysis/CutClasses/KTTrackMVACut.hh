/*
 * KTTrackMVACut.hh
 *
 *  Created on: Dec 14, 2016
 *      Author: ezayas
 */

#ifndef KTTRACKMVACUT_HH_
#define KTTRACKMVACUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTProcessedTrackData;

    /*
     @class KTTrackMVACut
     @author E. Zayas

     @brief Cuts on the MVA classifier value of KTProcessedTrackData

     @details
     KTProcessedTrackData objects must have the MVA result in a specified range to pass the cut

     Configuration name: "track-mva-cut"

     Available configuration values:
     - "min-mva": double -- minimum MVA classifier value to accept
     - "max-mva": double -- maximum MVA classifier value to accept
    */

    class KTTrackMVACut : public Nymph::KTCutOneArg< KTProcessedTrackData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTTrackMVACut(const std::string& name = "track-mva-cut");
        ~KTTrackMVACut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinMVAClassifier);
        MEMBERVARIABLE(double, MaxMVAClassifier);

    public:
        bool Apply(Nymph::KTData& data, KTProcessedTrackData& trackData);

    };
} // namespace Katydid

#endif /* KTTRACKMVACUT_HH_ */
