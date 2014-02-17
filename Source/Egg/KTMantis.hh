/**
 @file KTMantis.hh
 @brief Contains KTMantis
 @details [does something]
 @author: N. Oblath
 @date: February 14, 2014
 */

#ifndef KTMANTIS_HH_
#define KTMANTIS_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"

#include "mt_param.hh"

namespace mantis
{
}

namespace Katydid
{
    class KTEggHeader;
    class KTPStoreNode;

    class KTMantisClientConfig : public mantis::param_node
    {
        public:
            KTMantisClientConfig();
            virtual ~KTMantisClientConfig();
    };

    /*!
     @class KTMantis
     @author N. Oblath

     @brief Mantis client

     @details


     Configuration name: "mantis-client"

     Available configuration values:
     - "some-name": [type] -- [what it does]

     Slots:

     Signals:
     - "header": void (const KTEggHeader*) -- emitted when the file header is parsed.
     - "raw-ts" void (KTDataPtr) -- emitted when a new raw time series is produced; guarantees KTRawTimeSeriesData
     - "ts": void (KTDataPtr) -- emitted when the new calibrated time series is produced; Guarantees KTTimeSeriesData
     - "egg-done": void () --  emitted when a file is finished.
     - "summary": void (const KTProcSummary*) -- emitted when a file is finished (after "egg-done")
    */

    class KTMantis : public KTProcessor
    {
        public:
            KTMantis(const std::string& name = "mantis-client");
            virtual ~KTMantis();

            bool Configure(const KTPStoreNode* node);

        private:
            mantis::param_node fConfig;

        public:
            bool Run();

            bool RunClient();


        private:


            //***************
            // Signals
            //***************

        private:
            //KTSignalData f[SomeName]Signal;

            //***************
            // Slots
            //***************

        private:
            //KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            //KTSlotDataOneType< [Input Data Type] > f[SomeName]Slot;

    };

    inline bool KTMantis::Run()
    {
        return RunClient();
    }
} /* namespace Katydid */




#include "mt_atomic.hh"
#include "mt_callable.hh"



namespace mantis
{
        class run_context_dist;
        class client_file_writing;

    class setup_loop : public callable
    {
        public:
            setup_loop( run_context_dist* a_run_context );
            virtual ~setup_loop();

            void execute();
            void cancel();

            int get_return();

        private:
            run_context_dist* f_run_context;
            atomic_bool f_canceled;
            int f_return;
    };

    class run_loop : public callable
    {
        public:
            run_loop( ::mantis::run_context_dist* a_run_context, ::mantis::client_file_writing* a_file_writing = NULL );
            virtual ~run_loop();

            void execute();
            void cancel();

            int get_return();

        private:
            run_context_dist* f_run_context;
            client_file_writing* f_file_writing;
            atomic_bool f_canceled;
            int f_return;
    };


}
 /* namespace mantis */

#endif /* KTMANTIS_HH_ */
