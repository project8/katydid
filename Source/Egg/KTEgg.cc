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

using std::ifstream;
using std::fstream;
using std::string;
using std::stringstream;
using std::vector;

namespace Katydid
{

    const ifstream::pos_type KTEgg::sPreludeSize = 9;

    KTEgg::KTEgg() :
            fFileName(),
            fEggStream(),
            fPrelude(),
            fHeaderSize(0),
            fHeader(),
            fHeaderInfo()
    {
    }

    KTEgg::~KTEgg()
    {
        if (fEggStream.is_open()) fEggStream.close();
    }

    bool KTEgg::BreakEgg()
    {
        if (fEggStream.is_open()) fEggStream.close();

        // open the file stream
        fEggStream.open(fFileName.c_str(), ifstream::in|ifstream::binary);

        if (! fEggStream.is_open())
        {
            return false;
        }

        // read the prelude (which states how long the header is in hex)
        // add one to the size of the array to allow it to terminate in a null character
        //int readBufferSize = (int)sPreludeSize + 1;
        char* readBuffer = new char [(int)sPreludeSize];
        int readSize = (int)sPreludeSize - 1;
        fEggStream.read(readBuffer, readSize);
        if (! fEggStream.good()) return false;
        string newPrelude(readBuffer, sPreludeSize);
        this->SetPrelude(newPrelude);

        // convert the prelude to the header size
        stringstream conversion;
        conversion << readBuffer;
        conversion >> std::hex >> fHeaderSize;
        //std::cout << "header size: " << fHeaderSize << std::endl;

        // read the header
        delete [] readBuffer;
        readBuffer = new char [fHeaderSize];
        fEggStream.read(readBuffer, fHeaderSize);
        if (! fEggStream.good()) return false;
        string newHeader(readBuffer, fHeaderSize);
        this->SetHeader(newHeader);
        std::cout << "Header: " << newHeader << std::endl;

        delete [] readBuffer;

        return true;
    }

    bool KTEgg::ParseEggHeader()
    {
        // these items aren't included in the header, but maybe will be someday?
        this->SetHertzPerSampleRateUnit(1.e6);
        this->SetSecondsPerRunLengthUnit(1.e-3);

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
            return false;
        }
        //std::cout << headerDOM;

        rapidxml::xml_node<char>* nodeHeader = headerDOM.first_node("header");
        if (nodeHeader == NULL)
        {
            std::cerr << "No header node" << std::endl;
            return false;
        }

        rapidxml::xml_node<char>* nodeDataFormat = nodeHeader->first_node("data_format");
        if (nodeDataFormat == NULL)
        {
            std::cerr << "No data format node" << std::endl;
            return false;
        }

        rapidxml::xml_attribute<char>* attr = nodeDataFormat->first_attribute("id");
        if (attr == NULL)        {
            std::cerr << "No id attribute in the data format node" << std::endl;
            return false;
        };
        this->SetFrameIDSize(ConvertFromCharArray< int >(attr->value()));

        attr = nodeDataFormat->first_attribute("ts");
        if (attr == NULL)
        {
            std::cerr << "No ts attribute in the data format node" << std::endl;
            return false;
        }
        this->SetTimeStampSize(ConvertFromCharArray< int >(attr->value()));

        attr = nodeDataFormat->first_attribute("data");
        if (attr == NULL)
        {
            std::cerr << "No data attribute in the data format node" << std::endl;
            return false;
        }
        this->SetRecordSize(ConvertFromCharArray< int >(attr->value()));

        this->SetEventSize(this->GetFrameIDSize() + this->GetTimeStampSize() + this->GetRecordSize());

        rapidxml::xml_node<char>* nodeDigitizer = nodeHeader->first_node("digitizer");
        if (nodeDigitizer == NULL)
        {
            std::cerr << "No digitizer node" << std::endl;
            return false;
        }

        attr = nodeDigitizer->first_attribute("rate");
        if (attr == NULL)
        {
            std::cerr << "No rate attribute in the digitizer node" << std::endl;
            return false;
        }
        this->SetSampleRate(ConvertFromCharArray< double >(attr->value()) * this->GetHertzPerSampleRateUnit());

        rapidxml::xml_node<char>* nodeRun = nodeHeader->first_node("run");
        if (nodeRun == NULL)
        {
            std::cerr << "No run node" << std::endl;
            return false;
        }

        attr = nodeRun->first_attribute("length");
        if (attr == NULL)        {
            std::cerr << "No length attribute in the run node" << std::endl;
            return false;
        }
        this->SetRunLength(ConvertFromCharArray< double >(attr->value()) * this->GetSecondsPerRunLengthUnit());

        delete [] headerCopy;

        std::cout << "Parsed header:\n";
        std::cout << "Frame ID Size: " << this->GetFrameIDSize() << '\n';
        std::cout << "Time Stamp Size: " << this->GetTimeStampSize() << '\n';
        std::cout << "Record Size: " << this->GetRecordSize() << '\n';
        std::cout << "Run Length: " << this->GetRunLength() << " s" << '\n';
        std::cout << "Sample Rate: " << this->GetSampleRate() << " Hz " << '\n';

        return true;
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
            vector< unsigned > newTimeStamp(readBuffer, readBuffer + this->GetTimeStampSize()/sizeof(unsigned char));
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
            vector< unsigned > newFrameID(readBuffer, readBuffer + this->GetFrameIDSize()/sizeof(unsigned char));
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
            vector< unsigned > newRecord(readBuffer, readBuffer + this->GetRecordSize()/sizeof(unsigned char));
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
        event->SetRecordLength((double)(this->GetRecordSize()) * event->GetBinWidth());

        return event;
    }

} /* namespace Katydid */


