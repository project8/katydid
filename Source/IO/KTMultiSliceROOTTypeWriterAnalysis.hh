/*
 * KTBasicROOTTypeWriterAnalysis.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTTYPEWRITERANALYSIS_HH_
#define KTMULTISLICEROOTTYPEWRITERANALYSIS_HH_

#include "KTMultiSliceROOTWriter.hh"

#include <boost/shared_ptr.hpp>

class TH1D;

namespace Katydid
{
    struct KTData;

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

            void AddCorrelationData(boost::shared_ptr< KTData >);

        private:
            std::vector< TH1D* > fCorrHists;

    };


} /* namespace Katydid */
#endif /* KTMULTISLICEROOTTYPEWRITERANALYSIS_HH_ */
