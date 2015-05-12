/*
 * KTDataTypeDisplayEgg.hh
 *
 *  Created on: December 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDATATYPEDISPLAYEGG_HH_
#define KTDATATYPEDISPLAYEGG_HH_

#include "KTDataDisplay.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTDataTypeDisplayEgg : public KTDataTypeDisplay
    {
        public:
            KTDataTypeDisplayEgg();
            virtual ~KTDataTypeDisplayEgg();

            void RegisterSlots();

        public:
            void DrawRawTimeSeriesData(KTDataPtr data);
            void DrawTimeSeriesData(KTDataPtr data);
            void DrawTimeSeriesDataDistribution(KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTDATATYPEDISPLAYEGG_HH_ */
