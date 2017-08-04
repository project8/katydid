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


namespace Katydid
{
	KTLOGGER(seqlog, "KTSeqLine");



    LineRef::LineRef(const double& InitialSlope):
        fStartTimeInRunC(0.0),
        fStartTimeInAcq(0.0),
        fEndTimeInRunC(0.0),
        fEndTimeInAcq(0.0),
        fStartFrequency(0.0),
        fEndFrequency(0.0),
        fInitialSlope(InitialSlope),
        fSlope(0.0),
        fNPoints(0),
        fComponent(0),
        fAmplitudeSum(0.0),
        fAcquisitionID(0)
        {}

    LineRef::~LineRef()
    {}

    void LineRef::InsertPoint(const Point& Point, const double& new_trimming_limits)
    {
        KTDEBUG(seqlog, "Insert point");
        fTrimmingLimits.push_back(new_trimming_limits);
        fAmplitudeList.push_back(Point.fAmplitude);

        LinePoint newPoint(Point.fBinInSlice, Point.fPointFreq, Point.fTimeInAcq, Point.fTimeInRunC, Point.fAmplitude, Point.fAcquisitionID, Point.fComponent);
        fLinePoints.push_back(newPoint);

        this->UpdateLineParameters();
        this->CalculateSlope();
    }

    inline void LineRef::CalculateSlope()
    {
        /*
        // Makes a first loop over the points to calculate the weighted average in one time slice
        vector< double > timeBinInAcq;
        vector< double > timeBinInRunC;
        vector< double > sumPf;
        vector< double > sumP;
        vector< double > average;

        for (std::vector <KTSeqLine::LinePoint>::const_iterator pIt = fLinePoints.begin(); pIt != fLinePoints.end(); ++pIt)
        {
            bool addToList = true;
            for (unsigned iTimeBin=0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
            {
                if (pIt->fTimeInAcq == timeBinInAcq[iTimeBin])
                {
                    addToList = false;
                    break;
                }
            }
            if (addToList)
            {
                timeBinInAcq.push_back(pIt->fTimeInAcq);
                timeBinInRunC.push_back(pIt->fTimeInRunC);
            }
        }

        for (unsigned iTimeBin = 0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
        {
            sumPf.push_back(0.);
            sumP.push_back(0.);
        }

        for (std::vector<KTSeqLine::LinePoint>::const_iterator pIt = fLinePoints.begin(); pIt != fLinePoints.end(); ++pIt)
        {
            for (int iTimeBin=0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
            {
                if (pIt->fTimeInAcq == timeBinInAcq[iTimeBin])
                {
                    sumPf[iTimeBin] += pIt->fPointFreq * pIt->fAmplitude;
                    sumP[iTimeBin] += pIt->fAmplitude;
                }
            }
        }

        KTDEBUG(seqlog, "Averaging");
        for (unsigned iTimeBin = 0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
        {
            average.push_back(sumPf[iTimeBin]/sumP[iTimeBin]);
            KTDEBUG(seqlog, timeBinInAcq[iTimeBin] << '\t' << average[iTimeBin]);
        }

        // Determining the slope and intercept from Chi-2 minimization
        double sumXY = 0, sumXX=0, sumX=0, sumY=0, sumOne = 0, amplitudeSum = 0;

        for (unsigned iTimeBin = 0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
        {
            sumXY += average[iTimeBin] * timeBinInAcq[iTimeBin];
            sumXX += timeBinInAcq[iTimeBin] * timeBinInAcq[iTimeBin];
            sumX += timeBinInAcq[iTimeBin];
            sumY += average[iTimeBin];
            sumOne += 1.;
            amplitudeSum += sumP[iTimeBin];
        }
        double slope = (sumXY*sumOne-sumY*sumX)/(sumXX*sumOne-sumX*sumX);
        double intercept = sumY/sumOne-slope*sumX/sumOne;
        double rho = -sumX/sqrt(sumXX*sumOne); // correlation coefficient between slope and intercept
        KTDEBUG(seqlog, "Weighted average results: \n" <<
                "\tSlope: " << '\t' << slope << '\n' <<
                "\tIntercept: " << '\t' << intercept);
        KTDEBUG(seqlog, "Amplitude of the track: " << amplitudeSum );
        */




        KTDEBUG(seqlog, "Calulcating line slope");
        double weightedSlope = 0.0;
        double wSum = 0.0;

        if (fNPoints > 1)
        {
            for(std::vector<LinePoint>::iterator PointIt = fLinePoints.begin(); PointIt != fLinePoints.end(); ++PointIt)
            {
                if (PointIt->fPointFreq > fStartFrequency)
                {
                    weightedSlope += (PointIt->fPointFreq - fStartFrequency)/(PointIt->fTimeInAcq - fStartTimeInAcq) * PointIt->fAmplitude;
                    wSum += PointIt->fAmplitude;
                }
            }
            fSlope = weightedSlope/wSum;
        }
        else
        {
            fSlope = fInitialSlope;
        }
    }

    void LineRef::LineTrimming(const double& TrimmingFactor, const unsigned& MinPoints)
    {

        if (!fAmplitudeList.empty())
        {
            while (fAmplitudeList.back() < TrimmingFactor * fTrimmingLimits.back() and fNPoints >= MinPoints)
            {
                fAmplitudeList.erase(fAmplitudeList.end() -1);
                fTrimmingLimits.erase(fTrimmingLimits.end() -1);
                fLinePoints.erase(fLinePoints.end() - 1);
                fNPoints = fLinePoints.size();
            }
            while (fAmplitudeList.front() < TrimmingFactor * fTrimmingLimits.front() and fNPoints >= MinPoints)
            {
                fAmplitudeList.erase(fAmplitudeList.begin());
                fTrimmingLimits.erase(fTrimmingLimits.begin());
                fLinePoints.erase(fLinePoints.begin());
                fNPoints = fLinePoints.size();
            }
        }        
        this->UpdateLineParameters();
        this->CalculateSlope();
    }
        

    inline void (LineRef::UpdateLineParameters())
    {
        KTDEBUG(seqlog, "Updating line parameters");
        fAcquisitionID = fLinePoints.front().fAcquisitionID;
        fComponent = fLinePoints.front().fComponent;
        fStartTimeInRunC = fLinePoints.front().fTimeInRunC;
        fStartTimeInAcq = fLinePoints.front().fTimeInAcq;
        fEndTimeInRunC = fLinePoints.back().fTimeInRunC;
        fEndTimeInAcq = fLinePoints.back().fTimeInAcq;
        fStartFrequency = fLinePoints.front().fPointFreq;
        fEndFrequency = fLinePoints.back().fPointFreq;
        fNPoints = fLinePoints.size();

        fAmplitudeSum = 0.0;

        for (std::vector<LinePoint>::iterator PointIt=fLinePoints.begin(); PointIt != fLinePoints.end(); PointIt++)
        {
            fAmplitudeSum += PointIt->fAmplitude;

            if (PointIt->fTimeInRunC < fStartTimeInRunC)
            {
                fStartTimeInRunC = PointIt->fTimeInRunC;
                fStartTimeInAcq = PointIt->fTimeInAcq;
                fStartFrequency = PointIt->fPointFreq;
            }
            if (PointIt->fTimeInRunC > fEndTimeInRunC)
            {
                fEndTimeInRunC = PointIt->fTimeInRunC;
                fEndTimeInAcq = PointIt->fTimeInAcq;
                fEndFrequency = PointIt->fPointFreq;
            }
        }
    }
} /* namespace Katydid */

