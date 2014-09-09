/*
 * KTHDF5TypeWriterEgg.hh
 *
 *  Created on: Sept 9, 2014
 *      Author: nsoblath
 */

#ifndef KTHDF5TYPEWRITEREGG_HH_
#define KTHDF5TYPEWRITEREGG_HH_

#include "KTROOTTreeWriter.hh"

#include "KTData.hh"

#include "Rtypes.h"
#include "TString.h"

class TTree;

namespace Katydid
{
    class KTEggHeader;


    class KTHDF5TypeWriterEgg : public KTROOTTreeTypeWriter
    {
        public:
            KTHDF5TypeWriterEgg();
            virtual ~KTHDF5TypeWriterEgg();

            void RegisterSlots();

        public:
            void WriteEggHeader(KTEggHeader* data);

            void WriteRawTimeSeriesData(KTDataPtr data);

            void WriteTimeSeriesData(KTDataPtr data);

        private:
            // any data structures that need to be kept in the class?
    };

} /* namespace Katydid */


#endif /* KTHDF5TYPEWRITEREGG_HH_ */
