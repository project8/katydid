/*
 * KTProcSummary.hh
 *
 *  Created on: May 9, 2013
 *      Author: nsoblath
 */

#ifndef KTPROCSUMMARY_HH_
#define KTPROCSUMMARY_HH_

#include "KTMemberVariable.hh"

#include <string>

namespace Katydid
{
    class KTProcSummary
    {
        public:
            KTProcSummary();
            KTProcSummary(const KTProcSummary& orig);
            ~KTProcSummary();

            KTProcSummary& operator=(const KTProcSummary& rhs);

        public:
            MEMBERVARIABLE(unsigned, NSlicesProcessed);
            MEMBERVARIABLE(unsigned, NRecordsProcessed); /// if any samples from a record were used, it's counted
            MEMBERVARIABLE(unsigned, IntegratedTime); /// # of slices * slice size * bin width
    };

} /* namespace Katydid */
#endif /* KTPROCSUMMARY_HH_ */
