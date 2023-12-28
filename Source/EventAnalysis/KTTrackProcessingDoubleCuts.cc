/**
 @file KTTrackProcessingDoubleCuts.cc
 @brief Contains KTTrackProcessingDoubleCuts
 @details Extracts physics-relevant information about tracks using a double-cuts algorithm
 @author: N.S. Oblath, B. LaRoque & M. Guigue
 @date: July 22, 2013
 */

#include "KTTrackProcessingDoubleCuts.hh"

#include "KTHoughData.hh"
#include "logger.hh"

#include "KTProcessedTrackData.hh"
#include "KTSmooth.hh"
#include "KTSparseWaterfallCandidateData.hh"
// #include "KTSequentialLineData.hh"

#include <limits>
#include <vector>
#include <algorithm>
#include <numeric>

using boost::shared_ptr;
using std::vector;
using std::string;

namespace Katydid
{
    LOGGER(tlog, "KTTrackProcessingDoubleCuts");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTTrackProcessingDoubleCuts, "track-proc-dc");

    KTTrackProcessingDoubleCuts::KTTrackProcessingDoubleCuts(const std::string& name) :
            KTProcessor(name),
            fPointLineDistCut1(0.1),
            fPointLineDistCut2(0.05),
            fSlopeMinimum(-std::numeric_limits< double >::max()),
            fProcTrackMinPoints(0),
            fProcTrackAssignedError(0.),
            fTrackSignal("track", this),
            // fTrackProcPtr(&KTTrackProcessingDoubleCuts::ProcessTrackDoubleCuts),
            fSWFAndHoughSlot("swfc-and-hough", this, &KTTrackProcessingDoubleCuts::ProcessTrack, &fTrackSignal)
            // fSeqAndHoughSlot("seqc-and-hough", this, &KTTrackProcessingDoubleCuts::ProcessTrack, &fTrackSignal)
    {
    }

    KTTrackProcessingDoubleCuts::~KTTrackProcessingDoubleCuts()
    {
    }

    bool KTTrackProcessingDoubleCuts::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetPointLineDistCut1(node->get_value("pl-dist-cut1", GetPointLineDistCut1()));
        SetPointLineDistCut2(node->get_value("pl-dist-cut2", GetPointLineDistCut2()));
        SetSlopeMinimum(node->get_value("min-slope", GetSlopeMinimum()));
        SetProcTrackMinPoints(node->get_value("min-points", GetProcTrackMinPoints()));
        SetProcTrackAssignedError(node->get_value("assigned-error", GetProcTrackAssignedError()));

        return true;
    }

    template<typename TracklikeCandidate> 
    bool KTTrackProcessingDoubleCuts::ProcessTrack(TracklikeCandidate& tlcData, KTHoughData& htData)
    // Method associated with the <SWFC> slot
    {

        TrackID trackID = ExtractTrackID(tlcData);
        Points& points = tlcData.GetPoints();
        // The & is important!!
        KTProcessedTrackData& procTrack = tlcData.template Of< KTProcessedTrackData >();

        LDEBUG(tlog, "Setting track reconstruction using \"weighted-slope\" algorithm");
        return KTTrackProcessingDoubleCuts::DoDoubleCutsAlgorithm(points, htData, trackID, &procTrack);
    }

    bool KTTrackProcessingDoubleCuts::DoDoubleCutsAlgorithm(Points& points, KTHoughData& htData, TrackID trackID, KTProcessedTrackData* procTrack)
    {
        // not const because the HT will be smoothed in place
        KTPhysicalArray< 2, double >* houghTransform = htData.GetTransform(trackID.fComponent);

        // NOTE: smoothes the actual data, not a copy
        if (! KTSmooth::Smooth(houghTransform))
        {
            LERROR(tlog, "Error while smoothing Hough Transform");
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

        LDEBUG(tlog, "Hough Transform track processing results:\n"
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
        double sumX = 0., sumY = 0., sumX2 = 0., sumXY = 0.; // for least-squares calculation
        // first distance cut, plus calculation of the initial least-squares line calculations
        for (Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            //cout << "calculating a distance..." << endl;
            pointsUnscaled.push_back(SimplePoints2D::value_type(pIt->fTimeInRunC, pIt->fFrequency));
            pointsUnscaledInAcq.push_back(SimplePoints2D::value_type(pIt->fTimeInAcq, pIt->fFrequency));
            double xScaled = (pIt->fTimeInRunC - xOffset) / xScale;
            double yScaled = (pIt->fFrequency - yOffset) / yScale;
            pointsScaled.push_back(SimplePoints2D::value_type(xScaled, yScaled));
            //cout << "i: " << iPoint << "\t y_i: " << ys[iPoint] << "\t x_i: " << xs[iPoint] << endl;
            //cout << "scaled: y_isc: " << yiscaled << "\t xisc: " << xiscaled << endl;
            double distance = PointLineDistance(xScaled, yScaled, htCosAngle, htSinAngle, -htRadius);
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
        LDEBUG(tlog, "Points removed with cut 1: " << nPointsCut1);

        // Refine the line with a least-squares line calculation
        double xMean = sumX / (double)nPointsUsed;
        double yMean = sumY / (double)nPointsUsed;
        double lsSlopeScaled = (sumXY - sumX * yMean) / (sumX2 - sumX * xMean);
        double lsInterceptScaled = yMean - lsSlopeScaled * xMean;
        double lsSlope = lsSlopeScaled * yScale / xScale;
        double lsIntercept = lsInterceptScaled * yScale + yOffset - lsSlope * xOffset;
        LDEBUG(tlog, "Least-squares fit result\n"
            << "\tSlope: " << lsSlope << " Hz/s\n"
            << "\tIntercept: " << lsIntercept << " Hz");

        // second distance cut based on LS fit
        double startTime = std::numeric_limits< double >::max();
        double stopTime = -1.;
        double startTimeInAcq = 0;
        //nPointsUsed = 0;
        double startFreq = 0., stopFreq = 0.;
        unsigned nPointsCut2 = 0;
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            double distance = PointLineDistance(pointsScaled[iPoint].first, pointsScaled[iPoint].second, lsSlopeScaled, -1., lsInterceptScaled);;

            if (pointsCuts[iPoint] == 0 && distance < fPointLineDistCut2)
            {
                // point is not cut
                //++nPointsUsed;
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
        LDEBUG(tlog, "Points removed with cut 2: " << nPointsCut2);

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
                trackNUP.push_back((pItCache->fAmplitude - pItCache->fMean) / sqrt(pItCache->fVariance));

                wideTrackSNR.push_back(pItCache->fNeighborhoodAmplitude / pItCache->fMean);
                wideTrackNUP.push_back((pItCache->fNeighborhoodAmplitude - pItCache->fMean) / sqrt(pItCache->fVariance));
            }
            else
            {
                points.erase(pItCache);
            }
        }
        LDEBUG(tlog, "Points present after cuts: " << points.size());

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

    template<typename TracklikeCandidate> 
    KTTrackProcessingDoubleCuts::TrackID KTTrackProcessingDoubleCuts::ExtractTrackID(TracklikeCandidate tlcData)
    {
        TrackID trackID;
        trackID.fComponent = tlcData.GetComponent();
        trackID.fAcquisitionID = tlcData.GetAcquisitionID();
        trackID.fCandidateID = tlcData.GetCandidateID();
        return trackID;
    }

} /* namespace Katydid */
