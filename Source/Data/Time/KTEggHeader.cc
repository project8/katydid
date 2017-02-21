/*
 * KTEggHeader.cc
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#include "KTEggHeader.hh"

#include "KTConstants.hh"

#include <ostream>

using std::vector;

namespace Katydid
{
    const std::string KTEggHeader::sName("egg-header");

    KTChannelHeader::KTChannelHeader() :
            fNumber(0),
            fSource(),
            fRawSliceSize(0),
            fSliceSize(0),
            fSliceStride(0),
            fRecordSize(0),
            fSampleSize(0),
            fDataTypeSize(0),
            fDataFormat(sInvalidFormat),
            fBitDepth(0),
            fBitAlignment(sBitsAlignedLeft),
            fVoltageOffset(0),
            fVoltageRange(0.),
            fDACGain(0.),
            fTSDataType(kUnknown)
    {
    }

    KTChannelHeader::KTChannelHeader(const KTChannelHeader& orig) :
            fNumber(orig.fNumber),
            fSource(orig.fSource),
            fRawSliceSize(orig.fRawSliceSize),
            fSliceSize(orig.fSliceSize),
            fSliceStride(orig.fSliceStride),
            fRecordSize(orig.fRecordSize),
            fSampleSize(orig.fSampleSize),
            fDataTypeSize(orig.fDataTypeSize),
            fDataFormat(orig.fDataFormat),
            fBitDepth(orig.fBitDepth),
            fBitAlignment(orig.fBitAlignment),
            fVoltageOffset(orig.fVoltageOffset),
            fVoltageRange(orig.fVoltageRange),
            fDACGain(orig.fDACGain),
            fTSDataType(orig.fTSDataType)
    {
    }

    KTChannelHeader::~KTChannelHeader()
    {
    }

    KTChannelHeader& KTChannelHeader::operator=(const KTChannelHeader& rhs)
    {
        fNumber = rhs.fNumber;
        fSource = rhs.fSource;
        fRawSliceSize = rhs.fRawSliceSize;
        fSliceSize = rhs.fSliceSize;
        fSliceStride = rhs.fSliceStride;
        fRecordSize = rhs.fRecordSize;
        fSampleSize = rhs.fSampleSize;
        fDataTypeSize = rhs.fDataTypeSize;
        fDataFormat = rhs.fDataFormat;
        fBitDepth = rhs.fBitDepth;
        fBitAlignment = rhs.fBitAlignment;
        fVoltageOffset = rhs.fVoltageOffset;
        fVoltageRange = rhs.fVoltageRange;
        fDACGain = rhs.fDACGain;
        fTSDataType = rhs.fTSDataType;
        return *this;
    }




    KTEggHeader::KTEggHeader() :
            KTExtensibleData< KTEggHeader >(),
            fFilename(),
            fAcquisitionMode(1),
            fRunDuration(0),
            fAcquisitionRate(0),
            fCenterFrequency(0.0),
            fMinimumFrequency(0.0),
            fMaximumFrequency(0.0),
            fTimestamp(),
            fDescription(),
            fChannelHeaders()
    {
    }

    KTEggHeader::KTEggHeader(const KTEggHeader& orig) :
            fFilename(orig.fFilename),
            fAcquisitionMode(orig.fAcquisitionMode),
            fRunDuration(orig.fRunDuration),
            fAcquisitionRate(orig.fAcquisitionRate),
            fCenterFrequency(orig.fCenterFrequency),
            fMinimumFrequency(orig.fMinimumFrequency),
            fMaximumFrequency(orig.fMaximumFrequency),
            fTimestamp(orig.fTimestamp),
            fDescription(orig.fDescription),
            fChannelHeaders()
    {
        for( vector< KTChannelHeader* >::const_iterator chIt = orig.fChannelHeaders.begin(); chIt != orig.fChannelHeaders.end(); ++chIt)
        {
            fChannelHeaders.push_back(new KTChannelHeader(**chIt));
        }
    }

    KTEggHeader::~KTEggHeader()
    {
    }

    KTEggHeader& KTEggHeader::operator=(const KTEggHeader& rhs)
    {
        fFilename = rhs.fFilename;
        fAcquisitionMode = rhs.fAcquisitionMode;
        fRunDuration = rhs.fRunDuration;
        fAcquisitionRate = rhs.fAcquisitionRate;
        fCenterFrequency = rhs.fCenterFrequency;
        fMinimumFrequency = rhs.fMinimumFrequency;
        fMaximumFrequency = rhs.fMaximumFrequency;
        fTimestamp = rhs.fTimestamp;
        fDescription = rhs.fDescription;
        fChannelHeaders.clear();
        fChannelHeaders.resize(rhs.fChannelHeaders.size());
        for( vector< KTChannelHeader* >::const_iterator chIt = rhs.fChannelHeaders.begin(); chIt != rhs.fChannelHeaders.end(); ++chIt)
        {
            fChannelHeaders.push_back(new KTChannelHeader(**chIt));
        }

        return *this;
    }

    KTEggHeader& KTEggHeader::SetNChannels(unsigned num)
    {
        unsigned oldSize = fChannelHeaders.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fChannelHeaders[iComponent];
        }
        fChannelHeaders.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fChannelHeaders[iComponent] = NULL;
        }
        return *this;
    }

    std::ostream& operator<<(std::ostream& out, const KTChannelHeader& header)
    {
        out << "Channel header:\n"
                << "\tNumber: " << header.GetNumber() << '\n'
                << "\tSource: " << header.GetSource() << '\n'
                << "\tRaw Slice Size: " << header.GetRawSliceSize() << '\n'
                << "\tSlice Size: " << header.GetSliceSize() << '\n'
                << "\tSlice Stride: " << header.GetSliceStride() << '\n'
                << "\tRecord Size: " << header.GetRecordSize() << '\n'
                << "\tSample Size: " << header.GetSampleSize() << '\n'
                << "\tData Type Size: " << header.GetDataTypeSize() << " bytes\n"
                << "\tData Format: " << header.GetDataFormat() << '\n'
                << "\tBit Depth: " << header.GetBitDepth() << " bits\n"
                << "\tBit Alignment: " << header.GetBitAlignment() << '\n'
                << "\tVoltage Offset: " << header.GetVoltageOffset() << " V\n"
                << "\tVoltage Range: " << header.GetVoltageRange() << " V\n"
                << "\tDAC Gain: " << header.GetDACGain() << '\n'
                << "\tTS Data Type: " << header.GetTSDataType() << '\n';
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const KTEggHeader& header)
    {
        out << "File header:\n"
                << "\tFilename: " << header.GetFilename() << '\n'
                << "\tAcquisition Mode: " << header.GetAcquisitionMode() << '\n'
                << "\tNumber of Channels: " << header.GetNChannels() << '\n'
                << "\tRun Duration: " << header.GetRunDuration() << " ms\n"
                << "\tAcquisition Rate: " << header.GetAcquisitionRate() << " Hz\n"
                << "\tCenter Frequency: " << header.GetCenterFrequency() << " Hz\n"
                << "\tFrequency Span: " << header.GetMaximumFrequency()  - header.GetMinimumFrequency()<< " Hz\n"
                << "\tTimestamp: " << header.GetTimestamp() << '\n'
                << "\tDescription: " << header.GetDescription() << '\n';
        for (unsigned iChan = 0; iChan < header.GetNChannels(); ++iChan)
        {
            out << *header.GetChannelHeader(iChan);
        }
        return out;
    }


} /* namespace Katydid */
