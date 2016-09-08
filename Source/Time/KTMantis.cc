/*
 * KTMantis.cc
 *
 *  Created on: February 14, 2014
 *      Author: N. Oblath
 */

#include "KTMantis.hh"

#include "KTCommandLineHandler.hh"
#include "KTEggHeader.hh"
#include "KTMantisClientWriting.hh"
#include "param.hh"

#include "mt_configurator.hh"
#include "mt_client_config.hh"
#include "mt_client_worker.hh"
#include "mt_client.hh"
#include "mt_exception.hh"
#include "mt_run_client.hh"
#include "mt_run_context_dist.hh"
#include "mt_signal_handler.hh"
#include "mt_thread.hh"
#include "thorax.hh"

#include <algorithm> // for min
#include <string>
#include <unistd.h>
using std::string;

#define RETURN_SUCCESS 1
#define RETURN_ERROR -1
#define RETURN_CANCELED -2
#define RETURN_REVOKED -3



namespace Katydid
{
    KTLOGGER(mtlog, "KTMantis");

    KT_REGISTER_PROCESSOR(KTMantis, "mantis-client");


    KTMantisClientConfig::KTMantisClientConfig()
    {
        // default client configuration

        mantis::param_value clientValue;

        add( "port", clientValue << 98342 );

        add( "host", clientValue << "localhost" );

        add( "client-port", clientValue << 98343 );

        add( "client-host", clientValue << "localhost" );

        add( "rate", clientValue << 250.0 );

        add( "duration", clientValue << 1000 );

        add( "mode", clientValue << 0 );

        add( "file-writer", clientValue << "client" );
    }

    KTMantisClientConfig::~KTMantisClientConfig()
    {
    }


    KTMantis::KTMantis(const std::string& name) :
            Nymph::KTDataQueueProcessorTemplate< KTMantis >(name),
            fConfig(),
            fSliceSize(1024),
            fStride(0),
            fHeaderSignal("header", this),
            fSliceSignal("raw-ts", this),
            fMantisDoneSignal("mantis-done", this)

    {
        KTMantisClientConfig defaultConfig;
        fConfig.merge(&defaultConfig);
    }

    KTMantis::~KTMantis()
    {
    }

    bool KTMantis::ConfigureSubClass(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSliceSize(node->get_value< unsigned >("slice-size", fSliceSize));
        SetStride(node->get_value< unsigned >("stride", fStride));

        const scarab::param_node* clientNode = node->NodeAt("run-queue");
        if (clientNode != NULL)
        {
            // fill in fConfig
            mantis::param_value newValue;
            for (scarab::param_node::const_iterator it = clientNode->Begin(); it != clientNode->End(); ++it)
            {
                fConfig.add(it->first, newValue << it->second->AsValue().Get());
            }
        }

        return true;
    }

    bool KTMantis::RunClient()
    {
        try
        {
            mantis::run_client the_client( &fConfig, KTCommandLineHandler::GetInstance()->GetExecutableName() );

            the_client.execute();

            return the_client.get_return() == RETURN_SUCCESS;
        }
        catch( mantis::exception& e )
        {
            KTERROR( mtlog, "mantis::exception caught: " << e.what() );
            return false;
        }
        catch( std::exception& e )
        {
            KTERROR( mtlog, "std::exception caught: " << e.what() );
            return false;
        }

        return false;

    }

} /* namespace Katydid */
