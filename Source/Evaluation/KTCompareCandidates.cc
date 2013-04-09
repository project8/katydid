/*
 * KTCompareCandidates.cc
 *
 *  Created on: Apr 9, 2013
 *      Author: nsoblath
 */

#include "KTCompareCandidates.hh"

#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTCompareCandidates > sCompCandRegistrar("compare-candidates");

    KTCompareCandidates::KTCompareCandidates(const string& name) :
            KTProcessor(name)
    {
    }

    KTCompareCandidates::~KTCompareCandidates()
    {
    }

    Bool_t KTCompareCandidates::Configure(const KTPStoreNode* node)
    {
        return true;
    }

} /* namespace Katydid */
