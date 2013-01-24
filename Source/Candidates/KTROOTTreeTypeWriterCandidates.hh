/*
 * KTROOTTreeTypeWriterCandidates.hh
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERCANDIDATES_HH_
#define KTROOTTREETYPEWRITERCANDIDATES_HH_

#include "KTJSONWriter.hh"

namespace Katydid
{
    class KTFrequencyCandidateData;

    class KTROOTTreeTypeWriterCandidates : public KTJSONTypeWriter//, public KTTypeWriterCandidates
    {
        public:
            KTROOTTreeTypeWriterCandidates();
            virtual ~KTROOTTreeTypeWriterCandidates();

            void RegisterSlots();

        public:
            void WriteFrequencyCandidates(const KTFrequencyCandidateData* header);

    };

} /* namespace Katydid */
#endif /* KTROOTTREETYPEWRITERCANDIDATES_HH_ */
