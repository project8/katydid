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

    class KTBasicROOTTypeWriterEgg : public KTBasicROOTTypeWriter
    {
        public:
            KTBasicROOTTypeWriterEgg();
            virtual ~KTBasicROOTTypeWriterEgg();

            //Bool_t Configure(const KTPStoreNode* node);

            void RegisterSlots();

        public:
            void Write(const KTTimeSeriesData* data);

    };

    inline void KTBasicROOTTypeWriterEgg::RegisterSlots()
    {
        fFileWriter->RegisterSlot("ts-data", this, &KTBasicROOTTypeWriterEgg::Write, "void (const KTTimeSeriesData*)");
        return;
    }

} /* namespace Katydid */
#endif /* KTBASICROOTFILETYPEWRITER_HH_ */
