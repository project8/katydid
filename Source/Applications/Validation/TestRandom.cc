/*
 * TestRandom.cc
 *
 *  Created on: May 6, 2013
 *      Author: nsoblath
 */

#include "KTLogger.hh"
#include "KTRandom.hh"

using namespace Katydid;

KTLOGGER(vallog, "katydid.applications.validation");

int main(int argc, char** argv)
{
    KTRNGEngine* engine = KTGlobalRNGEngine::GetInstance();
    engine->SetSeed(20398);

    UInt_t nLoops = 10;

    KTRNGUniform01<> dist;

    for (UInt_t i = 0; i < nLoops; i++)
    {
        KTINFO(vallog, "loop " << i << ":  " << dist());
    }

    // test that the other distribution types compile
    KTRNGUniform<> distUniform;
    KTRNGGaussian<> distGaussian;
    KTRNGPoisson<> distPoisson;
    KTRNGExponential<> distExponential;
    KTRNGChiSquared<> distChiSquared;

    return 0;
}
