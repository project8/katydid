/*
 * KTSequentialTrackFinder.cc
 *
 *  Created on: Sep 15, 2016
 *      Author: Christine
 */

#include "KTSequentialTrackFinder.hh"

#include "KTLogger.hh"

#include "KTEggHeader.hh"
#include "KTSliceHeader.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTGainVariationData.hh"
#include "KTSequentialLineData.hh"
//#include "KTProcessedTrackData.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTDiscriminatedPoints1DData.hh"


#include <numeric>
#include <cmath>

using std::vector;


namespace Katydid
{
    KTLOGGER(stflog, "KTSequentialTrackFinder");

    KTSequentialTrackFinder::STFDiscriminatedPoint::STFDiscriminatedPoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator& pointIt, double newTimeInRunC, double newTimeInAcq) :
            KTDiscriminatedPoint(newTimeInRunC, pointIt->second.fAbscissa, pointIt->second.fOrdinate, newTimeInAcq, pointIt->second.fMean, pointIt->second.fVariance, pointIt->second.fNeighborhoodAmplitude, pointIt->first)
    {}

    KTSequentialTrackFinder::STFDiscriminatedPoint::STFDiscriminatedPoint(KTKDTreeData::SetOfPoints::const_iterator& pointIt, double time, double frequency, double timeScaling) :
            KTDiscriminatedPoint(time, frequency, pointIt->fAmplitude, pointIt->fTimeInAcq * timeScaling, pointIt->fMean, pointIt->fVariance, pointIt->fNeighborhoodAmplitude, pointIt->fBinInSlice)
    {}


    KT_REGISTER_PROCESSOR(KTSequentialTrackFinder, "sequential-track-finder");

    KTSequentialTrackFinder::KTSequentialTrackFinder(const std::string& name) :
                    KTProcessor(name),
                    fTrimmingThreshold(6),
                    fLinePowerRadius(4),
                    fPointAmplitudeAfterVisit(0),
                    fMinFreqBinDistance(10),
                    fTimeGapTolerance(0.0005),
                    fFrequencyAcceptance(56166.0528183),
                    fInitialFrequencyAcceptance(0.0),
                    fSearchRadius(6),
                    fConvergeDelta(1.5),
                    fMinPoints(3),
                    fMinSlope(0.0),
                    fInitialSlope(3.0*pow(10,8)),
                    fNSlopePoints(10),
                    fMinBin(0),
                    fMaxBin(1),
                    fFreqBinWidth(0.0),
                    fTimeBinWidth(0.0),
                    fMinFrequency(0.),
                    fMaxFrequency(1.),
                    fSlopeMethod(slopeMethod::weighted_first_point_ref),
                    fCalculateMinBin(true),
                    fCalculateMaxBin(true),
                    fActiveLines(),
                    fNLines(0),
                    fApplyTotalPowerCut(false),
                    fApplyAveragePowerCut(false),
                    fApplyTotalSNRCut(false),
                    fApplyAverageSNRCut(false),
                    fApplyTotalUnitlessResidualCut(false),
                    fApplyAverageUnitlessResidualCut(false),
                    fTotalPowerThreshold(0.0),
                    fAveragePowerThreshold(0.0),
                    fTotalSNRThreshold(0.0),
                    fAverageSNRThreshold(0.0),
                    fTotalUnitlessResidualThreshold(0.0),
                    fAverageUnitlessResidualThreshold(0.0),
                    fCalcSlope(&KTSequentialTrackFinder::CalculateSlopeFirstRef),
                    fLineSignal("seq-cand", this),
                    fClusterDoneSignal("clustering-done", this),
                    fHeaderSlot("header", this, &KTSequentialTrackFinder::InitializeWithHeader),
                    fDiscrimPowerSlot("disc-1d-ps", this, &KTSequentialTrackFinder::CollectDiscrimPointsFromSlice),
                    fDiscrimSlot("disc-1d", this, &KTSequentialTrackFinder::CollectDiscrimPointsFromSlice),
                    fDiscrimKDTreeSlot("kd-tree", this, &KTSequentialTrackFinder::CollectDiscrimPointsFromKDTree),
                    fDoneSlot("done", this, &KTSequentialTrackFinder::AcquisitionIsOver, &fClusterDoneSignal)
    {
    }

    KTSequentialTrackFinder::~KTSequentialTrackFinder()
    {
    }

    bool KTSequentialTrackFinder::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinFrequency(node->get_value("min-frequency", GetMinFrequency()));
        SetMaxFrequency(node->get_value("max-frequency", GetMaxFrequency()));

        SetTrimmingThreshold(node->get_value("trimming-threshold", GetTrimmingThreshold()));
        SetLinePowerRadius(node->get_value("line-power-radius", GetLinePowerRadius()));
        SetMinPoints(node->get_value("min-points", GetMinPoints()));
        SetMinSlope(node->get_value("min-slope", GetMinSlope()));

        SetTimeGapTolerance(node->get_value("time-gap-tolerance", GetTimeGapTolerance()));
        SetFrequencyAcceptance(node->get_value("frequency-acceptance", GetFrequencyAcceptance()));
        SetInitialSlope(node->get_value("initial-slope", GetInitialSlope()));


        if (node->has("min-bin"))
        {
            SetMinBin(node->get_value< unsigned >("min-bin"));
            SetCalculateMinBin(false);
        }
        if (node->has("max-bin"))
        {
            SetMaxBin(node->get_value< unsigned >("max-bin"));
            SetCalculateMaxBin(false);
        }
        if (node->has("minimum-line-bin-distance"))
        {
            SetMinFreqBinDistance(node->get_value<double>("minimum-line-distance"));
        }
        if (node->has("search-radius"))
        {
            SetSearchRadius(node->get_value<int>("search-radius"));
        }
        if (node->has("converge-delta"))
        {
            SetConvergeDelta(node->get_value<int>("converge-delta"));
        }
        if (node->has("initial-frequency-acceptance"))
        {
            SetInitialFrequencyAcceptance(node->get_value("initial-frequency-acceptance", GetInitialFrequencyAcceptance()));
        }
        else
        {
            SetInitialFrequencyAcceptance(node->get_value("frequency-acceptance", GetInitialFrequencyAcceptance()));
        }
        if (node->has("apply-power-cut"))
        {
            SetApplyTotalPowerCut(node->get_value("apply-total-power-cut", GetApplyTotalPowerCut()));
            SetTotalPowerThreshold(node->get_value("total-power-threshold", GetTotalPowerThreshold()));
        }
        if (node->has("apply-power-density-cut"))
        {
            SetApplyAveragePowerCut(node->get_value("apply-average-power-cut", GetApplyAveragePowerCut()));
            SetAveragePowerThreshold(node->get_value("average-power-threshold", GetAveragePowerThreshold()));
        }
        if (node->has("apply-total-snr-cut"))
        {
            SetApplyTotalSNRCut(node->get_value("apply-total-snr-cut", GetApplyTotalSNRCut()));
            SetTotalSNRThreshold(node->get_value("total-snr-threshold", GetTotalSNRThreshold()));
        }
        if (node->has("apply-average-snr-cut"))
        {
            SetApplyAverageSNRCut(node->get_value("apply-average-snr-cut", GetApplyAverageSNRCut()));
            SetAverageSNRThreshold(node->get_value("average-snr-threshold", GetAverageSNRThreshold()));
        }
        if (node->has("apply-total-nup-cut"))
        {
            SetApplyTotalUnitlessResidualCut(node->get_value("apply-total-nup-cut", GetApplyTotalUnitlessResidualCut()));
            SetTotalUnitlessResidualThreshold(node->get_value("total-nup-threshold", GetTotalUnitlessResidualThreshold()));
        }
        if (node->has("apply-average-nup-cut"))
        {
            SetApplyAverageUnitlessResidualCut(node->get_value("apply-average-nup-cut", GetApplyAverageUnitlessResidualCut()));
            SetAverageUnitlessResidualThreshold(node->get_value("average-nup-threshold", GetAverageUnitlessResidualThreshold()));
        }
        if (node->has("n-slope-points"))
        {
            SetNSlopePoints(node->get_value("n-slope-points", GetNSlopePoints()));
        }
        if (node->has("slope-method"))
        {
            if (node->get_value("slope-method") == "weighted-first-point-ref")
            {
                SetSlopeMethod(slopeMethod::weighted_first_point_ref);
            }
            else if (node->get_value("slope-method") == "weighted-last-point-ref")
            {
                SetSlopeMethod(slopeMethod::weighted_last_point_ref);
            }
            //else if (node->get_value("slope-method") == "weighted")
            //{
            //     SetSlopeMethod(slopeMethod::weighted);
            //}
            else if (node->get_value("slope-method") == "unweighted")
            {
                SetSlopeMethod(slopeMethod::unweighted);
            }
            else
            {
                KTERROR(stflog, "Set slope method not valid");
            }
        }
        if (fSlopeMethod == slopeMethod::weighted_first_point_ref)
        {
            fCalcSlope = &KTSequentialTrackFinder::CalculateSlopeFirstRef;
        }
        if (fSlopeMethod == slopeMethod::weighted_last_point_ref)
        {
            fCalcSlope = &KTSequentialTrackFinder::CalculateSlopeLastRef;
        }
        //if (fSlopeMethod == slopeMethod::weighted)
        //{
        //    fCalcSlope = &KTSequentialTrackFinder::CalculateWeightedSlope;
        //}
        if (fSlopeMethod == slopeMethod::unweighted)
        {
            fCalcSlope = &KTSequentialTrackFinder::CalculateUnweightedSlope;
        }

        return true;
    }

    bool KTSequentialTrackFinder::InitializeWithHeader(KTEggHeader& header)
    {
        fTimeBinWidth = 1. / header.GetAcquisitionRate();
        fFreqBinWidth = 1. / (fTimeBinWidth * header.GetChannelHeader(0)->GetSliceSize());

        return true;
    }

    bool KTSequentialTrackFinder::CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum, KTDiscriminatedPoints1DData& discrimPoints)
    {
        KTDEBUG(stflog, "Initial slope is: "<<fInitialSlope);

        unsigned nComponents = spectrum.GetNComponents();

        if (fCalculateMinBin)
        {
            SetMinBin(spectrum.GetSpectrum(0)->FindBin(fMinFrequency));
            KTDEBUG(stflog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(spectrum.GetSpectrum(0)->FindBin(fMaxFrequency));
            KTDEBUG(stflog, "Maximum bin set to " << fMaxBin);
        }


        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            uint64_t acqID = slHeader.GetAcquisitionID(iComponent);
            KTPowerSpectrum powerSpectrum= *spectrum.GetSpectrum(iComponent);

            unsigned nBins = fMaxBin - fMinBin + 1;
            double freqMin = powerSpectrum.GetBinLowEdge(fMinBin);
            double freqMax = powerSpectrum.GetBinLowEdge(fMaxBin) + powerSpectrum.GetBinWidth();

            //KTSpline* spline = fGVData.GetSpline(iComponent);
            //KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);
            //fReferenceThreshold = (*splineImp)((fMinBin+fMaxBin)/2)*fSNRPowerThreshold;

            fFreqBinWidth = powerSpectrum.GetBinWidth();

            double newTimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double newTimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(stflog, "new_TimeInAcq is " << newTimeInAcq);


            // this set will collect the discriminated points sorted by power
            STFDiscriminatedPowerSortedPoints points;

            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discrimPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin(); pIt != incomingPts.end(); ++pIt)
            {
                //KTINFO(stflog, "discriminated point: bin = " <<pIt->first<< ", frequency = "<<pIt->second.fAbscissa<< ", amplitude = "<<pIt->second.fOrdinate<<", "<<powerSpectrum(pIt->first) <<", threshold = "<<pIt->second.fThreshold);
                points.emplace(pIt, newTimeInRunC, newTimeInAcq);
            }

            KTDEBUG( stflog, "Collected "<<points.size()<<" points");
            // sort vector with points by power
            // std::sort(points.begin(), points.end(),std::less<KTSequentialLineData::Point>());

            // Loop over the high power points
            this->LoopOverHighPowerPoints(powerSpectrum, points, acqID, iComponent);

        }
        return true;
    }

    bool KTSequentialTrackFinder::CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discrimPoints)
    {
        KTDEBUG(stflog, "Initial slope is: " << fInitialSlope);

        unsigned nComponents = 1;
        fFreqBinWidth = (double) slHeader.GetSampleRate() / (double) slHeader.GetRawSliceSize();
        KTDEBUG(stflog, "Frequency bin width " << fFreqBinWidth);

        if (fCalculateMinBin)
        {
            SetMinBin((unsigned) ( fMinFrequency / fFreqBinWidth ) );
            KTDEBUG(stflog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin((unsigned) ( fMaxFrequency / fFreqBinWidth ) );
            KTDEBUG(stflog, "Maximum bin set to " << fMaxBin);
        }


        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            uint64_t acqID = slHeader.GetAcquisitionID(iComponent);

            unsigned nBins = fMaxBin - fMinBin + 1;


            double newTimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double newTimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(stflog, "new_TimeInAcq is " << newTimeInAcq);
            KTDEBUG(stflog, "new_TimeInRunC is " << newTimeInRunC);

            // this set will collect the discriminated points sorted by power
            STFDiscriminatedPowerSortedPoints points;

            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discrimPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin(); pIt != incomingPts.end(); ++pIt)
            {
                if ( pIt->first >= fMinBin and pIt->first <= fMaxBin )
                {
                    //KTINFO(stflog, "discriminated point: bin = " <<pIt->first<< ", frequency = "<<pIt->second.fAbscissa<< ", amplitude = "<<pIt->second.fOrdinate <<", threshold = "<<pIt->second.fThreshold);
                    points.emplace(pIt, newTimeInRunC, newTimeInAcq);
                }
            }

            // sort points by power
            //std::sort(points.begin(), points.end(),std::less<KTSequentialLineData::Point>());
            KTDEBUG( stflog, "Collected "<<points.size()<<" points");

            // Loop over the high power points
            this->LoopOverHighPowerPoints(points, acqID, iComponent);

        }
        return true;
    }

    bool KTSequentialTrackFinder::CollectDiscrimPointsFromKDTree(KTKDTreeData& kdTreeData)
    {
        KTDEBUG(stflog, "Initial slope is: " << fInitialSlope);

        unsigned nComponents = 1;
        KTDEBUG(stflog, "Frequency bin Width " << fFreqBinWidth);

        if (fCalculateMinBin)
        {
            SetMinBin((unsigned) ( fMinFrequency / fFreqBinWidth ) );
            KTDEBUG(stflog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin((unsigned) ( fMaxFrequency / fFreqBinWidth ) );
            KTDEBUG(stflog, "Maximum bin set to " << fMaxBin);
        }

        uint64_t acqID = kdTreeData.GetAcquisitionID();

        // We need to be able to detect when we've moved from slice to slice
        // So we define a threshold for delta-t, because time values might not be the exact same due to floating-point uncertainty.
        // We'll use the bin width, which is tiny relative to the slice size and stride.
        // 3-times the bin width should be large enough compared to uncertainty on the slice time and smaller than any reasonable stride.
        double deltaTThreshold = 3. * fTimeBinWidth;

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            unsigned nBins = fMaxBin - fMinBin + 1;

            const KTKDTreeData::SetOfPoints& allPoints = kdTreeData.GetSetOfPoints(iComponent);

            // this set will collect the discriminated points sorted by power
            STFDiscriminatedPowerSortedPoints points;

            double lastTime = -1000.;
            for (auto pointIt = allPoints.begin(); pointIt != allPoints.end(); ++pointIt)
            {
                double time = pointIt->fCoords[0] * kdTreeData.GetXScaling();
                double freq = pointIt->fCoords[1] * kdTreeData.GetYScaling();

                // check if we've moved onto the next slice and if we have points collected already
                if (time - lastTime > deltaTThreshold && ! points.empty())
                {
                    // this point is on the next slice; process these points then reset the points set
                    KTDEBUG( stflog, "Collected " << points.size() << " points");

                    // Loop over the high power points
                    this->LoopOverHighPowerPoints(points, acqID, iComponent);

                    // we're done with those points
                    points.clear();
                }

                // check if it's not noise, and that we're within the frequency bounds
                if (! pointIt->fNoiseFlag && freq > fMinFrequency && freq < fMaxFrequency)
                {
                    points.emplace(pointIt, time, freq, kdTreeData.GetXScaling());
                }

                lastTime = time;
            }

            if (! points.empty())
            {
                KTDEBUG( stflog, "Collected "<<points.size()<<" points");

                // Loop over the high power points
                this->LoopOverHighPowerPoints(points, acqID, iComponent);
            }
        }
        return true;
    }

    bool KTSequentialTrackFinder::LoopOverHighPowerPoints(KTPowerSpectrum& slice, STFDiscriminatedPowerSortedPoints& points, uint64_t acqID, unsigned component)
    {
        KTDEBUG(stflog, "Time and Frequency tolerances are "<<fTimeGapTolerance<<" "<<fFrequencyAcceptance);

        double newFreq = 0.0;
        bool match;

        //loop in reverse order (by power)
        for(STFDiscriminatedPowerSortedPoints::reverse_iterator pointIt = points.rbegin(); pointIt != points.rend(); ++pointIt)
        {
            newFreq = pointIt->fFrequency;

            // The amplitude of the bin the in the slice at the position of the point in the power spectrum gets set to zero after a visit (in SearchTrueLinePoint)
            // To prevent that in the next iteration the point gets re-found and added to another line the amplitude of the point is reassigned here

            STFDiscriminatedPoint tempPoint = *pointIt;
            tempPoint.fAmplitude = slice(pointIt->fBinInSlice);
            if (tempPoint.fAmplitude == 0.0)
            {
                KTDEBUG(stflog, "Point amplitude is 0, skipping point");
                continue;
            }
            else
            {

                this->UpdateLinePoint(tempPoint, slice);
                newFreq = tempPoint.fFrequency;
            }
            if (newFreq == 0.0 or tempPoint.fAmplitude==0.0)
            {
                KTDEBUG(stflog, "Point frequency and/or amplitude is zero, skipping point");
                continue;
            }
            else
            {
                match = false;

                // loop over active lines, in order of earliest start time
                // dont need to sort them because they are already sorted by the slice of the line's start point

                //KTDEBUG(stflog, "Currently there are N active Lines "<<fActiveLines.size());

                std::vector< KTSequentialLineData >::iterator lineIt = fActiveLines.begin();
                while( lineIt != fActiveLines.end())
                {
                    // Check whether line should still be active. If not then check whether the line is a valid new track candidate.
                    if (lineIt->GetEndTimeInRunC() <pointIt->fTimeInRunC-fTimeGapTolerance)
                    {
                        if (lineIt->GetNPoints() >= fMinPoints)
                        {
                            lineIt->LineSNRTrimming(fTrimmingThreshold, fMinPoints);

                            if (lineIt->GetNPoints() >= fMinPoints and lineIt->GetSlope() >= fMinSlope)
                            {
                                KTINFO(stflog, "Found track candidate");
                                (this->*fCalcSlope)(*lineIt);
                                this->EmitPreCandidate(*lineIt);
                            }
                        }
                        // in any case, this line should be removed from the vector with active lines
                        lineIt = fActiveLines.erase(lineIt);
                    }
                    else
                    {
                        // Under these conditions a point will be added to a line
                        bool timeCondition = tempPoint.fTimeInRunC > lineIt->GetEndTimeInRunC();
                        bool anyPointCondition = std::abs(tempPoint.fFrequency - (lineIt->GetEndFrequency() + lineIt->GetSlope()*(pointIt->fTimeInAcq - lineIt->GetEndTimeInAcq()))) < fFrequencyAcceptance;
                        bool secondPointCondition = std::abs(tempPoint.fFrequency - (lineIt->GetEndFrequency() + lineIt->GetSlope()*(pointIt->fTimeInAcq - lineIt->GetEndTimeInAcq()))) < fInitialFrequencyAcceptance;

                        // if point matches this line: insert
                        if (timeCondition and anyPointCondition)
                        {
                            lineIt->AddPoint(tempPoint);
                            (this->*fCalcSlope)(*lineIt);
                            match = true;
                            break;
                        }
                        // if this line consists of only one point so far, try again with different radius
                        else if (lineIt->GetNPoints() == 1 and timeCondition and secondPointCondition)
                        {
                            KTDEBUG(stflog, "Trying initial-frequency-acceptance "<<fInitialFrequencyAcceptance);
                            lineIt->AddPoint(tempPoint);
                            (this->*fCalcSlope)(*lineIt);
                            match = true;
                            break;
                        }
                        // if not try next line
                        else
                        {
                            ++lineIt;
                        }
                    }
                }//end of while loop. all existing lines were compared

                // if point was not picked up
                if (match == false)
                {
                    //KTDEBUG(stflog, "Starting new line");

                    KTSequentialLineData newLine;
                    newLine.SetSlope( fInitialSlope );
                    newLine.SetAcquisitionID( acqID );
                    newLine.SetComponent( component );
                    newLine.AddPoint(tempPoint);
                    (this->*fCalcSlope)(newLine);
                    fActiveLines.push_back(newLine);
                    match = true;
                }
            }
        }
        return true;
    }

    bool KTSequentialTrackFinder::LoopOverHighPowerPoints(STFDiscriminatedPowerSortedPoints& points, uint64_t acqID, unsigned component)
    {
        KTDEBUG(stflog, "Adding " << points.size() << " points to lines; Time and Frequency tolerances are " << fTimeGapTolerance<< "s and " << fFrequencyAcceptance << "Hz");

        double newFreq = 0.0;
        bool match;

        //loop in reverse order (by power)
        for (STFDiscriminatedPowerSortedPoints::reverse_iterator pointIt = points.rbegin(); pointIt != points.rend(); ++pointIt)
        {
            //KTWARN( stflog, "Comparing point to lines: time: " << pointIt->fTimeInRunC << ", freq: " << pointIt->fFrequency << ", bin: " << pointIt->fBinInSlice << ", power: " << pointIt->fAmplitude);
            newFreq = pointIt->fFrequency;

            // Need to think about how to prevent visiting the same neighborhood twice

            if (pointIt->fAmplitude == 0.0)
            {
                KTDEBUG(stflog, "Point amplitude is 0, skipping point");
                continue;
            }
            if (newFreq == 0.0 or pointIt->fAmplitude==0.0)
            {
                KTDEBUG(stflog, "Point frequency and/or amplitude is zero, skipping point");
                continue;
            }
            else
            {
                match = false;

                // loop over active lines, in order of earliest start time
                // dont need to sort them because they are already sorted by the slice of the line's start point

                //KTWARN(stflog, "Currently there are " << fActiveLines.size() << " active Lines ");

                std::vector< KTSequentialLineData >::iterator lineIt = fActiveLines.begin();
                while( lineIt != fActiveLines.end())
                {
                    // Check whether line should still be active. If not then check whether the line is a valid new track candidate.
                    if (lineIt->GetEndTimeInRunC() < pointIt->fTimeInRunC - fTimeGapTolerance)
                    {
                        //KTWARN(stflog, "Gap between end of a line and the current time-in-run (" << pointIt->fTimeInRunC - lineIt->GetEndTimeInRunC()  << ") is larger than the gap tolerance; evaluating line");
                        if (lineIt->GetNPoints() >= fMinPoints)
                        {
                            //KTWARN(stflog, "    line had more than the minimum number of points (" << lineIt->GetNPoints() << ")");
                            lineIt->LineSNRTrimming(fTrimmingThreshold, fMinPoints);

                            if (lineIt->GetNPoints() >= fMinPoints and lineIt->GetSlope() >= fMinSlope)
                            {
                                KTDEBUG(stflog, "Found line candidate");
                                (this->*fCalcSlope)(*lineIt);
                                this->EmitPreCandidate(*lineIt);
                            }
                            //else
                            //{
                            //    KTWARN(stflog, "    line cut after SNR trimming");
                            //    KTWARN(stflog, "    npoints: " << lineIt->GetNPoints() << "; slope: " << lineIt->GetSlope());
                           // }
                        }
                        //else
                        //{
                        //    KTWARN(stflog, "    line was cut because it didn't have enough points (" << lineIt->GetNPoints() << ")");
                        //}
                        // in any case, this line should be removed from the vector with active lines
                        lineIt = fActiveLines.erase(lineIt);
                    }
                    else
                    {
                        // Under these conditions a point will be added to a line
                        bool timeCondition = pointIt->fTimeInRunC > lineIt->GetEndTimeInRunC();
                        //KTWARN(stflog, "time condition: " << timeCondition << " = " << pointIt->fTimeInRunC << " > " << lineIt->GetEndTimeInRunC() );
                        bool anyPointCondition = std::abs(pointIt->fFrequency - (lineIt->GetEndFrequency() + lineIt->GetSlope()*(pointIt->fTimeInAcq - lineIt->GetEndTimeInAcq()))) < fFrequencyAcceptance;
                        //KTWARN(stflog, "any point condition: " << anyPointCondition << " = | " << pointIt->fFrequency << " - ( " << lineIt->GetEndFrequency() << " + " << lineIt->GetSlope() << " * ( " << pointIt->fTimeInAcq << " - " << lineIt->GetEndTimeInAcq() << " ))| < " << fFrequencyAcceptance);
                        bool secondPointCondition = std::abs(pointIt->fFrequency - (lineIt->GetEndFrequency() + lineIt->GetSlope()*(pointIt->fTimeInAcq - lineIt->GetEndTimeInAcq()))) < fInitialFrequencyAcceptance;
                        //KTWARN(stflog, "second point condition: " << secondPointCondition << " = |" << pointIt->fFrequency << " - ( " << lineIt->GetEndFrequency() << " + " << lineIt->GetSlope() << " * ( " << pointIt->fTimeInAcq << " - " << lineIt->GetEndTimeInAcq() << " ))| < " << fInitialFrequencyAcceptance);

                        // if point matches this line: insert
                        if (timeCondition and anyPointCondition)
                        {
                            KTDEBUG(stflog, "Matching conditions fulfilled");
                            lineIt->AddPoint(*pointIt);
                            (this->*fCalcSlope)(*lineIt);
                            match = true;
                            break;
                        }
                        // if this line consists of only one point so far, try again with different radius
                        else if (lineIt->GetNPoints() == 1 and timeCondition and secondPointCondition)
                        {
                            KTDEBUG(stflog, "Trying initial-frequency-acceptance "<<fInitialFrequencyAcceptance);
                            lineIt->AddPoint(*pointIt);
                            (this->*fCalcSlope)(*lineIt);
                            match = true;
                            break;
                        }
                        // if not try next line
                        else
                        {
                            ++lineIt;
                        }
                    }
                }//end of while loop. all existing lines were compared

                // if point was not picked up
                if (match == false)
                {
                    //KTWARN(stflog, "Starting new line");

                    KTSequentialLineData newLine;
                    newLine.SetSlope( fInitialSlope );
                    newLine.SetAcquisitionID( acqID );
                    newLine.SetComponent( component );
                    newLine.AddPoint(*pointIt);
                    (this->*fCalcSlope)(newLine);
                    fActiveLines.push_back(newLine);
                    match = true;
                }
            }
        }
        return true;
    }


    bool KTSequentialTrackFinder::EmitPreCandidate(KTSequentialLineData& line)
    {
        KTDEBUG(stflog, "applying cuts and then emitting candidate");
        bool lineIsCandidate = true;

        line.CalculateTotalPower();
        line.CalculateTotalSNR();
        line.CalculateTotalNUP();

        if ( fApplyTotalPowerCut )
        {
            if ( line.GetTotalWidePower() <= fTotalPowerThreshold )
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyAveragePowerCut )
        {
            if ( line.GetTotalWidePower()/(line.GetEndTimeInRunC()-line.GetStartTimeInRunC()) <= fAveragePowerThreshold )
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyTotalSNRCut )
        {
            if ( line.GetTotalWideSNR() <= fTotalSNRThreshold)
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyAverageSNRCut )
        {
            if ( line.GetTotalWideSNR() / ( line.GetEndTimeInRunC() - line.GetStartTimeInRunC() ) <= fAverageSNRThreshold )
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyTotalUnitlessResidualCut )
        {
            if ( line.GetTotalWideNUP() <= fTotalUnitlessResidualThreshold )
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyAverageUnitlessResidualCut )
        {
            if ( line.GetTotalWideNUP() / ( line.GetEndTimeInRunC() - line.GetStartTimeInRunC() ) <= fAverageUnitlessResidualThreshold )
            {
                lineIsCandidate = false;
            }
        }
        // after all cuts have been applied and point cluster is still a candidate, create SparseWaterfallCandidateData
        if (lineIsCandidate == true)
        {

            // Set up new data object
            Nymph::KTDataPtr data( new Nymph::KTData() );
            KTSequentialLineData& newCand = data->Of< KTSequentialLineData >();
            newCand.SetComponent( line.GetComponent() );
            newCand.SetAcquisitionID( line.GetAcquisitionID());
            newCand.SetCandidateID( fNLines );
            newCand.SetSlope( line.GetSlope() );
            newCand.SetTotalSNR( line.GetTotalSNR() );
            newCand.SetTotalWideSNR( line.GetTotalWideSNR() );
            newCand.SetTotalPower( line.GetTotalPower() );
            newCand.SetTotalWidePower( line.GetTotalWidePower() );
            newCand.SetTotalNUP( line.GetTotalNUP() );
            newCand.SetTotalWideNUP( line.GetTotalWideNUP() );

            ++fNLines;

            // Add line points to swf candidate
            KTDiscriminatedPoints& points = line.GetPoints();
            for(KTDiscriminatedPoints::iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt )
            {
                KTDiscriminatedPoint newPoint(*pointIt);
                newCand.AddPoint(newPoint);
            }

            // Process & emit new track

            //KTINFO(stflog, "Now processing track candidate");
            //ProcessNewTrack( newTrack );

            KTDEBUG(stflog, "Emitting track signal");
            fCandidates.insert( data );
            fLineSignal( data );
        }
        else
        {
            KTDEBUG(stflog, "Line did not make it above the cut and was not emitted as candidate");
        }
        return true;
    }

    /*
    void KTSequentialTrackFinder::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );

        // the slope is already calculated
        //myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );

    }*/

    void KTSequentialTrackFinder::UpdateLinePoint(STFDiscriminatedPoint& point, KTPowerSpectrum& slice)
    {
        double delta = fConvergeDelta + 1.0;
        unsigned loopCounter = 0;
        int maxIterations = 10;

        double frequency = point.fFrequency;
        double amplitude = point.fAmplitude;
        unsigned frequencyBin = point.fBinInSlice;
        double oldFrequencyBin;



        while(std::abs(delta) > fConvergeDelta and loopCounter < maxIterations)
        {
            ++loopCounter;

            oldFrequencyBin = frequencyBin;

            if (frequencyBin > fMinBin + fSearchRadius and frequencyBin < fMaxBin - fSearchRadius)
            {
                this-> WeightedAverage(slice, frequencyBin, frequency);
                delta = std::abs(frequencyBin - oldFrequencyBin);
            }
            else
            {
                KTDEBUG(stflog, "frequency bin was not in allowed range ");
                delta = 0.0;
                amplitude = 0.0;
            }
        }

        // Calculate "true" line amplitude in slice by summing neighboring point amplitudes
        // and set slice amplitude to zero
        if (frequencyBin > fMinBin + fLinePowerRadius and frequencyBin < fMaxBin - fLinePowerRadius)
        {
            amplitude = 0;
            for (int iBin = frequencyBin - fLinePowerRadius; iBin <= frequencyBin + fLinePowerRadius; iBin++)
            {
                amplitude += slice(iBin);
                slice(iBin) = fPointAmplitudeAfterVisit;
            }
            amplitude = amplitude - ( 2 * fLinePowerRadius - 1 ) * point.fMean;
            //amplitude = amplitude / fLinePowerRadius;
        }
        else
        {
            amplitude = slice(frequencyBin);
            slice(frequencyBin) = fPointAmplitudeAfterVisit;
        }

        // Set area around point to zero to prevent the area from being revisited
        for (int it = -1* fMinFreqBinDistance; it <= fMinFreqBinDistance; it++)
        {
            int iBin = frequencyBin + it;
            if ( iBin > fMinBin and iBin < fMaxBin )
            {
                slice(iBin) = fPointAmplitudeAfterVisit;
            }
        }
        // Replace values stored in discPoint
        point.fBinInSlice = frequencyBin;
        point.fFrequency = frequency;
        point.fNeighborhoodAmplitude = amplitude;

    }

    inline void KTSequentialTrackFinder::WeightedAverage(const KTPowerSpectrum& slice, unsigned& frequencyBin, double& frequency)
    {
        unsigned newFrequencyBin = 0;
        double newFrequency = 0.0;
        double weightedBin = 0.0;
        double wSum = 0.0;

        for (int iBin = -1*fSearchRadius; iBin <= fSearchRadius; ++iBin)
        {
            weightedBin += double(frequencyBin+iBin)*slice(frequencyBin+iBin);
            wSum +=slice(frequencyBin+iBin);
        }
        newFrequencyBin = unsigned(weightedBin/wSum);
        newFrequency = fFreqBinWidth * ((weightedBin/wSum)+0.5); //((double)newFrequencyBin + 0.5);

        frequency = newFrequency;
        frequencyBin = newFrequencyBin;

    }


    void KTSequentialTrackFinder::AcquisitionIsOver()
    {
        KTINFO(stflog, "Got egg-done signal. Checking remaining line candidates");

        std::vector< KTSequentialLineData >::iterator lineIt = fActiveLines.begin();
        while( lineIt != fActiveLines.end())
        {
            if (lineIt->GetNPoints() >= fMinPoints)
            {
                lineIt->LineSNRTrimming(fTrimmingThreshold, fMinPoints);

                if (lineIt->GetNPoints() >= fMinPoints and lineIt->GetSlope() > fMinSlope)
                {
                    this->EmitPreCandidate(*lineIt);
                }
            }
            lineIt = fActiveLines.erase(lineIt);
        }
        KTDEBUG(stflog, "Now there should be no lines left over " << fActiveLines.empty());
    }

    /*void KTSequentialTrackFinder::CalculateWeightedSlope(LineRef& line)
    {
        double SumX = 0.0;
        double SumY = 0.0;
        double SumXY = 0.0;
        double SumXX = 0.0;

        //KTDEBUG(seqlog, "Calculating line slope");
        double weightedSlope = 0.0;
        double weight = 0.0;
        double wSum = 0.0;
        line.fNPoints = line.fLinePoints.size();

        if (line.fNPoints > 1)
        {
            for(std::vector<LinePoint>::iterator pointIt = line.fLinePoints.begin(); pointIt != line.fLinePoints.end(); ++pointIt)
            {
                   weight = pointIt->fAmplitude;

                   SumX += line.fLinePoints.back().fTimeInRunC *weight;
                   SumY += line.fLinePoints.back().fPointFreq * weight;
                   SumXY += line.fLinePoints.back().fTimeInRunC * line.fLinePoints.back().fPointFreq * weight *weight;
                   SumXX += line.fLinePoints.back().fTimeInRunC * line.fLinePoints.back().fTimeInRunC * weight * weight;
            }
            //SumX = SumX/line.fAmplitudeSum;
            //SumY = SumY/line.fAmplitudeSum;
            //SumXY = SumXY/(line.fAmplitudeSum*line.fAmplitudeSum);
            //SumXX = SumXX/(line.fAmplitudeSum*line.fAmplitudeSum);
            line.fSlope = (line.fNPoints * SumXY - SumX * SumY)/(SumXX * line.fNPoints - SumX * SumX);
            KTDEBUG( stflog, "Weighted slope method. New slope "<<line.fSlope);
        }
        if (line.fNPoints <= 1)
        {
            line.fSlope = fInitialSlope;
        }
    }*/
    void KTSequentialTrackFinder::CalculateUnweightedSlope(KTSequentialLineData& line)
    {

        //KTDEBUG(stflog, "Calculating line slope");

        KTDiscriminatedPoints& points = line.GetPoints();
        line.SetSumX( line.GetSumX() + points.rbegin()->fTimeInRunC) ;
        line.SetSumY( line.GetSumY() + points.rbegin()->fFrequency);
        line.SetSumXY( line.GetSumXY() + points.rbegin()->fTimeInRunC * points.rbegin()->fFrequency);
        line.SetSumXX( line.GetSumXX() + points.rbegin()->fTimeInRunC * points.rbegin()->fTimeInRunC);

        if (line.GetNPoints() > 1)
        {
            line.SetSlope( (line.GetNPoints() * line.GetSumXY() - line.GetSumX() * line.GetSumY())/(line.GetSumXX() * line.GetNPoints() - line.GetSumX() * line.GetSumX()) );
        }
        else
        {
            line.SetSlope( fInitialSlope );
        }
        //KTDEBUG( stflog, "Unweighted slope method. New slope "<<line.GetSlope());
    }

    void KTSequentialTrackFinder::CalculateSlopeFirstRef(KTSequentialLineData& line)
    {

        //KTDEBUG(stflog, "Calculating line slope "<<line.GetSNRList().rbegin()[fNSlopePoints-1]);
        //double weightedSlope = 0.0;
        //double wSum = 0.0;

        if (line.GetNPoints() > fNSlopePoints)
        {
            KTDiscriminatedPoints& points = line.GetPoints();
            KTDiscriminatedPoints::iterator pointIt = points.end();
            std::advance(pointIt, -1);
            line.SetWeightedSlopeSum( line.GetWeightedSlopeSum() + ( pointIt->fFrequency - line.GetStartFrequency() ) / ( pointIt->fTimeInRunC - line.GetStartTimeInRunC() ) * line.GetSNRList().back() );

            std::advance(pointIt, -(fNSlopePoints-1));
            line.SetWeightedSlopeSum( line.GetWeightedSlopeSum() - (pointIt->fFrequency - line.GetStartFrequency() ) / ( pointIt->fTimeInRunC - line.GetStartTimeInRunC() ) * line.GetSNRList().rbegin()[fNSlopePoints-1] );
            line.SetTotalWideSNR( line.GetTotalWideSNR() - line.GetSNRList().rbegin()[fNSlopePoints-1]);
            line.SetSlope(line.GetWeightedSlopeSum()/line.GetTotalWideSNR());
            /*KTDiscriminatedPoints& points = line.GetPoints();
            KTDiscriminatedPoints::iterator pointIt = points.end();
            std::advance(pointIt, -fNSlopePoints);
            while(pointIt != points.end())
            {
                if (pointIt->fFrequency != line.GetStartFrequency())
                {
                    weightedSlope += (pointIt->fFrequency - line.GetStartFrequency())/(pointIt->fTimeInAcq - line.GetStartTimeInAcq()) * pointIt->fAmplitude;
                    wSum += pointIt->fAmplitude;
                }
                ++pointIt;
            }
            line.SetSlope( weightedSlope/wSum );*/
        }
        else if (line.GetNPoints() > 1)
        {
            KTDiscriminatedPoints& points = line.GetPoints();
            line.SetWeightedSlopeSum( line.GetWeightedSlopeSum() + ( points.rbegin()->fFrequency - line.GetStartFrequency() ) / ( points.rbegin()->fTimeInRunC - line.GetStartTimeInRunC() ) * line.GetSNRList().back() );
            line.SetSlope(line.GetWeightedSlopeSum()/line.GetTotalWideSNR());
            /*for(KTDiscriminatedPoints::iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt)
            {
                if (pointIt->fFrequency != line.GetStartFrequency())
                {
                    weightedSlope += (pointIt->fFrequency - line.GetStartFrequency())/(pointIt->fTimeInAcq - line.GetStartTimeInAcq()) * pointIt->fAmplitude;
                    wSum += pointIt->fAmplitude;
                }
            }
            line.SetSlope( weightedSlope/wSum );*/
        }
        else
        {
            line.SetSlope( fInitialSlope );
        }
        //KTDEBUG(stflog, "Ref point slope method. New slope is " << line.GetSlope());
    }

    void KTSequentialTrackFinder::CalculateSlopeLastRef(KTSequentialLineData& line)
    {

        //KTDEBUG(seqlog, "Calculating line slope");
        double weightedSlope = 0.0;

        if (line.GetNPoints() > fNSlopePoints)
        {
            double wSum = 0.0;
            KTDiscriminatedPoints& points = line.GetPoints();
            KTDiscriminatedPoints::iterator pointIt = points.end();
            std::advance(pointIt, -fNSlopePoints);

            while(pointIt != points.end())
            {
                if (pointIt->fFrequency != line.GetEndFrequency())
                {
                    weightedSlope += (line.GetEndFrequency() - pointIt->fFrequency)/(line.GetEndTimeInRunC() - pointIt->fTimeInRunC) * pointIt->fNeighborhoodAmplitude/pointIt->fMean;
                    wSum += pointIt->fNeighborhoodAmplitude/pointIt->fMean;
                }
                ++pointIt;
            }
            line.SetSlope( weightedSlope/wSum );
        }
        else if (line.GetNPoints() > 1)
        {
            KTDiscriminatedPoints& points = line.GetPoints();
            for(KTDiscriminatedPoints::iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt)
            {
                if (pointIt->fFrequency != line.GetEndFrequency())
                {
                    weightedSlope += (line.GetEndFrequency() - pointIt->fFrequency)/(line.GetEndTimeInRunC() - pointIt->fTimeInRunC) * pointIt->fNeighborhoodAmplitude / pointIt->fMean;
                    //wSum += pointIt->fNeighborhoodAmplitude;
                }
            }
            line.SetSlope( weightedSlope/line.GetTotalWideSNR() );
        }
        else
        {
            line.SetSlope( fInitialSlope );
        }
        //KTDEBUG(stflog, "Ref point slope method. fNSlopePoints: "<<fNSlopePoints<<" . New slope is " << line.GetSlope());
    }
} /* namespace Katydid */
