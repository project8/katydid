/*
 * KTMultiPeakTrackMultiplicityCut.hh
 *
 *  Created on: Dec 6, 2016
 *      Author: ezayas
 */

#ifndef KTMULTIPEAKTRACKMULTIPLICITYCUT_HH_
#define KTMULTIPEAKTRACKMULTIPLICITYCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiPeakTrackData;

    /*
     @class KTMultiPeakTrackMultiplicityCut
     @author E. Zayas
     @brief Cuts on the number of tracks in a MPT structure
     @details
     Specify and min and max number of tracks
     Configuration name: "mpt-multiplicity-cut"
     Available configuration values:
     - "min-mpt": int -- minimum number of tracks to accept
     - "max-mpt": int -- maximum number of tracks to accept
    */

    class KTMultiPeakTrackMultiplicityCut : public Nymph::KTCutOneArg< KTMultiPeakTrackData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTMultiPeakTrackMultiplicityCut(const std::string& name = "mpt-multiplicity-cut");
        ~KTMultiPeakTrackMultiplicityCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinMPT);
        MEMBERVARIABLE(double, MaxMPT);

    public:
        bool Apply(Nymph::KTData& data, KTMultiPeakTrackData& mptData);

    };
} // namespace Katydid

#endif /* KTMULTIPEAKTRACKMULTIPLICITYCUT_HH_ */
