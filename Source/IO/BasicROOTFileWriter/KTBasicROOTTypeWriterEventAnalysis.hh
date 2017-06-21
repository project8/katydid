/*
 * KTBasicROOTTypeWriterEventAnalysis.hh
 *
 *  Created on: Jul 29, 2014
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITEREVENTANALYSIS_HH_
#define KTBASICROOTTYPEWRITEREVENTANALYSIS_HH_

#include "KTBasicROOTFileWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    
    class KTBasicROOTTypeWriterEventAnalysis : public KTBasicROOTTypeWriter
    {
        public:
            KTBasicROOTTypeWriterEventAnalysis();
            virtual ~KTBasicROOTTypeWriterEventAnalysis();

            void RegisterSlots();


            //************************
            // Processed Track & Sparse Waterfall Candidate
            //************************
        public:
            void WriteProcTrackAndSWFC(Nymph::KTDataPtr data);


    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITEREVENTANALYSIS_HH_ */
