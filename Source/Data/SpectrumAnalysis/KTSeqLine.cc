/*
 * KTScoredSpectrum.cc
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
	KTLOGGER(seqlog, "KTSeqLine");



    LineRef::LineRef(const double& initialSlope):
        fStartTimeInRunC(0.0),
        fStartTimeInAcq(0.0),
        fEndTimeInRunC(0.0),
        fEndTimeInAcq(0.0),
        fStartFrequency(0.0),
        fEndFrequency(0.0),
        fInitialSlope(initialSlope),
        fSlope(0.0),
        fNPoints(0),
        fComponent(0),
        fAmplitudeSum(0.0),
        fAcquisitionID(0),
        fMinPoints(0),
        fStartFrequencySigma(0.),
        fEndFrequencySigma(0.),
        fSumX(0.),
        fSumY(0.),
        fSumXY(0.),
        fSumXX(0.)
        {
        }

    LineRef::~LineRef()
    {}

    void LineRef::InsertPoint(const Point& point)
    {

        fTrimmingLimits.push_back(point.fThreshold); //new_trimming_limits);
        fAmplitudeList.push_back(point.fNeighborhoodAmplitude);

        //fLinePoints.emplace_back(point.fBinInSlice, point.fPointFreq, point.fTimeInAcq, point.fTimeInRunC, point.fAmplitude, point.fThreshold, point.fAcquisitionID, point.fComponent);
        fLinePoints.push_back(point);
        //KTINFO(seqlog, "Adding point line "<<fLinePoints.size());
        this->UpdateLineProperties();
        //this->*f_calc_slope_func();
    }


    /*inline void LineRef::CalculateUnweightedSlope()
    {
        fSumX = 0.0;
        fSumY = 0.0;
        fSumXY = 0.0;
        fSumXX = 0.0;

        //KTDEBUG(seqlog, "Calculating line slope");
        double weightedSlope = 0.0;
        double weight = 0.0;
        double wSum = 0.0;
        fNPoints = fLinePoints.size();

        if (fNPoints > 1)
        {
            for(std::vector<LinePoint>::iterator pointIt = fLinePoints.begin(); pointIt != fLinePoints.end(); ++pointIt)
            {
                   weight = pointIt->fAmplitude/fAmplitudeSum*fNPoints;

                   fSumX += fLinePoints.back().fTimeInRunC * weight;
                   fSumY += fLinePoints.back().fPointFreq * weight;
                   fSumXY += fLinePoints.back().fTimeInRunC * fLinePoints.back().fPointFreq * weight;
                   fSumXX += fLinePoints.back().fTimeInRunC * fLinePoints.back().fTimeInRunC *weight;
            }
            fSlope = (fNPoints * fSumXY - fSumX * fSumY)/(fSumXX * fNPoints - fSumX * fSumX);
            KTDEBUG( seqlog, "New slope "<<fSlope);
        }
        if (fNPoints <= 1)
        {
            fSlope = fInitialSlope;
        }
    }

    inline void LineRef::CalculateSlope()
    {

            //KTDEBUG(seqlog, "Calculating line slope");
            double slope = 0.0;
            double weight = 0.0;
            double weightedSlope = 0.0;
            double wSum = 0.0;
            fNPoints = fLinePoints.size();


            //slope = (fLinePoints.back().fPointFreq - fStartFrequency)/(fLinePoints.back().fTimeInRunC - fStartTimeInRunC);
            //weight = fLinePoints.back().fAmplitude/fLinePoints.back().fThreshold;
            //weightedSlope += slope * weight;

            if (fNPoints > 1)
            {
                for(std::vector<LinePoint>::iterator pointIt = fLinePoints.begin(); pointIt != fLinePoints.end(); ++pointIt)
                {
                    if (pointIt->fPointFreq > fStartFrequency)
                    {
                        weightedSlope += (pointIt->fPointFreq - fStartFrequency)/(pointIt->fTimeInAcq - fStartTimeInAcq) * pointIt->fAmplitude;
                        wSum += pointIt->fAmplitude;
                    }
                }
                fSlope = weightedSlope/wSum;
            }

            if (fNPoints <= 1)
            {
                fSlope = fInitialSlope;
            }
        }*/

    void LineRef::LineTrimming(const double& trimmingFactor, const unsigned& minPoints)
    {
        KTDEBUG(seqlog, "Trimming line edges. Trimming factor is "<<trimmingFactor);

        if (!fAmplitudeList.empty())
        {
            while (fAmplitudeList.back() < trimmingFactor * fTrimmingLimits.back() and fNPoints >= minPoints)
            {
                KTDEBUG(seqlog, "Amplitude is "<<fAmplitudeList.back()<<" Threshold is "<<fTrimmingLimits.back());
                fAmplitudeList.erase(fAmplitudeList.end() -1);
                fTrimmingLimits.erase(fTrimmingLimits.end() -1);
                fLinePoints.erase(fLinePoints.end() - 1);
                fNPoints = fLinePoints.size();
            }
            while (fAmplitudeList.front() < trimmingFactor * fTrimmingLimits.front() and fNPoints >= minPoints)
            {
                fAmplitudeList.erase(fAmplitudeList.begin());
                fTrimmingLimits.erase(fTrimmingLimits.begin());
                fLinePoints.erase(fLinePoints.begin());
                fNPoints = fLinePoints.size();
            }
        }
        fAcquisitionID = fLinePoints.front().fAcquisitionID;
        fComponent = fLinePoints.front().fComponent;
        fStartTimeInRunC = fLinePoints.front().fTimeInRunC;
        fStartFrequency = fLinePoints.front().fPointFreq;
        fStartTimeInAcq = fLinePoints.front().fTimeInAcq;

        for(std::vector<Point>::iterator pointIt = fLinePoints.begin(); pointIt != fLinePoints.end(); ++pointIt)
        {
            fAmplitudeSum += pointIt->fNeighborhoodAmplitude;
        }

        this->UpdateLineProperties();
        //this->CalculateSlope();
    }
        

    inline void LineRef::UpdateLineProperties()
    {
        //KTDEBUG(seqlog, "Updating line parameters");
        fNPoints = fLinePoints.size();
        if (fNPoints == 1)
            {
                fAcquisitionID = fLinePoints.front().fAcquisitionID;
                fComponent = fLinePoints.front().fComponent;
                fStartTimeInRunC = fLinePoints.front().fTimeInRunC;
                fStartFrequency = fLinePoints.front().fPointFreq;
                fStartTimeInAcq = fLinePoints.front().fTimeInAcq;
            }
        fEndTimeInRunC = fLinePoints.back().fTimeInRunC;
        fEndTimeInAcq = fLinePoints.back().fTimeInAcq;
        fEndFrequency = fLinePoints.back().fPointFreq;
        //fAmplitudeSum += fLinePoints.back().fAmplitude;
    }
} /* namespace Katydid */

