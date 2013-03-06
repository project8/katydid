/*
 * KTJSONTypeWriterCandidates.hh
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONTYPEWRITERCANDIDATES_HH_
#define KTJSONTYPEWRITERCANDIDATES_HH_

#include "KTJSONWriter.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    class KTData;

    class KTJSONTypeWriterCandidates : public KTJSONTypeWriter//, public KTTypeWriterCandidates
    {
        public:
            KTJSONTypeWriterCandidates();
            virtual ~KTJSONTypeWriterCandidates();

            void RegisterSlots();

        public:
            void WriteFrequencyCandidates(boost::shared_ptr< KTData > data);

    };

} /* namespace Katydid */
#endif /* KTJSONTYPEWRITERCANDIDATES_HH_ */
