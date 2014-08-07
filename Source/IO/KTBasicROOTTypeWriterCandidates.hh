/*
 * KTBasicROOTTypeWriterCandidates.hh
 *
 *  Created on: Jul 29, 2014
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITERCANDIDATES_HH_
#define KTBASICROOTTYPEWRITERCANDIDATES_HH_

#include "KTBasicROOTFileWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTBasicROOTTypeWriterCandidates : public KTBasicROOTTypeWriter
    {
        public:
            KTBasicROOTTypeWriterCandidates();
            virtual ~KTBasicROOTTypeWriterCandidates();

            void RegisterSlots();


            //************************
            // Processed Track & Sparse Waterfall Candidate
            //************************
        public:
            void WriteProcTrackAndSWFC(KTDataPtr data);


    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITERCANDIDATES_HH_ */
