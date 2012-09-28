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
            KTFactory< KTProcessor >(),
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
            KTPStoreNode subNode = KTPStoreNode(it->second);
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
            string procName = /* extract the processor key*/;
            KTProcessor* newProc = this->Create(procType);
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
