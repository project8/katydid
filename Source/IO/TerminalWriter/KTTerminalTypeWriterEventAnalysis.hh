/*
 * KTTerminalTypeWriterEventAnalysis.hh
 *
 *  Created on: Oct 6, 2016
 *      Author: ezayas
 */

#ifndef KTTERMINALTYPEWRITEREVENTANALYSIS_HH_
#define KTTERMINALTYPEWRITEREVENTANALYSIS_HH_

#include "KTTerminalWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    
    class KTProcSummary;

    class KTTerminalTypeWriterEventAnalysis : public KTTerminalTypeWriter
    {
        public:
            KTTerminalTypeWriterEventAnalysis();
            virtual ~KTTerminalTypeWriterEventAnalysis();

            void RegisterSlots();

        public:
            void WriteProcessedTrackData(Nymph::KTDataPtr data);
            void WriteLinearFitData(Nymph::KTDataPtr data);
    };

} /* namespace Katydid */
#endif /* KTTERMINALTYPEWRITEREVENTANALYSIS_HH_ */
