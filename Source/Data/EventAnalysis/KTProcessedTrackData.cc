/*
 * KTProcessedTrackData.cc
 *
 *  Created on: July 22, 2014
 *      Author: nsoblath
 */

#include "KTProcessedTrackData.hh"

namespace Katydid
{
    const std::string KTProcessedTrackData::sName("processed-track");

    KTProcessedTrackData::KTProcessedTrackData() :
            KTExtensibleData< KTProcessedTrackData >(),
            fComponent(0),
            fAcquisitionID(0),
            fTrackID(0),
            fEventID(0),
            fEventSequenceID(-1),
            fFilename(""),
            fIsCut(false),
            fMVAClassifier(-999.),
            fMainband(true),
            fStartTimeInAcq(0.),
            fStartTimeInRunC(0.),
            fEndTimeInRunC(0.),
            fTimeLength(0.),
            fStartFrequency(0.),
            fEndFrequency(0.),
            fFrequencyWidth(0.),
            fSlope(0.),
            fIntercept(0.),
            fTotalPower(0.),
            fNTrackBins(0.),
            fTotalTrackSNR(0.),
            fMaxTrackSNR(0.),
            fTotalTrackNUP(0.),
            fMaxTrackNUP(0.),
            fTotalWideTrackSNR(0.),
            fTotalWideTrackNUP(0.),
            fStartTimeInRunCSigma(0.),
            fEndTimeInRunCSigma(0.),
            fTimeLengthSigma(0.),
            fStartFrequencySigma(0.),
            fEndFrequencySigma(0.),
            fFrequencyWidthSigma(0.),
            fSlopeSigma(0.),
            fInterceptSigma(0.),
            fTotalPowerSigma(0.)
    {
    }

    KTProcessedTrackData::KTProcessedTrackData(const KTProcessedTrackData& orig) :
            KTExtensibleData< KTProcessedTrackData >(orig),
            fComponent(orig.fComponent),
            fAcquisitionID(orig.fAcquisitionID),
            fTrackID(orig.fTrackID),
            fEventID(orig.fEventID),
            fEventSequenceID(orig.fEventSequenceID),
            fFilename(orig.fFilename),
            fIsCut(orig.fIsCut),
            fMVAClassifier(orig.fMVAClassifier),
            fMainband(orig.fMainband),
            fStartTimeInAcq(orig.fStartTimeInAcq),
            fStartTimeInRunC(orig.fStartTimeInRunC),
            fEndTimeInRunC(orig.fEndTimeInRunC),
            fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency),
            fEndFrequency(orig.fEndFrequency),
            fFrequencyWidth(orig.fFrequencyWidth),
            fSlope(orig.fSlope),
            fIntercept(orig.fIntercept),
            fTotalPower(orig.fTotalPower),
            fNTrackBins(orig.fNTrackBins),
            fTotalTrackSNR(orig.fTotalTrackSNR),
            fMaxTrackSNR(orig.fMaxTrackSNR),
            fTotalTrackNUP(orig.fTotalTrackNUP),
            fMaxTrackNUP(orig.fMaxTrackNUP),
            fTotalWideTrackSNR(orig.fTotalWideTrackSNR),
            fTotalWideTrackNUP(orig.fTotalWideTrackNUP),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma),
            fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma),
            fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma),
            fEndFrequencySigma(orig.fEndFrequencySigma),
            fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fSlopeSigma(orig.fSlopeSigma),
            fInterceptSigma(orig.fInterceptSigma),
            fTotalPowerSigma(orig.fTotalPowerSigma)
    {
    }

    KTProcessedTrackData::~KTProcessedTrackData()
    {
    }

    KTProcessedTrackData& KTProcessedTrackData::operator=(const KTProcessedTrackData& rhs)
    {
        fComponent = rhs.fComponent;
        fTrackID = rhs.fTrackID;
        fEventID = rhs.fEventID;
        fEventSequenceID = rhs.fEventSequenceID;
        fFilename = rhs.fFilename;
        fIsCut = rhs.fIsCut;
        fMVAClassifier = rhs.fMVAClassifier;
        fMainband = rhs.fMainband;
        fStartTimeInAcq = rhs.fStartTimeInAcq;
        fStartTimeInRunC = rhs.fStartTimeInRunC;
        fEndTimeInRunC = rhs.fEndTimeInRunC;
        fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency;
        fEndFrequency = rhs.fEndFrequency;
        fFrequencyWidth = rhs.fFrequencyWidth;
        fSlope = rhs.fSlope;
        fIntercept = rhs.fIntercept;
        fTotalPower = rhs.fTotalPower;
        fNTrackBins = rhs.fNTrackBins,
        fTotalTrackSNR = rhs.fTotalTrackSNR;
        fMaxTrackSNR = rhs.fMaxTrackSNR;
        fTotalTrackNUP = rhs.fTotalTrackNUP;
        fMaxTrackNUP = rhs.fMaxTrackNUP;
        fTotalWideTrackSNR = rhs.fTotalWideTrackSNR;
        fTotalWideTrackNUP = rhs.fTotalWideTrackNUP;
        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma;
        fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma;
        fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma;
        fEndFrequencySigma = rhs.fEndFrequencySigma;
        fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;
        fSlopeSigma = rhs.fSlopeSigma;
        fInterceptSigma = rhs.fInterceptSigma;
        fTotalPowerSigma = rhs.fTotalPowerSigma;
        return *this;
    }

} /* namespace Katydid */
