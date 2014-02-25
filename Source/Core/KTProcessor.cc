/*
 * KTProcessor.cc
 *
 *  Created on: Jan 5, 2012
 *      Author: nsoblath
 */

#include "KTProcessor.hh"

//#include "KTLogger.hh"

#include <boost/foreach.hpp>

#include <string>

using std::string;

namespace Katydid
{
    //KTLOGGER(proclog, "KTProcessor");

    ProcessorException::ProcessorException (std::string const& why)
      : std::logic_error(why)
    {}


    KTProcessor::KTProcessor(const string& name) :
            KTConfigurable(name),
            fSignalMap(),
            fSlotMap()
    {
    }

    KTProcessor::~KTProcessor()
    {
        for (SlotMapIt iter = fSlotMap.begin(); iter != fSlotMap.end(); iter++)
        {
            iter->second->Disconnect();
            delete iter->second;
        }
        for (SigMapIt iter = fSignalMap.begin(); iter != fSignalMap.end(); iter++)
        {
            delete iter->second;
        }
    }

    void KTProcessor::ConnectASlot(const std::string& signalName, KTProcessor* processor, const std::string& slotName, int groupNum)
    {
        KTSignalWrapper* signal = GetSignal(signalName);
        KTSlotWrapper* slot = processor->GetSlot(slotName);

        try
        {
            ConnectSignalToSlot(signal, slot, groupNum);
        }
        catch (std::exception& e)
        {
            string errorMsg = string("Exception caught in KTProcessor::ConnectASignal; signal: ") +
                    signalName + string(", slot: ") + slotName + string("\n") + e.what() + string("\n") +
                    string("\tIf the signal wrapper cannot be cast correctly, check that the signatures of the signal and slot match exactly.\n") +
                    string("\tIf the signal pointer is NULL, you may have the signal name wrong.\n") +
                    string("\tIf the slot pointer is NULL, you may have the slot name wrong.");
            throw std::logic_error(errorMsg);
        }
        KTDEBUG(processorlog, "Connected signal <" << signalName << "> to slot <" << slotName << ">");

        return;
    }

    void KTProcessor::ConnectASignal(KTProcessor* processor, const std::string& signalName, const std::string& slotName, int groupNum)
    {
        KTSignalWrapper* signal = processor->GetSignal(signalName);
        KTSlotWrapper* slot = GetSlot(slotName);

        try
        {
            ConnectSignalToSlot(signal, slot, groupNum);
        }
        catch (std::exception& e)
        {
            string errorMsg = string("Exception caught in KTProcessor::ConnectASignal; signal: ") +
                    signalName + string(", slot: ") + slotName + string("\n") + e.what() + string("\n") +
                    string("Check that the signatures of the signal and slot match exactly.");
            throw std::logic_error(errorMsg);
        }
        KTDEBUG(processorlog, "Connected slot <" << signalName << "> to signal <" << slotName << ">");

        return;
    }

    void KTProcessor::ConnectSignalToSlot(KTSignalWrapper* signal, KTSlotWrapper* slot, int groupNum)
    {
        if (signal == NULL)
        {
            throw ProcessorException("Signal pointer was NULL");
        }
        if (slot == NULL)
        {
            throw ProcessorException("Slot pointer was NULL");
        }

        slot->SetConnection(signal, groupNum);

        return;
    }

    KTSignalWrapper* KTProcessor::GetSignal(const std::string& name)
    {
        SigMapIt iter = fSignalMap.find(name);
        if (iter == fSignalMap.end())
        {
            return NULL;
        }
        return iter->second;
    }

    KTSlotWrapper* KTProcessor::GetSlot(const std::string& name)
    {
        SlotMapIt iter = fSlotMap.find(name);
        if (iter == fSlotMap.end())
        {
            return NULL;
        }
        return iter->second;
    }



} /* namespace Katydid */
