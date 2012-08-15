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
    A::A()
    {}
    A::~A()
    {}

    void A::UseFunc(B* bptr, int (B::*bfunc)())
    {
        //cout << (bptr->*bfunc)() << endl;

        //boost::function< int (B*) > func = bfunc;
        //cout << func(bptr) << endl;

        boost::function< int () > func = boost::bind(bfunc, bptr);
        cout << func() << endl;

    }

    B::B()
    {}
    B::~B()
    {}

    int B::FuncExample()
    {
        return 5;
    }

    ProcessorException::ProcessorException (std::string const& why)
      : std::logic_error(why)
    {}


    KTProcessor::KTProcessor() :
            fSignalMap(),
            fSlotMap()
    {
        // TODO Auto-generated constructor stub

    }

    KTProcessor::~KTProcessor()
    {
        for (SlotMapIt iter = fSlotMap.begin(); iter != fSlotMap.end(); iter++)
        {
            iter->second->Disconnect();
        }
    }

} /* namespace Katydid */
