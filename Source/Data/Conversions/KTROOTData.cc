/*
 * KTROOTData.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#include "KTROOTData.hh"

#include "KTProcessedTrackData.hh"

ClassImp(Katydid::TProcessedTrackData);

namespace Katydid
{
    TProcessedTrackData::TProcessedTrackData() :
            fComponent(), fIsCut(),
            fStartTimeInRunC(), fEndTimeInRunC(),fTimeLength(),
            fStartFrequency(), fEndFrequency(), fFrequencyWidth(),
            fSlope(), fIntercept(), fTotalPower(),
            fStartTimeInRunCSigma(), fEndTimeInRunCSigma(), fTimeLengthSigma(),
            fStartFrequencySigma(), fEndFrequencySigma(), fFrequencyWidthSigma(),
            fSlopeSigma(), fInterceptSigma(), fTotalPowerSigma()
    {}

    TProcessedTrackData::TProcessedTrackData(const TProcessedTrackData& orig) :
            fComponent(orig.fComponent), fIsCut(orig.fIsCut),
            fStartTimeInRunC(orig.fStartTimeInRunC), fEndTimeInRunC(orig.fEndTimeInRunC),fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency), fEndFrequency(orig.fEndFrequency), fFrequencyWidth(orig.fFrequencyWidth),
            fSlope(orig.fSlope), fIntercept(orig.fIntercept), fTotalPower(orig.fTotalPower),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma), fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma), fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma), fEndFrequencySigma(orig.fEndFrequencySigma), fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fSlopeSigma(orig.fSlopeSigma), fInterceptSigma(orig.fInterceptSigma), fTotalPowerSigma(orig.fTotalPowerSigma)
    {}

    TProcessedTrackData::TProcessedTrackData(const KTProcessedTrackData& orig)
    {
        Load(orig);
    }

    TProcessedTrackData::~TProcessedTrackData()
    {}

    TProcessedTrackData& TProcessedTrackData::operator=(const TProcessedTrackData& rhs)
    {
        fComponent = rhs.fComponent; fIsCut = rhs.fIsCut;
        fStartTimeInRunC = rhs.fStartTimeInRunC; fEndTimeInRunC = rhs.fEndTimeInRunC;fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency; fEndFrequency = rhs.fEndFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fSlope = rhs.fSlope; fIntercept = rhs.fIntercept; fTotalPower = rhs.fTotalPower;
        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma; fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma; fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma; fEndFrequencySigma = rhs.fEndFrequencySigma; fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;
        fSlopeSigma = rhs.fSlopeSigma; fInterceptSigma = rhs.fInterceptSigma; fTotalPowerSigma = rhs.fTotalPowerSigma;
        return *this;
    }

    void TProcessedTrackData::Load(const KTProcessedTrackData& data)
    {
        fComponent = data.GetComponent(); fIsCut = data.GetIsCut();
        fStartTimeInRunC = data.GetStartTimeInRunC(); fEndTimeInRunC = data.GetEndTimeInRunC();fTimeLength = data.GetTimeLength();
        fStartFrequency = data.GetStartFrequency(); fEndFrequency = data.GetEndFrequency(); fFrequencyWidth = data.GetFrequencyWidth();
        fSlope = data.GetSlope(); fIntercept = data.GetIntercept(); fTotalPower = data.GetTotalPower();
        fStartTimeInRunCSigma = data.GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetTimeLengthSigma();
        fStartFrequencySigma = data.GetStartFrequencySigma(); fEndFrequencySigma = data.GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetFrequencyWidthSigma();
        fSlopeSigma = data.GetSlopeSigma(); fInterceptSigma = data.GetInterceptSigma(); fTotalPowerSigma = data.GetTotalPowerSigma();
        return;
    }
    void TProcessedTrackData::Unload(KTProcessedTrackData& data) const
    {
        data.SetComponent(fComponent); data.SetIsCut(fIsCut);
        data.SetStartTimeInRunC(fStartTimeInRunC); data.SetEndTimeInRunC(fEndTimeInRunC); data.SetTimeLength(fTimeLength);
        data.SetStartFrequency(fStartFrequency); data.SetEndFrequency(fEndFrequency); data.SetFrequencyWidth(fFrequencyWidth);
        data.SetSlope(fSlope); data.SetIntercept(fIntercept); data.SetTotalPower(fTotalPower);
        data.SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data.SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data.SetTimeLengthSigma(fTimeLengthSigma);
        data.SetStartFrequencySigma(fStartFrequencySigma); data.SetEndFrequencySigma(fEndFrequencySigma); data.SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data.SetSlopeSigma(fSlopeSigma); data.SetInterceptSigma(fInterceptSigma); data.SetTotalPowerSigma(fTotalPowerSigma);
        return;
    }

}

