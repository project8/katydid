/*
 * KTSliceHeader.cc
 *
 *  Created on: Feb 14, 2013
 *      Author: nsoblath
 */

#include "KTSliceHeader.hh"

namespace Katydid
{
    KTSliceHeader::KTSliceHeader() :
            KTExtensibleData< KTSliceHeader >(),
            fIsNewAcquisition(true),
            fTimeInRun(0.),
            fSliceNumber(0),
            fSliceSize(1),
            fSliceLength(0.),
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
            fIsNewAcquisition(orig.fIsNewAcquisition),
            fTimeInRun(orig.fTimeInRun),
            fSliceNumber(orig.fSliceNumber),
            fSliceSize(orig.fSliceSize),
            fSliceLength(orig.fSliceLength),
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
        fIsNewAcquisition = rhs.fIsNewAcquisition;
        fTimeInRun = rhs.fTimeInRun;
        fSliceNumber = rhs.fSliceNumber;
        fSliceSize = rhs.fSliceSize;
        fSliceLength = rhs.fSliceLength;
        fSampleRate = rhs.fSampleRate;
        fBinWidth = rhs.fBinWidth;
        fStartRecordNumber = rhs.fStartRecordNumber;
        fStartSampleNumber = rhs.fStartSampleNumber;
        fEndRecordNumber = rhs.fEndRecordNumber;
        fEndSampleNumber = rhs.fEndSampleNumber;
        fRecordSize = rhs.fRecordSize;
        fComponentData = rhs.fComponentData;
        // temporary variables aren't copied
        return *this;
    }

} /* namespace Katydid */
