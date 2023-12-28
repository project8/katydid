/*
 * KTRandom.cc
 *
 *  Created on: May 6, 2013
 *      Author: nsoblath
 */

#include "KTRandom.hh"

//#include "KTFactory.hh"
//#include "logger.hh"

using std::string;

namespace Katydid
{
    //LOGGER(rnglog, "KTRandom");

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
            LWARN(rnglog, "The RNG engine <" << fConfigName << "> is being seeded with the default value");
        }
        return true;
    }

} /* namespace Katydid */
