/*
 * KTPowerFitData.cc
 *
 *  Created on: Oct 27, 2016
 *      Author: ezayas
 */

#include "KTPowerFitData.hh"

#include <ostream>

namespace Katydid
{
    const std::string KTPowerFitData::sName("power-fit");


    KTPowerFitData::KTPowerFitData() :
            KTExtensibleData< KTPowerFitData >(),
            fNorm(),
            fMean(),
            fSigma(),
            fMaximum(),
            fNormErr(),
            fMeanErr(),
            fSigmaErr(),
            fMaximumErr(),
            fIsValid(false),
            fMainPeak(0),
            fNPeaks(0),
            fSetOfPoints(),
            fAverage(0.),
            fRMS(0.),
            fSkewness(0.),
            fKurtosis(0.),
            fNormCentral(0.),
            fMeanCentral(0.),
            fSigmaCentral(0.),
            fMaximumCentral(0.),
            fRMSAwayFromCentral(0.),
            fCentralPowerFraction(0.),
            fTrackIntercept(0.),
            fTrackID(0)
    {
    }

    KTPowerFitData::KTPowerFitData(const KTPowerFitData& orig) :
            KTExtensibleData< KTPowerFitData >(orig),
            fNorm(orig.fNorm),
            fMean(orig.fMean),
            fSigma(orig.fSigma),
            fMaximum(orig.fMaximum),
            fNormErr(orig.fNormErr),
            fMeanErr(orig.fMeanErr),
            fSigmaErr(orig.fSigmaErr),
            fMaximumErr(orig.fMaximumErr),
            fIsValid(orig.fIsValid),
            fMainPeak(orig.fMainPeak),
            fNPeaks(orig.fNPeaks),
            fSetOfPoints(orig.fSetOfPoints),
            fAverage(orig.fAverage),
            fRMS(orig.fRMS),
            fSkewness(orig.fSkewness),
            fKurtosis(orig.fKurtosis),
            fNormCentral(orig.fNormCentral),
            fMeanCentral(orig.fMeanCentral),
            fSigmaCentral(orig.fSigmaCentral),
            fMaximumCentral(orig.fMaximumCentral),
            fRMSAwayFromCentral(orig.fRMSAwayFromCentral),
            fCentralPowerFraction(orig.fCentralPowerFraction),
            fTrackIntercept(orig.fTrackIntercept),
            fTrackID(orig.fTrackID)
    {
    }

    KTPowerFitData::~KTPowerFitData()
    {
    }

    KTPowerFitData& KTPowerFitData::operator=(const KTPowerFitData& rhs)
    {
        KTExtensibleData< KTPowerFitData >::operator=(rhs);
        fNorm = rhs.fNorm;
        fMean = rhs.fMean;
        fSigma = rhs.fSigma;
        fMaximum = rhs.fMaximum;
        fNormErr = rhs.fNormErr;
        fMeanErr = rhs.fMeanErr;
        fSigmaErr = rhs.fSigmaErr;
        fMaximumErr = rhs.fMaximumErr;
        fIsValid = rhs.fIsValid;
        fMainPeak = rhs.fMainPeak;
        fNPeaks = rhs.fNPeaks;
        fSetOfPoints = rhs.fSetOfPoints;
        fAverage = rhs.        fAverage;
        fRMS = rhs.fRMS;
        fSkewness = rhs.fSkewness;
        fKurtosis = rhs.fKurtosis;
        fNormCentral = rhs.fNormCentral;
        fMeanCentral = rhs.fMeanCentral;
        fSigmaCentral = rhs.fSigmaCentral;
        fMaximumCentral = rhs.fMaximumCentral;
        fRMSAwayFromCentral = rhs.fRMSAwayFromCentral;
        fCentralPowerFraction = rhs.fCentralPowerFraction;
        fTrackIntercept = rhs.fTrackIntercept;
        fTrackID = rhs.fTrackID;
        return *this;
    }

} /* namespace Katydid */
