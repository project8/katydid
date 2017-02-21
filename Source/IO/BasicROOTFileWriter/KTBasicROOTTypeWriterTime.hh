/*
 * KTBasicROOTTypeWriterTime.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITERTIME_HH_
#define KTBASICROOTTYPEWRITERTIME_HH_

#include "KTBasicROOTFileWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    
    class KTBasicROOTTypeWriterTime : public KTBasicROOTTypeWriter//, public KTTypeWriterTime
    {
        public:
            KTBasicROOTTypeWriterTime();
            virtual ~KTBasicROOTTypeWriterTime();

            void RegisterSlots();

        public:
            void WriteRawTimeSeriesData(Nymph::KTDataPtr data);
            //void WriteRawTimeSeriesDataDistribution(Nymph::KTDataPtr data);

            void WriteTimeSeriesData(Nymph::KTDataPtr data);
            void WriteTimeSeriesDataDistribution(Nymph::KTDataPtr data);

            void WriteTimeSeriesFFTWData(Nymph::KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTBASICROOTFILETYPEWRITERTIME_HH_ */
