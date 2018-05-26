/*
 * KTTrackProcessing.cc
 *
 *  Created on: July 22, 2013
 *      Author: N.S. Oblath & B. LaRoque
 */

#include "KTTrackProcessing.hh"

#include "KTHoughData.hh"
#include "KTLogger.hh"

#include "KTProcessedTrackData.hh"
#include "KTSmooth.hh"
#include "KTSparseWaterfallCandidateData.hh"

#include <limits>
#include <vector>
#include <algorithm>

using boost::shared_ptr;
using std::vector;
using std::string;

namespace Katydid
{
    KTLOGGER(tlog, "katydid.fft");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTTrackProcessing, "track-proc");

    KTTrackProcessing::KTTrackProcessing(const std::string& name) :
            KTProcessor(name),
            fTrackProcAlgorithm("weighted-slope"),
            fPointLineDistCut1(0.1),
            fPointLineDistCut2(0.05),
            fSlopeMinimum(-std::numeric_limits< double >::max()),
            fProcTrackMinPoints(0),
            fProcTrackAssError(0.),
            fTrackSignal("track", this),
            fTrackProcPtr(&KTTrackProcessing::ProcessTrackDoubleCuts),
            fSWFAndHoughSlot("swfc-and-hough", this, &KTTrackProcessing::ProcessTrack, &fTrackSignal),
            fSWFSlot("swfc", this, &KTTrackProcessing::ProcessTrack2, &fTrackSignal)
    {
    }

    KTTrackProcessing::~KTTrackProcessing()
    {
    }

    bool KTTrackProcessing::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetTrackProcAlgorithm(node->get_value("algorithm", GetTrackProcAlgorithm()));

        SetPointLineDistCut1(node->get_value("pl-dist-cut1", GetPointLineDistCut1()));
        SetPointLineDistCut2(node->get_value("pl-dist-cut2", GetPointLineDistCut2()));

        SetSlopeMinimum(node->get_value("min-slope", GetSlopeMinimum()));
        SetProcTrackMinPoints(node->get_value("min-points", GetProcTrackMinPoints()));
        SetProcTrackAssignedError(node->get_value("assigned-error", GetProcTrackAssignedError()));

        // KTDEBUG(tlog, "Making track reconstruction");
        // if (fTrackProcAlgorithm == "double-cuts")
        // {
        //     KTDEBUG(tlog, "Making track reconstruction using \"double-cuts\" algorithm");
        //     fTrackProcPtr = &KTTrackProcessing::ProcessTrackDoubleCuts;
        // }
        // else if (fTrackProcAlgorithm == "weighted-slope")
        // {
        //     KTDEBUG(tlog, "Setting track reconstruction using \"weighted-slope\" algorithm");
        //     fTrackProcPtr = &KTTrackProcessing::ProcessTrackWeightedSlope;
        //     fTrackProc2Ptr = &KTTrackProcessing::ProcessTrackWeightedSlope;
        // }
        // else
        // {
        //     KTERROR(tlog, "Invalid value for \"track-slope\": <" << fTrackProcAlgorithm << ">");
        //     return false;
        // }

        return true;
    }

    bool KTTrackProcessing::ProcessTrack(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData)
    {
        KTDEBUG(tlog, "Track processing");
        if (fTrackProcAlgorithm == "double-cuts")
        {
            KTDEBUG(tlog, "Making track reconstruction using \"double-cuts\" algorithm");
            return KTTrackProcessing::ProcessTrackDoubleCuts(swfData,htData);
            // fTrackProcPtr = &KTTrackProcessing::ProcessTrackDoubleCuts;
        }
        else if (fTrackProcAlgorithm == "weighted-slope")
        {
            KTDEBUG(tlog, "Setting track reconstruction using \"weighted-slope\" algorithm");
            return KTTrackProcessing::ProcessTrackWeightedSlope(swfData);

        }
        KTERROR(tlog, "Invalid value for \"track-slope\": <" << fTrackProcAlgorithm << ">");
        return false;

        // return (this->*fTrackProcPtr)(swfData,htData);
    }

    bool KTTrackProcessing::ProcessTrack2(KTSparseWaterfallCandidateData& swfData)
    {
        KTDEBUG(tlog, "Track processing");
        if (fTrackProcAlgorithm=="double-cuts") {
            KTERROR(tlog, "Cannot use " << fTrackProcAlgorithm << "algorithm with only SparseWaterfallCandidate!");
            return false;
        }
        KTDEBUG(tlog, "Setting track reconstruction using \"weighted-slope\" algorithm");
        return KTTrackProcessing::ProcessTrackWeightedSlope(swfData);
    }

    bool KTTrackProcessing::ProcessTrackDoubleCuts(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData)
    {
        unsigned component = swfData.GetComponent();
        unsigned trackID = swfData.GetCandidateID();

        typedef KTSparseWaterfallCandidateData::Points Points;
        // not const because points will be removed later
        Points& points = swfData.GetPoints();

        // not const because the HT will be smoothed in place
        KTPhysicalArray< 2, double >* houghTransform = htData.GetTransform(component);

        // NOTE: smoothes the actual data, not a copy
        if (! KTSmooth::Smooth(houghTransform))
        {
            KTERROR(tlog, "Error while smoothing Hough Transform");
            return false;
        }

        unsigned maxBinX, maxBinY;
        houghTransform->GetMaximumBin(maxBinX, maxBinY);

        double htAngle = houghTransform->GetBinCenter(1, maxBinX);
        double htRadius = houghTransform->GetBinCenter(2, maxBinY);

        // Hough line: a*x + b*y + c = 0, c = -radius
        double htCosAngle = cos(htAngle); // ht_a
        double htSinAngle = sin(htAngle); // ht_b

        // scaling for Hough Transform, used throughout for line fits and modifications
        // "scaled" means the axis has been scaled to 0-1, as was the case when the Hough Transform was calculated
        // "unscaled" means the axis is in the original units
        // unscaled = scaled * scale + offset
        double xScale = htData.GetXScale(component);
        double yScale = htData.GetYScale(component);
        double xOffset = htData.GetXOffset(component);
        double yOffset = htData.GetYOffset(component);

        double htSlope = -1.0 * (htCosAngle * yScale) / (htSinAngle * xScale);
        double htIntercept = (htRadius / htSinAngle) * yScale + yOffset - htSlope * xOffset; // this is r/sin(angle), rescaled, plus extra from the xOffset

        KTDEBUG(tlog, "Hough Transform track processing results:\n"
            << "\tmaxBinX: " << maxBinX << "\t maxBinY: " << maxBinY << "\t angle: " << htAngle << "\t radius: " << htRadius << '\n'
            << "\ta=cos(angle): " << htCosAngle << "\t b=sin(angle): " << htSinAngle << '\n'
            << "\txScale: " << xScale << "\t yScale: " << yScale << '\n'
            << "\tSlope: " << htSlope << " Hz/s" << '\n'
            << "\tIntercept: " << htIntercept << " Hz");

        // First loop over points, and first point-line-distance cut

        unsigned nPoints = points.size();
        typedef vector< std::pair< double, double > > SimplePoints2D;
        SimplePoints2D pointsScaled, pointsUnscaled, pointsUnscaledInAcq;
        pointsScaled.reserve(nPoints);
        pointsUnscaledInAcq.reserve(nPoints);
        vector< unsigned > pointsCuts;
        pointsCuts.reserve(nPoints);
        unsigned nPointsUsed = 0;
        unsigned nPointsCut1 = 0;
        double xScaled, yScaled, distance;
        double sumX = 0., sumY = 0., sumX2 = 0., sumXY = 0.; // for least-squares calculation
        // first distance cut, plus calculation of the initial least-squares line calculations
        for (Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            //cout << "calculating a distance..." << endl;
            pointsUnscaled.push_back(SimplePoints2D::value_type(pIt->fTimeInRunC, pIt->fFrequency));
            pointsUnscaledInAcq.push_back(SimplePoints2D::value_type(pIt->fTimeInAcq, pIt->fFrequency));
            xScaled = (pIt->fTimeInRunC - xOffset) / xScale;
            yScaled = (pIt->fFrequency - yOffset) / yScale;
            pointsScaled.push_back(SimplePoints2D::value_type(xScaled, yScaled));
            //cout << "i: " << iPoint << "\t y_i: " << ys[iPoint] << "\t x_i: " << xs[iPoint] << endl;
            //cout << "scaled: y_isc: " << yiscaled << "\t xisc: " << xiscaled << endl;
            distance = PointLineDistance(xScaled, yScaled, htCosAngle, htSinAngle, -htRadius);
            //cout << "distance: " << distance << endl;

            if (distance < fPointLineDistCut1 )
            {
                // point is not cut
                ++nPointsUsed;
                sumX += xScaled;
                sumY += yScaled;
                sumX2 += xScaled * xScaled;
                sumXY += xScaled * yScaled;
                pointsCuts.push_back(0);
            }
            else
            {
                // point is cut
                ++nPointsCut1;
                pointsCuts.push_back(1);
            }
        } // loop over points
        KTDEBUG(tlog, "Points removed with cut 1: " << nPointsCut1);

        // Refine the line with a least-squares line calculation
        double xMean = sumX / (double)nPointsUsed;
        double yMean = sumY / (double)nPointsUsed;
        double lsSlopeScaled = (sumXY - sumX * yMean) / (sumX2 - sumX * xMean);
        double lsInterceptScaled = yMean - lsSlopeScaled * xMean;
        double lsSlope = lsSlopeScaled * yScale / xScale;
        double lsIntercept = lsInterceptScaled * yScale + yOffset - lsSlope * xOffset;
        KTDEBUG(tlog, "Least-squares fit result\n"
            << "\tSlope: " << lsSlope << " Hz/s\n"
            << "\tIntercept: " << lsIntercept << " Hz");

        // second distance cut based on LS fit
        double startTime = std::numeric_limits< double >::max();
        double stopTime = -1.;
        double startTimeInAcq = 0;
        double startFreq, stopFreq;
        nPointsUsed = 0;
        unsigned nPointsCut2 = 0;
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            distance = PointLineDistance(pointsScaled[iPoint].first, pointsScaled[iPoint].second, lsSlopeScaled, -1., lsInterceptScaled);;

            if (pointsCuts[iPoint] == 0 && distance < fPointLineDistCut2)
            {
                // point is not cut
                ++nPointsUsed;
                if (pointsUnscaled[iPoint].first < startTime) //possibly update start time/frequency
                {
                    startTime = pointsUnscaled[iPoint].first;
                    startFreq = startTime * lsSlope + lsIntercept;
                    startTimeInAcq = pointsUnscaledInAcq[iPoint].first;
                }
                if (pointsUnscaled[iPoint].first > stopTime)
                { //possibly update stop time/frequency
                    stopTime = pointsUnscaled[iPoint].first;
                    stopFreq = stopTime * lsSlope + lsIntercept;
                }
            }
            else if (pointsCuts[iPoint] == 0)
            {
                // point is cut
                ++nPointsCut2;
                pointsCuts[iPoint] = 2;
            }
        }
        KTDEBUG(tlog, "Points removed with cut 2: " << nPointsCut2);

        // Remove points and sum amplitudes
        Points::iterator pItMaster = points.begin();
        Points::iterator pItCache;
        double amplitudeSum = 0.;
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            pItCache = pItMaster;
            ++pItMaster;
            if (pointsCuts[iPoint] == 0)
            {
                amplitudeSum += pItCache->fAmplitude;
            }
            else
            {
                points.erase(pItCache);
            }
        }
        KTDEBUG(tlog, "Points present after cuts: " << points.size());

        // Add the new data
        KTProcessedTrackData& procTrack = htData.Of< KTProcessedTrackData >();
        procTrack.SetComponent(component);
        procTrack.SetAcquisitionID(swfData.GetAcquisitionID());
        procTrack.SetTrackID(trackID);

        if (lsSlope < fSlopeMinimum || points.size() < fProcTrackMinPoints)
        {
            procTrack.SetIsCut(true);
        }

        procTrack.SetStartTimeInAcq(startTimeInAcq);
        procTrack.SetStartTimeInRunC(startTime);
        procTrack.SetEndTimeInRunC(stopTime);
        procTrack.SetTimeLength(stopTime - startTime);
        procTrack.SetStartFrequency(startFreq);
        procTrack.SetEndFrequency(stopFreq);
        procTrack.SetFrequencyWidth(std::abs(stopFreq - startFreq));
        procTrack.SetSlope(lsSlope);
        procTrack.SetIntercept(lsIntercept);
        procTrack.SetTotalPower(amplitudeSum);
        //TODO: Add calculation of uncertainties

        return true;
    }


    bool KTTrackProcessing::ProcessTrackWeightedSlope(KTSparseWaterfallCandidateData& swfData)
    {
        unsigned component = swfData.GetComponent();
        unsigned trackID = swfData.GetCandidateID();

        typedef KTSparseWaterfallCandidateData::Points Points;
        // not const because points will be removed later
        Points& points = swfData.GetPoints();

        // Makes a first loop over the points to calculate the weighted average in one time slice
        vector< double > timeBinInAcq;
        vector< double > timeBinInRunC;
        vector< double > sumPf;
        vector< double > sumP;
        vector< double > average;

        for (Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            bool addToList = true;
            for (unsigned iTimeBin=0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
            {
                if (pIt->fTimeInAcq == timeBinInAcq[iTimeBin])
                {
                  addToList = false;
                  KTDEBUG(tlog, "Duplicate time: " << pIt->fTimeInAcq << '\t' << pIt->fTimeInRunC);
                  break;
                }
            }
            if (addToList)
            {
                KTDEBUG(tlog, "Adding Time: " << pIt->fTimeInAcq << '\t' << pIt->fTimeInRunC);
                timeBinInAcq.push_back(pIt->fTimeInAcq);
                timeBinInRunC.push_back(pIt->fTimeInRunC);
            }
        }

        for (unsigned iTimeBin = 0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
        {
            sumPf.push_back(0.);
            sumP.push_back(0.);
        }

        for (Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            for (int iTimeBin=0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
            {
                if (pIt->fTimeInAcq == timeBinInAcq[iTimeBin])
                {
                  sumPf[iTimeBin] += pIt->fFrequency * pIt->fAmplitude;
                  sumP[iTimeBin] += pIt->fAmplitude;
                }
            }
        }

        KTDEBUG(tlog, "Averaging");
        for (unsigned iTimeBin = 0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
        {
            average.push_back(sumPf[iTimeBin]/sumP[iTimeBin]);
            KTDEBUG(tlog, timeBinInAcq[iTimeBin] << '\t' << average[iTimeBin]);
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
        KTDEBUG(tlog, "Weighted average results: \n" <<
                      "\tSlope: " << '\t' << slope << '\n' <<
                      "\tIntercept: " << '\t' << intercept);
        KTDEBUG(tlog, "Amplitude of the track: " << amplitudeSum );

        //Calculating Chi^2_min
        double chi2min = 0;
        double residual = 0;
        for (unsigned iTimeBin = 0; iTimeBin<timeBinInAcq.size(); ++iTimeBin)
        {
            residual = average[iTimeBin] - slope*timeBinInAcq[iTimeBin] - intercept;
            chi2min += residual * residual;
            KTDEBUG(tlog, "Residuals : " << residual );
        }
        // Calculate error on slope and intercept for a rescaled Ch^2_min = 1
        double deltaSlope = 0;
        double deltaIntercept = 0;
        double sigmaStartFreq = 0;
        double sigmaEndFreq = 0;

        // need at least 3 points to get a non-zero Ndf
        if (timeBinInAcq.size()>2)
        {
            KTDEBUG(tlog, "Chi2min : " << chi2min );

            if (chi2min < 0.1)
            {
                KTDEBUG(tlog, "Chi2min too small (points are mostlikely aligned): assigning arbitrary errors to the averaged points (" << fProcTrackAssError << ")");
                deltaSlope = 1.52/(sqrt(sumXX)/fProcTrackAssError);
                deltaIntercept = 1.52/(sqrt(sumOne)/fProcTrackAssError);
            }
            else
            {
                double ndf = timeBinInAcq.size() - 2; // 2: two fitting parameters
                deltaSlope = 1.52/sqrt(sumXX*ndf/chi2min);
                deltaIntercept = 1.52/sqrt(sumOne*ndf/chi2min);
            }
            KTDEBUG(tlog, "Error calculations results: \n" <<
                          "\tSlope: " << '\t' << deltaSlope << '\n' <<
                          "\tIntercept: " << '\t' << deltaIntercept << '\n' <<
                          "\tCorrelation coefficifent: " << '\t' << rho);
            //Calculating error on the starting frequency and the end frequency
            double startTime = *std::min_element(timeBinInAcq.begin(), timeBinInAcq.end());
            double endTime = *std::max_element(timeBinInAcq.begin(), timeBinInAcq.end());
            sigmaStartFreq = sqrt( startTime*startTime *  deltaSlope*deltaSlope + deltaIntercept*deltaIntercept + 2 * startTime * rho * deltaSlope * deltaIntercept );
            sigmaEndFreq = sqrt( endTime*endTime *  deltaSlope*deltaSlope + deltaIntercept*deltaIntercept + 2 * endTime * rho * deltaSlope * deltaIntercept );
        }

        // TODO: Calculate distance to track and see for a possible alpha [%] rejection of noise.

        // Adding resuts to ProcessedTrackData object
        Nymph::KTDataPtr data(new Nymph::KTData());
        KTProcessedTrackData& procTrack = data->Of< KTProcessedTrackData >();
        procTrack.SetComponent(component);
        procTrack.SetAcquisitionID(swfData.GetAcquisitionID());
        procTrack.SetTrackID(trackID);

        procTrack.SetStartTimeInAcq(*std::min_element(timeBinInAcq.begin(), timeBinInAcq.end()));
        procTrack.SetStartTimeInRunC(*std::min_element(timeBinInRunC.begin(), timeBinInRunC.end()));
        procTrack.SetEndTimeInRunC(*std::max_element(timeBinInRunC.begin(), timeBinInRunC.end()));
        procTrack.SetTimeLength(procTrack.GetEndTimeInRunC() - procTrack.GetStartTimeInRunC());
        procTrack.SetStartFrequency(procTrack.GetStartTimeInAcq() * slope + intercept);
        procTrack.SetEndFrequency((procTrack.GetStartTimeInAcq() + procTrack.GetTimeLength()) * slope + intercept);
        procTrack.SetFrequencyWidth(std::abs(procTrack.GetEndFrequency() - procTrack.GetStartFrequency()));
        procTrack.SetSlope(slope);
        procTrack.SetIntercept(intercept);
        procTrack.SetTotalPower(amplitudeSum);
        if (!(slope > fSlopeMinimum))
        {
            procTrack.SetIsCut(true);
        }
        procTrack.SetSlopeSigma(deltaSlope);
        procTrack.SetInterceptSigma(deltaIntercept);
        procTrack.SetStartFrequencySigma(sigmaStartFreq);
        procTrack.SetEndFrequencySigma(sigmaEndFreq);

        return true;
    }
} /* namespace Katydid */
