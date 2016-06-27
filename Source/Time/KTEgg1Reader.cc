/*
 * KTEgg1Reader.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#include "KTEgg1Reader.hh"

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTRawTimeSeries.hh"

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
    KTLOGGER(eggreadlog, "KTEgg1Reader");

    const ifstream::pos_type KTEgg1Reader::sPreludeSize = 9;

    KT_REGISTER_EGGREADER(KTEgg1Reader, "egg1");

    KTEgg1Reader::KTEgg1Reader() :
            KTEggReader(),
            fFileName(),
            fEggStream(),
            fPrelude(),
            fHeaderSize(0),
            fHeader(),
            fHeaderInfo(),
            fRecordsRead(0),
            fLastFrameID(0)
    {
    }

    KTEgg1Reader::~KTEgg1Reader()
    {
    }

    bool KTEgg1Reader::Configure(const KTEggProcessor& eggProc)
    {
        // not really configurable
        return true;
    }

    KTDataPtr KTEgg1Reader::BreakEgg(const std::string& filename)
    {
        // First, read all of the information from the file and put it in the right places
        if (fEggStream.is_open()) fEggStream.close();

        // open the file stream
        KTINFO(eggreadlog, "Opening egg file <" << filename << ">")
        fEggStream.open(filename.c_str(), ifstream::in|ifstream::binary);

        if (! fEggStream.is_open())
        {
            KTERROR(eggreadlog, "Egg filestream did not open (file: " << filename << ")");
            return KTDataPtr();
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
            return KTDataPtr();
        }
        string newPrelude(readBuffer, sPreludeSize);
        fPrelude = newPrelude;

        // convert the prelude to the header size
        stringstream conversion;
        conversion << readBuffer;
        conversion >> std::hex >> fHeaderSize;
        //KTDEBUG(eggreadlog, "header size: " << fHeaderSize);

        // read the header
        delete [] readBuffer;
        readBuffer = new char [fHeaderSize+1];
        fEggStream.read(readBuffer, fHeaderSize);
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Egg filestream is bad after reading the header");
            delete [] readBuffer;
            return KTDataPtr();
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
            return KTDataPtr();
        }
        //std::cout << headerDOM;

        rapidxml::xml_node<char>* nodeHeader = headerDOM.first_node("header");
        if (nodeHeader == NULL)
        {
            KTERROR(eggreadlog, "No header node");
            return KTDataPtr();
        }

        rapidxml::xml_node<char>* nodeDataFormat = nodeHeader->first_node("data_format");
        if (nodeDataFormat == NULL)
        {
            KTERROR(eggreadlog, "No data format node");
            return KTDataPtr();
        }

        rapidxml::xml_attribute<char>* attr = nodeDataFormat->first_attribute("id");
        if (attr == NULL)        {
            KTERROR(eggreadlog, "No id attribute in the data format node");
            return KTDataPtr();
        };
        fHeaderInfo.fFrameIDSize = ConvertFromArray< int >(attr->value());

        attr = nodeDataFormat->first_attribute("ts");
        if (attr == NULL)
        {
            KTERROR(eggreadlog, "No ts attribute in the data format node");
            return KTDataPtr();
        }
        fHeaderInfo.fTimeStampSize = ConvertFromArray< int >(attr->value());

        attr = nodeDataFormat->first_attribute("data");
        if (attr == NULL)
        {
            KTERROR(eggreadlog, "No data attribute in the data format node");
            return KTDataPtr();
        }
        fHeaderInfo.fRecordSize = ConvertFromArray< int >(attr->value());

        fHeaderInfo.fSliceSize = fHeaderInfo.fFrameIDSize + fHeaderInfo.fTimeStampSize + fHeaderInfo.fRecordSize;

        rapidxml::xml_node<char>* nodeDigitizer = nodeHeader->first_node("digitizer");
        if (nodeDigitizer == NULL)
        {
            KTERROR(eggreadlog, "No digitizer node");
            return KTDataPtr();
        }

        attr = nodeDigitizer->first_attribute("rate");
        if (attr == NULL)
        {
            KTERROR(eggreadlog, "No rate attribute in the digitizer node");
            return KTDataPtr();
        }
        fHeaderInfo.fSampleRate = ConvertFromArray< double >(attr->value()) * fHeaderInfo.fHertzPerSampleRateUnit;

        rapidxml::xml_node<char>* nodeRun = nodeHeader->first_node("run");
        if (nodeRun == NULL)
        {
            KTERROR(eggreadlog, "No run node");
            return KTDataPtr();
        }

        attr = nodeRun->first_attribute("length");
        if (attr == NULL)        {
            KTERROR(eggreadlog, "No length attribute in the run node");
            return KTDataPtr();
        }
        fHeaderInfo.fRunLength = ConvertFromArray< double >(attr->value()) * fHeaderInfo.fSecondsPerRunLengthUnit; // in seconds

        delete [] headerCopy;

        KTDEBUG(eggreadlog, "Parsed header:\n"
             << "\tFrame ID Size: " << fHeaderInfo.fFrameIDSize << '\n'
             << "\tTime Stamp Size: " << fHeaderInfo.fTimeStampSize << '\n'
             << "\tRecord Size: " << fHeaderInfo.fRecordSize << '\n'
             << "\tRun Length: " << fHeaderInfo.fRunLength << " s" << '\n'
             << "\tSample Rate: " << fHeaderInfo.fSampleRate << " Hz ");

        fRecordsRead = 0;

        // Everything should have been done correctly at this point,
        // and we're ready to create, fill, and return the KTEggHeader

        KTDataPtr eggHeaderPtr(new KTData());
        KTEggHeader& eggHeader = eggHeaderPtr->Of< KTEggHeader >();
        eggHeader.SetTSDataType(KTEggHeader::kReal);
        eggHeader.SetFilename(filename);
        eggHeader.SetAcquisitionMode(1);
        eggHeader.SetRunDuration(fHeaderInfo.fRunLength / fHeaderInfo.fSecondsPerRunLengthUnit);
        eggHeader.SetAcquisitionRate(fHeaderInfo.fSampleRate * fHeaderInfo.fHertzPerSampleRateUnit);
        // timestamp
        // description
        // run type
        unsigned iChannel = 0;
        KTDEBUG(eggreadlog, "Adding header for channel " << iChannel);
        //const M3ChannelHeader& channelHeader = monarchHeader->GetChannelHeaders()[iChanInFile];
        KTChannelHeader* newChanHeader = new KTChannelHeader();
        newChanHeader->SetNumber(iChannel);
        newChanHeader->SetSource("Monarch1");
        newChanHeader->SetRawSliceSize(fHeaderInfo.fRecordSize);
        newChanHeader->SetSliceSize(fHeaderInfo.fRecordSize);
        newChanHeader->SetSliceStride(fHeaderInfo.fRecordSize);
        newChanHeader->SetRecordSize(fHeaderInfo.fRecordSize);
        newChanHeader->SetSampleSize(1);
        newChanHeader->SetDataTypeSize(1);
        newChanHeader->SetDataFormat(sDigitizedUS);
        newChanHeader->SetBitDepth(8);
        newChanHeader->SetBitAlignment(sBitsAlignedLeft);
        newChanHeader->SetVoltageOffset(-0.25);
        newChanHeader->SetVoltageRange(0.5);
        newChanHeader->SetDACGain(newChanHeader->GetVoltageRange() / (double)(1 << newChanHeader->GetBitDepth()));
        eggHeader.SetChannelHeader(newChanHeader, iChannel);

        return eggHeaderPtr;
    }

    KTDataPtr KTEgg1Reader::HatchNextSlice()
    {
        if (! fEggStream.good()) return KTDataPtr();

        KTDataPtr newData(new KTData());

        KTSliceHeader& sliceHeader = newData->Of< KTSliceHeader >().SetNComponents(1);

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
            sliceHeader.SetTimeStamp(newTimeStamp);
            //std::cout << "Time stamp (" << newTimeStamp.size() << " chars): ";
            //for (int i=0; i<newTimeStamp.size(); i++)
            //    std::cout << newTimeStamp[i];
            //std::cout << std::endl;
        }
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Reached end of file after reading time stamp size");
            return KTDataPtr();
        }

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
            sliceHeader.SetAcquisitionID(newFrameID);
            if (newFrameID == fLastFrameID && fRecordsRead > 0)
            {
                sliceHeader.SetIsNewAcquisition(false);
            }
            else
            {
                sliceHeader.SetIsNewAcquisition(true);
                fLastFrameID = newFrameID;
            }
        }
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Reached end of file after reading frame size");
            return KTDataPtr();
        }

        // Other information
        sliceHeader.SetSampleRate(double(fHeaderInfo.fSampleRate));
        sliceHeader.SetBinWidth(1. / double(fHeaderInfo.fSampleRate));
        sliceHeader.SetRawSliceSize(fHeaderInfo.fRecordSize);
        sliceHeader.SetSliceSize(fHeaderInfo.fRecordSize);
        sliceHeader.SetSliceLength(double(fHeaderInfo.fRecordSize) * sliceHeader.GetBinWidth());
        sliceHeader.SetTimeInRun(GetTimeInRun());
        sliceHeader.SetTimeInAcq(GetTimeInAcq());
        sliceHeader.SetSliceNumber((uint64_t)fRecordsRead);
        sliceHeader.SetStartRecordNumber(fRecordsRead);
        sliceHeader.SetStartSampleNumber(0);
        sliceHeader.SetEndRecordNumber(fRecordsRead);
        sliceHeader.SetEndSampleNumber(fHeaderInfo.fRecordSize - 1);
        sliceHeader.SetRecordSize(fHeaderInfo.fRecordSize);
        sliceHeader.SetRawDataFormatType(fHeaderInfo.fDataFormat, 0);

        // read the record
        readBuffer = new unsigned char [fHeaderInfo.fRecordSize];
        fEggStream.read((char*)(&readBuffer[0]), fHeaderInfo.fRecordSize);
        if (fEggStream.gcount() != fHeaderInfo.fRecordSize)
        {
            KTWARN(eggreadlog, "Size of the data read for the record did not match the amount expected"
                    << "\tExpected: :" << fHeaderInfo.fRecordSize << '\n'
                    << "\tRead: " << fEggStream.gcount());
            delete [] readBuffer;
            return KTDataPtr();
        }
        else
        {
            //vector< DataType >* newRecord = new vector< DataType >(readBuffer, readBuffer + fHeaderInfo.fRecordSize/sizeof(unsigned char));
            KTRawTimeSeries* newRecord = new KTRawTimeSeries(1, sDigitizedUS, fHeaderInfo.fRecordSize, 0., double(fHeaderInfo.fRecordSize) * sliceHeader.GetBinWidth());
            for (int iBin=0; iBin<fHeaderInfo.fRecordSize; iBin++)
            {
                newRecord->SetAt(readBuffer[iBin], iBin);
            }
            delete [] readBuffer;
            KTRawTimeSeriesData& tsData = newData->Of< KTRawTimeSeriesData >().SetNComponents(1);
            tsData.SetTimeSeries(newRecord);
            fRecordsRead++;
        }
        if (! fEggStream.good())
        {
            KTERROR(eggreadlog, "Egg stream state is not good after reading in this slice.");
        }

        return newData;
    }

    bool KTEgg1Reader::CloseEgg()
    {
        if (fEggStream.is_open()) fEggStream.close();
        return true;
    }


} /* namespace Katydid */
