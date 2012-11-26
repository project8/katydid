/*
 * KTProcessorToolbox.cc
 *
 *  Created on: Sep 27, 2012
 *      Author: nsoblath
 */

#include "KTProcessorToolbox.hh"

#include "KTLogger.hh"
#include "KTPrimaryProcessor.hh"
#include "KTPStoreNode.hh"

#include <boost/thread.hpp>

#include <vector>

using std::deque;
using std::set;
using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(proclog, "katydid.core");

    KTProcessorToolbox::KTProcessorToolbox() :
            KTConfigurable(),
            fProcFactory(KTFactory< KTProcessor >::GetInstance()),
            fRunQueue(),
            fProcMap()
    {
        fConfigName = "processor-toolbox";
    }

    KTProcessorToolbox::~KTProcessorToolbox()
    {
        ClearProcessors();
    }

    Bool_t KTProcessorToolbox::Configure(const KTPStoreNode* node)
    {
        // Deal with "processor" blocks first
        KTPStoreNode::csi_pair itPair = node->EqualRange("processor");
        for (KTPStoreNode::const_sorted_iterator it = itPair.first; it != itPair.second; it++)
        {
            KTPStoreNode subNode = KTPStoreNode(&(it->second));
            if (! subNode.HasData("type"))
            {
                KTERROR(proclog, "Unable to create processor: no processor type given");
                return false;
            }
            string procType = subNode.GetData("type");

            string procName;
            if (! subNode.HasData("name"))
            {
                KTINFO(proclog, "No name given for processor of type <" << procType << ">; using type as name.");
                procName = procType;
            }
            else
            {
                procName = subNode.GetData("name");
            }
            KTProcessor* newProc = fProcFactory->Create(procType);
            if (newProc == NULL)
            {
                KTERROR(proclog, "Unable to create processor of type <" << procType << ">");
                return false;
            }

            Bool_t isTopLevel = false;
            if (subNode.HasData("is-top-level"))
            {
                isTopLevel = subNode.GetData< Bool_t >("is-top-level");
            }

            if (! AddProcessor(procName, newProc, isTopLevel))
            {
                KTERROR(proclog, "Unable to add processor <" << procName << ">");
                delete newProc;
                return false;
            }
        }

        // Then deal with "connection blocks"
        itPair = node->EqualRange("connection");
        for (KTPStoreNode::const_sorted_iterator it = itPair.first; it != itPair.second; it++)
        {
            KTPStoreNode subNode = KTPStoreNode(&(it->second));
            if (! subNode.HasData("signal-processor") || ! subNode.HasData("signal-name") ||
                    ! subNode.HasData("slot-processor") || ! subNode.HasData("slot-name"))
            {
                KTERROR(proclog, "Signal/Slot connection information is incomplete!");
                if (subNode.HasData("signal-processor"))
                {
                    KTWARN(proclog, "signal-processor = " << subNode.GetData<string>("signal-processor"));
                }
                else
                {
                    KTERROR(proclog, "signal-processor = MISSING");
                }
                if (subNode.HasData("signal-name"))
                {
                    KTWARN(proclog, "signal-name = " << subNode.GetData<string>("signal-name"));
                }
                else
                {
                    KTERROR(proclog, "signal-name = MISSING");
                }
                if (subNode.HasData("slot-processor"))
                {
                    KTWARN(proclog, "slot-processor = " << subNode.GetData<string>("slot-processor"));
                }
                else
                {
                    KTERROR(proclog, "slot-processor = MISSING");
                }
                if (subNode.HasData("slot-name"))
                {
                    KTWARN(proclog, "slot-name = " << subNode.GetData<string>("slot-name"));
                }
                else
                {
                    KTERROR(proclog, "slot-namer = MISSING");
                }
                return false;
            }

            KTProcessor* signalProc = GetProcessor(subNode.GetData("signal-processor"));
            KTProcessor* slotProc = GetProcessor(subNode.GetData("slot-processor"));

            if (signalProc == NULL)
            {
                KTERROR(proclog, "Processor named <" << subNode.GetData("signal-processor") << "> was not found!");
                return false;
            }
            if (slotProc == NULL)
            {
                KTERROR(proclog, "Processor named <" << subNode.GetData("slot-processor") << "> was not found!");
                return false;
            }

            string signalName = subNode.GetData("signal-name");
            string slotName = subNode.GetData("slot-name");

            Bool_t useGroupOrdering = false;
            Int_t groupOrder = 0;
            if (subNode.HasData("group-order"))
            {
                useGroupOrdering = true;
                groupOrder = subNode.GetData< Int_t >("group-order");
            }

            try
            {
                if (useGroupOrdering)
                {
                    signalProc->ConnectASlot(signalName, slotProc, slotName, groupOrder);
                }
                else
                {
                    signalProc->ConnectASlot(signalName, slotProc, slotName);
                }
            }
            catch (std::exception& e)
            {
                KTERROR(proclog, "An error occurred while connecting signals and slots:\n"
                        << "\tSignal " << signalName << " from processor " << subNode.GetData("signal-processor") << " (a.k.a. " << signalProc->GetConfigName() << ")" << '\n'
                        << "\tSlot " << slotName << " from processor " << subNode.GetData("slot-processor") << " (a.k.a. " << slotProc->GetConfigName() << ")" << '\n'
                        << '\t' << e.what());
                return false;
            }
        }

        // Finally, deal with processor-run specifications
        const KTPStoreNode* subNodePtr = node->GetChild("run-queue");
        if (subNodePtr != NULL)
        {
            ThreadGroup threadGroup;
            for (KTPStoreNode::const_iterator iter = subNodePtr->Begin(); iter != subNodePtr->End(); iter++)
            {
                KTPStoreNode subSubNode = KTPStoreNode(&(iter->second));
                string procName = subSubNode.GetValue< string >();
                KTProcessor* procForRunQueue = GetProcessor(procName);
                KTDEBUG(proclog, "Adding processor of type " << procName << " to the run queue");
                if (procForRunQueue == NULL)
                {
                    KTERROR(proclog, "Unable to find processor <" << procName << "> requested for the run queue");
                    delete subNodePtr;
                    return false;
                }

                KTPrimaryProcessor* primaryProc = dynamic_cast< KTPrimaryProcessor* >(procForRunQueue);
                if (primaryProc == NULL)
                {
                    KTERROR(proclog, "Processor <" << procName << "> is not a primary processor.");
                    delete subNodePtr;
                    return false;
                }

                threadGroup.insert(primaryProc);
            }
            fRunQueue.push_back(threadGroup);
        }
        else
        {
            KTWARN(proclog, "No run queue was specified during configuration.");
        }
        delete subNodePtr;

        return true;
    }

    Bool_t KTProcessorToolbox::ConfigureProcessors(const KTPStoreNode* node)
    {
        for (ProcMapIt iter = fProcMap.begin(); iter != fProcMap.end(); iter++)
        {
            if (iter->second.fIsTopLevel)
            {
                KTDEBUG(proclog, "Attempting to configure top-level processor <" << iter->first << ">");
                string procName = iter->first;
                string nameUsed;
                const KTPStoreNode* subNode = node->GetChild(procName);
                if (subNode == NULL)
                {
                    string configName = iter->second.fProc->GetConfigName();
                    KTWARN(proclog, "Did not find a PSToreNode <" << procName << ">\n"
                            "\tWill check using the generic name of the processor, <" << configName << ">.");
                    subNode = node->GetChild(configName);
                    if (subNode == NULL)
                    {
                        KTWARN(proclog, "Did not find a PStoreNode <" << configName << ">\n"
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
                    KTERROR(proclog, "An error occurred while configuring processor <" << iter->first << "> with PStoreNode <" << nameUsed << ">");
                    return false;
                }
            }
        }
        return true;
    }

    Bool_t KTProcessorToolbox::Run()
    {
        UInt_t iGroup = 0;
        for (RunQueue::const_iterator rqIter = fRunQueue.begin(); rqIter != fRunQueue.end(); rqIter++)
        {
            KTDEBUG(proclog, "Starting thread group " << iGroup);
            boost::thread_group parallelThreads;
            UInt_t iThread = 0;
            for (ThreadGroup::const_iterator tgIter = rqIter->begin(); tgIter != rqIter->end(); tgIter++)
            {
                // create a boost::thread object to launch the thread
                // use boost::ref to avoid copying the processor
                KTDEBUG(proclog, "Starting thread " << iThread);
                parallelThreads.create_thread(boost::ref(**tgIter));
                iThread++;
            }
            // wait for execution to complete
            parallelThreads.join_all();
            iGroup++;
        }
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

    Bool_t KTProcessorToolbox::AddProcessor(const std::string& procName, KTProcessor* proc, Bool_t isTopLevel)
    {
        ProcMapIt it = fProcMap.find(procName);
        if (it == fProcMap.end())
        {
            ProcessorInfo pInfo;
            pInfo.fProc = proc;
            pInfo.fIsTopLevel = isTopLevel;
            fProcMap.insert(ProcMapValue(procName, pInfo));
            KTDEBUG(proclog, "Added processor <" << procName << ">");
            return true;
        }
        KTWARN(proclog, "Processor <" << procName << "> already exists; new processor was not added.");
        return false;
    }

    Bool_t KTProcessorToolbox::RemoveProcessor(const std::string& procName)
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
        return;
    }

} /* namespace Katydid */
