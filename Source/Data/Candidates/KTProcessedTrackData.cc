/*
 * KTProcessedTrackData.cc
 *
 *  Created on: July 22, 2014
 *      Author: nsoblath
 */

#include "KTProcessedTrackData.hh"

namespace Katydid
{
    KTProcessedTrackData::KTProcessedTrackData() :
            KTExtensibleData< KTProcessedTrackData >(),
            fComponent(0),
            fTrackID(0),
            fIsCut(false),
            fStartTimeInRunC(0.),
            fEndTimeInRunC(0.),
            fTimeLength(0.),
            fStartFrequency(0.),
            fEndFrequency(0.),
            fFrequencyWidth(0.),
            fSlope(0.),
            fIntercept(0.),
            fTotalPower(0.),
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
            fTrackID(orig.fTrackID),
            fIsCut(orig.fIsCut),
            fStartTimeInRunC(orig.fStartTimeInRunC),
            fEndTimeInRunC(orig.fEndTimeInRunC),
            fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency),
            fEndFrequency(orig.fEndFrequency),
            fFrequencyWidth(orig.fFrequencyWidth),
            fSlope(orig.fSlope),
            fIntercept(orig.fIntercept),
            fTotalPower(orig.fTotalPower),
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
        fIsCut = rhs.fIsCut;
        fStartTimeInRunC = rhs.fStartTimeInRunC;
        fEndTimeInRunC = rhs.fEndTimeInRunC;
        fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency;
        fEndFrequency = rhs.fEndFrequency;
        fFrequencyWidth = rhs.fFrequencyWidth;
        fSlope = rhs.fSlope;
        fIntercept = rhs.fIntercept;
        fTotalPower = rhs.fTotalPower;
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
