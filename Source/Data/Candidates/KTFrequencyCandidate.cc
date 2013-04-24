/*
 * KTFrequencyCandidate.cc
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidate.hh"

namespace Katydid
{

    KTFrequencyCandidate::KTFrequencyCandidate() :
            fFirstBin(0),
            fLastBin(0),
            fMeanFrequency(0.),
            fPeakAmplitude(0.),
            fAmplitudeSum(0.)
    {
    }

    KTFrequencyCandidate::KTFrequencyCandidate(const KTFrequencyCandidate& orig) :
            fFirstBin(orig.fFirstBin),
            fLastBin(orig.fLastBin),
            fMeanFrequency(orig.fMeanFrequency),
            fPeakAmplitude(orig.fPeakAmplitude),
            fAmplitudeSum(orig.fAmplitudeSum)
    {
    }

    KTFrequencyCandidate::~KTFrequencyCandidate()
    {
    }

    KTFrequencyCandidate& KTFrequencyCandidate::operator=(const KTFrequencyCandidate& rhs)
    {
        fFirstBin = rhs.fFirstBin;
        fLastBin = rhs.fLastBin;
        fMeanFrequency = rhs.fMeanFrequency;
        fPeakAmplitude = rhs.fPeakAmplitude;
        fAmplitudeSum = rhs.fAmplitudeSum;
        return *this;
    }

} /* namespace Katydid */
