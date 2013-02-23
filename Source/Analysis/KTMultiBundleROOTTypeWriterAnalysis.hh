/*
 * KTBasicROOTTypeWriterAnalysis.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTIBUNDLEROOTTYPEWRITERANALYSIS_HH_
#define KTMULTIBUNDLEROOTTYPEWRITERANALYSIS_HH_

#include "KTMultiBundleROOTWriter.hh"

class TH1D;

namespace Katydid
{
    class KTCorrelationData;

    class KTMultiBundleROOTTypeWriterAnalysis : public KTMEROOTTypeWriterBase//, public KTTypeWriterAnalysis
    {

        public:
            KTMultiBundleROOTTypeWriterAnalysis();
            virtual ~KTMultiBundleROOTTypeWriterAnalysis();

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
#endif /* KTMULTIBUNDLEROOTTYPEWRITERANALYSIS_HH_ */
