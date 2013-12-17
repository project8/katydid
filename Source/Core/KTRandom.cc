/*
 * KTRandom.cc
 *
 *  Created on: May 6, 2013
 *      Author: nsoblath
 */

#include "KTRandom.hh"

#include "KTFactory.hh"
//#include "KTLogger.hh"

using std::string;

namespace Katydid
{
    //KTLOGGER(rnglog, "katydid.core");

    KTRNGEngine::KTRNGEngine(const string& name) :
            KTSelfConfigurable(name),
            fGenerator()
    {
    }

    KTRNGEngine::~KTRNGEngine()
    {
    }

    Bool_t KTRNGEngine::Configure(const KTPStoreNode* node)
    {
        if (node->HasData("seed"))
        {
            SetSeed(node->GetData< unsigned >("seed"));
        }
        else
        {
            KTWARN(rnglog, "The RNG engine <" << fConfigName << "> is being seeded with the default value");
        }
        return true;
    }

} /* namespace Katydid */
