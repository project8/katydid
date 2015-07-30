/*
 * KTJSONTypeWriterEvaluation.hh
 *
 *  Created on: May 30, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONTYPEWRITEREVALUATION_HH_
#define KTJSONTYPEWRITEREVALUATION_HH_

#include "KTJSONWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    using namespace Nymph;
    class KTJSONTypeWriterEvaluation : public KTJSONTypeWriter
    {
        public:
            KTJSONTypeWriterEvaluation();
            virtual ~KTJSONTypeWriterEvaluation();

            void RegisterSlots();

        public:
            void WriteCCResults(KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTJSONTYPEWRITEREVALUATION_HH_ */
