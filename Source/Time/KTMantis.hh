/**
 @file KTMantis.hh
 @brief Contains KTMantis
 @details [does something]
 @author: N. Oblath
 @date: February 14, 2014
 */

#ifndef KTMANTIS_HH_
#define KTMANTIS_HH_

#include "KTDataQueueProcessor.hh"

#include "KTSlot.hh"

#include "mt_atomic.hh"
#include "mt_callable.hh"
#include "mt_param.hh"


namespace mantis
{
    class run_context_dist;
}

namespace Katydid
{
    class KTMantisClientWriting;
    class KTEggHeader;
    class KTParamNode;

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
     - "slice-size": unsigned -- Specify the size of the time series (required)
     - "stride": unsigned -- Specify how many bins to advance between slices; must be equal to or larger than the slice size
                             Leave unset to make stride == slice-size; i.e. no overlap or skipping between slices
     - "client": object -- Mantis client configuration values (see Mantis documentation for more information)

     Slots:

     Signals:
     - "header": void (KTEggHeader*) -- emitted when the file header is parsed.
     - "raw-ts" void (KTDataPtr) -- emitted when a new raw time series is produced; guarantees KTRawTimeSeriesData
     - "mantis-done": void () --  emitted when a file is finished.
    */

    class KTMantis : public KTDataQueueProcessorTemplate< KTMantis >
    {
        private:
            friend class KTMantisWriterToTS;

        public:
            KTMantis(const std::string& name = "mantis-client");
            virtual ~KTMantis();

            bool ConfigureSubClass(const KTParamNode* node);

        public:
            unsigned GetSliceSize() const;
            void SetSliceSize(unsigned size);

            unsigned GetStride() const;
            void SetStride(unsigned stride);

        private:
            mantis::param_node fConfig;

            unsigned fSliceSize;
            unsigned fStride;

        public:
            bool Run();

            bool RunClient();


        private:
            void EmitHeaderSignal(KTEggHeader* header);
            void EmitSliceSignal(KTDataPtr data);
            void EmitMantisDoneSignal();



            //***************
            // Signals
            //***************

        private:
            KTSignalOneArg< KTEggHeader* > fHeaderSignal;
            KTSignalData fSliceSignal;
            KTSignalOneArg< void > fMantisDoneSignal;

            //***************
            // Slots
            //***************

        private:

    };

    inline unsigned KTMantis::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTMantis::SetSliceSize(unsigned size)
    {
        fSliceSize = size;
        return;
    }

    inline unsigned KTMantis::GetStride() const
    {
        return fStride;
    }

    inline void KTMantis::SetStride(unsigned stride)
    {
        fStride = stride;
        return;
    }

    inline bool KTMantis::Run()
    {
        return RunClient();
    }

    inline void KTMantis::EmitHeaderSignal(KTEggHeader* header)
    {
        fHeaderSignal(header);
        return;
    }

    inline void KTMantis::EmitSliceSignal(KTDataPtr data)
    {
        fSliceSignal(data);
        return;
    }


    inline void KTMantis::EmitMantisDoneSignal()
    {
        fMantisDoneSignal();
        return;
    }



    class KTSetupLoop : public mantis::callable
    {
        public:
            KTSetupLoop( mantis::run_context_dist* a_run_context );
            virtual ~KTSetupLoop();

            void execute();
            void cancel();

            int get_return();

        private:
            mantis::run_context_dist* f_run_context;
            mantis::atomic_bool f_canceled;
            int f_return;
    };

    class KTRunLoop : public mantis::callable
    {
        public:
            KTRunLoop( ::mantis::run_context_dist* a_run_context, KTMantisClientWriting* a_file_writing = NULL );
            virtual ~KTRunLoop();

            void execute();
            void cancel();

            int get_return();

        private:
            mantis::run_context_dist* f_run_context;
            KTMantisClientWriting* f_writing;
            mantis::atomic_bool f_canceled;
            int f_return;
    };


}
 /* namespace mantis */

#endif /* KTMANTIS_HH_ */
