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

    REGISTER_SERVICE(Katydid, KTRNGEngine, "rng-engine");

    KTRNGEngine::KTRNGEngine(const string& name) :
            Nymph::Service(name),
            fGenerator()
    {
    }

    KTRNGEngine::~KTRNGEngine()
    {
    }

    void KTRNGEngine::Configure(const scarab::param_node& node)
    {
        if (node.has("seed"))
        {
            SetSeed(node["seed"]().as_uint());
        }
        else
        {
            LWARN(rnglog, "The RNG engine <" << fName << "> is being seeded with the default value");
        }
        return;
    }

} /* namespace Katydid */
