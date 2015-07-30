/*
 * KTProcessorToolbox.cc
 *
 *  Created on: Sep 27, 2012
 *      Author: nsoblath
 */

#include "KTProcessorToolbox.hh"

#include "KTLogger.hh"
#include "KTPrimaryProcessor.hh"
#include "KTParam.hh"

#ifndef SINGLETHREADED
#include <boost/thread.hpp>
#endif

#include <vector>

using std::deque;
using std::set;
using std::string;
using std::vector;

namespace Nymph
{
    KTLOGGER(proclog, "KTProcessorToolbox");

    KTProcessorToolbox::KTProcessorToolbox(const std::string& name) :
            KTConfigurable(name),
            fProcFactory(KTNOFactory< KTProcessor >::GetInstance()),
            fRunQueue(),
            fProcMap()
    {
    }

    KTProcessorToolbox::~KTProcessorToolbox()
    {
        ClearProcessors();
    }

    bool KTProcessorToolbox::Configure(const KTParamNode* node)
    {
        KTPROG(proclog, "Configuring . . .");
        // Deal with "processor" blocks first
        const KTParamArray* procArray = node->ArrayAt( "processors" );
        if (procArray == NULL)
        {
            KTWARN(proclog, "No processors were specified");
        }
        else
        {
            for( KTParamArray::const_iterator procIt = procArray->Begin(); procIt != procArray->End(); ++procIt )
            {
                if( ! (*procIt)->IsNode() )
                {
                    KTERROR( proclog, "Invalid processor entry: not a node" );
                    return false;
                }
                const KTParamNode* procNode = &( (*procIt)->AsNode() );

                if (! procNode->Has("type"))
                {
                    KTERROR(proclog, "Unable to create processor: no processor type given");
                    return false;
                }
                string procType = procNode->GetValue("type");

                string procName;
                if (! procNode->Has("name"))
                {
                    KTINFO(proclog, "No name given for processor of type <" << procType << ">; using type as name.");
                    procName = procType;
                }
                else
                {
                    procName = procNode->GetValue("name");
                }
                KTProcessor* newProc = fProcFactory->CreateNamed(procType);
                if (newProc == NULL)
                {
                    KTERROR(proclog, "Unable to create processor of type <" << procType << ">");
                    return false;
                }

                if (! AddProcessor(procName, newProc))
                {
                    KTERROR(proclog, "Unable to add processor <" << procName << ">");
                    delete newProc;
                    return false;
                }
            }
        }


        // Then deal with connections"
        const KTParamArray* connArray = node->ArrayAt( "connections" );
        if (connArray == NULL)
        {
            KTWARN(proclog, "No connections were specified");
        }
        else
        {
            for( KTParamArray::const_iterator connIt = connArray->Begin(); connIt != connArray->End(); ++connIt )
            {
                if( ! (*connIt)->IsNode() )
                {
                    KTERROR( proclog, "Invalid connection entry: not a node" );
                    return false;
                }
                const KTParamNode* connNode = &( (*connIt)->AsNode() );

                if ( ! connNode->Has("signal") || ! connNode->Has("slot") )
                {
                    KTERROR(proclog, "Signal/Slot connection information is incomplete!");
                    if (connNode->Has("signal"))
                    {
                        KTWARN(proclog, "signal = " << connNode->GetValue("signal"));
                    }
                    else
                    {
                        KTERROR(proclog, "signal = MISSING");
                    }

                    if (connNode->Has("slot"))
                    {
                        KTWARN(proclog, "slot = " << connNode->GetValue("slot"));
                    }
                    else
                    {
                        KTERROR(proclog, "slot = MISSING");
                    }
                    return false;
                }

                string signalProcName, signalName;
                if (! ParseSignalSlotName(connNode->GetValue("signal"), signalProcName, signalName))
                {
                    KTERROR(proclog, "Unable to parse signal name: <" << connNode->GetValue("signal") << ">");
                    return false;
                }

                string slotProcName, slotName;
                if (! ParseSignalSlotName(connNode->GetValue("slot"), slotProcName, slotName))
                {
                    KTERROR(proclog, "Unable to parse slot name: <" << connNode->GetValue("slot") << ">");
                    return false;
                }


                KTProcessor* signalProc = GetProcessor(signalProcName);
                if (signalProc == NULL)
                {
                    KTERROR(proclog, "Processor named <" << signalProcName << "> was not found!");
                    return false;
                }

                KTProcessor* slotProc = GetProcessor(slotProcName);
                if (slotProc == NULL)
                {
                    KTERROR(proclog, "Processor named <" << slotProcName << "> was not found!");
                    return false;
                }

                try
                {
                    if (connNode->Has("order"))
                    {
                        signalProc->ConnectASlot(signalName, slotProc, slotName, connNode->GetValue< int >("order"));
                    }
                    else
                    {
                        signalProc->ConnectASlot(signalName, slotProc, slotName);
                    }
                }
                catch (std::exception& e)
                {
                    KTERROR(proclog, "An error occurred while connecting signals and slots:\n"
                            << "\tSignal " << signalName << " from processor " << signalProcName << " (a.k.a. " << signalProc->GetConfigName() << ")" << '\n'
                            << "\tSlot " << slotName << " from processor " << slotProcName << " (a.k.a. " << slotProc->GetConfigName() << ")" << '\n'
                            << '\t' << e.what());
                    return false;
                }
                KTINFO(proclog, "Signal <" << signalProcName << ":" << signalName << "> connected to slot <" << slotProcName << ":" << signalName << ">");

            }
        }


        // Finally, deal with processor-run specifications
        // The run queue is an array of processor names, or groups of names, which will be run sequentially.
        // If names are grouped (in another array), those in that group will be run in parallel.
        // In single threaded mode all threads will be run sequentially in the order they were specified.
        const KTParamArray* rqArray = node->ArrayAt( "run-queue" );
        if (rqArray == NULL)
        {
            KTWARN(proclog, "Run queue was not specified");
        }
        else
        {
            for (KTParamArray::const_iterator rqIt = rqArray->Begin(); rqIt != rqArray->End(); ++rqIt)
            {
                ThreadGroup threadGroup;
                if ((*rqIt)->IsValue())
                {
                    const KTParamValue* rqValue = &( (*rqIt)->AsValue() );

                    if (! AddProcessorToThreadGroup(rqValue, threadGroup)) return false;
                }
                else if ((*rqIt)->IsNode())
                {
                    const KTParamArray* rqNode = &( (*rqIt)->AsArray() );

                    for (KTParamArray::const_iterator rqArrayIt = rqNode->Begin(); rqArrayIt != rqNode->End(); ++rqArrayIt)
                    {
                        if (! (*rqArrayIt)->IsValue())
                        {
                            KTERROR(proclog, "Invalid run-queue array entry: not a value");
                            return false;
                        }
                        const KTParamValue* rqValue = &( (*rqArrayIt)->AsValue() );
                        if (! AddProcessorToThreadGroup(rqValue, threadGroup)) return false;
                    }
                }
                else
                {
                    KTERROR(proclog, "Invalid run-queue entry: not a value or array");
                    return false;
                }
                fRunQueue.push_back(threadGroup);
            }
        }

        return true;
    }

    bool KTProcessorToolbox::ParseSignalSlotName(const std::string& toParse, std::string& nameOfProc, std::string& nameOfSigSlot) const
    {
        size_t sepPos = toParse.find_first_of(fSigSlotNameSep);
        if (sepPos == string::npos)
        {
            KTERROR(proclog, "Unable to find separator between processor and signal/slot name in <" << toParse << ">");
            return false;
        }
        nameOfProc = toParse.substr(0, sepPos);
        nameOfSigSlot = toParse.substr(sepPos + 1);
        return true;
    }

    bool KTProcessorToolbox::AddProcessorToThreadGroup(const KTParamValue* param, ThreadGroup& group)
    {
        string procName = param->Get();
        KTProcessor* procForRunQueue = GetProcessor(procName);
        KTDEBUG(proclog, "Adding processor of type " << procName << " to the run queue");
        if (procForRunQueue == NULL)
        {
            KTERROR(proclog, "Unable to find processor <" << procName << "> requested for the run queue");
            return false;
        }

        KTPrimaryProcessor* primaryProc = dynamic_cast< KTPrimaryProcessor* >(procForRunQueue);
        if (primaryProc == NULL)
        {
            KTERROR(proclog, "Processor <" << procName << "> is not a primary processor.");
            return false;
        }
        //group.insert(primaryProc);
        group.insert(Thread(primaryProc, procName));
        return true;
    }


    bool KTProcessorToolbox::ConfigureProcessors(const KTParamNode* node)
    {
        for (ProcMapIt iter = fProcMap.begin(); iter != fProcMap.end(); iter++)
        {
            KTDEBUG(proclog, "Attempting to configure processor <" << iter->first << ">");
            string procName = iter->first;
            string nameUsed;
            const KTParamNode* subNode = node->NodeAt(procName);
            if (subNode == NULL)
            {
                string configName = iter->second.fProc->GetConfigName();
                KTWARN(proclog, "Did not find a parameter node <" << procName << ">\n"
                        "\tWill check using the generic name of the processor, <" << configName << ">.");
                subNode = node->NodeAt(configName);
                if (subNode == NULL)
                {
                    KTWARN(proclog, "Did not find a parameter node <" << configName << ">\n"
                            "\tProcessor <" << iter->first << "> was not configured.");
                    continue;
                }
                nameUsed = configName;
            }
            else
            {
                nameUsed = procName;
            }
            if (! iter->second.fProc->Configure(subNode))
            {
                KTERROR(proclog, "An error occurred while configuring processor <" << iter->first << "> with parameter node <" << nameUsed << ">");
                return false;
            }
        }
        return true;
    }

    bool KTProcessorToolbox::Run()
    {
        KTPROG(proclog, "Beginning processing . . .");
#ifndef SINGLETHREADED
        unsigned iGroup = 0;
#endif
        for (RunQueue::const_iterator rqIter = fRunQueue.begin(); rqIter != fRunQueue.end(); ++rqIter)
        {
#ifdef SINGLETHREADED
            for (ThreadGroup::const_iterator tgIter = rqIter->begin(); tgIter != rqIter->end(); ++tgIter)
            {
                if (! tgIter->fProc->Run())
                {
                    return false;
                }
            }
#else
            KTDEBUG(proclog, "Starting thread group " << iGroup);
            boost::thread_group parallelThreads;
            unsigned iThread = 0;
            for (ThreadGroup::const_iterator tgIter = rqIter->begin(); tgIter != rqIter->end(); ++tgIter)
            {
                // create a boost::thread object to launch the thread
                // use boost::ref to avoid copying the processor
                KTDEBUG(proclog, "Starting thread " << iThread << ": " << tgIter->fName);
                parallelThreads.create_thread(boost::ref(*(tgIter->fProc)));
                //parallelThreads.create_thread(boost::ref(**tgIter));
                iThread++;
            }
            // wait for execution to complete
            parallelThreads.join_all();
            iGroup++;
#endif
        }
        KTPROG(proclog, ". . . processing complete.");
        return true;
    }

    KTProcessor* KTProcessorToolbox::GetProcessor(const std::string& procName)
    {
        ProcMapIt it = fProcMap.find(procName);
        if (it == fProcMap.end())
        {
            KTWARN(proclog, "Processor <" << procName << "> was not found.");
            return NULL;
        }
        return it->second.fProc;
    }

    const KTProcessor* KTProcessorToolbox::GetProcessor(const std::string& procName) const
    {
        ProcMapCIt it = fProcMap.find(procName);
        if (it == fProcMap.end())
        {
            KTWARN(proclog, "Processor <" << procName << "> was not found.");
            return NULL;
        }
        return it->second.fProc;
    }

    bool KTProcessorToolbox::AddProcessor(const std::string& procName, KTProcessor* proc)
    {
        ProcMapIt it = fProcMap.find(procName);
        if (it == fProcMap.end())
        {
            ProcessorInfo pInfo;
            pInfo.fProc = proc;
            fProcMap.insert(ProcMapValue(procName, pInfo));
            KTDEBUG(proclog, "Added processor <" << procName << "> (a.k.a. " << proc->GetConfigName() << ")");
            return true;
        }
        KTWARN(proclog, "Processor <" << procName << "> already exists; new processor was not added.");
        return false;
    }

    bool KTProcessorToolbox::RemoveProcessor(const std::string& procName)
    {
        KTProcessor* procToRemove = ReleaseProcessor(procName);
        if (procToRemove == NULL)
        {
            return false;
        }
        delete procToRemove;
        KTDEBUG(proclog, "Processor <" << procName << "> deleted.");
        return true;
    }

    KTProcessor* KTProcessorToolbox::ReleaseProcessor(const std::string& procName)
    {
        ProcMapIt it = fProcMap.find(procName);
        if (it == fProcMap.end())
        {
            KTWARN(proclog, "Processor <" << procName << "> was not found.");
            return NULL;
        }
        KTProcessor* procToRelease = it->second.fProc;
        fProcMap.erase(it);
        return procToRelease;
    }

    void KTProcessorToolbox::ClearProcessors()
    {
        for (ProcMapIt it = fProcMap.begin(); it != fProcMap.end(); it++)
        {
            delete it->second.fProc;
        }
        fProcMap.clear();
        fRunQueue.clear();
        return;
    }

} /* namespace Nymph */
