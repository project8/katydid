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
            fTimeInRun(0.),
            fTimeLength(0.),
            fMinFrequency(0.),
            fMaxFrequency(0.),
            fFrequencyWidth(0.),
            fSlope(0.),
            fIntercept(0.),
            fTimeInRunSigma(0.),
            fTimeLengthSigma(0.),
            fMinFrequencySigma(0.),
            fMaxFrequencySigma(0.),
            fFrequencyWidthSigma(0.),
            fSlopeSigma(0.),
            fInterceptSigma(0.)
    {
    }

    KTProcessedTrackData::~KTProcessedTrackData()
    {
    }

} /* namespace Katydid */
