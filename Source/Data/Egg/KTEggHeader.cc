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
            fSliceSize(0),
            fRecordSize(0),
            fRunDuration(0),
            fAcquisitionRate(0),
            fTimestamp(),
            fDescription(),
            fRunType(sRunTypeOther),
            fRunSource(sSourceMantis),
            fFormatMode(sFormatSingle)
    {
    }

    KTEggHeader::KTEggHeader(const KTEggHeader& orig) :
            fFilename(orig.fFilename),
            fAcquisitionMode(orig.fAcquisitionMode),
            fNChannels(orig.fNChannels),
            fSliceSize(orig.fSliceSize),
            fRecordSize(orig.fRecordSize),
            fRunDuration(orig.fRunDuration),
            fAcquisitionRate(orig.fAcquisitionRate),
            fTimestamp(orig.fTimestamp),
            fDescription(orig.fDescription),
            fRunType(orig.fRunType),
            fRunSource(orig.fRunSource),
            fFormatMode(orig.fFormatMode)
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
        fSliceSize = rhs.fSliceSize;
        fRecordSize = rhs.fRecordSize;
        fRunDuration = rhs.fRunDuration;
        fAcquisitionRate = rhs.fAcquisitionRate;
        fTimestamp = rhs.fTimestamp;
        fDescription = rhs.fDescription;
        fRunType = rhs.fRunType;
        fRunSource = rhs.fRunSource;
        fFormatMode = rhs.fFormatMode;
        return *this;
    }

    std::ostream& operator<<(std::ostream& out, const KTEggHeader& header)
    {
        out << "Header information:\n"
                << "\tFilename: " << header.GetFilename() << '\n'
                << "\tAcquisition Mode: " << header.GetAcquisitionMode() << '\n'
                << "\tNumber of Channels: " << header.GetNChannels() << '\n'
                << "\tSlice Size: " << header.GetSliceSize() << '\n'
                << "\tRecord Size: " << header.GetRecordSize() << '\n'
                << "\tRun Duration: " << header.GetRunDuration() << " s\n"
                << "\tAcquisition Rate: " << header.GetAcquisitionRate() << " Hz \n"
                << "\tTimestamp: " << header.GetTimestamp() << '\n'
                << "\tDescription: " << header.GetDescription() << '\n'
                << "\tRun Type: " << header.GetRunType() << '\n'
                << "\tRun Source: " << header.GetRunSource() << '\n'
                << "\tFormat Mode: " << header.GetFormatMode();
        return out;
    }


} /* namespace Katydid */
