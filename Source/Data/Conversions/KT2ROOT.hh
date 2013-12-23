/*
 * KT2ROOT.hh
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#ifndef KT2ROOT_HH_
#define KT2ROOT_HH_

class TH1I;

namespace Katydid
{
    class KTRawTimeSeries;

    class KT2ROOT
    {
        public:
            KT2ROOT();
            virtual ~KT2ROOT();

            static TH1I* CreateHistogram(const KTRawTimeSeries* ts, const std::string& histName = "hRawTimeSeries");
            static TH1I* CreateAmplitudeDistributionHistogram(const KTRawTimeSeries* ts, const std::string& histName = "hRawTSDist");
    };

} /* namespace Katydid */
#endif /* KT2ROOT_HH_ */
