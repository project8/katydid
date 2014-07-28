/*
 * KTTrackProcessing.cc
 *
 *  Created on: July 22, 2013
 *      Author: N.S. Oblath & B. LaRoque
 */

#include "KTTrackProcessing.hh"

#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"
#include "KTProcessedTrackData.hh"
#include "KTSmooth.hh"
#include "KTSparseWaterfallCandidateData.hh"

#include <limits>
#include <vector>

using boost::shared_ptr;
using std::vector;

namespace Katydid
{
    KTLOGGER(tlog, "katydid.fft");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTTrackProcessing, "track-proc");

    KTTrackProcessing::KTTrackProcessing(const std::string& name) :
            KTProcessor(name),
            fPointLineDistCut1(0.1),
            fPointLineDistCut2(0.05),
            fTrackSignal("track", this),
            fSWFAndHoughSlot("swfc-and-hough", this, &KTTrackProcessing::ProcessTrack, &fTrackSignal)
    {
    }

    KTTrackProcessing::~KTTrackProcessing()
    {
    }

    bool KTTrackProcessing::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetPointLineDistCut1(node->GetValue("pl-dist-cut1", GetPointLineDistCut1()));
        SetPointLineDistCut2(node->GetValue("p2-dist-cut2", GetPointLineDistCut2()));

        return true;
    }

    bool KTTrackProcessing::ProcessTrack(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData)
    {
        unsigned component = swfData.GetComponent();

        typedef KTSparseWaterfallCandidateData::Points Points;
        const Points& points = swfData.GetPoints();

        // NOTE: smoothes the actual data, not a copy
        KTPhysicalArray< 2, double >* houghTransform = htData.GetTransform(component);

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
        typedef vector< std::pair< double, double > > SimplePoints;
        SimplePoints pointsScaled;
        pointsScaled.reserve(nPoints);
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
            xScaled = (pIt->fTimeInRun - xOffset) / xScale;
            yScaled = (pIt->fFrequency - yOffset) / yScale;
            pointsScaled.push_back(SimplePoints::value_type(xScaled, yScaled));
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
        double lsSlopeUnscaled = (sumXY - sumX * yMean) / (sumX2 - sumX * xMean);
        double lsInterceptUnscaled = yMean - lsSlopeUnscaled * xMean;
        // calculate a, b and c before we rescale
        double lsSlope = lsSlopeUnscaled * yScale / xScale;
        double lsIntercept = lsInterceptUnscaled * yScale + yOffset - lsSlope * xOffset;
        KTDEBUG(tlog, "Lease-squares fit result\n"
            << "\tSlope: " << lsSlope << " Hz/s\n"
            << "\tIntercept: " << lsIntercept << " Hz");

        // second distance cut based on LS fit
        double startT = std::numeric_limits< double >::max();
        double stopT = 0.;
        double startF = 0.;
        double stopF = 0.;
        nPointsUsed = 0;
        unsigned nPointsCut2 = 0;
        for (unsigned iPoint = 0; iPoint < nPoints; ++iPoint)
        {
            distance = PointLineDistance(pointsScaled[iPoint].first, pointsScaled[iPoint].second, lsSlopeUnscaled, -1., lsInterceptUnscaled);;

            if (pointsCuts[iPoint] == 0 && distance < fPointLineDistCut2)
            {
                // point is not cut
                ++nPointsUsed;
                if (pointsScaled[iPoint].first < startT) //possibly update start time/frequency
                {
                    startT = pointsScaled[iPoint].first;
                    startF = startT * lsSlope + lsIntercept;
                }
                if (pointsScaled[iPoint].first > stopT)
                { //possibly update stop time/frequency
                    stopT = pointsScaled[iPoint].first;
                    stopF = stopT * lsSlope + lsIntercept;
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

        // Add the new data
        KTProcessedTrackData& procTrack = htData.Of< KTProcessedTrackData >();
        procTrack.SetComponent(component);
        procTrack.SetTimeInRun(startT);
        procTrack.SetTimeLength(stopT - startT);
        procTrack.SetMinimumFrequency(startF);
        procTrack.SetMaximumFrequency(stopF);
        procTrack.SetFrequencyWidth(stopF - startF);
        procTrack.SetSlope(lsSlope);
        procTrack.SetIntercept(lsIntercept);

        return true;
    }


} /* namespace Katydid */
