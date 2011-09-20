/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEgg.hh"

#include "KTArrayUC.hh"

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
                fPrelude(NULL),
                fHeader(NULL),
                fData(NULL)
    {
    }

    KTEgg::~KTEgg()
    {
        if (fEggStream.is_open()) fEggStream.close();
        delete fPrelude;
        delete fHeader;
        delete fData;
    }

    Bool_t KTEgg::BreakEgg()
    {
        fEggStream.open(fFileName.c_str(), ifstream::in|ifstream::binary);

        if (! fEggStream.is_open())
        {
            return kFALSE;
        }

        unsigned char* readBuffer = new unsigned char [sPreludeSize];
        fEggStream.read((char*)(&readBuffer[0]), sPreludeSize);
        if (! fEggStream.good()) return kFALSE;
        KTArrayUC* newPrelude = new KTArrayUC(sPreludeSize);
        newPrelude->Adopt(sPreludeSize, readBuffer);
        this->SetPrelude(newPrelude);

        stringstream conversion;
        conversion << readBuffer;
        conversion >> std::hex >> fHeaderSize;

        delete [] readBuffer;
        readBuffer = new unsigned char [fHeaderSize];
        fEggStream.read((char*)(&readBuffer[0]), fHeaderSize);
        if (! fEggStream.good()) return kFALSE;
        KTArrayUC* newHeader = new KTArrayUC(fHeaderSize);
        newHeader->Adopt(fHeaderSize, readBuffer);
        this->SetHeader(newHeader);

        delete fData;
        fData = new KTEvent();

        return kTRUE;
    }

    Bool_t KTEgg::ParseEggHeader()
    {
        mxml_node_t* tree = mxmlLoadString(NULL, (Char_t*)(fHeader->GetArray()), MXML_TEXT_CALLBACK);

        mxml_node_t* dataFormat = mxmlFindElement(tree, tree, "data_format", NULL, NULL, MXML_DESCEND);
        if (dataFormat == NULL) return kFALSE;

        stringstream conv; Int_t intConv;
        conv << mxmlElementGetAttr(dataFormat, "id");
        conv >> intConv;
        fData->SetFrameIDSize(intConv);

        stringstream conv2;
        conv2 << mxmlElementGetAttr(dataFormat, "ts");
        conv2 >> intConv;
        fData->SetTimeStampSize(intConv);

        stringstream conv3;
        conv3 << mxmlElementGetAttr(dataFormat, "data");
        conv3 >> intConv;
        fData->SetRecordSize(intConv);

        fData->SetEventSize(fData->GetFrameIDSize() + fData->GetTimeStampSize() + fData->GetRecordSize());

        stringstream conv4;
        mxml_node_t* digitizer = mxmlFindElement(tree, tree, "digitizer", NULL, NULL, MXML_DESCEND);
        if (digitizer == NULL) return kFALSE;
        conv4 << mxmlElementGetAttr(digitizer, "rate");
        conv4 >> intConv;
        fData->SetSampleRate(intConv);

        stringstream conv5;
        mxml_node_t* run = mxmlFindElement(tree, tree, "run", NULL, NULL, MXML_DESCEND);
        if (run == NULL) return kFALSE;
        conv5 << mxmlElementGetAttr(run, "length");
        conv5 >> intConv;
        fData->SetSampleLength(intConv);

        std::cout << "Parsed header\n";
        std::cout << "Frame ID Size: " << fData->GetFrameIDSize() << '\n';
        std::cout << "Time Stamp Size: " << fData->GetTimeStampSize() << '\n';
        std::cout << "Record Size: " << fData->GetRecordSize() << '\n';
        std::cout << "Sample Rate: " << fData->GetSampleRate() << '\n';
        std::cout << "Sample Length: " << fData->GetSampleLength() << std::endl;

        // these items aren't included in the header, but maybe will be someday?
        fData->SetHertzPerSampleRateUnit(1.e6);
        fData->SetSecondsPerSampleLengthUnit(1.e-3);

        return kTRUE;
    }

    Int_t KTEgg::HatchNextEvent()
    {
        Bool_t flag = kTRUE;
        unsigned char* readBuffer;

        // read the time stamp
        readBuffer = new unsigned char [fData->GetTimeStampSize()];
        fEggStream.read((char*)(&readBuffer[0]), fData->GetTimeStampSize());
        if (fEggStream.gcount() == 0) flag = kFALSE;
        else
        {
            KTArrayUC* newTimeStamp = new KTArrayUC(fData->GetTimeStampSize());
            newTimeStamp->Adopt(fData->GetTimeStampSize(), readBuffer);
            fData->SetTimeStamp(newTimeStamp);
            std::cout << "Time stamp (" << newTimeStamp->GetSize() << " chars): ";
            for (Int_t i=0; i<newTimeStamp->GetSize(); i++)
                std::cout << newTimeStamp->At(i);
            std::cout << std::endl;
        }
        //delete [] readBuffer;
        if (! fEggStream.good()) return kFALSE;

        // read the frame size
        readBuffer = new unsigned char [fData->GetFrameIDSize()];
        fEggStream.read((char*)(&readBuffer[0]), fData->GetFrameIDSize());
        if (fEggStream.gcount() == 0)  flag = kFALSE;
        else
        {
            KTArrayUC* newFrameID = new KTArrayUC(fData->GetFrameIDSize());
            newFrameID->Adopt(fData->GetFrameIDSize(), readBuffer);
            fData->SetFrameID(newFrameID);
        }
        //delete [] readBuffer;
        if (! fEggStream.good()) return kFALSE;

        // read the record
        readBuffer = new unsigned char [fData->GetRecordSize()];
        fEggStream.read((char*)(&readBuffer[0]), fData->GetRecordSize());
        if (fEggStream.gcount() == 0) flag = kFALSE;
        else
        {
            KTArrayUC* newRecord = new KTArrayUC(fData->GetRecordSize());
            newRecord->Adopt(fData->GetRecordSize(), readBuffer);
            fData->SetRecord(newRecord);
        }
        //delete [] readBuffer;
        //if (! fEggStream.good()) return kFALSE;

        return flag;
    }

    const string& KTEgg::GetFileName() const
    {
        return fFileName;
    }

    const KTEvent* KTEgg::GetData() const
    {
        return fData;
    }

    const ifstream& KTEgg::GetEggStream() const
    {
        return fEggStream;
    }

    const KTArrayUC* KTEgg::GetHeader() const
    {
        return fHeader;
    }

    const KTArrayUC* KTEgg::GetPrelude() const
    {
        return fPrelude;
    }

    void KTEgg::SetFileName(const string& fileName)
    {
        this->fFileName = fileName;
    }

    void KTEgg::SetData(KTEvent* data)
    {
        this->fData = data;
    }

    void KTEgg::SetHeaderSize(UInt_t size)
    {
        this->fHeaderSize = size;
    }

    void KTEgg::SetHeader(KTArrayUC* header)
    {
        delete fHeader;
        this->fHeader = header;
    }

    void KTEgg::SetPrelude(KTArrayUC* prelude)
    {
        delete fPrelude;
        this->fPrelude = prelude;
    }

} /* namespace Katydid */


