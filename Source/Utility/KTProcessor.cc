/*
 * KTProcessor.cc
 *
 *  Created on: Jan 5, 2012
 *      Author: nsoblath
 */

#include "KTProcessor.hh"

#include <boost/foreach.hpp>

#include <iostream>
using std::cout;
using std::endl;

namespace Katydid
{
    ProcessorException::ProcessorException (std::string const& why)
      : std::logic_error(why)
    {}


    KTProcessor::KTProcessor() :
            fSignalMap(),
            fSlotMap()
    {
    }

    KTProcessor::~KTProcessor()
    {
        for (SlotMapIt iter = fSlotMap.begin(); iter != fSlotMap.end(); iter++)
        {
            iter->second->Disconnect();
            delete iter->second;
        }
        for (SigMapIt iter = fSignalMap.begin(); iter != fSignalMap.end(); iter++)
        {
            delete iter->second;
        }
    }

} /* namespace Katydid */
