/*
 * KTDataTypeDisplayTime.hh
 *
 *  Created on: December 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDATATYPEDISPLAYTIME_HH_
#define KTDATATYPEDISPLAYTIME_HH_

#include "KTDataDisplay.hh"

#include "KTData.hh"

namespace Katydid
{
    
    class KTDataTypeDisplayTime : public KTDataTypeDisplay
    {
        public:
            KTDataTypeDisplayTime();
            virtual ~KTDataTypeDisplayTime();

            void RegisterSlots();

        public:
            void DrawRawTimeSeriesData(Nymph::KTDataPtr data);
            void DrawTimeSeriesData(Nymph::KTDataPtr data);
            void DrawTimeSeriesDataDistribution(Nymph::KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTDATATYPEDISPLAYTIME_HH_ */
