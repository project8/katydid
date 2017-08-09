/*
 * KTROOTSpectrogramTypeWriterEventAnalysis.hh
 *
 *  Created on: Aug 8, 2017
 *      Author: nsoblath
 */

#ifndef KTROOTSPECTROGRAMTYPEWRITEREVENTANALYSIS_HH_
#define KTROOTSPECTROGRAMTYPEWRITEREVENTANALYSIS_HH_

#include "KTROOTSpectrogramWriter.hh"

class TOrdCollection;

namespace Katydid
{
    
    class KTROOTSpectrogramTypeWriterEventAnalysis : public KTROOTSpectrogramTypeWriter//, public KTTypeWriterEventAnalysis
    {
        public:
            KTROOTSpectrogramTypeWriterEventAnalysis();
            virtual ~KTROOTSpectrogramTypeWriterEventAnalysis();

            void RegisterSlots();

        public:
            void AddProcessedTrackData(Nymph::KTDataPtr data);

            void TakeLine(Nymph::KTDataPtr data);


        private:
            TOrdCollection *fLineCollection;


        public:
            void OutputSpectrograms();
            void OutputLines();
            void ClearLines();

    };


} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMTYPEWRITEREVENTANALYSIS_HH_ */
