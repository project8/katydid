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
    KTLOGGER(proclog, "katydid.core");

    KTPrimaryProcessor::KTPrimaryProcessor() :
            KTProcessor()
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
