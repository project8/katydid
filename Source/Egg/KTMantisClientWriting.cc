/*
 * KTMantisClientWriting.cc
 *
 *  Created on: Nov 26, 2013
 *      Author: nsoblath
 */

#include "KTMantisClientWriting.hh"

#include "KTLogger.hh"
#include "KTMantisWriterToTS.hh"

#include "mt_buffer.hh"
#include "mt_client_worker.hh"
#include "mt_condition.hh"
#include "mt_exception.hh"
#include "mt_record_receiver.hh"
#include "mt_run_context_dist.hh"
#include "mt_server.hh"
#include "mt_signal_handler.hh"
#include "mt_thread.hh"

namespace Katydid
{
    KTLOGGER( mtlog, "KTMantisClientWriting" );

    KTMantisClientWriting::KTMantisClientWriting( KTMantis* a_client, const mantis::param_node* a_config, mantis::run_context_dist* a_run_context, int a_write_port ) :
            f_config( a_config ),
            f_server( NULL ),
            f_buffer_condition( NULL ),
            f_buffer( NULL ),
            f_receiver( NULL ),
            f_writer( NULL ),
            f_worker( NULL ),
            f_thread( NULL )
    {
        // objects for receiving and writing data
        try
        {
            f_server = new mantis::server( a_write_port );
        }
        catch( mantis::exception& e)
        {
            a_run_context->lock_client_status_out()->set_state( mantis::client_status_state_t_error );
            a_run_context->push_client_status_no_mutex();
            a_run_context->unlock_outbound();
            throw mantis::exception() << "unable to create record-receiver server: " << e.what();
        }

        mantis::status* t_status = a_run_context->lock_status_in();

        f_buffer_condition = new mantis::condition();
        f_buffer = new mantis::buffer( t_status->buffer_size(), t_status->record_size() );

        f_receiver = new mantis::record_receiver( f_server );
        f_receiver->set_data_chunk_size( t_status->data_chunk_size() );
        f_receiver->set_data_type_size( t_status->data_type_size() );
        f_receiver->allocate( f_buffer, f_buffer_condition );

        a_run_context->unlock_inbound();

        f_writer = new KTMantisWriterToTS();
        f_writer->set_client( a_client );
        f_writer->set_buffer( f_buffer, f_buffer_condition );
        f_writer->configure( f_config );

        try
        {
            f_worker = new mantis::client_worker( a_run_context->lock_request_out(), f_receiver, f_writer, f_buffer_condition );
        }
        catch( mantis::exception& e )
        {
            a_run_context->unlock_outbound();
            a_run_context->lock_client_status_out()->set_state( mantis::client_status_state_t_error );
            a_run_context->push_client_status_no_mutex();
            a_run_context->unlock_outbound();
            throw mantis::exception() << "unable to create client's writer: " << e.what();
        }
        a_run_context->unlock_outbound();

        f_thread = new mantis::thread( f_worker );

        KTINFO( mtlog, "starting record receiver" );

        try
        {
            mantis::signal_handler t_sig_hand;
            t_sig_hand.push_thread( f_thread );

            f_thread->start();
        }
        catch( mantis::exception& e )
        {
            throw mantis::exception() << "unable to start record-receiving server";
        }
    }

    KTMantisClientWriting::~KTMantisClientWriting()
    {
        delete f_thread;
        delete f_worker;
        delete f_writer;
        delete f_receiver;
        delete f_buffer;
        delete f_buffer_condition;
        delete f_server;
    }

    void KTMantisClientWriting::wait_for_finish()
    {
        f_thread->join();
        mantis::signal_handler t_sig_hand;
        t_sig_hand.pop_thread();
        return;
    }

    void KTMantisClientWriting::cancel()
    {
        f_worker->cancel();
        return;
    }

    bool KTMantisClientWriting::is_done()
    {
        return f_worker->is_done();
    }

} /* namespace Katydid */
