/*
 * KTSequentialLineNUPCut.hh
 *
 *  Created on: June 13, 2018
 *      Author: C. Claessens
 */

#ifndef KTSEQUENTIALLINENUPCUT_HH_
#define KTSEQUENTIALLINENUPCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTSequentialLineData;

    /*
     @class KTSequentialLineNUPCut
     @author C. Claessens

     @brief Cuts on total and average NUP of KTSequentialLineData

     @details
     KTSequentialLineData objects must have an NUP and average NUP above a set value to pass the cut

     Configuration name: "sq-line-nup-cut"

     Available configuration values:
     - "min-total-nup": double -- minimum summed NUP to accept
     - "min-average-nup": double -- minimum average NUP to accept
     - "wide-or-narrow": string -- decides whether to use "wide" NUP or "narrow" NUP (default: "wide")
     - "time-or-bin-average": string -- decides whether to divide total NUP by track time length or track NTrackBins
    */

    class KTSequentialLineNUPCut : public Nymph::KTCutOneArg< KTSequentialLineData >
    {

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
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTSequentialLineNUPCut(const std::string& name = "seq-line-nup-cut");
        ~KTSequentialLineNUPCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinTotalNUP);
        MEMBERVARIABLE(double, MinAverageNUP);
        MEMBERVARIABLE(wide_or_narrow, WideOrNarrowLine);
        MEMBERVARIABLE(time_or_bin_average, TimeOrBinAverage);

    public:
        bool Apply(Nymph::KTData& data, KTSequentialLineData& seqLineData);

    };
} // namespace Katydid

#endif /* KTSequentialLineNUPCut_HH_ */
