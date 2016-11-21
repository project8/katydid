/*
 * KTTrackFreqCut.hh
 *
 *  Created on: Nov 10, 2016
 *      Author: ezayas
 */

#ifndef KTTRACKFREQCUT_HH_
#define KTTRACKFREQCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTProcessedTrackData;

    /*
     @class KTTrackFreqCut
     @author E. Zayas

     @brief Cuts on the start frequency of KTProcessedTrackData

     @details
     KTProcessedTrackData objects must have the start frequency in a specified range to pass the cut

     Configuration name: "track-frequency-cut"

     Available configuration values:
     - "min-freq": double -- minimum start frequency to accept
     - "max-freq": double -- maximum start frequency to accept
    */

    class KTTrackFreqCut : public Nymph::KTCutOneArg< KTProcessedTrackData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTTrackFreqCut(const std::string& name = "track-frequency-cut");
        ~KTTrackFreqCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinStartFreq);
        MEMBERVARIABLE(double, MaxStartFreq);

    public:
        bool Apply(Nymph::KTData& data, KTProcessedTrackData& trackData);

    };
} // namespace Katydid

#endif /* KTTRACKFREQCUT_HH_ */
