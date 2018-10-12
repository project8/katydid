/*
 * KTSequentialLineSNRCut.hh
 *
 *  Created on: June 13, 2018
 *      Author: C. Claessens
 */

#ifndef KTSEQUENTIALLINESNRCUT_HH_
#define KTSEQUENTIALLINESNRCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTSequentialLineData;

    /*
     @class KTSequentialLineSNRCut
     @author C. Claessens

     @brief Cuts on total and average SNR of KTSequentialLineData

     @details
     KTSequentialLineData objects must have an SNR and average SNR above a set value to pass the cut

     Configuration name: "sq-line-snr-cut"

     Available configuration values:
     - "min-total-snr": double -- minimum summed SNR to accept
     - "min- average-snr": double -- minimum average SNR to accept
     - "wide-or-narrow": string -- decides whether to use "wide" SNR or "narrow" SNR (default: "wide")
     - "time-or-bin-average": string -- decides whether to divide total NUP by track time length or track NTrackBins
    */

    class KTSequentialLineSNRCut : public Nymph::KTCutOneArg< KTSequentialLineData >
    {

    public:
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
        KTSequentialLineSNRCut(const std::string& name = "seq-line-snr-cut");
        ~KTSequentialLineSNRCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinTotalSNR);
        MEMBERVARIABLE(double, MinAverageSNR);
        MEMBERVARIABLE(wide_or_narrow, WideOrNarrowLine);
        MEMBERVARIABLE(time_or_bin_average, TimeOrBinAverage);

    public:
        bool Apply(Nymph::KTData& data, KTSequentialLineData& seqLineData);

    };
} // namespace Katydid

#endif /* KTSequentialLineSNRCut_HH_ */
