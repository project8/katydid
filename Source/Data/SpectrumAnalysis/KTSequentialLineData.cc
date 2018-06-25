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
        fSNRList(),
        fStartTimeInRunC(0.0),
        fStartTimeInAcq(0.0),
        fEndTimeInRunC(0.0),
        fEndTimeInAcq(0.0),
        fStartFrequency(0.0),
        fEndFrequency(0.0),
        fWeightedSlopeSum(0.0),
        fSlope(0.0),
        fNPoints(0),
        fComponent(0),
        fAcquisitionID(0),
        fCandidateID(0),
        fTotalPower(0.0),
        fTotalSNR(0.0),
        fTotalNUP(0.0),
        fTotalWidePower(0.0),
        fTotalWideSNR(0.0),
        fTotalWideNUP(0.0),
        fSumX(0.),
        fSumY(0.),
        fSumXY(0.),
        fSumXX(0.)
        {}

    KTSequentialLineData::~KTSequentialLineData()
    {}

    void KTSequentialLineData::AddPoint( const KTDiscriminatedPoint& point )
    {
        fSNRList.push_back(point.fNeighborhoodAmplitude/point.fMean);
        fTotalWideSNR += point.fNeighborhoodAmplitude/point.fMean;
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
                fSNRList.erase(fSNRList.end() -1);
                fLinePoints.erase(*fLinePoints.rbegin());
                fNPoints = fLinePoints.size();
            }
            while ( fSNRList.front() < trimmingThreshold and fNPoints >= minPoints )
            {
                KTDEBUG( seqlog, "SNR is "<<fSNRList.back() );
                fSNRList.erase(fSNRList.begin());
                fLinePoints.erase(*fLinePoints.begin());
                fNPoints = fLinePoints.size();
            }
        }
        SetStartTimeInRunC( fLinePoints.begin()->fTimeInRunC);
        SetStartFrequency( fLinePoints.begin()->fFrequency);
        SetStartTimeInAcq( fLinePoints.begin()->fTimeInAcq);

        SetEndTimeInRunC( fLinePoints.rbegin()->fTimeInRunC);
        SetEndTimeInAcq( fLinePoints.rbegin()->fTimeInAcq);
        SetEndFrequency( fLinePoints.rbegin()->fFrequency);
    }


    void KTSequentialLineData::UpdateLineProperties()
    {
        //KTDEBUG(seqlog, "Updating line parameters");
        SetNPoints( fLinePoints.size() );
        if ( fNPoints == 1 or fLinePoints.begin()->fTimeInRunC < GetStartTimeInRunC())
        {
            SetStartTimeInRunC( fLinePoints.begin()->fTimeInRunC );
            SetStartFrequency( fLinePoints.begin()->fFrequency );
            SetStartTimeInAcq( fLinePoints.begin()->fTimeInAcq );
        }
        // This shouldn't actually be possible because fLinePoints are of tape KTDiscriminatedPoints
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
    void KTSequentialLineData::CalculateTotalPower()
    {
        fTotalPower = 0.;
        fTotalWidePower = 0.;
        for(KTDiscriminatedPoints::iterator pointIt = fLinePoints.begin(); pointIt != fLinePoints.end(); ++pointIt)
        {
            fTotalPower += pointIt->fAmplitude;
            fTotalWidePower += pointIt->fNeighborhoodAmplitude;
        }
    }
    void KTSequentialLineData::CalculateTotalSNR()
    {
        fTotalSNR = 0.;
        fTotalWideSNR = 0.;
        for(KTDiscriminatedPoints::iterator pointIt = fLinePoints.begin(); pointIt != fLinePoints.end(); ++pointIt)
        {
            fTotalSNR += pointIt->fAmplitude/pointIt->fMean;
            fTotalWideSNR += pointIt->fNeighborhoodAmplitude/pointIt->fMean;
        }
    }
    void KTSequentialLineData::CalculateTotalNUP()
    {
        fTotalNUP = 0.;
        fTotalWideNUP = 0.;
        for(KTDiscriminatedPoints::iterator pointIt = fLinePoints.begin(); pointIt != fLinePoints.end(); ++pointIt)
        {
            fTotalNUP += ( pointIt->fAmplitude - pointIt->fMean ) / sqrt( pointIt->fVariance );
            fTotalWideNUP += ( pointIt->fNeighborhoodAmplitude - pointIt->fMean ) / sqrt( pointIt->fVariance );
        }
    }
} /* namespace Katydid */

