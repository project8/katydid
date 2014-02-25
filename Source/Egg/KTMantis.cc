/*
 * KTMantis.cc
 *
 *  Created on: February 14, 2014
 *      Author: N. Oblath
 */

#include "KTMantis.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTMantisClientWriting.hh"
#include "KTNOFactory.hh"
#include "KTPStoreNode.hh"

#include "mt_configurator.hh"
#include "mt_client_config.hh"
#include "mt_client_worker.hh"
#include "mt_client.hh"
#include "mt_exception.hh"
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

    static KTNORegistrar< KTProcessor, KTMantis > sProcTempRegistrar("mantis-client");


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
            KTDataQueueProcessorTemplate< KTMantis >(name),
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

    bool KTMantis::ConfigureSubClass(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        SetSliceSize(node->GetData< unsigned >("slice-size", fSliceSize));
        SetStride(node->GetData< unsigned >("stride", fStride));

        const KTPStoreNode clientNode = node->GetChild("run-queue");
        if (clientNode.IsValid())
        {
            // fill in fConfig
            mantis::param_value newValue;
            for (KTPStoreNode::const_iterator it = clientNode.Begin(); it != clientNode.End(); ++it)
            {
                fConfig.add(it->first, newValue << it->second.data());
            }
        }

        return true;
    }

    bool KTMantis::RunClient()
    {
        try
        {
            KTINFO( mtlog, "creating request objects..." );

            bool t_client_writes_file = true;
            if( fConfig.get_value< string >( "file-writer" ) == std::string( "server" ) )
            {
                t_client_writes_file = false;
            }

            string t_request_host = fConfig.get_value< string >( "host" );
            int t_request_port = fConfig.get_value< int >( "port" );

            string t_write_host;
            int t_write_port = -1;
            if( t_client_writes_file )
            {
                t_write_host = fConfig.get_value< string >( "client-host" );
                t_write_port = fConfig.get_value< int >( "client-port", t_request_port + 1 );
            }

            double t_duration = fConfig.get_value< double >( "duration" );

            mantis::run_context_dist t_run_context;

            mantis::request* t_request = t_run_context.lock_request_out();
            t_request->set_write_host( t_write_host );
            t_request->set_write_port( t_write_port );
            t_request->set_file( "Katydid" );
            t_request->set_description( fConfig.get_value< string >( "description", "default client run" ) );
            t_request->set_date( get_absolute_time_string() );
            t_request->set_mode( (mantis::request_mode_t)fConfig.get_value< int >( "mode" ) );
            t_request->set_rate( fConfig.get_value< double >( "rate" ) );
            t_request->set_duration( t_duration );
            t_request->set_file_write_mode( mantis::request_file_write_mode_t_local );
            if( t_client_writes_file )
            {
                t_request->set_file_write_mode( mantis::request_file_write_mode_t_remote );
            }
            t_run_context.unlock_outbound();

            // start the client for sending the request
            KTINFO( mtlog, "connecting with the server...");

            mantis::client* t_request_client;
            try
            {
                t_request_client = new mantis::client( t_request_host, t_request_port );
            }
            catch( mantis::exception& e )
            {
                KTERROR( mtlog, "unable to start client: " << e.what() );
                return RETURN_ERROR;
            }

            t_run_context.set_connection( t_request_client );


            KTINFO( mtlog, "starting communicator" );

            mantis::thread t_comm_thread( &t_run_context );

            mantis::signal_handler t_sig_hand;

            try
            {
                t_sig_hand.push_thread( &t_comm_thread );

                t_comm_thread.start();
                t_run_context.wait_until_active();
            }
            catch( mantis::exception& e )
            {
                KTERROR( mtlog, "an error occurred while running the communication thread" );
                delete t_request_client;
                return RETURN_ERROR;
            }


            KTINFO( mtlog, "sending request..." )

            if( ! t_run_context.push_request() )
            {
                t_run_context.cancel();
                t_comm_thread.cancel();
                delete t_request_client;
                KTERROR( mtlog, "error sending request" );
                return RETURN_ERROR;
            }


            KTSetupLoop t_setup_loop( &t_run_context );
            mantis::thread t_setup_loop_thread( &t_setup_loop );
            t_sig_hand.push_thread( & t_setup_loop_thread );
            t_setup_loop_thread.start();
            t_setup_loop_thread.join();
            if( ! t_sig_hand.got_exit_signal() )
            {
                t_sig_hand.pop_thread();
            }
            if( t_setup_loop.get_return() == RETURN_ERROR )
            {
                KTERROR( mtlog, "exiting due to error during setup loop" );
                t_run_context.cancel();
                t_comm_thread.cancel();
                delete t_request_client;
                return RETURN_ERROR;
            }

            // Client has now received Acknowledged status from the server
            // Status message should now contain any information the client might need from the server
            // Server is now waiting for a client status update


            // get the data type size
            mantis::status* t_status = t_run_context.lock_status_in();
            //unsigned t_data_type_size = t_status->data_type_size();
            // record receiver is given data_type_size in client_file_writing's constructor
            fConfig.add( "data-type-size", new mantis::param_value( t_status->data_type_size() ) );
            fConfig.add( "bit-depth", new mantis::param_value( t_status->bit_depth() ) );
            fConfig.add( "voltage-min", new mantis::param_value( t_status->voltage_min() ) );
            fConfig.add( "voltage-range", new mantis::param_value( t_status->voltage_range() ) );
            t_run_context.unlock_inbound();

            /****************************************************************/
            /*********************** file writing ***************************/
            /****************************************************************/
            KTMantisClientWriting* t_writing = NULL;
            if( t_client_writes_file )
            {
                KTINFO( mtlog, "creating file-writing objects..." );

                try
                {
                    t_writing = new KTMantisClientWriting( this, &fConfig, &t_run_context, t_write_port );
                }
                catch( mantis::exception& e )
                {
                    KTERROR( mtlog, "error setting up file writing: " << e.what() );
                    t_run_context.cancel();
                    t_comm_thread.cancel();
                    delete t_request_client;
                    return RETURN_ERROR;
                }

            }
            /****************************************************************/
            /****************************************************************/
            /****************************************************************/


            KTINFO( mtlog, "transmitting status: ready" )

            t_run_context.lock_client_status_out()->set_state( mantis::client_status_state_t_ready );

            bool t_push_result = t_run_context.push_client_status_no_mutex();
            t_run_context.unlock_outbound();
            if( ! t_push_result )
            {
                KTERROR( mtlog, "error sending client status" );
                delete t_writing;
                t_run_context.cancel();
                t_comm_thread.cancel();
                delete t_request_client;
                return RETURN_ERROR;
            }

            KTRunLoop t_run_loop( &t_run_context, t_writing );
            mantis::thread t_run_loop_thread( &t_run_loop );
            t_sig_hand.push_thread( & t_run_loop_thread );
            t_run_loop_thread.start();
            t_run_loop_thread.join();
            if( ! t_sig_hand.got_exit_signal() )
            {
                t_sig_hand.pop_thread();
            }
            int t_run_success = t_run_loop.get_return();
            if( t_run_success == RETURN_ERROR )
            {
                KTERROR( mtlog, "exiting due to error during run loop" );
                t_run_context.cancel();
                t_comm_thread.cancel();
                delete t_request_client;
                return RETURN_ERROR;
            }

            /****************************************************************/
            /*********************** file writing ***************************/
            /****************************************************************/
            if( t_client_writes_file )
            {
                if( t_run_success < 0 )
                {
                    t_writing->cancel();
                }

                KTINFO( mtlog, "waiting for record reception to end..." );

                t_writing->wait_for_finish();

                KTINFO( mtlog, "shutting down record receiver" );

                delete t_writing;
                t_writing = NULL;
            }
            /****************************************************************/
            /****************************************************************/
            /****************************************************************/



            if( t_run_success > 0 || t_run_success == RETURN_CANCELED )
            {
                mantis::response* t_response;
                // wait for a completed response from the server
                bool t_can_get_response = true;
                while( t_can_get_response )
                {
                    t_response = t_run_context.lock_response_in();
                    if( t_response->state() == mantis::response_state_t_complete ) break;
                    t_run_context.unlock_inbound();

                    t_can_get_response = t_run_context.wait_for_response();
                }

                if( t_can_get_response )
                {
                    KTINFO( mtlog, "printing response from server..." );

                    KTINFO( mtlog, "digitizer summary:\n"
                            << "  record count: " << t_response->digitizer_records() << " [#]\n"
                            << "  acquisition count: " << t_response->digitizer_acquisitions() << " [#]\n"
                            << "  live time: " << t_response->digitizer_live_time() << " [sec]\n"
                            << "  dead time: " << t_response->digitizer_dead_time() << " [sec]\n"
                            << "  megabytes: " << t_response->digitizer_megabytes() << " [Mb]\n"
                            << "  rate: " << t_response->digitizer_rate() << " [Mb/sec]\n");


                    KTINFO( mtlog, "writer summary:\n"
                            << "  record count: " << t_response->writer_records() << " [#]\n"
                            << "  acquisition count: " << t_response->writer_acquisitions() << " [#]\n"
                            << "  live time: " << t_response->writer_live_time() << " [sec]\n"
                            << "  megabytes: " << t_response->writer_megabytes() << "[Mb]\n"
                            << "  rate: " << t_response->writer_rate() << " [Mb/sec]\n");

                }

                t_run_context.unlock_inbound();
            }

            t_run_context.cancel();
            t_comm_thread.cancel();
            delete t_request_client;
            return t_run_success == RETURN_SUCCESS;
        }
        catch( std::exception& e )
        {
            KTERROR( mtlog, "General exception caught: \n" << e.what() );
            return false;
        }
    }


    KTSetupLoop::KTSetupLoop( mantis::run_context_dist* a_run_context ) :
            f_run_context( a_run_context ),
            f_canceled( false ),
            f_return( 0 )
    {}
    KTSetupLoop::~KTSetupLoop()
    {}

    void KTSetupLoop::execute()
    {
        while( ! f_canceled.load() )
        {
            mantis::status_state_t t_state = f_run_context->lock_status_in()->state();
            f_run_context->unlock_inbound();

            if( t_state == mantis::status_state_t_acknowledged )
            {
                KTINFO( mtlog, "run request acknowledged...\n" );
                f_return = RETURN_SUCCESS;
                break;
            }
            else if( t_state == mantis::status_state_t_error )
            {
                KTERROR( mtlog, "error reported; run was not acknowledged\n" );
                f_return = RETURN_ERROR;
                break;
            }
            else if( t_state == mantis::status_state_t_revoked )
            {
                KTINFO( mtlog, "request revoked; run did not take place\n" );
                f_return = RETURN_ERROR;
                break;
            }
            else if( t_state != mantis::status_state_t_created )
            {
                KTERROR( mtlog, "server reported unusual status: " << t_state );
                f_return = RETURN_ERROR;
                break;
            }

            if( f_run_context->wait_for_status() )
                continue;

            KTERROR( mtlog, "(setup loop) unable to communicate with server" );
            f_return = RETURN_ERROR;
            break;
        }
        return;
    }

    void KTSetupLoop::cancel()
    {
        f_canceled.store( true );
        return;
    }

    int KTSetupLoop::get_return()
    {
        return f_return;
    }


    KTRunLoop::KTRunLoop( mantis::run_context_dist* a_run_context, KTMantisClientWriting* a_writing ) :
            f_run_context( a_run_context ),
            f_writing( a_writing ),
            f_canceled( false ),
            f_return( 0 )
    {}
    KTRunLoop::~KTRunLoop()
    {}

    void KTRunLoop::execute()
    {
        while( ! f_canceled.load() )
        {
            mantis::status_state_t t_state = f_run_context->lock_status_in()->state();
            f_run_context->unlock_inbound();

            if( t_state == mantis::status_state_t_waiting )
            {
                KTINFO( mtlog, "waiting for run...\n" );
                //continue;
            }
            else if( t_state == mantis::status_state_t_started )
            {
                KTINFO( mtlog, "run has started...\n" );
                //continue;
            }
            else if( t_state == mantis::status_state_t_running )
            {
                KTINFO( mtlog, "run is in progress...\n" );
                //continue;
            }
            else if( t_state == mantis::status_state_t_stopped )
            {
                KTINFO( mtlog, "run status: stopped; data acquisition has finished\n" );
                f_return = RETURN_SUCCESS;
                break;
            }
            else if( t_state == mantis::status_state_t_error )
            {
                KTINFO( mtlog, "error reported; run did not complete\n" );
                f_return = RETURN_ERROR;
                break;
            }
            else if( t_state == mantis::status_state_t_canceled )
            {
                KTINFO( mtlog, "cancellation reported; some data may have been written\n" );
                f_return = RETURN_CANCELED;
                break;
            }
            else if( t_state == mantis::status_state_t_revoked )
            {
                KTINFO( mtlog, "request revoked; run did not take place\n" );
                f_return = RETURN_REVOKED;
                break;
            }
            else if( f_writing != NULL && f_writing->is_done() )
            {
                KTINFO( mtlog, "file writing is done, but run status still does not indicate run is complete"
                        << "                exiting run now!" );
                f_return = RETURN_CANCELED;
                break;
            }

            if( f_run_context->wait_for_status() )
                continue;

            KTERROR( mtlog, "(run loop) unable to communicate with server" );
            f_return = RETURN_ERROR;
            break;
        }
    }

    void KTRunLoop::cancel()
    {
        f_canceled.store( true );
        return;
    }
    int KTRunLoop::get_return()
    {
        return f_return;
    }

} /* namespace mantis */
