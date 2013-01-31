/*
 * KTBasicROOTTypeWriterAnalysis.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTIEVENTROOTTYPEWRITERANALYSIS_HH_
#define KTMULTIEVENTROOTTYPEWRITERANALYSIS_HH_

#include "KTMultiEventROOTWriter.hh"

class TH1D;

namespace Katydid
{
    class KTCorrelationData;

    class KTMultiEventROOTTypeWriterAnalysis : public KTMEROOTTypeWriterBase//, public KTTypeWriterAnalysis
    {

        public:
            KTMultiEventROOTTypeWriterAnalysis();
            virtual ~KTMultiEventROOTTypeWriterAnalysis();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:

            void AddCorrelationData(const KTCorrelationData* data);

        private:
            std::vector< TH1D* > fCorrHists;

    };


} /* namespace Katydid */
#endif /* KTMULTIEVENTROOTTYPEWRITERANALYSIS_HH_ */
