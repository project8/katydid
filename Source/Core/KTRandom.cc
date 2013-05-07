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

} /* namespace Katydid */
