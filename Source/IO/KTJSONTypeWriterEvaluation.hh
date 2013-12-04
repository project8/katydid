/*
 * KTJSONTypeWriterEvaluation.hh
 *
 *  Created on: May 30, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONTYPEWRITEREVALUATION_HH_
#define KTJSONTYPEWRITEREVALUATION_HH_

#include "KTJSONWriter.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    struct KTData;

    class KTJSONTypeWriterEvaluation : public KTJSONTypeWriter
    {
        public:
            KTJSONTypeWriterEvaluation();
            virtual ~KTJSONTypeWriterEvaluation();

            void RegisterSlots();

        public:
            void WriteCCResults(boost::shared_ptr< KTData > data);

    };

} /* namespace Katydid */
#endif /* KTJSONTYPEWRITEREVALUATION_HH_ */
