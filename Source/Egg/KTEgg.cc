/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEgg.hh"

#include "KTEvent.hh"

#include "KTArrayUC.hh"

#include "TArrayC.h"

#include "mxml.h"

#include <iostream>
#include <sstream>
using std::stringstream;

ClassImp(Katydid::KTEgg);

namespace Katydid
{

    const ifstream::pos_type KTEgg::sPreludeSize = 8;

    KTEgg::KTEgg() :
                fEggStream(),
                fPrelude(),
                fHeader()
    {
    }

    KTEgg::~KTEgg()
    {
        if (fEggStream.is_open()) fEggStream.close();
        //delete fPrelude;
        //delete fHeader;
    }

    Bool_t KTEgg::BreakEgg()
    {
        if (fEggStream.is_open()) fEggStream.close();

        // open the file stream
        fEggStream.open(fFileName.c_str(), ifstream::in|ifstream::binary);

        if (! fEggStream.is_open())
        {
            return kFALSE;
        }

        // read the prelude (which states how long the header is in hex)
        char* readBuffer = new char [(int)sPreludeSize];
        fEggStream.read(readBuffer, sPreludeSize);
        if (! fEggStream.good()) return kFALSE;
        string newPrelude(readBuffer, sPreludeSize);
        this->SetPrelude(newPrelude);

        // convert the prelude to the header size
        stringstream conversion;
        conversion << readBuffer;
        conversion >> std::hex >> fHeaderSize;
        std::cout << "header size: " << fHeaderSize << std::endl;

        // read the header
        delete [] readBuffer;
        readBuffer = new char [fHeaderSize];
        fEggStream.read(readBuffer, fHeaderSize);
        if (! fEggStream.good()) return kFALSE;
        string newHeader(readBuffer, fHeaderSize);
        this->SetHeader(newHeader);

        delete [] readBuffer;

        return kTRUE;
    }

    Bool_t KTEgg::ParseEggHeader()
    {
        // these items aren't included in the header, but maybe will be someday?
        this->SetHertzPerSampleRateUnit(1.e6);
        this->SetSecondsPerApproxRecordLengthUnit(1.e-3);

        //mxml_node_t* tree = mxmlLoadString(NULL, (Char_t*)(fHeader->GetArray()), MXML_TEXT_CALLBACK);
        mxml_node_t* tree = mxmlLoadString(NULL, fHeader.c_str(), MXML_TEXT_CALLBACK);

        mxml_node_t* dataFormat = mxmlFindElement(tree, tree, "data_format", NULL, NULL, MXML_DESCEND);
        if (dataFormat == NULL) return kFALSE;

        stringstream conv;
        Int_t intConv;
        conv << mxmlElementGetAttr(dataFormat, "id");
        conv >> intConv;
        this->SetFrameIDSize(intConv);

        stringstream conv2;
        conv2 << mxmlElementGetAttr(dataFormat, "ts");
        conv2 >> intConv;
        this->SetTimeStampSize(intConv);

        stringstream conv3;
        conv3 << mxmlElementGetAttr(dataFormat, "data");
        conv3 >> intConv;
        this->SetRecordSize(intConv);

        this->SetEventSize(this->GetFrameIDSize() + this->GetTimeStampSize() + this->GetRecordSize());

        stringstream conv4;
        mxml_node_t* digitizer = mxmlFindElement(tree, tree, "digitizer", NULL, NULL, MXML_DESCEND);
        if (digitizer == NULL) return kFALSE;
        conv4 << mxmlElementGetAttr(digitizer, "rate");
        conv4 >> intConv;
        this->SetSampleRate((Double_t)intConv * this->GetHertzPerSampleRateUnit());

        stringstream conv5;
        mxml_node_t* run = mxmlFindElement(tree, tree, "run", NULL, NULL, MXML_DESCEND);
        if (run == NULL) return kFALSE;
        conv5 << mxmlElementGetAttr(run, "length");
        conv5 >> intConv;
        this->SetApproxRecordLength((Double_t)intConv * this->GetSecondsPerApproxRecordLengthUnit());

        std::cout << "Parsed header\n";
        std::cout << "Frame ID Size: " << this->GetFrameIDSize() << '\n';
        std::cout << "Time Stamp Size: " << this->GetTimeStampSize() << '\n';
        std::cout << "Record Size: " << this->GetRecordSize() << '\n';
        std::cout << "Approximate Record Length: " << this->GetApproxRecordLength() << " s" << '\n';
        std::cout << "Sample Rate: " << this->GetSampleRate() << " Hz " << '\n';

        return kTRUE;
    }

    KTEvent* KTEgg::HatchNextEvent()
    {
        if (! fEggStream.good()) return NULL;

        KTEvent* event = new KTEvent();

        unsigned char* readBuffer;

        // read the time stamp
        readBuffer = new unsigned char [this->GetTimeStampSize()];
        fEggStream.read((char*)(&readBuffer[0]), this->GetTimeStampSize());
        if (fEggStream.gcount() == 0)
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: No data was read for the time stamp" << std::endl;
            delete [] readBuffer;
        }
        else
        {
            KTArrayUC* newTimeStamp = new KTArrayUC(this->GetTimeStampSize());
            newTimeStamp->Adopt(this->GetTimeStampSize(), readBuffer);
            event->SetTimeStamp(newTimeStamp);
            std::cout << "Time stamp (" << newTimeStamp->GetSize() << " chars): ";
            for (Int_t i=0; i<newTimeStamp->GetSize(); i++)
                std::cout << (*newTimeStamp)[i];
            std::cout << std::endl;
        }
        if (! fEggStream.good())
        {
            delete event;
            return NULL;
        }

        // read the frame size
        readBuffer = new unsigned char [this->GetFrameIDSize()];
        fEggStream.read((char*)(&readBuffer[0]), this->GetFrameIDSize());
        if (fEggStream.gcount() == 0)
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: No data was read for the frame ID" << std::endl;
            delete [] readBuffer;
        }
        else
        {
            KTArrayUC* newFrameID = new KTArrayUC(this->GetFrameIDSize());
            newFrameID->Adopt(this->GetFrameIDSize(), readBuffer);
            event->SetFrameID(newFrameID);
        }
        if (! fEggStream.good())
        {
            delete event;
            return NULL;
        }

        // read the record
        readBuffer = new unsigned char [this->GetRecordSize()];
        fEggStream.read((char*)(&readBuffer[0]), this->GetRecordSize());
        if (fEggStream.gcount() == 0)
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: No data was read for the record" << std::endl;
            delete [] readBuffer;
        }
        else
        {
            KTArrayUC* newRecord = new KTArrayUC(this->GetRecordSize());
            newRecord->Adopt(this->GetRecordSize(), readBuffer);
            event->SetRecord(newRecord);
        }
        //if (! fEggStream.good()) return kFALSE;

        //
        event->SetSampleRate(this->GetSampleRate());
        event->SetBinWidth(1. / this->GetSampleRate());
        event->SetRecordLength((Double_t)(this->GetRecordSize()) * event->GetBinWidth());

        return event;
    }

    const string& KTEgg::GetFileName() const
    {
        return fFileName;
    }

    const ifstream& KTEgg::GetEggStream() const
    {
        return fEggStream;
    }

    const string& KTEgg::GetHeader() const
    {
        return fHeader;
    }

    const string& KTEgg::GetPrelude() const
    {
        return fPrelude;
    }

    Int_t KTEgg::GetEventSize() const
    {
        return fEventSize;
    }

    Int_t KTEgg::GetFrameIDSize() const
    {
        return fFrameIDSize;
    }

    Int_t KTEgg::GetRecordSize() const
    {
        return fRecordSize;
    }

    Double_t KTEgg::GetApproxRecordLength() const
    {
        return fApproxRecordLength;
    }

    Double_t KTEgg::GetSampleRate() const
    {
        return fSampleRate;
    }

    Int_t KTEgg::GetTimeStampSize() const
    {
        return fTimeStampSize;
    }

    Double_t KTEgg::GetHertzPerSampleRateUnit() const
    {
        return fHertzPerSampleRateUnit;
    }

    Double_t KTEgg::GetSecondsPerApproxRecordLengthUnit() const
    {
        return fSecondsPerApproxRecordLengthUnit;
    }


    void KTEgg::SetFileName(const string& fileName)
    {
        this->fFileName = fileName;
    }

    void KTEgg::SetHeaderSize(UInt_t size)
    {
        this->fHeaderSize = size;
    }

    void KTEgg::SetHeader(const string& header)
    {
        //delete fHeader;
        this->fHeader = header;
    }

    void KTEgg::SetPrelude(const string& prelude)
    {
        //delete fPrelude;
        this->fPrelude = prelude;
    }

    void KTEgg::SetEventSize(Int_t size)
    {
        fEventSize = size;
    }

    void KTEgg::SetFrameIDSize(Int_t size)
    {
        fFrameIDSize = size;
    }

    void KTEgg::SetRecordSize(Int_t size)
    {
        fRecordSize = size;
    }

    void KTEgg::SetApproxRecordLength(Double_t length)
    {
        fApproxRecordLength = length;
    }

    void KTEgg::SetSampleRate(Double_t rate)
    {
        fSampleRate = rate;
    }

    void KTEgg::SetTimeStampSize(Int_t size)
    {
        fTimeStampSize = size;
    }

    void KTEgg::SetHertzPerSampleRateUnit(Double_t hpsru)
    {
        fHertzPerSampleRateUnit = hpsru;
    }

    void KTEgg::SetSecondsPerApproxRecordLengthUnit(Double_t spslu)
    {
        fSecondsPerApproxRecordLengthUnit = spslu;
    }

} /* namespace Katydid */


