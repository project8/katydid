/*
 * KTEventFirstTrackNUPCut.hh
 *
 *  Created on: June 14, 2018
 *      Author: C. Claessens
 */

#ifndef KTEVENTFIRSTTRACKNUPCUT_HH_
#define KTEVENTFIRSTTRACKNUPCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTMultiTrackEventData;

    /*
     @class KTEventFirstTrackNUPCut
     @author C. Claessens

     @brief Cut on the total NUP and the NUP per unit length of the first track in KTMultiTrackEventData

     @details


     Configuration name: "event-first-track-nup-cut"

     Available configuration values:
     - "min-average-nup": double -- minimum nup per unit length in the first track for the event to pass the cut
     - "min-total-nup": double -- minimum total nup in the first track for the event to pass the cut
     - "wide-or-narrow": string -- decides whether to use "wide" NUP or "narrow" NUP (default: "wide")
     - "time-or-bin-average": string -- decides whether to divide total NUP by track time length or track NTrackBins
    */

    class KTEventFirstTrackNUPCut : public Nymph::KTCutOneArg< KTMultiTrackEventData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    private:
        enum class wide_or_narrow
        {
            wide,
            narrow
        };
        enum class time_or_bin_average
        {
            time,
            bin
        };

    public:
        KTEventFirstTrackNUPCut(const std::string& name = "event-first-track-nup-cut");
        ~KTEventFirstTrackNUPCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinTotalNUP);
        MEMBERVARIABLE(double, MinAverageNUP);
        MEMBERVARIABLE(wide_or_narrow, WideOrNarrow);
        MEMBERVARIABLE(time_or_bin_average, TimeOrBinAverage);

    public:
        bool Apply(Nymph::KTData& data, KTMultiTrackEventData& eventData);

    };
} // namespace Katydid

#endif /* KTEVENTFIRSTTRACKNUPCUT_HH_ */
