/*
 * KTSequentialTrackFinder.cc
 *
 *  Created on: Sep 15, 2016
 *      Author: Christine
 */

#include "KTSequentialTrackFinder.hh"
#include "KTLogger.hh"


#include <numeric>
#include <cmath>

using std::vector;


namespace Katydid
{
    KTLOGGER(stflog, "KTSequentialTrackFinder");


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
            fBinWidth(0.0),
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
            fTrackSignal("swf-cand", this),
            fClusterDoneSignal("clustering-done", this),
            fDiscrimPowerSlot("disc1d-ps", this, &KTSequentialTrackFinder::CollectDiscrimPointsFromSlice),
            fDiscrimSlot("disc1d", this, &KTSequentialTrackFinder::CollectDiscrimPointsFromSlice),
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
            SetAverageSNRThreshold(node->get_value("power-average-snr-threshold", GetAverageSNRThreshold()));
        }
        if (node->has("apply-total-residual-cut"))
        {
            SetApplyTotalUnitlessResidualCut(node->get_value("apply-total-residual-cut", GetApplyTotalUnitlessResidualCut()));
            SetTotalUnitlessResidualThreshold(node->get_value("total-residual-threshold", GetTotalUnitlessResidualThreshold()));
        }
        if (node->has("apply-average-residual-cut"))
        {
            SetApplyAverageUnitlessResidualCut(node->get_value("apply-average-residual-cut", GetApplyAverageUnitlessResidualCut()));
            SetAverageUnitlessResidualThreshold(node->get_value("power-average-residual-threshold", GetAverageUnitlessResidualThreshold()));
        }
        if (node->has("n-slope-points"))
        {
            SetNSlopePoints(node->get_value("n-slope-points", GetNSlopePoints()));
        }
        if (node->has("slope-method"))
        {
            if (node->get_value("slope-method") == "weighted_first_point_ref")
            {
                SetSlopeMethod(slopeMethod::weighted_first_point_ref);
            }
            if (node->get_value("slope-method") == "weighted_last_point_ref")
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

    bool KTSequentialTrackFinder::CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum, KTDiscriminatedPoints1DData& discrimPoints)
    {
        this->CollectDiscrimPoints(slHeader, spectrum, discrimPoints);
        return true;
    }
    bool KTSequentialTrackFinder::CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discrimPoints)
    {
        this->CollectDiscrimPoints(slHeader, discrimPoints);
        return true;
    }


    bool KTSequentialTrackFinder::CollectDiscrimPoints(const KTSliceHeader& slHeader, const KTPowerSpectrumData& spectrum, const KTDiscriminatedPoints1DData& discrimPoints)
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

            fBinWidth = powerSpectrum.GetBinWidth();

            double newTimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double newTimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(stflog, "new_TimeInAcq is " << newTimeInAcq);


            // this vector will collect the discriminated points
            std::vector<KTSequentialLine::Point> points;

            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discrimPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin(); pIt != incomingPts.end(); ++pIt)
            {
                //KTINFO(stflog, "discriminated point: bin = " <<pIt->first<< ", frequency = "<<pIt->second.fAbscissa<< ", amplitude = "<<pIt->second.fOrdinate<<", "<<powerSpectrum(pIt->first) <<", threshold = "<<pIt->second.fThreshold);
                KTSequentialLine::Point newPoint(pIt->first, pIt->second.fAbscissa, newTimeInAcq, newTimeInRunC, pIt->second.fOrdinate, pIt->second.fThreshold, pIt->second.fMean, pIt->second.fVariance, pIt->second.fNeighborhoodAmplitude, acqID, iComponent);
                points.push_back(newPoint);
            }

            KTDEBUG( stflog, "Collected "<<points.size()<<" points");
            // sort points by power
            std::sort(points.begin(), points.end(),std::less<Point>());

            // Loop over the high power points
            this->LoopOverHighPowerPoints(powerSpectrum, points, iComponent);

        }
        return true;
    }

    bool KTSequentialTrackFinder::CollectDiscrimPoints(const KTSliceHeader& slHeader, const KTDiscriminatedPoints1DData& discrimPoints)
    {
        KTDEBUG(stflog, "Initial slope is: "<<fInitialSlope);

        unsigned nComponents = 1;
        fBinWidth = (double) slHeader.GetSampleRate() / (double) slHeader.GetRawSliceSize();
        KTDEBUG(stflog, "Bin Width "<<fBinWidth);

        if (fCalculateMinBin)
        {
            SetMinBin((unsigned) ( fMinFrequency / fBinWidth ) );
            KTDEBUG(stflog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin((unsigned) ( fMaxFrequency / fBinWidth ) );
            KTDEBUG(stflog, "Maximum bin set to " << fMaxBin);
        }


        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            uint64_t acqID = slHeader.GetAcquisitionID(iComponent);

            unsigned nBins = fMaxBin - fMinBin + 1;


            double newTimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double newTimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(stflog, "new_TimeInAcq is " << newTimeInAcq);


            // this vector will collect the discriminated points
            std::vector<Point> points;

            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discrimPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin(); pIt != incomingPts.end(); ++pIt)
            {
                if ( pIt->first >= fMinBin and pIt->first <= fMaxBin )
                {
                    //KTINFO(stflog, "discriminated point: bin = " <<pIt->first<< ", frequency = "<<pIt->second.fAbscissa<< ", amplitude = "<<pIt->second.fOrdinate<<", "<<powerSpectrum(pIt->first) <<", threshold = "<<pIt->second.fThreshold);
                    Point newPoint(pIt->first, pIt->second.fAbscissa, newTimeInAcq, newTimeInRunC, pIt->second.fOrdinate, pIt->second.fThreshold, pIt->second.fMean, pIt->second.fVariance, pIt->second.fNeighborhoodAmplitude, acqID, iComponent);
                    points.push_back(newPoint);
                }
            }

            // sort points by power
            std::sort(points.begin(), points.end(),std::less<Point>());
            KTDEBUG( stflog, "Collected "<<points.size()<<" points");

            // Loop over the high power points
            this->LoopOverHighPowerPoints(points, iComponent);

        }
        return true;
    }


    bool KTSequentialTrackFinder::LoopOverHighPowerPoints(KTPowerSpectrum& slice, std::vector<Point>& points, unsigned component)
     {
         KTDEBUG(stflog, "Time and Frequency tolerances are "<<fTimeGapTolerance<<" "<<fFrequencyAcceptance);

         double newFreq = 0.0;
         bool match;

         //loop in reverse order (by power)
         for(std::vector<Point>::reverse_iterator pointIt = points.rbegin(); pointIt != points.rend(); ++pointIt)
         {
             newFreq = pointIt->fPointFreq;

             // The amplitude of the bin the in the slice at the position of the point in the power spectrum gets set to zero after a visit (in SearchTrueLinePoint)
             // To prevent that in the next iteration the point gets re-found and added to another line the amplitude of the point is reassigned here
             pointIt->fAmplitude = slice(pointIt->fBinInSlice);

             if (pointIt->fAmplitude == 0.0)
             {
                 KTDEBUG(stflog, "Point amplitude is 0, skipping point");
                 continue;
             }
             else
             {
                 this->UpdateLinePoint(*pointIt, slice);
                 newFreq = pointIt->fPointFreq;
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

                 //KTDEBUG(stflog, "Currently there are N active Lines "<<fActiveLines.size());

                 std::vector< LineRef >::iterator lineIt = fActiveLines.begin();
                 while( lineIt != fActiveLines.end())
                 {
                     // Check whether line should still be active. If not then check whether the line is a valid new track candidate.
                     if (lineIt->fEndTimeInRunC<pointIt->fTimeInRunC-fTimeGapTolerance)
                     {
                         if (lineIt->fNPoints >= fMinPoints)
                         {
                             lineIt->LineSNRTrimming(fTrimmingThreshold, fMinPoints);

                             if (lineIt->fNPoints >= fMinPoints and lineIt->fSlope >= fMinSlope)
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
                         bool timeCondition = pointIt->fTimeInRunC > lineIt->fEndTimeInRunC;
                         bool anyPointCondition = std::abs(pointIt->fPointFreq - (lineIt->fEndFrequency + lineIt->fSlope*(pointIt->fTimeInAcq - lineIt->fEndTimeInAcq))) < fFrequencyAcceptance;
                         bool secondPointCondition = std::abs(pointIt->fPointFreq - (lineIt->fEndFrequency + lineIt->fSlope*(pointIt->fTimeInAcq - lineIt->fEndTimeInAcq))) < fInitialFrequencyAcceptance;

                         // if point matches this line: insert
                         if (timeCondition and anyPointCondition)
                         {
                             lineIt->InsertPoint(*pointIt);
                             (this->*fCalcSlope)(*lineIt);
                             match = true;
                             ++lineIt;
                         }
                         // if this line consists of only one point so far, try again with different radius
                         else if (lineIt->fNPoints == 1 and timeCondition and secondPointCondition)
                         {
                             KTDEBUG(stflog, "Trying initial-frequency-acceptance "<<fInitialFrequencyAcceptance);
                             lineIt->InsertPoint(*pointIt);
                             (this->*fCalcSlope)(*lineIt);
                             match = true;
                             ++lineIt;
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

                     LineRef new_line(fInitialSlope);
                     new_line.InsertPoint(*pointIt);
                     (this->*fCalcSlope)(new_line);
                     fActiveLines.push_back(new_line);
                     match = true;
                 }
             }
         }
         return true;
     }

    bool KTSequentialTrackFinder::LoopOverHighPowerPoints(std::vector<Point>& points, unsigned component)
     {
         KTDEBUG(stflog, "Time and Frequency tolerances are "<<fTimeGapTolerance<<" "<<fFrequencyAcceptance);

         double newFreq = 0.0;
         bool match;

         //loop in reverse order (by power)
         for(std::vector<Point>::reverse_iterator pointIt = points.rbegin(); pointIt != points.rend(); ++pointIt)
         {
             newFreq = pointIt->fPointFreq;

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

                 //KTDEBUG(stflog, "Currently there are N active Lines "<<fActiveLines.size());

                 std::vector< LineRef >::iterator lineIt = fActiveLines.begin();
                 while( lineIt != fActiveLines.end())
                 {
                     // Check whether line should still be active. If not then check whether the line is a valid new track candidate.
                     if (lineIt->fEndTimeInRunC<pointIt->fTimeInRunC-fTimeGapTolerance)
                     {
                         if (lineIt->fNPoints >= fMinPoints)
                         {
                             lineIt->LineSNRTrimming(fTrimmingThreshold, fMinPoints);

                             if (lineIt->fNPoints >= fMinPoints and lineIt->fSlope >= fMinSlope)
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
                         bool timeCondition = pointIt->fTimeInRunC > lineIt->fEndTimeInRunC;
                         bool anyPointCondition = std::abs(pointIt->fPointFreq - (lineIt->fEndFrequency + lineIt->fSlope*(pointIt->fTimeInAcq - lineIt->fEndTimeInAcq))) < fFrequencyAcceptance;
                         bool secondPointCondition = std::abs(pointIt->fPointFreq - (lineIt->fEndFrequency + lineIt->fSlope*(pointIt->fTimeInAcq - lineIt->fEndTimeInAcq))) < fInitialFrequencyAcceptance;

                         // if point matches this line: insert
                         if (timeCondition and anyPointCondition)
                         {
                             lineIt->InsertPoint(*pointIt);
                             (this->*fCalcSlope)(*lineIt);
                             match = true;
                             ++lineIt;
                         }
                         // if this line consists of only one point so far, try again with different radius
                         else if (lineIt->fNPoints == 1 and timeCondition and secondPointCondition)
                         {
                             KTDEBUG(stflog, "Trying initial-frequency-acceptance "<<fInitialFrequencyAcceptance);
                             lineIt->InsertPoint(*pointIt);
                             (this->*fCalcSlope)(*lineIt);
                             match = true;
                             ++lineIt;
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

                     LineRef new_line(fInitialSlope);
                     new_line.InsertPoint(*pointIt);
                     (this->*fCalcSlope)(new_line);
                     fActiveLines.push_back(new_line);
                     match = true;
                 }
             }
         }
         return true;
     }


    bool KTSequentialTrackFinder::EmitPreCandidate(LineRef line)
    {
        bool lineIsCandidate = true;

        if ( fApplyTotalPowerCut )
        {
            if ( line.fAmplitudeSum <= fTotalPowerThreshold )
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyAveragePowerCut )
        {
            if ( line.fAmplitudeSum/(line.fEndTimeInRunC-line.fStartTimeInRunC) <= fAveragePowerThreshold )
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyTotalSNRCut )
        {
            if ( line.fSNRSum <= fTotalSNRThreshold)
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyAverageSNRCut )
        {
            if ( line.fSNRSum / ( line.fEndTimeInRunC - line.fStartTimeInRunC ) <= fAverageSNRThreshold )
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyTotalUnitlessResidualCut )
        {
            if ( line.fNUPSum <= fTotalUnitlessResidualThreshold )
            {
                lineIsCandidate = false;
            }
        }
        if ( fApplyAverageUnitlessResidualCut )
        {
            if ( line.fNUPSum / ( line.fEndTimeInRunC - line.fStartTimeInRunC ) <= fAverageUnitlessResidualThreshold )
            {
                lineIsCandidate = false;
            }
        }
        // after all cuts have been applied and point cluster is still a candidate, create SparseWaterfallCandidateData
        if (lineIsCandidate == true)
        {

            /*// Set up new data object
            Nymph::KTDataPtr data( new Nymph::KTData() );
            KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
            newTrack.SetComponent( line.fComponent );
            newTrack.SetTrackID(fNLines);
            ++fNLines;


            newTrack.SetStartTimeInRunC( line.fStartTimeInRunC );
            newTrack.SetEndTimeInRunC( line.fEndTimeInRunC );
            newTrack.SetStartTimeInAcq( line.fStartTimeInAcq);
            newTrack.SetStartFrequency( line.fStartFrequency );
            newTrack.SetEndFrequency( line.fEndFrequency );
            newTrack.SetSlope(line.fSlope);
            newTrack.SetSlopeSigma(line.fSlopeSigma);
            newTrack.SetInterceptSigma(line.fInterceptSigma);
            newTrack.SetStartFrequencySigma(line.fStartFrequencySigma);
            newTrack.SetEndFrequencySigma(line.fEndFrequencySigma);
            newTrack.SetTotalPower(line.fAmplitudeSum);*/

            // Set up new data object
            Nymph::KTDataPtr data( new Nymph::KTData() );
            KTSparseWaterfallCandidateData& newSwfCand = data->Of< KTSparseWaterfallCandidateData >();
            newSwfCand.SetComponent( line.fComponent );
            newSwfCand.SetAcquisitionID( line.fAcquisitionID);
            newSwfCand.SetCandidateID( fNLines );
            newSwfCand.SetMinimumFrequency( line.fStartFrequency );
            newSwfCand.SetMaximumFrequency( line.fEndFrequency );
            newSwfCand.SetTimeInRunC( line.fStartTimeInRunC );
            newSwfCand.SetTimeInAcq( line.fStartTimeInAcq );
            newSwfCand.SetMinimumTimeInRunC( line.fStartTimeInRunC );
            newSwfCand.SetMinimumTimeInAcq( line.fStartTimeInAcq );
            newSwfCand.SetMaximumTimeInRunC( line.fEndTimeInRunC );
            newSwfCand.SetMaximumTimeInAcq( line.fEndTimeInAcq );
            newSwfCand.SetFrequencyWidth( line.fEndFrequency - line.fStartFrequency );
            newSwfCand.SetSlope( line.fSlope );
            ++fNLines;

            // Add line points to swf candidate
            for(std::vector<Point>::iterator pointIt = line.fLinePoints.begin(); pointIt != line.fLinePoints.end(); ++pointIt )
            {
                KTDEBUG( stflog, "Adding points from SeqLine to newSwfCand: "<<pointIt->fTimeInRunC<<" "<<pointIt->fPointFreq<<" "<<pointIt->fAmplitude<<" "<<pointIt->fNeighborhoodAmplitude );
                KTSparseWaterfallCandidateData::Point newSwfPoint(pointIt->fTimeInRunC, pointIt->fPointFreq, pointIt->fAmplitude, pointIt->fTimeInAcq );
                newSwfCand.AddPoint(newSwfPoint);
            }


            // Process & emit new track

            //KTINFO(stflog, "Now processing track candidate");
            //ProcessNewTrack( newTrack );

            KTDEBUG(stflog, "Emitting track signal");
            fCandidates.insert( data );
            fTrackSignal( data );
        }
        else
        {
            KTDEBUG(stflog, "Line did not make it above the cut and was not emitted as track");
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

    void KTSequentialTrackFinder::UpdateLinePoint(Point& point, KTPowerSpectrum& slice)
    {
        double Delta = fConvergeDelta + 1.0;
        unsigned loopCounter = 0;
        int maxIterations = 10;

        double frequency = point.fPointFreq;
        double amplitude = point.fAmplitude;
        unsigned frequencyBin = point.fBinInSlice;
        double oldFrequencyBin;
        


        while(std::abs(Delta) > fConvergeDelta and loopCounter < maxIterations)
        {
            ++loopCounter;

            oldFrequencyBin = frequencyBin;

            if (frequencyBin > fMinBin + fSearchRadius and frequencyBin < fMaxBin - fSearchRadius)
            {
                this-> WeightedAverage(slice, frequencyBin, frequency);
                Delta = std::abs(frequencyBin - oldFrequencyBin);
            }
            else
            {
                KTDEBUG(stflog, "frequency bin was not in allowed range ");
                Delta = 0.0;
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
        point.fPointFreq = frequency;
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
        newFrequency = fBinWidth * ((weightedBin/wSum)+0.5); //((double)newFrequencyBin + 0.5);

        frequency = newFrequency;
        frequencyBin = newFrequencyBin;

    }


    void KTSequentialTrackFinder::AcquisitionIsOver()
    {
        KTINFO(stflog, "Got egg-done signal. Checking remaining line candidates");

        std::vector< LineRef >::iterator lineIt = fActiveLines.begin();
        while( lineIt != fActiveLines.end())
        {
            if (lineIt->fNPoints >= fMinPoints)
            {
                lineIt->LineSNRTrimming(fTrimmingThreshold, fMinPoints);

                if (lineIt->fNPoints >= fMinPoints and lineIt->fSlope > fMinSlope)
                {
                    this->EmitPreCandidate(*lineIt);
                }
            }
            lineIt = fActiveLines.erase(lineIt);
        }
        KTDEBUG(stflog, "Now there should be no lines left over " << fActiveLines.empty());
    }

    /*void KTSequentialTrackFinder::CalculateWeightedSlope(LineRef& Line)
    {
        double SumX = 0.0;
        double SumY = 0.0;
        double SumXY = 0.0;
        double SumXX = 0.0;

        //KTDEBUG(seqlog, "Calculating line slope");
        double weightedSlope = 0.0;
        double weight = 0.0;
        double wSum = 0.0;
        Line.fNPoints = Line.fLinePoints.size();

        if (Line.fNPoints > 1)
        {
            for(std::vector<LinePoint>::iterator pointIt = Line.fLinePoints.begin(); pointIt != Line.fLinePoints.end(); ++pointIt)
            {
                   weight = pointIt->fAmplitude;

                   SumX += Line.fLinePoints.back().fTimeInRunC *weight;
                   SumY += Line.fLinePoints.back().fPointFreq * weight;
                   SumXY += Line.fLinePoints.back().fTimeInRunC * Line.fLinePoints.back().fPointFreq * weight *weight;
                   SumXX += Line.fLinePoints.back().fTimeInRunC * Line.fLinePoints.back().fTimeInRunC * weight * weight;
            }
            //SumX = SumX/Line.fAmplitudeSum;
            //SumY = SumY/Line.fAmplitudeSum;
            //SumXY = SumXY/(Line.fAmplitudeSum*Line.fAmplitudeSum);
            //SumXX = SumXX/(Line.fAmplitudeSum*Line.fAmplitudeSum);
            Line.fSlope = (Line.fNPoints * SumXY - SumX * SumY)/(SumXX * Line.fNPoints - SumX * SumX);
            KTDEBUG( stflog, "Weighted slope method. New slope "<<Line.fSlope);
        }
        if (Line.fNPoints <= 1)
        {
            Line.fSlope = fInitialSlope;
        }
    }*/
    void KTSequentialTrackFinder::CalculateUnweightedSlope(LineRef& Line)
    {

        //KTDEBUG(stflog, "Calculating line slope");
        Line.fNPoints = Line.fLinePoints.size();

        Line.fSumX += Line.fLinePoints.back().fTimeInRunC;
        Line.fSumY += Line.fLinePoints.back().fPointFreq;
        Line.fSumXY += Line.fLinePoints.back().fTimeInRunC * Line.fLinePoints.back().fPointFreq;
        Line.fSumXX += Line.fLinePoints.back().fTimeInRunC * Line.fLinePoints.back().fTimeInRunC;

        if (Line.fNPoints > 1)
        {
            Line.fSlope = (Line.fNPoints * Line.fSumXY - Line.fSumX * Line.fSumY)/(Line.fSumXX * Line.fNPoints - Line.fSumX * Line.fSumX);
        }
        else
        {
            Line.fSlope = fInitialSlope;
        }
        //KTDEBUG( stflog, "Unweighted slope method. New slope "<<Line.fSlope);
    }

    void KTSequentialTrackFinder::CalculateSlopeFirstRef(LineRef& Line)
    {

        //KTDEBUG(seqlog, "Calculating line slope");
        double weightedSlope = 0.0;
        double wSum = 0.0;
        Line.fNPoints = Line.fLinePoints.size();

        if (Line.fNPoints > fNSlopePoints)
        {
            for(std::vector<Point>::iterator pointIt = Line.fLinePoints.end() - fNSlopePoints; pointIt != Line.fLinePoints.end(); ++pointIt)
            {
                if (pointIt->fPointFreq != Line.fStartFrequency)
                {
                    weightedSlope += (pointIt->fPointFreq - Line.fStartFrequency)/(pointIt->fTimeInAcq - Line.fStartTimeInAcq) * pointIt->fNeighborhoodAmplitude;
                    wSum += pointIt->fNeighborhoodAmplitude;
                }
            }
            Line.fSlope = weightedSlope/wSum;
        }
        else if (Line.fNPoints > 1)
        {
            for(std::vector<Point>::iterator pointIt = Line.fLinePoints.begin(); pointIt != Line.fLinePoints.end(); ++pointIt)
            {
                if (pointIt->fPointFreq != Line.fStartFrequency)
                {
                    weightedSlope += (pointIt->fPointFreq - Line.fStartFrequency)/(pointIt->fTimeInAcq - Line.fStartTimeInAcq) * pointIt->fNeighborhoodAmplitude;
                    wSum += pointIt->fNeighborhoodAmplitude;
                }
            }
            Line.fSlope = weightedSlope/wSum;
        }
        else
        {
            Line.fSlope = fInitialSlope;
        }
        //KTDEBUG(stflog, "Ref point slope method. New slope is " << Line.fSlope);
    }

    void KTSequentialTrackFinder::CalculateSlopeLastRef(LineRef& Line)
    {

        //KTDEBUG(seqlog, "Calculating line slope");
        double weightedSlope = 0.0;
        double wSum = 0.0;
        Line.fNPoints = Line.fLinePoints.size();

        if (Line.fNPoints > fNSlopePoints)
        {
            for(std::vector<Point>::iterator pointIt = Line.fLinePoints.end() - fNSlopePoints; pointIt != Line.fLinePoints.end(); ++pointIt)
            {
                if (pointIt->fPointFreq != Line.fEndFrequency)
                {
                    weightedSlope += (Line.fEndFrequency - pointIt->fPointFreq)/(Line.fEndTimeInRunC - pointIt->fTimeInRunC) * pointIt->fNeighborhoodAmplitude;
                    wSum += pointIt->fNeighborhoodAmplitude;
                }
            }
            Line.fSlope = weightedSlope/wSum;
        }
        else if (Line.fNPoints > 1)
        {
            for(std::vector<Point>::iterator pointIt = Line.fLinePoints.begin(); pointIt != Line.fLinePoints.end(); ++pointIt)
            {
                if (pointIt->fPointFreq != Line.fEndFrequency)
                {
                    weightedSlope += (Line.fEndFrequency - pointIt->fPointFreq)/(Line.fEndTimeInRunC - pointIt->fTimeInRunC) * pointIt->fNeighborhoodAmplitude;
                    wSum += pointIt->fNeighborhoodAmplitude;
                }
            }
            Line.fSlope = weightedSlope/wSum;
        }
        else
        {
            Line.fSlope = fInitialSlope;
        }
        //KTDEBUG(stflog, "Ref point slope method. fNSlopePoints: "<<fNSlopePoints<<" . New slope is " << Line.fSlope);
    }
} /* namespace Katydid */
