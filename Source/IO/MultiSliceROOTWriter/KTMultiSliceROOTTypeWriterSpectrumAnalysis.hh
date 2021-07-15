/*
 * KTBasicROOTTypeWriterSpectrumAnalysis.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTTYPEWRITERSPECTRUMANALYSIS_HH_
#define KTMULTISLICEROOTTYPEWRITERSPECTRUMANALYSIS_HH_

#include "KTMultiSliceROOTWriter.hh"

#include "KTData.hh"

class TH1D;

namespace Katydid
{
    
    class KTMultiSliceROOTTypeWriterSpectrumAnalysis : public KTMEROOTTypeWriterBase//, public KTTypeWriterSpectrumAnalysis
    {

        public:
            KTMultiSliceROOTTypeWriterSpectrumAnalysis();
            virtual ~KTMultiSliceROOTTypeWriterSpectrumAnalysis();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:

            void AddCorrelationData(Nymph::KTDataPtr);
      
            void WriteMFScores(Nymph::KTDataPtr);

        private:
            std::vector< TH1D* > fCorrHists;

    };


} /* namespace Katydid */
#endif /* KTMULTISLICEROOTTYPEWRITERSPECTRUMANALYSIS_HH_ */
