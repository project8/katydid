/*
 * KTProcSummary.cc
 *
 *  Created on: May 9, 2013
 *      Author: nsoblath
 */

#include "KTProcSummary.hh"

namespace Katydid
{

    KTProcSummary::KTProcSummary() :
            fNSlicesProcessed(0),
            fNRecordsProcessed(0),
            fIntegratedTime(0.)
    {
    }

    KTProcSummary::KTProcSummary(const KTProcSummary& orig) :
            fNSlicesProcessed(orig.fNSlicesProcessed),
            fNRecordsProcessed(orig.fNRecordsProcessed),
            fIntegratedTime(orig.fIntegratedTime)
    {
    }

    KTProcSummary::~KTProcSummary()
    {
    }

    KTProcSummary& KTProcSummary::operator=(const KTProcSummary& rhs)
    {
        fNRecordsProcessed = rhs.fNSlicesProcessed;
        fNSlicesProcessed = rhs.fNSlicesProcessed;
        fIntegratedTime = rhs.fIntegratedTime;
        return *this;
    }

} /* namespace Katydid */
