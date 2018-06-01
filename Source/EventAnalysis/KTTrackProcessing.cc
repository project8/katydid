/**
 @file KTTrackProcessing.cc
 @brief Contains KTTrackProcessing
 @details Extracts physics-relevant information about tracks
 @author: N.S. Oblath, B. LaRoque & M. Guigue
 @date: July 22, 2013
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
#include <numeric>

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
            fProcTrackAssignedError(0.),
            fTrackSignal("track", this),
            // fTrackProcPtr(&KTTrackProcessing::ProcessTrackDoubleCuts),
            fSWFAndHoughSlot("swfc-and-hough", this, &KTTrackProcessing::ProcessTrackSWFAndHough, &fTrackSignal),
            fSWFSlot("swfc", this, &KTTrackProcessing::ProcessTrackSWF, &fTrackSignal)
    {
    }

    KTTrackProcessing::~KTTrackProcessing()
    {
    }

    bool KTTrackProcessing::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetTrackProcAlgorithm(node->get_value("algorithm", GetTrackProcAlgorithm()));

        // Setting parameters if the algorithm is "double-cuts"
        if (fTrackProcAlgorithm == "double-cuts")
        {
            SetPointLineDistCut1(node->get_value("pl-dist-cut1", GetPointLineDistCut1()));
            SetPointLineDistCut2(node->get_value("pl-dist-cut2", GetPointLineDistCut2()));
        };
        SetSlopeMinimum(node->get_value("min-slope", GetSlopeMinimum()));
        SetProcTrackMinPoints(node->get_value("min-points", GetProcTrackMinPoints()));
        SetProcTrackAssignedError(node->get_value("assigned-error", GetProcTrackAssignedError()));

        return true;
    }

    bool KTTrackProcessing::ProcessTrackSWFAndHough(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData)
    // Method associated with the <SWFC, Hough> slot
    {
        TrackID trackID = ExtractTrackID(swfData);
        Points& points = swfData.GetPoints();
        // The & is important!!
        KTProcessedTrackData& procTrack = swfData.Of< KTProcessedTrackData >();     

        KTDEBUG(tlog, "Track processing");
        if (fTrackProcAlgorithm == "double-cuts")
        {
            KTDEBUG(tlog, "Making track reconstruction using \"double-cuts\" algorithm");
            return KTTrackProcessing::ProcessTrackDoubleCuts(points, htData, trackID, &procTrack);
        }
        else if (fTrackProcAlgorithm == "weighted-slope")
        {
            KTDEBUG(tlog, "Setting track reconstruction using \"weighted-slope\" algorithm");
            return KTTrackProcessing::ProcessTrackWeightedSlope(points, trackID, &procTrack);

        }
        KTERROR(tlog, "Invalid value for \"track-slope\": <" << fTrackProcAlgorithm << ">");
        return false;
    }

    bool KTTrackProcessing::ProcessTrackSWF(KTSparseWaterfallCandidateData& swfData)
    // Method associated with the <SWFC> slot
    {

        TrackID trackID = ExtractTrackID(swfData);
        Points& points = swfData.GetPoints();
        // The & is important!!
        KTProcessedTrackData& procTrack = swfData.Of< KTProcessedTrackData >();

        KTDEBUG(tlog, "Track processing");
        if (fTrackProcAlgorithm=="double-cuts") {
            KTERROR(tlog, "Cannot use " << fTrackProcAlgorithm << "algorithm with only SparseWaterfallCandidate!");
            return false;
        }
        else if (fTrackProcAlgorithm == "weighted-slope")
        {
            KTDEBUG(tlog, "Setting track reconstruction using \"weighted-slope\" algorithm");
            return KTTrackProcessing::ProcessTrackWeightedSlope(points, trackID, &procTrack);
        }
        KTERROR(tlog, "Invalid value for \"track-slope\": <" << fTrackProcAlgorithm << ">");
        return false;
    }

    bool KTTrackProcessing::ProcessTrackDoubleCuts(Points& points, KTHoughData& htData, TrackID trackID, KTProcessedTrackData* procTrack)
    {
        // not const because the HT will be smoothed in place
        KTPhysicalArray< 2, double >* houghTransform = htData.GetTransform(trackID.fComponent);

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
        double xScale = htData.GetXScale(trackID.fComponent);
        double yScale = htData.GetYScale(trackID.fComponent);
        double xOffset = htData.GetXOffset(trackID.fComponent);
        double yOffset = htData.GetYOffset(trackID.fComponent);

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

        vector<double> amplitude;
        vector<double> mean;
        vector<double> variance;
        vector<double> neighborhoodAmplitude;

        vector<double> trackSNR;
        vector<double> trackNUP;

        vector<double> wideTrackSNR;
        vector<double> wideTrackNUP;

        //Has to be a better way (remove_if?)
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            pItCache = pItMaster;
            ++pItMaster;
            if (pointsCuts[iPoint] == 0)
            {
                amplitude.push_back(pItCache->fAmplitude);
                mean.push_back(pItCache->fMean);
                variance.push_back(pItCache->fVariance);
                neighborhoodAmplitude.push_back(pItCache->fNeighborhoodAmplitude);

                trackSNR.push_back(pItCache->fAmplitude / pItCache->fMean);
                trackNUP.push_back((pItCache->fAmplitude - pItCache->fMean) / pItCache->fVariance);

                wideTrackSNR.push_back(pItCache->fNeighborhoodAmplitude / pItCache->fMean);
                wideTrackNUP.push_back((pItCache->fNeighborhoodAmplitude - pItCache->fMean) / pItCache->fVariance);
            }
            else
            {
                points.erase(pItCache);
            }
        }
        KTDEBUG(tlog, "Points present after cuts: " << points.size());

        // Add the new data
        procTrack->SetComponent(trackID.fComponent);
        procTrack->SetAcquisitionID(trackID.fAcquisitionID);
        procTrack->SetTrackID(trackID.fCandidateID);

        if (lsSlope < fSlopeMinimum || points.size() < fProcTrackMinPoints)
        {
            procTrack->SetIsCut(true);
        }

        procTrack->SetStartTimeInAcq(startTimeInAcq);
        procTrack->SetStartTimeInRunC(startTime);
        procTrack->SetEndTimeInRunC(stopTime);
        procTrack->SetTimeLength(stopTime - startTime);
        procTrack->SetStartFrequency(startFreq);
        procTrack->SetEndFrequency(stopFreq);
        procTrack->SetFrequencyWidth(std::abs(stopFreq - startFreq));
        procTrack->SetSlope(lsSlope);
        procTrack->SetIntercept(lsIntercept);
        procTrack->SetTotalPower(std::accumulate(amplitude.begin(), amplitude.end(), 0.));

        procTrack->SetNTrackBins(points.size());
        procTrack->SetTotalTrackSNR(std::accumulate(trackSNR.begin(),trackSNR.end(),0.));
        procTrack->SetMaxTrackSNR(*std::max_element(trackSNR.begin(), trackSNR.end()));
        procTrack->SetTotalTrackNUP(std::accumulate(trackNUP.begin(),trackNUP.end(),0.));
        procTrack->SetMaxTrackNUP(*std::max_element(trackNUP.begin(), trackNUP.end()));
        procTrack->SetTotalWideTrackSNR(std::accumulate(wideTrackSNR.begin(),wideTrackSNR.end(),0.));
        procTrack->SetTotalWideTrackNUP(std::accumulate(wideTrackNUP.begin(),wideTrackNUP.end(),0.));
        
        //TODO: Add calculation of uncertainties

        return true;
    }


    bool KTTrackProcessing::ProcessTrackWeightedSlope(Points& points, TrackID trackID,KTProcessedTrackData* procTrack)
    {
        vector< double > timeBinInAcq;
        vector< double > timeBinInRunC;
        vector< double > averageFrequency;

        // Makes a first loop over the points to calculate the weighted average in one time slice
        for (Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            timeBinInAcq.push_back(pIt->fTimeInAcq);
            timeBinInRunC.push_back(pIt->fTimeInRunC);
        }
        //Make these lists duplicate-free
        sort( timeBinInAcq.begin(), timeBinInAcq.end() );
        timeBinInAcq.erase( unique( timeBinInAcq.begin(), timeBinInAcq.end() ), timeBinInAcq.end() );

        sort( timeBinInRunC.begin(), timeBinInRunC.end() );
        timeBinInRunC.erase( unique( timeBinInRunC.begin(), timeBinInRunC.end() ), timeBinInRunC.end() );

        const int nTimeBins = timeBinInAcq.size();
        //Derived Quantites
        vector< double > mean(nTimeBins);
        vector< double > variance(nTimeBins);
        vector< double > neighborhoodAmplitude(nTimeBins);
        vector< double > sumPf(nTimeBins);
        vector< double > sumP(nTimeBins);
        vector< double > trackSNR(nTimeBins);
        vector< double > trackNUP(nTimeBins);
        vector< double > wideTrackSNR(nTimeBins);
        vector< double > wideTrackNUP(nTimeBins);
        int nTrackBins = 0.;
        // Calculate the averaged points
        for (Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            for (int iTimeBin=0; iTimeBin<nTimeBins; ++iTimeBin)
            {
                if (pIt->fTimeInAcq == timeBinInAcq[iTimeBin])
                {
                    sumPf[iTimeBin] += pIt->fFrequency * pIt->fAmplitude;
                    sumP[iTimeBin] += pIt->fAmplitude;
                    mean[iTimeBin] += pIt->fMean;
                    variance[iTimeBin] += pIt->fVariance;
                    neighborhoodAmplitude[iTimeBin] += pIt->fNeighborhoodAmplitude;

                    trackSNR[iTimeBin] += pIt->fAmplitude/pIt->fMean;
                    trackNUP[iTimeBin] += (pIt->fAmplitude - pIt->fMean) / pIt->fVariance;

                    wideTrackSNR[iTimeBin] += pIt->fNeighborhoodAmplitude/pIt->fMean;
                    wideTrackNUP[iTimeBin] += (pIt->fNeighborhoodAmplitude - pIt->fMean) / pIt->fVariance;
                    
                    ++nTrackBins;

                    // break; //Since time bins are unique
                }
            }
        }

        KTDEBUG(tlog, "Averaging");
        for (unsigned iTimeBin = 0; iTimeBin<nTimeBins; ++iTimeBin)
        {
            averageFrequency.push_back(sumPf[iTimeBin]/sumP[iTimeBin]);
            KTDEBUG(tlog, timeBinInAcq[iTimeBin] << '\t' << averageFrequency[iTimeBin]);
        }

        // Determining the slope and intercept from Chi-2 minimization
        double sumXY = 0, sumXX=0, sumX=0, sumY=0, sumOne = 0, amplitudeSum = 0;

        for (unsigned iTimeBin = 0; iTimeBin<nTimeBins; ++iTimeBin)
        {
            sumXY += averageFrequency[iTimeBin] * timeBinInAcq[iTimeBin];
            sumXX += timeBinInAcq[iTimeBin] * timeBinInAcq[iTimeBin];
            sumX += timeBinInAcq[iTimeBin];
            sumY += averageFrequency[iTimeBin];
            sumOne += 1.;
            amplitudeSum += sumP[iTimeBin];
        }
        double slope = (sumXY*sumOne-sumY*sumX)/(sumXX*sumOne-sumX*sumX);
        double intercept = sumY/sumOne-slope*sumX/sumOne;
        double rho = -sumX/sqrt(sumXX*sumOne); // correlation coefficient between slope and intercept
        KTDEBUG(tlog, "Weighted average Frequency results: \n" <<
                      "\tSlope: " << '\t' << slope << '\n' <<
                      "\tIntercept: " << '\t' << intercept);
        KTDEBUG(tlog, "Amplitude of the track: " << amplitudeSum );

        //Calculating Chi^2_min
        double chi2min = 0;
        double residual = 0;
        for (unsigned iTimeBin = 0; iTimeBin<nTimeBins; ++iTimeBin)
        {
            residual = averageFrequency[iTimeBin] - slope*timeBinInAcq[iTimeBin] - intercept;
            chi2min += residual * residual;
            KTDEBUG(tlog, "Residuals : " << residual );
        }
        // Calculate error on slope and intercept for a rescaled Ch^2_min = 1
        double deltaSlope = 0;
        double deltaIntercept = 0;
        double sigmaStartFreq = 0;
        double sigmaEndFreq = 0;

        // need at least 3 points to get a non-zero Ndf
        if (nTimeBins>2)
        {
            KTDEBUG(tlog, "Chi2min : " << chi2min );

            if (chi2min < 0.1)
            {
                KTDEBUG(tlog, "Chi2min too small (points are mostlikely aligned): assigning arbitrary errors to the averaged Frequency points (" << fProcTrackAssignedError << ")");
                deltaSlope = 1.52/(sqrt(sumXX)/fProcTrackAssignedError);
                deltaIntercept = 1.52/(sqrt(sumOne)/fProcTrackAssignedError);
            }
            else
            {
                double ndf = nTimeBins - 2; // 2: two fitting parameters
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
        // KTProcessedTrackData& procTrack = swfData.Of< KTProcessedTrackData >();
        procTrack->SetComponent(trackID.fComponent);
        procTrack->SetAcquisitionID(trackID.fAcquisitionID);
        procTrack->SetTrackID(trackID.fCandidateID);

        procTrack->SetStartTimeInAcq(*std::min_element(timeBinInAcq.begin(), timeBinInAcq.end()));
        procTrack->SetStartTimeInRunC(*std::min_element(timeBinInRunC.begin(), timeBinInRunC.end()));
        procTrack->SetEndTimeInRunC(*std::max_element(timeBinInRunC.begin(), timeBinInRunC.end()));
        procTrack->SetTimeLength(procTrack->GetEndTimeInRunC() - procTrack->GetStartTimeInRunC());
        procTrack->SetStartFrequency(procTrack->GetStartTimeInAcq() * slope + intercept);
        procTrack->SetEndFrequency((procTrack->GetStartTimeInAcq() + procTrack->GetTimeLength()) * slope + intercept);
        procTrack->SetFrequencyWidth(std::abs(procTrack->GetEndFrequency() - procTrack->GetStartFrequency()));
        procTrack->SetSlope(slope);
        procTrack->SetIntercept(intercept);
        procTrack->SetTotalPower(amplitudeSum);

        procTrack->SetNTrackBins(nTrackBins);
        procTrack->SetTotalTrackSNR(std::accumulate(trackSNR.begin(),trackSNR.end(),0.));
        procTrack->SetMaxTrackSNR(*std::max_element(trackSNR.begin(), trackSNR.end()));
        procTrack->SetTotalTrackNUP(std::accumulate(trackNUP.begin(),trackNUP.end(),0.));
        procTrack->SetMaxTrackNUP(*std::max_element(trackNUP.begin(), trackNUP.end()));
        procTrack->SetTotalWideTrackSNR(std::accumulate(wideTrackSNR.begin(),wideTrackSNR.end(),0.));
        procTrack->SetTotalWideTrackNUP(std::accumulate(wideTrackNUP.begin(),wideTrackNUP.end(),0.));


        if (!(slope > fSlopeMinimum))
        {
            procTrack->SetIsCut(true);
        }
        procTrack->SetSlopeSigma(deltaSlope);
        procTrack->SetInterceptSigma(deltaIntercept);
        procTrack->SetStartFrequencySigma(sigmaStartFreq);
        procTrack->SetEndFrequencySigma(sigmaEndFreq);

        return true;
    }

    KTTrackProcessing::TrackID KTTrackProcessing::ExtractTrackID(KTSparseWaterfallCandidateData swfData)
    {
        TrackID trackID;
        trackID.fComponent = swfData.GetComponent();
        trackID.fAcquisitionID = swfData.GetAcquisitionID();
        trackID.fCandidateID = swfData.GetCandidateID();
        return trackID;
    }

} /* namespace Katydid */
