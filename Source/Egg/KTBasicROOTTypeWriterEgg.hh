/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITEREGG_HH_
#define KTBASICROOTTYPEWRITEREGG_HH_

#include "KTBasicROOTFileWriter.hh"

namespace Katydid
{
    class KTTimeSeriesData;
/*
    class KTTypeWriterEgg
    {
        public:
            KTTypeWriterEgg();
            virtual ~KTTypeWriterEgg();

        protected:
            virtual void WriteTimeSeriesDataByVisitor(const KTWriteableData* data) = 0;
    };
*/

    class KTBasicROOTTypeWriterEgg : public KTBasicROOTTypeWriter//, public KTTypeWriterEgg
    {
        public:
            friend class KTTimeSeriesData;

        public:
            KTBasicROOTTypeWriterEgg();
            virtual ~KTBasicROOTTypeWriterEgg();

            void RegisterSlots();

        public:
            void WriteTimeSeriesData(const KTTimeSeriesData* data);

        //protected:
            //void WriteTimeSeriesDataByVisitor(const KTWriteableData* data);

    };

} /* namespace Katydid */
#endif /* KTBASICROOTFILETYPEWRITER_HH_ */
