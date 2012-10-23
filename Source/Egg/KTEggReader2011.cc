/*
 * KTEggReader2011.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#include "KTEggReader2011.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesDataReal.hh"
#include "KTTimeSeriesReal.hh"

#include "rapidxml.hpp"
//#include "rapidxml_print.hpp"

#include <cstring>
#include <vector>

using std::ifstream;
using std::fstream;
using std::string;
using std::stringstream;
using std::vector;


namespace Katydid
{
    KTLOGGER(eggreadlog, "katydid.egg");

    const ifstream::pos_type KTEggReader2011::sPreludeSize = 9;

    KTEggReader2011::KTEggReader2011() :
            KTEggReader(),
            fFileName(),
            fEggStream(),
            fPrelude(),
            fHeaderSize(0),
            fHeader(),
            fHeaderInfo()
    {
    }

    KTEggReader2011::~KTEggReader2011()
    {
    }

    KTEggHeader* KTEggReader2011::BreakEgg(const std::string& filename)
    {
        // First, read all of the information from the file and put it in the right places
        if (fEggStream.is_open()) fEggStream.close();

        // open the file stream
        fEggStream.open(filename.c_str(), ifstream::in|ifstream::binary);

        if (! fEggStream.is_open())
        {
            KTERROR(eggreadlog, "Egg filestream did not open (file: " << filename << ")");
            return NULL;
        }

        // read the prelude (which states how long the header is in hex)
        // add one to the size of the array to allow it to terminate in a null character
        //int readBufferSize = (int)sPreludeSize + 1;
        char* readBuffer = new char [(int)sPreludeSize];
        int readSize = (int)sPreludeSize - 1;
        fEggStream.read(readBuffer, readSize);
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Egg filestream is bad after reading the prelude");
            delete [] readBuffer;
            return NULL;
        }
        string newPrelude(readBuffer, sPreludeSize);
        fPrelude = newPrelude;

        // convert the prelude to the header size
        stringstream conversion;
        conversion << readBuffer;
        conversion >> std::hex >> fHeaderSize;
        //KTDEBUG("header size: " << fHeaderSize);

        // read the header
        delete [] readBuffer;
        readBuffer = new char [fHeaderSize+1];
        fEggStream.read(readBuffer, fHeaderSize);
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Egg filestream is bad after reading the header");
            delete [] readBuffer;
            return NULL;
        }
        readBuffer[fHeaderSize] = '\0';
        string newHeader(readBuffer);
        fHeader = newHeader;
        KTDEBUG(eggreadlog, "Header: " << newHeader);

        delete [] readBuffer;

        // All data has been read from the file at this point

        // Parse the header

        // Non-SI unit conversions; not included in the 2011 header
        fHeaderInfo.fHertzPerSampleRateUnit = 1.e6;
        fHeaderInfo.fSecondsPerRunLengthUnit = 1.e-3;

        // Parse the XML header
        rapidxml::xml_document<char> headerDOM;
        char* headerCopy = new char [fHeader.size()+1];
        strcpy(headerCopy, fHeader.c_str());
        try
        {
            headerDOM.parse<0>(headerCopy);
        }
        catch (rapidxml::parse_error& e)
        {
            KTERROR(eggreadlog, "Caught exception while parsing header:\n" <<
                    '\t' << e.what() << '\n' <<
                    '\t' << e.where<char>());
            return NULL;
        }
        //std::cout << headerDOM;

        rapidxml::xml_node<char>* nodeHeader = headerDOM.first_node("header");
        if (nodeHeader == NULL)
        {
            KTERROR(eggreadlog, "No header node");
            return NULL;
        }

        rapidxml::xml_node<char>* nodeDataFormat = nodeHeader->first_node("data_format");
        if (nodeDataFormat == NULL)
        {
            KTERROR(eggreadlog, "No data format node");
            return NULL;
        }

        rapidxml::xml_attribute<char>* attr = nodeDataFormat->first_attribute("id");
        if (attr == NULL)        {
            KTERROR(eggreadlog, "No id attribute in the data format node");
            return NULL;
        };
        fHeaderInfo.fFrameIDSize = ConvertFromArray< int >(attr->value());

        attr = nodeDataFormat->first_attribute("ts");
        if (attr == NULL)
        {
            KTERROR(eggreadlog, "No ts attribute in the data format node");
            return NULL;
        }
        fHeaderInfo.fTimeStampSize = ConvertFromArray< int >(attr->value());

        attr = nodeDataFormat->first_attribute("data");
        if (attr == NULL)
        {
            KTERROR(eggreadlog, "No data attribute in the data format node");
            return NULL;
        }
        fHeaderInfo.fRecordSize = ConvertFromArray< int >(attr->value());

        fHeaderInfo.fEventSize = fHeaderInfo.fFrameIDSize + fHeaderInfo.fTimeStampSize + fHeaderInfo.fRecordSize;

        rapidxml::xml_node<char>* nodeDigitizer = nodeHeader->first_node("digitizer");
        if (nodeDigitizer == NULL)
        {
            KTERROR(eggreadlog, "No digitizer node");
            return NULL;
        }

        attr = nodeDigitizer->first_attribute("rate");
        if (attr == NULL)
        {
            KTERROR(eggreadlog, "No rate attribute in the digitizer node");
            return NULL;
        }
        fHeaderInfo.fSampleRate = ConvertFromArray< double >(attr->value()) * fHeaderInfo.fHertzPerSampleRateUnit;

        rapidxml::xml_node<char>* nodeRun = nodeHeader->first_node("run");
        if (nodeRun == NULL)
        {
            KTERROR(eggreadlog, "No run node");
            return NULL;
        }

        attr = nodeRun->first_attribute("length");
        if (attr == NULL)        {
            KTERROR(eggreadlog, "No length attribute in the run node");
            return NULL;
        }
        fHeaderInfo.fRunLength = ConvertFromArray< double >(attr->value()) * fHeaderInfo.fSecondsPerRunLengthUnit;

        delete [] headerCopy;

        KTDEBUG("Parsed header:\n"
             << "\tFrame ID Size: " << fHeaderInfo.fFrameIDSize << '\n'
             << "\tTime Stamp Size: " << fHeaderInfo.fTimeStampSize << '\n'
             << "\tRecord Size: " << fHeaderInfo.fRecordSize << '\n'
             << "\tRun Length: " << fHeaderInfo.fRunLength << " s" << '\n'
             << "\tSample Rate: " << fHeaderInfo.fSampleRate << " Hz ");

        // Everything should have been done correctly at this point,
        // and we're ready to create, fill, and return the KTEggHeader

        KTEggHeader* eggHeader = new KTEggHeader();
        eggHeader->SetFilename(filename);
        eggHeader->SetAcquisitionMode(sOneChannel);
        eggHeader->SetRecordSize(fHeaderInfo.fRecordSize);
        eggHeader->SetMonarchRecordSize(fHeaderInfo.fRecordSize);
        eggHeader->SetAcquisitionTime(fHeaderInfo.fRunLength * fHeaderInfo.fSecondsPerRunLengthUnit);
        eggHeader->SetAcquisitionRate(fHeaderInfo.fSampleRate * fHeaderInfo.fHertzPerSampleRateUnit);

        return eggHeader;
    }

    KTTimeSeriesData* KTEggReader2011::HatchNextEvent()
    {
        if (! fEggStream.good()) return NULL;

        KTTimeSeriesData* eventData = new KTTimeSeriesDataReal(1);

        unsigned char* readBuffer;

        // read the time stamp
        readBuffer = new unsigned char [fHeaderInfo.fTimeStampSize];
        fEggStream.read((char*)(&readBuffer[0]), fHeaderInfo.fTimeStampSize);
        if (fEggStream.gcount() != fHeaderInfo.fTimeStampSize)
        {
            KTWARN(eggreadlog, "Size of the data read for the time stamp did not match the size expected\n"
                    << "\tExpected: " << fHeaderInfo.fTimeStampSize << '\n'
                    << "\tRead: " << fEggStream.gcount());
            delete [] readBuffer;
        }
        else
        {
            //vector< unsigned > newTimeStamp(readBuffer, readBuffer + fHeaderInfo.fTimeStampSize/sizeof(unsigned char));
            unsigned long int newTimeStamp = ConvertFromArray< unsigned long >(readBuffer);
            delete [] readBuffer;
            eventData->SetTimeStamp(newTimeStamp);
            //std::cout << "Time stamp (" << newTimeStamp.size() << " chars): ";
            //for (int i=0; i<newTimeStamp.size(); i++)
            //    std::cout << newTimeStamp[i];
            //std::cout << std::endl;
        }
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Reached end of file after reading time stamp size");
            delete eventData;
            return NULL;
        }

        // channel number is always 0
        eventData->SetChannelID(0);

        // read the frame size
        readBuffer = new unsigned char [fHeaderInfo.fFrameIDSize];
        fEggStream.read((char*)(&readBuffer[0]), fHeaderInfo.fFrameIDSize);
        if (fEggStream.gcount() != fHeaderInfo.fFrameIDSize)
        {
            KTWARN(eggreadlog, "The size of the data read for the frame ID did not match the expected size\n"
                    << "\tExpected: " << fHeaderInfo.fFrameIDSize << '\n'
                    << "\tRead: " << fEggStream.gcount());
            delete [] readBuffer;
        }
        else
        {
            //vector< unsigned > newFrameID(readBuffer, readBuffer + fHeaderInfo.fFrameIDSize()/sizeof(unsigned char));
            unsigned newFrameID = ConvertFromArray< unsigned >(readBuffer);
            delete [] readBuffer;
            eventData->SetAcquisitionID(newFrameID);
        }
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Reached end of file after reading frame size");
            delete eventData;
            return NULL;
        }

        // Other information
        eventData->SetSampleRate(double(fHeaderInfo.fSampleRate));
        eventData->SetBinWidth(1. / double(fHeaderInfo.fSampleRate));
        eventData->SetRecordSize(fHeaderInfo.fRecordSize);
        eventData->SetRecordLength(double(fHeaderInfo.fRecordSize) * eventData->GetBinWidth());

        // read the record
        readBuffer = new unsigned char [fHeaderInfo.fRecordSize];
        fEggStream.read((char*)(&readBuffer[0]), fHeaderInfo.fRecordSize);
        if (fEggStream.gcount() != fHeaderInfo.fRecordSize)
        {
            KTWARN(eggreadlog, "Size of the data read for the record did not match the amount expected"
                    << "\tExpected: :" << fHeaderInfo.fRecordSize << '\n'
                    << "\tRead: " << fEggStream.gcount());
            delete [] readBuffer;
            delete eventData;
            return NULL;
        }
        else
        {
            //vector< DataType >* newRecord = new vector< DataType >(readBuffer, readBuffer + fHeaderInfo.fRecordSize/sizeof(unsigned char));
            KTTimeSeries* newRecord = new KTTimeSeriesReal(fHeaderInfo.fRecordSize, 0., Double_t(fHeaderInfo.fRecordSize) * eventData->GetBinWidth());
            for (unsigned iBin=0; iBin<fHeaderInfo.fRecordSize; iBin++)
            {
                //(*newRecord)(iBin) = Double_t(readBuffer[iBin]);
                newRecord->SetValue(iBin, Double_t(readBuffer[iBin]));
            }
            delete [] readBuffer;
            eventData->SetRecord(newRecord);
        }
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Warning from KTEgg::HatchNextEvent: Egg stream state is not good after reading in this event.");
        }

        return eventData;
    }

    bool KTEggReader2011::CloseEgg()
    {
        if (fEggStream.is_open()) fEggStream.close();
        return true;
    }


} /* namespace Katydid */
