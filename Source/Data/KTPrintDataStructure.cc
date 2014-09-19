/*
 * KTPrintDataStructure.cc
 *
 *  Created on: Sept 12, 2014
 *      Author: N.S. Oblath
 */

#include "KTPrintDataStructure.hh"

#include "KTLogger.hh"
#include "KTParam.hh"

#include <sstream>

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(datalog, "KTPrintDataStructure");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTPrintDataStructure, "print-data-structure");

    KTPrintDataStructure::KTPrintDataStructure(const std::string& name) :
            KTProcessor(name),
            fDataSignal("data", this),
            fDataStructSlot("print-data", this, &KTPrintDataStructure::PrintDataStructure)
    {
    }

    KTPrintDataStructure::~KTPrintDataStructure()
    {
    }

    bool KTPrintDataStructure::Configure(const KTParamNode* node)
    {
        return true;
    }

    void KTPrintDataStructure::PrintDataStructure(KTDataPtr dataPtr)
    {
        std::stringstream printbuf;
        printbuf << "Data Structure:\n";
        printbuf << "\t- " << dataPtr->Name() << '\n';
        KTDEBUG(datalog, "Found data type " << dataPtr->Name());
        KTDataCore* nextData = dataPtr->Next();
        while (nextData != NULL)
        {
            printbuf << "\t- " << nextData->Name() << '\n';
            KTDEBUG(datalog, "Found data type " << nextData->Name());
            nextData = nextData->Next();
        }
        KTINFO(datalog, printbuf.str());

        fDataSignal(dataPtr);

        return;
    }

    //void KTPrintDataStructure::PrintCutStructure(KTDataPtr dataPtr);
    //void KTPrintDataStructure::PrintDataAndCutStructure(KTDataPtr dataPtr);

} /* namespace Katydid */
