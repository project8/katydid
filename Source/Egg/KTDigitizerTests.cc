/*
 * KTDigitizerTests.cc
 *
 *  Created on: December 17, 2013
 *      Author: N. Oblath
 */

#include "KTDigitizerTests.hh"

#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTPStoreNode.hh"

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(dtlog, "katydid.egg");

    static KTDerivedNORegistrar< KTProcessor, KTDigitizerTests > sDigTestRegistrar("digitizer-tests");

    KTDigitizerTests::KTDigitizerTests(const std::string& name) :
            KTProcessor(name),
            fNDigitizerBits(8)
    {
    }

    KTDigitizerTests::~KTDigitizerTests()
    {
    }

    Bool_t KTDigitizerTests::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        fNDigitizerBits = node->GetData< UInt_t >("n-digitizer-bits", fNDigitizerBits);

        return true;
    }


} /* namespace Katydid */
