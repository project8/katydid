/*
 * KTBasicROOTTypeWriterAnalysis.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTTYPEWRITERANALYSIS_HH_
#define KTMULTISLICEROOTTYPEWRITERANALYSIS_HH_

#include "KTMultiSliceROOTWriter.hh"

#include "KTData.hh"

class TH1D;

namespace Katydid
{
    class KTMultiSliceROOTTypeWriterAnalysis : public KTMEROOTTypeWriterBase//, public KTTypeWriterAnalysis
    {

        public:
            KTMultiSliceROOTTypeWriterAnalysis();
            virtual ~KTMultiSliceROOTTypeWriterAnalysis();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:

            void AddCorrelationData(KTDataPtr);

        private:
            std::vector< TH1D* > fCorrHists;

    };


} /* namespace Katydid */
#endif /* KTMULTISLICEROOTTYPEWRITERANALYSIS_HH_ */
