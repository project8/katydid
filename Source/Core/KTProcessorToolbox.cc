/*
 * KTProcessorToolbox.cc
 *
 *  Created on: Sep 27, 2012
 *      Author: nsoblath
 */

#include "KTProcessorToolbox.hh"

#include "KTLogger.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(proclog, "katydid.core");

    KTProcessorToolbox::KTProcessorToolbox() :
            KTConfigurable(),
            fProcFactory(KTFactory< KTProcessor >::GetInstance()),
            fProcMap()
    {
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
            if (! AddProcessor(procName, newProc))
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
            if (! subNode.HasData("signal_processor") || ! subNode.HasData("signal_name") ||
                    ! subNode.HasData("slot_processor") || ! subNode.HasData("slot_name"))
            {
                KTERROR(proclog, "Signal/Slot connection information is incomplete!");
                return false;
            }

            KTProcessor* signalProc = GetProcessor(subNode.GetData("signal_processor"));
            KTProcessor* slotProc = GetProcessor(subNode.GetData("slot_processor"));

            if (signalProc == NULL)
            {
                KTERROR(proclog, "Processor named <" << subNode.GetData("signal_processor") << "> was not found!");
                return false;
            }
            if (slotProc == NULL)
            {
                KTERROR(proclog, "Processor named <" << subNode.GetData("slot_processor") << "> was not found!");
                return false;
            }

            string signalName = subNode.GetData("signal_name");
            string slotName = subNode.GetData("slot_name");

            try
            {
                signalProc->ConnectASlot(signalName, slotProc, slotName);
            }
            catch (std::exception& e)
            {
                KTERROR(proclog, "An error occurred while connecting signals and slots:\n"
                        << "\tSignal " << signalName << " from processor " << subNode.GetData("signal_processor") << '\n'
                        << "\tSlot " << slotName << " from processor " << subNode.GetData("slot_processor") << '\n'
                        << '\t' << e.what());
                return false;
            }
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
        return it->second;
    }

    const KTProcessor* KTProcessorToolbox::GetProcessor(const std::string& procName) const
    {
        ProcMapCIt it = fProcMap.find(procName);
        if (it == fProcMap.end())
        {
            KTWARN(proclog, "Processor <" << procName << "> was not found.");
            return NULL;
        }
        return it->second;
    }

    Bool_t KTProcessorToolbox::AddProcessor(const std::string& procName, KTProcessor* proc)
    {
        ProcMapIt it = fProcMap.find(procName);
        if (it == fProcMap.end())
        {
            fProcMap.insert(ProcMapValue(procName, proc));
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
        KTProcessor* procToRelease = it->second;
        fProcMap.erase(it);
        return procToRelease;
    }

    void KTProcessorToolbox::ClearProcessors()
    {
        for (ProcMapIt it = fProcMap.begin(); it != fProcMap.end(); it++)
        {
            delete it->second;
        }
        fProcMap.clear();
        return;
    }

} /* namespace Katydid */
