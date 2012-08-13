/*
 * KTWaterfallCandidate.cc
 *
 *  Created on: Mar 14, 2012
 *      Author: nsoblath
 */

#include "KTWaterfallCandidate.hh"

ClassImp(Katydid::KTWaterfallCandidate);

namespace Katydid
{

    KTWaterfallCandidate::KTWaterfallCandidate() :
            TObject(),
            fData(),
            fEventNumber(0)
    {
    }

    KTWaterfallCandidate::KTWaterfallCandidate(const KTWaterfallCandidate& orig) :
            fData(orig.fData),
            fEventNumber(orig.fEventNumber)
    {
    }

    KTWaterfallCandidate::~KTWaterfallCandidate()
    {
    }

    KTWaterfallCandidate& KTWaterfallCandidate::operator=(const KTWaterfallCandidate& rhs)
    {
        fData = rhs.fData;
        fEventNumber = rhs.fEventNumber;
        return *this;
    }

} /* namespace Katydid */
