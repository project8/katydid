/*
 * TestRandom.cc
 *
 *  Created on: May 6, 2013
 *      Author: nsoblath
 */

#include "logger.hh"
#include "KTRandom.hh"

using namespace Katydid;

LOGGER(vallog, "TestRandom");

int main()
{
    KTRNGEngine* engine = KTGlobalRNGEngine::get_instance();
    engine->SetSeed(20398);

    unsigned nLoops = 10;

    KTRNGUniform01<> dist;

    for (unsigned i = 0; i < nLoops; i++)
    {
        LINFO(vallog, "loop " << i << ":  " << dist());
    }

    // test that the other distribution types compile
    KTRNGUniform<> distUniform;
    KTRNGGaussian<> distGaussian;
    KTRNGPoisson<> distPoisson;
    KTRNGExponential<> distExponential;
    KTRNGChiSquared<> distChiSquared;

    return 0;
}
