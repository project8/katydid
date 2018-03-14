/*
 * KTRPTrackData.cc
 *
 *  Created on: Nov 9, 2017
 *      Author: ezayas
 */

#include "KTRPTrackData.hh"

namespace Katydid
{
    const std::string KTRPTrackData::sName("rp-track-data");


    KTRPTrackData::KTRPTrackData() :
            KTProcessedTrackData()
    {
        fIsValid = false;
        fMainPeak = 0;
        fNPeaks = 0;
        fAverage = 0.;
        fRMS = 0.;
        fSkewness = 0.;
        fKurtosis = 0.;
        fNormCentral = 0.;
        fMeanCentral = 0.;
        fSigmaCentral = 0.;
        fMaximumCentral = 0.;
        fRMSAwayFromCentral = 0.;
        fCentralPowerFraction = 0.;
    }

    KTRPTrackData::~KTRPTrackData()
    {
    }

    KTRPTrackData& KTRPTrackData::operator=(const KTProcessedTrackData& rhs)
    {
        SetComponent(rhs.GetComponent());
        SetTrackID(rhs.GetTrackID());
        SetEventID(rhs.GetEventID());
        SetEventSequenceID(rhs.GetEventSequenceID());
        SetIsCut(rhs.GetIsCut());
        SetStartTimeInAcq(rhs.GetStartTimeInAcq());
        SetStartTimeInRunC(rhs.GetStartTimeInRunC());
        SetEndTimeInRunC(rhs.GetEndTimeInRunC());
        SetTimeLength(rhs.GetTimeLength());
        SetStartFrequency(rhs.GetStartFrequency());
        SetEndFrequency(rhs.GetEndFrequency());
        SetFrequencyWidth(rhs.GetFrequencyWidth());
        SetSlope(rhs.GetSlope());
        SetIntercept(rhs.GetIntercept());
        SetTotalPower(rhs.GetTotalPower());
        SetStartTimeInRunCSigma(rhs.GetStartTimeInRunCSigma());
        SetEndTimeInRunCSigma(rhs.GetEndTimeInRunCSigma());
        SetTimeLengthSigma(rhs.GetTimeLengthSigma());
        SetStartFrequencySigma(rhs.GetStartFrequencySigma());
        SetEndFrequencySigma(rhs.GetEndFrequencySigma());
        SetFrequencyWidthSigma(rhs.GetFrequencyWidthSigma());
        SetSlopeSigma(rhs.GetSlopeSigma());
        SetInterceptSigma(rhs.GetInterceptSigma());
        SetTotalPowerSigma(rhs.GetTotalPowerSigma());
        return *this;

    }

} /* namespace Katydid */
