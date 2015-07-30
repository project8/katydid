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
#include "KTNOFactory.hh"

#include <deque>
#include <set>

namespace Nymph
{
    class KTParamNode;
    class KTParamValue;
    class KTPrimaryProcessor;
    class KTProcessor;

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
         <li>processors (array of objects) -- create a processor; each object in the array should consist of:
             <ul>
                 <li>type -- string specifying the processor type (matches the string given to the Registrar, which should be specified before the class implementation in each processor's .cc file).</li>
                 <li>name -- string giving the individual processor a name so that multiple processors of the same type can be created.</li>
             </ul>
         </li>
         <li>connection (array of objects) -- connect a signal to a slot; each object should consist of:
             <ul>
                 <li>signal -- <i>proc-name:signal-name</i>; name (i.e. the name given in the array of processors above) of the processor, and the signal that will be emitted.</li>
                 <li>slot -- <i>proc-name:slot-name</li>; name of the processor with the slot that will receive the signal.</li>
                 <li>group-order -- (optional) integer specifying the order in which slots should be called.
             </ul>
         </li>
         <li>run-queue -- (array of strings and arrays of strings) define the queue of processors that will control the running of Nymph.
         The elements of this array specify processors that are run sequentially.
         If an element is itself an array, those processors listed in the sub-array will be run in parallel.
             <ul>
                 <li>processor name -- add a processor to the run queue, or </li>
                 <li>array of processor names -- add a group of processors to the run queue.</li>
             </ul>
             In single-threaded mode, all processors will be run sequentially, in the order specified.
         </li>
     </ul>
    */
    class KTProcessorToolbox : public KTConfigurable
    {
        public:
            KTProcessorToolbox(const std::string& name = "processor-toolbox");
            virtual ~KTProcessorToolbox();

        private:
            KTNOFactory< KTProcessor >* fProcFactory; // singleton; not owned by KTProcessorToolbox

        public:
            /// Configure the toolbox: create the processors; connnect signals and slots; and setup the run queue.
            bool Configure(const KTParamNode* node);

            /// Configure processors (only those specified in the toolbox)
            bool ConfigureProcessors(const KTParamNode* node);

        private:
            bool ParseSignalSlotName(const std::string& toParse, std::string& nameOfProc, std::string& nameOfSigSlot) const;
            static const char fSigSlotNameSep = ':';


            struct Thread
            {
                    KTPrimaryProcessor* fProc;
                    std::string fName;
                    Thread(KTPrimaryProcessor* proc, const std::string& name) : fProc(proc), fName(name)
                    {}
            };
            //typedef std::set< KTPrimaryProcessor* > ThreadGroup;
            struct CompareThread
            {
                bool operator() (const Thread& lhs, const Thread& rhs)
                {
                    return lhs.fName < rhs.fName;
                }
            };
            typedef std::set< Thread, CompareThread > ThreadGroup;
            typedef std::deque< ThreadGroup > RunQueue;
            bool AddProcessorToThreadGroup(const KTParamValue* param, ThreadGroup& group);

        public:
            /// Process the run queue.
            /// This will call Run() on all of the processors in the queue.
            bool Run();

        private:
            RunQueue fRunQueue;

        private:
            struct ProcessorInfo
            {
                KTProcessor* fProc;
            };
            typedef std::map< std::string, ProcessorInfo > ProcessorMap;
            typedef ProcessorMap::iterator ProcMapIt;
            typedef ProcessorMap::const_iterator ProcMapCIt;
            typedef ProcessorMap::value_type ProcMapValue;

        public:
            KTProcessor* GetProcessor(const std::string& procName);
            const KTProcessor* GetProcessor(const std::string& procName) const;
            bool AddProcessor(const std::string& procName, KTProcessor* proc);
            bool RemoveProcessor(const std::string& procName);
            KTProcessor* ReleaseProcessor(const std::string& procName);
            void ClearProcessors();

        private:
            ProcessorMap fProcMap;

    };

} /* namespace Nymph */
#endif /* KTPROCESSORTOOLBOX_HH_ */
