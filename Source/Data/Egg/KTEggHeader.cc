/*
 * KTEggHeader.cc
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#include "KTEggHeader.hh"

#include "MonarchTypes.hpp"

#include <ostream>

namespace Katydid
{

    KTEggHeader::KTEggHeader() :
            fFilename(),
            fAcquisitionMode(1),
            fNChannels(1),
            fRawSliceSize(0),
            fSliceSize(0),
            fSliceStride(0),
            fRecordSize(0),
            fRunDuration(0),
            fAcquisitionRate(0),
            fTimestamp(),
            fDescription(),
            fRunType(monarch::sRunTypeOther),
            fRunSource(monarch::sSourceMantis),
            fFormatMode(monarch::sFormatSingle),
            fDataTypeSize(1),
            fBitDepth(8),
            fVoltageMin(-0.25),
            fVoltageRange(0.5),
            fCenterFrequency(0.0),
            fMinimumFrequency(0.0),
            fMaximumFrequency(0.0)
    {
    }

    KTEggHeader::KTEggHeader(const KTEggHeader& orig) :
            fFilename(orig.fFilename),
            fAcquisitionMode(orig.fAcquisitionMode),
            fNChannels(orig.fNChannels),
            fRawSliceSize(orig.fRawSliceSize),
            fSliceSize(orig.fSliceSize),
            fSliceStride(orig.fSliceStride),
            fRecordSize(orig.fRecordSize),
            fRunDuration(orig.fRunDuration),
            fAcquisitionRate(orig.fAcquisitionRate),
            fTimestamp(orig.fTimestamp),
            fDescription(orig.fDescription),
            fRunType(orig.fRunType),
            fRunSource(orig.fRunSource),
            fFormatMode(orig.fFormatMode),
            fDataTypeSize(orig.fDataTypeSize),
            fBitDepth(orig.fBitDepth),
            fVoltageMin(orig.fVoltageMin),
            fVoltageRange(orig.fVoltageRange),
            fCenterFrequency(orig.fCenterFrequency),
            fMinimumFrequency(orig.fMinimumFrequency),
            fMaximumFrequency(orig.fMaximumFrequency)
    {
    }

    KTEggHeader::~KTEggHeader()
    {
    }

    KTEggHeader& KTEggHeader::operator=(const KTEggHeader& rhs)
    {
        fFilename = rhs.fFilename;
        fAcquisitionMode = rhs.fAcquisitionMode;
        fNChannels = rhs.fNChannels;
        fRawSliceSize = rhs.fRawSliceSize;
        fSliceSize = rhs.fSliceSize;
        fSliceStride = rhs.fSliceStride;
        fRecordSize = rhs.fRecordSize;
        fRunDuration = rhs.fRunDuration;
        fAcquisitionRate = rhs.fAcquisitionRate;
        fTimestamp = rhs.fTimestamp;
        fDescription = rhs.fDescription;
        fRunType = rhs.fRunType;
        fRunSource = rhs.fRunSource;
        fFormatMode = rhs.fFormatMode;
        fDataTypeSize = rhs.fDataTypeSize;
        fBitDepth = rhs.fBitDepth;
        fVoltageMin = rhs.fVoltageMin;
        fVoltageRange = rhs.fVoltageRange;
        fCenterFrequency = rhs.fCenterFrequency;
        fMinimumFrequency = rhs.fMinimumFrequency;
        fMaximumFrequency = rhs.fMaximumFrequency;
        return *this;
    }

    std::ostream& operator<<(std::ostream& out, const KTEggHeader& header)
    {
        out << "Header information:\n"
                << "\tFilename: " << header.GetFilename() << '\n'
                << "\tAcquisition Mode: " << header.GetAcquisitionMode() << '\n'
                << "\tNumber of Channels: " << header.GetNChannels() << '\n'
                << "\tRaw Slice Size: " << header.GetRawSliceSize() << '\n'
                << "\tSlice Size: " << header.GetSliceSize() << '\n'
                << "\tSlice Stride: " << header.GetSliceStride() << '\n'
                << "\tRecord Size: " << header.GetRecordSize() << '\n'
                << "\tRun Duration: " << header.GetRunDuration() << " s\n"
                << "\tAcquisition Rate: " << header.GetAcquisitionRate() << " Hz\n"
                << "\tTimestamp: " << header.GetTimestamp() << '\n'
                << "\tDescription: " << header.GetDescription() << '\n'
                << "\tRun Type: " << header.GetRunType() << '\n'
                << "\tRun Source: " << header.GetRunSource() << '\n'
                << "\tFormat Mode: " << header.GetFormatMode() << '\n'
                << "\tData Type Size: " << header.GetDataTypeSize() << " Byte(s)\n"
                << "\tBit Depth: " << header.GetBitDepth() << " bits\n"
                << "\tVoltage Min: " << header.GetVoltageMin() << " V\n"
                << "\tVoltage Range: " << header.GetVoltageRange() << " V\n"
                << "\tCenter Frequency: " << header.GetCenterFrequency() << " Hz\n"
                << "\tFrequency Span: " << header.GetMaximumFrequency()  - header.GetMinimumFrequency()<< " Hz\n";
        return out;
    }


} /* namespace Katydid */
