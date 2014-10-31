/*
 * KTSliceHeader.cc
 *
 *  Created on: Feb 14, 2013
 *      Author: nsoblath
 */

#include "KTSliceHeader.hh"

#include <ostream>

namespace Katydid
{
    const std::string KTSliceHeader::sName("slice-header");

    KTSliceHeader::KTSliceHeader() :
            KTExtensibleData< KTSliceHeader >(),
            fTimeInRun(0.),
            fSliceNumber(0),
            fNSlicesIncluded(1),
            fIsNewAcquisition(true),
            fRawSliceSize(1),
            fSliceSize(1),
            fSliceLength(0.),
            fNonOverlapFrac(0.),
            fSampleRate(0.),
            fBinWidth(1.),
            fStartRecordNumber(0),
            fStartSampleNumber(0),
            fEndRecordNumber(0),
            fEndSampleNumber(0),
            fRecordSize(1),
            fComponentData(1),
            fTemp1(0), fTemp2(0), fTemp3(0)
    {
    }

    KTSliceHeader::KTSliceHeader(const KTSliceHeader& orig) :
            KTExtensibleData< KTSliceHeader >(orig),
            fTimeInRun(orig.fTimeInRun),
            fSliceNumber(orig.fSliceNumber),
            fNSlicesIncluded(orig.fNSlicesIncluded),
            fIsNewAcquisition(orig.fIsNewAcquisition),
            fRawSliceSize(orig.fRawSliceSize),
            fSliceSize(orig.fSliceSize),
            fSliceLength(orig.fSliceLength),
            fNonOverlapFrac(orig.fNonOverlapFrac),
            fSampleRate(orig.fSampleRate),
            fBinWidth(orig.fBinWidth),
            fStartRecordNumber(orig.fStartRecordNumber),
            fStartSampleNumber(orig.fStartSampleNumber),
            fEndRecordNumber(orig.fEndRecordNumber),
            fEndSampleNumber(orig.fEndSampleNumber),
            fRecordSize(orig.fRecordSize),
            fComponentData(orig.fComponentData),
            fTemp1(0), fTemp2(0), fTemp3(0) // temporary variables aren't copied
    {
    }

    KTSliceHeader::~KTSliceHeader()
    {
    }

    KTSliceHeader& KTSliceHeader::operator=(const KTSliceHeader& rhs)
    {
        KTExtensibleData< KTSliceHeader >::operator=(rhs);
        CopySliceHeaderOnly(rhs);
        return *this;
    }

    void KTSliceHeader::CopySliceHeaderOnly(const KTSliceHeader& rhs)
    {
        fIsNewAcquisition = rhs.fIsNewAcquisition;
        fTimeInRun = rhs.fTimeInRun;
        fSliceNumber = rhs.fSliceNumber;
        fNSlicesIncluded = rhs.fNSlicesIncluded;
        fRawSliceSize = rhs.fRawSliceSize;
        fSliceSize = rhs.fSliceSize;
        fSliceLength = rhs.fSliceLength;
        fNonOverlapFrac = rhs.fNonOverlapFrac;
        fSampleRate = rhs.fSampleRate;
        fBinWidth = rhs.fBinWidth;
        fStartRecordNumber = rhs.fStartRecordNumber;
        fStartSampleNumber = rhs.fStartSampleNumber;
        fEndRecordNumber = rhs.fEndRecordNumber;
        fEndSampleNumber = rhs.fEndSampleNumber;
        fRecordSize = rhs.fRecordSize;
        fComponentData = rhs.fComponentData;
        // temporary variables aren't copied
        return;
    }

    std::ostream& operator<<(std::ostream& out, const KTSliceHeader& hdr)
    {
        out << "Slice Header Contents:\n" <<
                "\tSlice number: " << hdr.GetSliceNumber() << '\n' <<
                "\tIncludes " << hdr.GetNSlicesIncluded() << " slices\n" <<
                "\tRaw slice size: " << hdr.GetRawSliceSize() << '\n' <<
                "\tSlice size: " << hdr.GetSliceSize() << '\n' <<
                "\tSlice Length: " << hdr.GetSliceLength() << " s\n" <<
                "\tNon-Overlap Fraction: " << hdr.GetNonOverlapFrac() << '\n' <<
                "\tSample Rate: " << hdr.GetSampleRate() << " Hz\n" <<
                "\tBin Width: " << hdr.GetBinWidth() << " s\n" <<
                "\tTime in Run: " << hdr.GetTimeInRun() << " s\n" <<
                "\tIs New Acquisition?: " << hdr.GetIsNewAcquisition() << '\n' <<
                "\tStart Record: " << hdr.GetStartRecordNumber() << '\n' <<
                "\tStart Sample: " << hdr.GetStartSampleNumber() << '\n' <<
                "\tEnd Record: " << hdr.GetEndRecordNumber() << '\n' <<
                "\tEnd Sample: " << hdr.GetEndSampleNumber() << '\n' <<
                "\tRecord Size: " << hdr.GetRecordSize() << '\n' <<
                "\t# of Components: " << hdr.GetNComponents();
        for (unsigned iComponent = 0; iComponent < hdr.GetNComponents(); ++iComponent)
        {
            out << "\n\tComponent " << iComponent << '\n' <<
                    "\t\tTimestamp: " << hdr.GetTimeStamp(iComponent) << " ns\n" <<
                    "\t\tAcquisition ID: " << hdr.GetAcquisitionID(iComponent) << '\n' <<
                    "\t\tRecord ID: " << hdr.GetRecordID(iComponent);
        }
        return out;
    }

} /* namespace Katydid */


