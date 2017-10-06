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
        fProcTrackMinPoints(0),
        fProcTrackAssError(0.),
        fIntercept(0.),
        fSlopeSigma(0.),
        fInterceptSigma(0.),
        fStartFrequencySigma(0.),
        fEndFrequencySigma(0.)
        {}

    LineRef::~LineRef()
    {}

    void LineRef::InsertPoint(const Point& point)
    {
        fTrimmingLimits.push_back(point.fThreshold); //new_trimming_limits);
        fAmplitudeList.push_back(point.fAmplitude);

        fLinePoints.emplace_back(point.fBinInSlice, point.fPointFreq, point.fTimeInAcq, point.fTimeInRunC, point.fAmplitude, point.fThreshold, point.fAcquisitionID, point.fComponent);

        this->UpdateLineParameters();
        this->CalculateSlope();
    }

    void LineRef::FinishTrack()
    {
        // This is the weighted slope algorithm from the track processing.
        // However it did not result in better slopes and it's currently not used.

        std::vector< double > timeBinInAcq;
        std::vector< double > timeBinInRunC;
        std::vector< double > sumPf;
        std::vector< double > sumP;
        std::vector< double > average;

        for (std::vector <LinePoint>::const_iterator pIt = fLinePoints.begin(); pIt != fLinePoints.end(); ++pIt)
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

        for (std::vector<LinePoint>::const_iterator pIt = fLinePoints.begin(); pIt != fLinePoints.end(); ++pIt)
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
        //KTDEBUG(seqlog, "Weighted average results: \n" <<
        //        "\tSlope: " << '\t' << slope << '\n' <<
        //        "\tIntercept: " << '\t' << intercept);
        //KTDEBUG(seqlog, "Amplitude of the track: " << amplitudeSum );

        //Calculating Chi^2_min
        double chi2min = 0;
        double residual = 0;
        for (unsigned iTimeBin = 0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
        {
            residual = average[iTimeBin] - slope*timeBinInAcq[iTimeBin] - intercept;
            chi2min += residual * residual;
        //   KTDEBUG(seqlog, "Residuals : " << residual );
        }
        // Calculate error on slope and intercept for a rescaled Ch^2_min = 1
        double deltaSlope = 0;
        double deltaIntercept = 0;
        double sigmaStartFreq = 0;
        double sigmaEndFreq = 0;

        // need at least 3 points to get a non-zero Ndf
        if (timeBinInAcq.size()>2)
        {
            KTDEBUG(seqlog, "Chi2min : " << chi2min );

            if (chi2min < 0.1)
            {
                KTDEBUG(seqlog, "Chi2min too small (points are mostlikely aligned): assigning arbitrary errors to the averaged points (" << fProcTrackAssError << ")");
                deltaSlope = 1.52/(sqrt(sumXX)/fProcTrackAssError);
                deltaIntercept = 1.52/(sqrt(sumOne)/fProcTrackAssError);
            }
            else
            {
                double ndf = timeBinInAcq.size() - 2; // 2: two fitting parameters
                deltaSlope = 1.52/sqrt(sumXX*ndf/chi2min);
                deltaIntercept = 1.52/sqrt(sumOne*ndf/chi2min);
            }
            //           KTDEBUG(seqlog, "Error calculations results: \n" <<
            //                   "\tSlope: " << '\t' << deltaSlope << '\n' <<
            //                   "\tIntercept: " << '\t' << deltaIntercept << '\n' <<
            //                   "\tCorrelation coefficifent: " << '\t' << rho);
            //Calculating error on the starting frequency and the end frequency
            double startTime = *std::min_element(timeBinInAcq.begin(), timeBinInAcq.end());
            double endTime = *std::max_element(timeBinInAcq.begin(), timeBinInAcq.end());
            sigmaStartFreq = sqrt( startTime*startTime *  deltaSlope*deltaSlope + deltaIntercept*deltaIntercept + 2 * startTime * rho * deltaSlope * deltaIntercept );
            sigmaEndFreq = sqrt( endTime*endTime *  deltaSlope*deltaSlope + deltaIntercept*deltaIntercept + 2 * endTime * rho * deltaSlope * deltaIntercept );
        }

        // TODO: Calculate distance to track and see for a possible alpha [%] rejection of noise.

        //fSlope = slope;
        fIntercept = intercept;
        fSlopeSigma = deltaSlope;
        fInterceptSigma = deltaIntercept;
        fStartFrequencySigma = sigmaStartFreq;
        fEndFrequencySigma = sigmaEndFreq;

    }

    inline void LineRef::CalculateSlope()
    {

        //KTDEBUG(seqlog, "Calculating line slope");
        double weightedSlope = 0.0;
        double wSum = 0.0;

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
        else
        {
            fSlope = fInitialSlope;
        }
    }

    void LineRef::LineTrimming(const double& trimmingFactor, const unsigned& minPoints)
    {
        KTDEBUG(seqlog, "Trimming line edges");

        if (!fAmplitudeList.empty())
        {
            while (fAmplitudeList.back() < trimmingFactor * fTrimmingLimits.back() and fNPoints >= minPoints)
            {
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
        this->UpdateLineParameters();
        this->CalculateSlope();
    }
        

    inline void (LineRef::UpdateLineParameters())
    {
        //KTDEBUG(seqlog, "Updating line parameters");
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

