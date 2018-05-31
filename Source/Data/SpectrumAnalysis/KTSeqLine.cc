/*
 * KTSequentialLine.cc
 *
 *  Created on: Mar 10, 2016
 *      Author: Christine
 */

#include <KTSeqLine.hh>

#include "KTLogger.hh"

#include <iostream>
#include <cmath>
#include <algorithm>

namespace Katydid
{
    const std::string KTSequentialLine::sName("sequential-line");
	KTLOGGER(seqlog, "KTSeqLine");

    KTSequentialLine::KTSequentialLine():
        fStartTimeInRunC(0.0),
        fStartTimeInAcq(0.0),
        fEndTimeInRunC(0.0),
        fEndTimeInAcq(0.0),
        fStartFrequency(0.0),
        fEndFrequency(0.0),
        fInitialSlope(0.0),
        fSlope(0.0),
        fNPoints(0),
        fComponent(0),
        fAmplitudeSum(0.0),
        fSNRSum(0.0),
        fNUPSum(0.0),
        fAcquisitionID(0),
        fCandidateID(0),
        fMinPoints(0),
        fStartFrequencySigma(0.),
        fEndFrequencySigma(0.),
        fSumX(0.),
        fSumY(0.),
        fSumXY(0.),
        fSumXX(0.)
        {}

    KTSequentialLine::~KTSequentialLine()
    {}

    void KTSequentialLine::AppendPoint( const Point& point )
    {

        fTrimmingLimits.push_back(point.fThreshold); //new_trimming_limits);
        fAmplitudeList.push_back(point.fNeighborhoodAmplitude);
        fSNRList.push_back(point.fNeighborhoodAmplitude/point.fMean);

        fLinePoints.push_back(point);
        //KTINFO(seqlog, "Adding point line "<<fLinePoints.size());
        this->UpdateLineProperties();
    }

    void KTSequentialLine::LineSNRTrimming( const double& trimmingThreshold, const unsigned& minPoints )
    {
        //KTDEBUG( seqlog, "Trimming line edges. Trimming SNR threshold is "<<trimmingThreshold );

        if ( !fSNRList.empty() )
        {
            while ( fSNRList.back() < trimmingThreshold and fNPoints >= minPoints )
            {
                KTDEBUG( seqlog, "SNR is "<<fSNRList.back() );
                fAmplitudeList.erase(fAmplitudeList.end() -1);
                fSNRList.erase(fSNRList.end() -1);
                fTrimmingLimits.erase(fTrimmingLimits.end() -1);
                fLinePoints.erase(fLinePoints.end() - 1);
                fNPoints = fLinePoints.size();
            }
            while ( fSNRList.front() < trimmingThreshold and fNPoints >= minPoints )
            {
                fAmplitudeList.erase(fAmplitudeList.begin());
                fSNRList.erase(fSNRList.begin());
                fTrimmingLimits.erase(fTrimmingLimits.begin());
                fLinePoints.erase(fLinePoints.begin());
                fNPoints = fLinePoints.size();
            }
        }
        SetAcquisitionID(fLinePoints.front().fAcquisitionID );
        SetComponent( fLinePoints.front().fComponent);
        SetStartTimeInRunC( fLinePoints.front().fTimeInRunC);
        SetStartFrequency( fLinePoints.front().fFrequency);
        SetStartTimeInAcq( fLinePoints.front().fTimeInAcq);

        for(std::vector<Point>::iterator pointIt = fLinePoints.begin(); pointIt != fLinePoints.end(); ++pointIt)
        {
            fAmplitudeSum += pointIt->fNeighborhoodAmplitude;
            fSNRSum += pointIt->fNeighborhoodAmplitude/pointIt->fMean;
            fNUPSum += ( pointIt->fNeighborhoodAmplitude - pointIt->fMean ) / sqrt( pointIt->fVariance );
        }
        SetEndTimeInRunC( fLinePoints.back().fTimeInRunC);
        SetEndTimeInAcq( fLinePoints.back().fTimeInAcq);
        SetEndFrequency( fLinePoints.back().fFrequency);
    }


    inline void KTSequentialLine::UpdateLineProperties()
    {
        //KTDEBUG(seqlog, "Updating line parameters");
        SetNPoints( fLinePoints.size() );
        if ( fNPoints == 1 )
            {
                SetAcquisitionID( fLinePoints.front().fAcquisitionID );
                SetComponent( fLinePoints.front().fComponent );
                SetStartTimeInRunC( fLinePoints.front().fTimeInRunC );
                SetStartFrequency( fLinePoints.front().fFrequency );
                SetStartTimeInAcq( fLinePoints.front().fTimeInAcq );
            }
        SetEndTimeInRunC( fLinePoints.back().fTimeInRunC);
        SetEndTimeInAcq( fLinePoints.back().fTimeInAcq);
        SetEndFrequency( fLinePoints.back().fFrequency);
    }
} /* namespace Katydid */

