/*
 * KTSignal.cc
 *
 *  Created on: Aug 8, 2012
 *      Author: nsoblath
 */

#include "KTSignal.hh"

#include "KTConnection.hh"

#include <boost/foreach.hpp>

using std::multiset;

namespace Katydid
{

    KTSignal::KTSignal() :
            fConnections()
    {
        // TODO Auto-generated constructor stub

    }

    KTSignal::~KTSignal()
    {
        // TODO Auto-generated destructor stub
    }

    void KTSignal::AddConnection(KTConnection* connection)
    {
        fConnections.insert(connection);
        return;
    }

    void KTSignal::ExecuteSignals()
    {
        BOOST_FOREACH( KTConnection* connection, fConnections )
        {
            //connection->Execute();
        }
    }


} /* namespace Katydid */
