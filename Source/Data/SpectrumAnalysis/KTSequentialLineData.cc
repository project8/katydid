/**
 @file KTSequentialLineData.cc
 @brief Contains KTSequentialLineData
 @details KTDiscriminatedPoint cluster with some track properties
 @author: C. Claessens
 @date: May 31, 2018
 */
#include <KTSequentialLineData.hh>

#include "KTLogger.hh"

#include <iostream>
#include <cmath>
#include <algorithm>

namespace Katydid
{
    const std::string KTSequentialLineData::sName("sequential-line");
	KTLOGGER(seqlog, "KTSeqLine");

    KTSequentialLineData::KTSequentialLineData():
        fLinePoints(),
        fAmplitudeList(),
        fSNRList(),
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
        fAcquisitionID(0),
        fCandidateID(0),
        fAmplitudeSum(0.0),
        fSNRSum(0.0),
        fNUPSum(0.0),
        fMinPoints(0),
        fSumX(0.),
        fSumY(0.),
        fSumXY(0.),
        fSumXX(0.)
        {}

    KTSequentialLineData::~KTSequentialLineData()
    {}

    void KTSequentialLineData::AddPoint( const KTDiscriminatedPoint& point )
    {
        fAmplitudeList.push_back(point.fNeighborhoodAmplitude);
        fSNRList.push_back(point.fNeighborhoodAmplitude/point.fMean);

        fLinePoints.insert(point);
        //KTINFO(seqlog, "Adding point line "<<fLinePoints.size());
        this->UpdateLineProperties();
    }

    void KTSequentialLineData::LineSNRTrimming( const double& trimmingThreshold, const unsigned& minPoints )
    {
        //KTDEBUG( seqlog, "Trimming line edges. Trimming SNR threshold is "<<trimmingThreshold );

        if ( !fSNRList.empty() )
        {
            while ( fSNRList.back() < trimmingThreshold and fNPoints >= minPoints )
            {
                KTDEBUG( seqlog, "SNR is "<<fSNRList.back() );
                fAmplitudeList.erase(fAmplitudeList.end() -1);
                fSNRList.erase(fSNRList.end() -1);
                fLinePoints.erase(*fLinePoints.rbegin());
                fNPoints = fLinePoints.size();
            }
            while ( fSNRList.front() < trimmingThreshold and fNPoints >= minPoints )
            {
                KTDEBUG( seqlog, "SNR is "<<fSNRList.back() );
                fAmplitudeList.erase(fAmplitudeList.begin());
                fSNRList.erase(fSNRList.begin());
                fLinePoints.erase(*fLinePoints.begin());
                fNPoints = fLinePoints.size();
            }
        }
        SetStartTimeInRunC( fLinePoints.begin()->fTimeInRunC);
        SetStartFrequency( fLinePoints.begin()->fFrequency);
        SetStartTimeInAcq( fLinePoints.begin()->fTimeInAcq);

        fAmplitudeSum = 0.;
        fSNRSum = 0;
        fNUPSum = 0;

        for(KTDiscriminatedPoints::iterator pointIt = fLinePoints.begin(); pointIt != fLinePoints.end(); ++pointIt)
        {
            fAmplitudeSum += pointIt->fNeighborhoodAmplitude;
            fSNRSum += pointIt->fNeighborhoodAmplitude/pointIt->fMean;
            fNUPSum += ( pointIt->fNeighborhoodAmplitude - pointIt->fMean ) / sqrt( pointIt->fVariance );
        }
        SetEndTimeInRunC( fLinePoints.rbegin()->fTimeInRunC);
        SetEndTimeInAcq( fLinePoints.rbegin()->fTimeInAcq);
        SetEndFrequency( fLinePoints.rbegin()->fFrequency);
    }


    inline void KTSequentialLineData::UpdateLineProperties()
    {
        //KTDEBUG(seqlog, "Updating line parameters");
        SetNPoints( fLinePoints.size() );
        if ( fNPoints == 1 or fLinePoints.begin()->fTimeInRunC < GetStartTimeInRunC())
        {
            SetStartTimeInRunC( fLinePoints.begin()->fTimeInRunC );
            SetStartFrequency( fLinePoints.begin()->fFrequency );
            SetStartTimeInAcq( fLinePoints.begin()->fTimeInAcq );
        }
        if (fLinePoints.rbegin()->fTimeInRunC < GetStartTimeInRunC() )
        {
            SetStartTimeInRunC( fLinePoints.rbegin()->fTimeInRunC );
            SetStartFrequency( fLinePoints.rbegin()->fFrequency );
            SetStartTimeInAcq( fLinePoints.rbegin()->fTimeInAcq );
        }
        if ( fLinePoints.rbegin()->fTimeInRunC > GetEndTimeInRunC() )
        {
            SetEndTimeInRunC( fLinePoints.rbegin()->fTimeInRunC);
            SetEndTimeInAcq( fLinePoints.rbegin()->fTimeInAcq);
            SetEndFrequency( fLinePoints.rbegin()->fFrequency);
        }
    }
} /* namespace Katydid */

