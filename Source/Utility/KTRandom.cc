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
    //KTLOGGER(rnglog, "KTRandom");

    KTRNGEngine::KTRNGEngine(const string& name) :
            KTSelfConfigurable(name),
            fGenerator()
    {
    }

    KTRNGEngine::~KTRNGEngine()
    {
    }

    bool KTRNGEngine::Configure(const scarab::param_node* node)
    {
        if (node->has("seed"))
        {
            SetSeed(node->get_value< unsigned >("seed"));
        }
        else
        {
            KTWARN(rnglog, "The RNG engine <" << fConfigName << "> is being seeded with the default value");
        }
        return true;
    }

} /* namespace Katydid */
