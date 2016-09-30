/*
 * KTLines.cc
 *
 *  Created on: Sep 28, 2016
 *      Author: Christine
 */

#include "KTLines.hh"


namespace Katydid
{
    KTLines::KTLines() :
            fLines()
    {
    }

    KTLines::~KTLines()
    {
        while (! fLines.empty())
        {
            delete fLines.back();
            fLines.pop_back();
        }
    }




} /* namespace Katydid */


