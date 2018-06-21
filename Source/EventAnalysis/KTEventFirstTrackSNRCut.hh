/*
 * KTEventFirstTrackSNRCut.hh
 *
 *  Created on: June 14, 2018
 *      Author: C. Claessens
 */

#ifndef KTEVENTFIRSTTRACKSNRCUT_HH_
#define KTEVENTFIRSTTRACKSNRCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventFirstTrackSNRCut
     @author C. Claessens

     @brief Cut on the total SNR and the SNR per unit length of the first track in KTMultiTrackEventData

     @details


     Configuration name: "event-first-track-snr-cut"

     Available configuration values:
     - "min-average-snr": double -- minimum snr per unit length in the first track for the event to pass the cut
     - "min-total-snr": double -- minimum total snr in the first track for the event to pass the cut
    */

    class KTEventFirstTrackSNRCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTEventFirstTrackSNRCut(const std::string& name = "event-first-track-snr-cut");
        ~KTEventFirstTrackSNRCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinTotalSNR);
        MEMBERVARIABLE(double, MinAverageSNR);

    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEVENTFIRSTTRACKSNRCUT_HH_ */
