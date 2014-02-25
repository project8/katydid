/*
 * KTPrimaryProcessor.cc
 *
 *  Created on: Oct 10, 2012
 *      Author: nsoblath
 */

#include "KTPrimaryProcessor.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(proclog, "KTPrimaryProcessor");

    KTPrimaryProcessor::KTPrimaryProcessor(const std::string& name) :
            KTProcessor(name)
    {
    }

    KTPrimaryProcessor::~KTPrimaryProcessor()
    {
    }

    void KTPrimaryProcessor::operator ()()
    {
        if (! Run())
        {
            KTERROR(proclog, "An error occurred during processor running.");
        }
        return;
    }

} /* namespace Katydid */
