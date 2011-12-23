/*
 * KTEgg.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEgg.hh"

#include "KTEvent.hh"

#include "rapidxml.hpp"
//#include "rapidxml_print.hpp"

#include <iostream>
#include <vector>
using std::vector;

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

        rapidxml::xml_document<char> headerDOM;
        char* headerCopy = new char [fHeader.size() + 1];
        strcpy(headerCopy, fHeader.c_str());
        try
        {
            headerDOM.parse<0>(headerCopy);
        }
        catch (rapidxml::parse_error& e)
        {
            std::cout << e.what() << std::endl;
            return kFALSE;
        }
        //std::cout << headerDOM;

        rapidxml::xml_node<char>* nodeHeader = headerDOM.first_node("header");
        if (nodeHeader == NULL)
        {
            std::cerr << "No header node" << std::endl;
            return kFALSE;
        }

        rapidxml::xml_node<char>* nodeDataFormat = nodeHeader->first_node("data_format");
        if (nodeDataFormat == NULL)
        {
            std::cerr << "No data format node" << std::endl;
            return kFALSE;
        }

        rapidxml::xml_attribute<char>* attr = nodeDataFormat->first_attribute("id");
        if (attr == NULL)        {
            std::cerr << "No id attribute in the data format node" << std::endl;
            return kFALSE;
        };
        this->SetFrameIDSize(ConvertFromCharArray< Int_t >(attr->value()));

        attr = nodeDataFormat->first_attribute("ts");
        if (attr == NULL)
        {
            std::cerr << "No ts attribute in the data format node" << std::endl;
            return kFALSE;
        }
        this->SetTimeStampSize(ConvertFromCharArray< Int_t >(attr->value()));

        attr = nodeDataFormat->first_attribute("data");
        if (attr == NULL)
        {
            std::cerr << "No data attribute in the data format node" << std::endl;
            return kFALSE;
        }
        this->SetRecordSize(ConvertFromCharArray< Int_t >(attr->value()));

        this->SetEventSize(this->GetFrameIDSize() + this->GetTimeStampSize() + this->GetRecordSize());

        rapidxml::xml_node<char>* nodeDigitizer = nodeHeader->first_node("digitizer");
        if (nodeDigitizer == NULL)
        {
            std::cerr << "No digitizer node" << std::endl;
            return kFALSE;
        }

        attr = nodeDigitizer->first_attribute("rate");
        if (attr == NULL)
        {
            std::cerr << "No rate attribute in the digitizer node" << std::endl;
            return kFALSE;
        }
        this->SetSampleRate(ConvertFromCharArray< Double_t >(attr->value()) * this->GetHertzPerSampleRateUnit());

        rapidxml::xml_node<char>* nodeRun = nodeHeader->first_node("run");
        if (nodeRun == NULL)
        {
            std::cerr << "No run node" << std::endl;
            return kFALSE;
        }

        attr = nodeRun->first_attribute("length");
        if (attr == NULL)        {
            std::cerr << "No length attribute in the run node" << std::endl;
            return kFALSE;
        }
        this->SetApproxRecordLength(ConvertFromCharArray< Double_t >(attr->value()) * this->GetSecondsPerApproxRecordLengthUnit());

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
        if (fEggStream.gcount() != this->GetTimeStampSize())
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: Size of the data read for the time stamp did not match the size expected" << std::endl;
            std::cerr << "   Expected: " << this->GetTimeStampSize() << "   Read: " << fEggStream.gcount() << std::endl;
            delete [] readBuffer;
        }
        else
        {
            vector< UInt_t > newTimeStamp(readBuffer, readBuffer + this->GetTimeStampSize()/sizeof(unsigned char));
            delete [] readBuffer;
            event->SetTimeStamp(newTimeStamp);
            std::cout << "Time stamp (" << newTimeStamp.size() << " chars): ";
            for (int i=0; i<newTimeStamp.size(); i++)
                std::cout << newTimeStamp[i];
            std::cout << std::endl;
        }
        if (! fEggStream.good())
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: Reached end of file after reading time stamp size" << std::endl;
            delete event;
            return NULL;
        }

        // read the frame size
        readBuffer = new unsigned char [this->GetFrameIDSize()];
        fEggStream.read((char*)(&readBuffer[0]), this->GetFrameIDSize());
        if (fEggStream.gcount() != this->GetFrameIDSize())
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: The size of the data read for the frame ID did not match the expected size" << std::endl;
            std::cerr << "   Expected: " << this->GetFrameIDSize() << "   Read: " << fEggStream.gcount() << std::endl;
            delete [] readBuffer;
        }
        else
        {
            vector< UInt_t > newFrameID(readBuffer, readBuffer + this->GetFrameIDSize()/sizeof(unsigned char));
            delete [] readBuffer;
            event->SetFrameID(newFrameID);
        }
        if (! fEggStream.good())
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: Reached end of file after reading frame size" << std::endl;
            delete event;
            return NULL;
        }

        // read the record
        readBuffer = new unsigned char [this->GetRecordSize()];
        fEggStream.read((char*)(&readBuffer[0]), this->GetRecordSize());
        if (fEggStream.gcount() != this->GetRecordSize())
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: Size of the data read for the record did not match the amount expected" << std::endl;
            std::cerr << "   Expected: :" << this->GetRecordSize() << "  Read: " << fEggStream.gcount() << std::endl;
            delete [] readBuffer;
            delete event;
            return NULL;
        }
        else
        {
            vector< UInt_t > newRecord(readBuffer, readBuffer + this->GetRecordSize()/sizeof(unsigned char));
            delete [] readBuffer;
            event->SetRecord(newRecord);
        }
        if (! fEggStream.good())
        {
            std::cerr << "Warning from KTEgg::HatchNextEvent: Egg stream state is not good after reading in this event." << std::endl;
        }

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

    UInt_t KTEgg::GetHeaderSize() const
    {
        return fHeaderSize;
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


