/*
 * KTMantisClientWriting.hh
 *
 *  Created on: Nov 26, 2013
 *      Author: nsoblath
 */

#ifndef KT_CLIENT_WRITING_HH_
#define KT_CLIENT_WRITING_HH_

namespace mantis
{
    class buffer;
    class client_worker;
    class condition;
    class param_node;
    class record_receiver;
    class run_context_dist;
    class server;
    class thread;
} /* namespace mantis */

namespace Katydid
{
    using namespace Nymph;
    class KTMantis;
    class KTMantisWriterToTS;

    class KTMantisClientWriting
    {
        public:
            KTMantisClientWriting( KTMantis* a_client, const mantis::param_node* a_config, mantis::run_context_dist* a_run_context_dist, int a_write_port );
            virtual ~KTMantisClientWriting();

            void wait_for_finish();

            void cancel();

            bool is_done();

        private:
            const mantis::param_node* f_config;
            mantis::server* f_server;
            mantis::condition *f_buffer_condition;
            mantis::buffer *f_buffer;
            mantis::record_receiver* f_receiver;
            KTMantisWriterToTS* f_writer;
            mantis::client_worker* f_worker;
            mantis::thread* f_thread;
    };

} /* namespace Katydid */
#endif /* KT_CLIENT_WRITING_HH_ */
