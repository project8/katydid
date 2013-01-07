/*
 * KTJSONTypeWriterCandidates.hh
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONTYPEWRITERCANDIDATES_HH_
#define KTJSONTYPEWRITERCANDIDATES_HH_

#include "KTJSONWriter.hh"

namespace Katydid
{
    class KTFrequencyCandidateData;

    class KTJSONTypeWriterCandidates : public KTJSONTypeWriter//, public KTTypeWriterCandidates
    {
        public:
            KTJSONTypeWriterCandidates();
            virtual ~KTJSONTypeWriterCandidates();

            void RegisterSlots();

        public:
            void WriteFrequencyCandidates(const KTFrequencyCandidateData* header);

    };

} /* namespace Katydid */
#endif /* KTJSONTYPEWRITERCANDIDATES_HH_ */
