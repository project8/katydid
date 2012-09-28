/**
 @file KTProcessorToolbox.hh
 @brief Contains KTProcessorToolbox
 @details Manages processors requested by the user at run time.
 @author: N. S. Oblath
 @date: Sep 27, 2012
 */

#ifndef KTPROCESSORTOOLBOX_HH_
#define KTPROCESSORTOOLBOX_HH_

#include "KTConfigurable.hh"
#include "KTFactory.hh"

#include "KTProcessor.hh"

#include <deque>

namespace Katydid
{

    /*!
     @class KTProcessorToolbox
     @author N. S. Oblath

     @brief Manages processors requested by the user at run time.

     @details
     KTProcessorToolbox allows the user to setup an application at runtime.

     The user chooses the processors to be used, how they're linked with signals and slots, and what is used to "run"
     the program via a configuration file.

     While this does result in longer configuration files, it drastically simplifies the space of executables that are needed.

     The following order should be used for configuring the processor toolbox:
     <ol>
         <li>Create processors</li>
         <li>Connect signals and slots</li>
         <li>Create the run queue</li>
     </ol>

     Available (nested) configuration values:
     <ul>
         <li>processor -- create a processor (multiple processor options are allowed)
             <ul>
                 <li>type -- string specifying the processor type (matches the string given to the Registrar, which should be specified before the class implementation in each processor's .cc file).</li>
                 <li>name -- string giving the individual processor a name so that multiple processors of the same type can be created.</li>
                 <li>is-top-level -- boolean specifying whether this processor is configured independent of other processors.</li>
             </ul>
         </li>
         <li>connection -- connect a signal to a slot (multiple connection options are allowed)
             <ul>
                 <li>signal-processor -- <i>name</i> (i.e. the name described immediately above) of the processor that will emit the signal.</li>
                 <li>signal-name -- name of the signal being emitted.</li>
                 <li>slot-processor -- <i>name</li> of the processor with the slot that will receive the signal.</li>
                 <li>slot-name -- name of the slot being used to receive the signal.</li>
             </ul>
         </li>
         <li>run-queue -- define the queue of processors for which Run() will be called (this option should NOT be repeated)
             <ul>
                 <li>processor -- add a processor to the run queue. This option can be repeated, but please note: this is a FIFO queue, so processors will be called in the order they are specified here!</li>
             </ul>
         </li>
     </ul>
    */
    class KTProcessorToolbox : public KTConfigurable
    {
        public:
            KTProcessorToolbox();
            virtual ~KTProcessorToolbox();

        protected:
            KTFactory< KTProcessor >* fProcFactory; // singleton; not owned by KTProcessorToolbox

        public:
            /// Configure the toolbox: create the processors; connnect signals and slots; and setup the run queue.
            Bool_t Configure(const KTPStoreNode* node);

            /// Configure top-level processors (i.e. those with top-level blocks in the config. file)
            Bool_t ConfigureProcessors(const KTPStoreNode* node);

        public:
            /// Process the run queue.
            /// This will call Run() on all of the processors in the queue.
            Bool_t Run();

        protected:
            std::deque< KTProcessor* > fRunQueue;

        protected:
            struct ProcessorInfo
            {
                KTProcessor* fProc;
                Bool_t fIsTopLevel;
            };
            typedef std::map< std::string, ProcessorInfo > ProcessorMap;
            typedef ProcessorMap::iterator ProcMapIt;
            typedef ProcessorMap::const_iterator ProcMapCIt;
            typedef ProcessorMap::value_type ProcMapValue;

        public:
            KTProcessor* GetProcessor(const std::string& procName);
            const KTProcessor* GetProcessor(const std::string& procName) const;
            Bool_t AddProcessor(const std::string& procName, KTProcessor* proc, Bool_t isTopLevel=false);
            Bool_t RemoveProcessor(const std::string& procName);
            KTProcessor* ReleaseProcessor(const std::string& procName);
            void ClearProcessors();

        protected:
            ProcessorMap fProcMap;

    };

} /* namespace Katydid */
#endif /* KTPROCESSORTOOLBOX_HH_ */
