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
            fMode(eSNR_Power),
            fTrimmingFactor(3.2),
            fLinePowerWidth(4),
            fPointAmplitudeAfterVisit(0),
            fMinFreqBinDistance(10),
            fTimeGapTolerance(0.0005),
            fFrequencyAcceptance(56166.0528183),
            fSNRPowerThreshold(6.),
            fSearchRadius(6),
            fConvergeDelta(1.5),
            fMinPoints(3),
            fMinSlope(0.0),
            fInitialSlope(3.0*pow(10,8)),
            fMinBin(0),
            fMaxBin(1),
            fBinWidth(0.0),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fActiveLines(),
            fNLines(0),
            fApplyPowerCut(false),
            fApplyDensityCut(false),
            fPowerThreshold(0.0),
            fDensityThreshold(0.0),
            fTrackSignal("pre-candidate", this),
            fClusterDoneSignal("clustering-done", this),
            fGainVarSlot("gv", this, &KTSequentialTrackFinder::SetPreCalcGainVar),
            //fPSPreCalcSlot("ps-pre", this, &KTSeqTrack::PointLineAssignment),
            fPSSlot("ps-in", this, &KTSequentialTrackFinder::CollectPointsFromSlice),
            fDiscrimSlot("disc-1d", this, &KTSequentialTrackFinder::CollectDiscrimPointsFromSlice),
            fDoneSlot("done", this, &KTSequentialTrackFinder::AcquisitionIsOver, &fClusterDoneSignal)
    {
    }

    KTSequentialTrackFinder::~KTSequentialTrackFinder()
    {
    }

    bool KTSequentialTrackFinder::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSNRPowerThreshold(node->get_value("snr-threshold-power", GetSNRPowerThreshold()));
        SetTrimmingFactor(node->get_value("trimming-factor", GetTrimmingFactor()));
        SetLinePowerWidth(node->get_value("half-line-width", GetLinePowerWidth()));
        SetTimeGapTolerance(node->get_value("time-gap-tolerance", GetTimeGapTolerance()));
        SetFrequencyAcceptance(node->get_value("frequency-acceptance", GetFrequencyAcceptance()));
        SetInitialSlope(node->get_value("initial-slope", GetInitialSlope()));
        SetMinPoints(node->get_value("min-points", GetMinPoints()));
        SetMinSlope(node->get_value("min-slope", GetMinSlope()));

        SetMinFrequency(node->get_value("min-frequency", GetMinFrequency()));
        SetMaxFrequency(node->get_value("max-frequency", GetMaxFrequency()));


        if (node->has("apply-power-cut"))
        {
            SetApplyPowerCut(node->get_value("apply-power-cut", GetApplyPowerCut()));
            SetPowerThreshold(node->get_value("power-threshold", GetPowerThreshold()));
        }
        if (node->has("apply-power-density-cut"))
        {
            SetApplyDensityCut(node->get_value("apply-power-density-cut", GetApplyDensityCut()));
            SetDensityThreshold(node->get_value("power-density-threshold", GetDensityThreshold()));
        }
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
        	SetMinFreqBinDistance(node->get_value<double>("minimum-line-bin-distance"));
		}
        if (node->has("search-radius"))
        {
        	SetSearchRadius(node->get_value<int>("search-radius"));
        }
        if (node->has("converge-delta"))
        {
            SetConvergeDelta(node->get_value<int>("converge_delta"));
        }



        return true;
    }

    bool KTSequentialTrackFinder::SetPreCalcGainVar(KTGainVariationData& gvData)
    {
        fGVData = gvData;
        return true;
    }

    bool KTSequentialTrackFinder::CollectPointsFromSlice(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum)
    {
        this->CollectPoints(slHeader, spectrum, fGVData);
        return true;
    }

    bool KTSequentialTrackFinder::CollectDiscrimPointsFromSlice(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum, KTDiscriminatedPoints1DData& discrimPoints)
    {
        this->CollectDiscrimPoints(slHeader, spectrum, discrimPoints);
        return true;
    }

    bool KTSequentialTrackFinder::CollectPoints(const KTSliceHeader& slHeader, const KTPowerSpectrumData& spectrum, const KTGainVariationData& gvData)
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
            const KTPowerSpectrum* powerSpectrum=spectrum.GetSpectrum(iComponent);
            KTSpline* spline = fGVData.GetSpline(iComponent);

            unsigned nBins = fMaxBin - fMinBin + 1;
            double freqMin = powerSpectrum->GetBinLowEdge(fMinBin);
            double freqMax = powerSpectrum->GetBinLowEdge(fMaxBin) + powerSpectrum->GetBinWidth();
            KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);


            fBinWidth = powerSpectrum->GetBinWidth();

            // this will be the deepcopy of the power spectrum slice
            std::vector< double > slice(fMaxBin +1);
            
            // filling it with zeros first shouldn't be necessary
            //std::fill(slice.begin(), slice.end(), 0.0);

            double newTimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double newTimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(stflog, "new_TimeInAcq is " << newTimeInAcq);


            // this vector will collect the high power bins in this power slice
            std::vector<Point> points;

            double value;

            // loop over bins, checking against the threshold
            KTDEBUG(stflog, "Collecting high power bins");
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = (*powerSpectrum)(iBin);
                slice.at(iBin) = value;
                //new_TrimmingLimits += value;
                double threshold = fSNRPowerThreshold * (*splineImp)(iBin - fMinBin);

                // currently this is the only implemented mode
                if (fMode == eSNR_Power and value >= threshold)
                {
                    double pointFreq = fBinWidth * ((double)iBin + 0.5);
                    Point newPoint(iBin, pointFreq, newTimeInAcq, newTimeInRunC, value, threshold, acqID, iComponent);
                    points.push_back(newPoint);
                    //KTDEBUG(stflog, "Collected track point candidate");
                }
            }


            // sort points by power
            std::sort(points.begin(), points.end(),std::less<Point>());

            // Loop over the high power points
            this->LoopOverHighPowerPoints(slice, points, iComponent);

        }
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

            fBinWidth = powerSpectrum.GetBinWidth();

            double newTimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double newTimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(stflog, "new_TimeInAcq is " << newTimeInAcq);


            // this vector will collect the discriminated points
            std::vector<Point> points;

            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discrimPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin(); pIt != incomingPts.end(); ++pIt)
            {
                //KTINFO(stflog, "discriminated point: bin = " <<pIt->first<< ", frequency = "<<pIt->second.fAbscissa<< ", amplitude = "<<pIt->second.fOrdinate<<", "<<powerSpectrum(pIt->first) <<", threshold = "<<pIt->second.fThreshold);
                Point newPoint(pIt->first, pIt->second.fAbscissa, newTimeInAcq, newTimeInRunC, pIt->second.fOrdinate, pIt->second.fThreshold, acqID, iComponent);
                points.push_back(newPoint);
            }

            // sort points by power
            std::sort(points.begin(), points.end(),std::less<Point>());

            // Loop over the high power points
            this->LoopOverHighPowerPoints(powerSpectrum, points, iComponent);

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

                 this->SearchTrueLinePoint(*pointIt, slice);
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
                     // Under these conditions a point will be added to a line
                     bool condition1 = pointIt->fTimeInRunC > lineIt->fEndTimeInRunC;
                     bool condition2 = std::abs(pointIt->fPointFreq - (lineIt->fEndFrequency + lineIt->fSlope*(pointIt->fTimeInAcq - lineIt->fEndTimeInAcq))) < fFrequencyAcceptance;

                     // Check whether line should still be active. If not then check whether the line is a valid new track candidate.
                     if (lineIt->fEndTimeInRunC<pointIt->fTimeInRunC-fTimeGapTolerance)
                     {
                         if (lineIt->fNPoints >= fMinPoints)
                         {
                             lineIt->LineTrimming(fTrimmingFactor, fMinPoints);

                             if (lineIt->fNPoints >= fMinPoints and lineIt->fSlope > fMinSlope)
                             {
                                 KTINFO(stflog, "Found track candidate");
                                 this->EmitPreCandidate(*lineIt);
                             }
                         }
                         // in any case, this line should be removed from the vector with active lines
                         lineIt = fActiveLines.erase(lineIt);
                     }
                     // if point matches this line: attach
                     else if (condition1 and condition2)
                     {
                         lineIt->InsertPoint(*pointIt);
                         match = true;
                         ++lineIt;
                     }
                     // if not try next line
                     else
                     {
                         ++lineIt;
                     }
                 }

                 // if point was not picked up
                 if (match == false)
                 {
                     //KTDEBUG(stflog, "Starting new line");

                     LineRef new_line(fInitialSlope);
                     new_line.InsertPoint(*pointIt);
                     fActiveLines.push_back(new_line);
                     match = true;
                 }
             }
         }
         return true;
     }


    bool KTSequentialTrackFinder::LoopOverHighPowerPoints(std::vector<double>& slice, std::vector<Point>& points, unsigned component)
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
            pointIt->fAmplitude = slice[pointIt->fBinInSlice];

    	    if (pointIt->fAmplitude == 0.0)
    	    {
                KTDEBUG(stflog, "Point amplitude is 0, skipping point");
    	        continue;
    	    }

    	    else
    	    {

    	        this->SearchTrueLinePoint(*pointIt, slice);
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
                    // Under these conditions a point will be added to a line
                    bool condition1 = pointIt->fTimeInRunC > lineIt->fEndTimeInRunC;
                    bool condition2 = std::abs(pointIt->fPointFreq - (lineIt->fEndFrequency + lineIt->fSlope*(pointIt->fTimeInAcq - lineIt->fEndTimeInAcq))) < fFrequencyAcceptance;

                    // Check whether line should still be active. If not then check whether the line is a valid new track candidate.
                    if (lineIt->fEndTimeInRunC<pointIt->fTimeInRunC-fTimeGapTolerance)
                    {
                        if (lineIt->fNPoints >= fMinPoints)
                        {
                            lineIt->LineTrimming(fTrimmingFactor, fMinPoints);

                            if (lineIt->fNPoints >= fMinPoints and lineIt->fSlope > fMinSlope)
                            {
                                KTINFO(stflog, "Found track candidate");
                                this->EmitPreCandidate(*lineIt);
                            }
                        }
                        // in any case, this line should be removed from the vector with active lines
                        lineIt = fActiveLines.erase(lineIt);
                    }
                    // if point matches this line: attach
                    else if (condition1 and condition2)
                    {
                        lineIt->InsertPoint(*pointIt);
                        match = true;
                        ++lineIt;
                    }
                    // if not try next line
                    else
                    {
                        ++lineIt;
                    }
                }

                // if point was not picked up
                if (match == false)
                {
                    //KTDEBUG(stflog, "Starting new line");

                    LineRef new_line(fInitialSlope);
                    new_line.InsertPoint(*pointIt);
                    fActiveLines.push_back(new_line);
                    match = true;
                }
            }
    	}
    	return true;
    }

    bool KTSequentialTrackFinder::EmitPreCandidate(LineRef line)
    {
        bool lineIsTrack = true;

        if (fApplyPowerCut)
        {
            if (line.fAmplitudeSum <= fPowerThreshold)
            {
                lineIsTrack = false;
            }
        }
        if (fApplyDensityCut)
        {
            if ((double)line.fNPoints/(line.fEndTimeInRunC-line.fStartTimeInRunC)*1000.0 <= fDensityThreshold)
            {
                lineIsTrack = false;
            }
        }

        if (lineIsTrack == true)
        {

            // Set up new data object
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
            newTrack.SetTotalPower(line.fAmplitudeSum);

            // Process & emit new track

            KTINFO(stflog, "Now processing track candidate");
            ProcessNewTrack( newTrack );

            KTDEBUG(stflog, "Emitting track signal");
            fTrackSignal( data );
        }
        else
        {
            KTDEBUG(stflog, "Line did not make it above the cut and was not emitted as track");
        }
        return true;
    }

    void KTSequentialTrackFinder::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );

        // the slope is already calculated
        //myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );

    }

    void KTSequentialTrackFinder::SearchTrueLinePoint(Point& point, KTPowerSpectrum& slice)
    {
        double Delta = fConvergeDelta + 1.0;
        unsigned loopCounter = 0;
        double dF = fBinWidth;
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
        if (frequencyBin > fMinBin + fLinePowerWidth and frequencyBin < fMaxBin - fLinePowerWidth)
        {
            amplitude = 0;
            for (int iBin = frequencyBin - fLinePowerWidth; iBin <= frequencyBin + fLinePowerWidth; ++iBin)
            {
                amplitude += slice(iBin);
                slice(iBin) = fPointAmplitudeAfterVisit;
            }
        }
        else
        {
            amplitude = slice(frequencyBin);
            slice(frequencyBin) = fPointAmplitudeAfterVisit;
        }

        // Set larger area to zero if possible
        if (frequencyBin > fMinBin + fMinFreqBinDistance and frequencyBin < fMaxBin - fMinFreqBinDistance)
        {
            for (int iBin = frequencyBin - fMinFreqBinDistance; iBin <= frequencyBin + fMinFreqBinDistance; ++iBin)
            {
                slice(iBin)=fPointAmplitudeAfterVisit;
            }
        }
        // if point was to close to edge, try again with smaller range
        else if (frequencyBin > fMinBin + fSearchRadius and frequencyBin < fMaxBin - fSearchRadius)
        {
            for (int iBin = frequencyBin - fSearchRadius; iBin <= frequencyBin + fSearchRadius; ++iBin)
            {
                slice(iBin) = fPointAmplitudeAfterVisit;
            }
        }

        // Correct values currently still stored in Point
        point.fBinInSlice = frequencyBin;
        point.fPointFreq = frequency;
        point.fAmplitude = amplitude;
    }

    void KTSequentialTrackFinder::SearchTrueLinePoint(Point& point, std::vector<double>& slice)
        {
            double Delta = fConvergeDelta + 1.0;
            unsigned loopCounter = 0;
            double dF = fBinWidth;
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
                    KTDEBUG(stflog, "frequency bin was not in correct range ");
                    Delta = 0.0;
                    amplitude = 0.0;
                }
            }

            // Calculate "true" line amplitude in slice by summing neighboring point amplitudes
            // and set slice amplitude to zero
            if (frequencyBin > fMinBin + fLinePowerWidth and frequencyBin < fMaxBin - fLinePowerWidth)
            {
                amplitude = 0;
                for (int iBin = frequencyBin - fLinePowerWidth; iBin <= frequencyBin + fLinePowerWidth; ++iBin)
                {
                    amplitude += slice[iBin];
                    slice.at(iBin)=fPointAmplitudeAfterVisit;
                }
            }
            else
            {
                amplitude = slice[frequencyBin];
                slice.at(frequencyBin)=fPointAmplitudeAfterVisit;
            }
            // Set larger area to zero if possible
            if (frequencyBin > fMinBin + fMinFreqBinDistance and frequencyBin < fMaxBin - fMinFreqBinDistance)
            {
                for (int iBin = frequencyBin - fMinFreqBinDistance; iBin <= frequencyBin + fMinFreqBinDistance; ++iBin)
                {
                    slice.at(iBin)=fPointAmplitudeAfterVisit;
                }
            }
            // if point was to close to edge, try again with smaller range
            else if (frequencyBin > fMinBin + fSearchRadius and frequencyBin < fMaxBin - fSearchRadius)
            {
                for (int iBin = frequencyBin - fSearchRadius; iBin <= frequencyBin + fSearchRadius; ++iBin)
                {
                    slice.at(iBin)=fPointAmplitudeAfterVisit;
                }
            }

            // Correct values currently still stored in Point
            point.fBinInSlice = frequencyBin;
            point.fPointFreq = frequency;
            point.fAmplitude = amplitude;
        }


    inline void KTSequentialTrackFinder::WeightedAverage(const std::vector<double>& slice, unsigned& frequencyBin, double& frequency)
    {
        unsigned newFrequencyBin = 0;
        double newFrequency = 0.0;
        double weightedBin = 0.0;
        double wSum = 0.0;

        for (int iBin = -1*fSearchRadius; iBin <= fSearchRadius; ++iBin)
        {
            weightedBin += double(frequencyBin+iBin)*slice[frequencyBin+iBin];
            wSum +=slice[frequencyBin+iBin];
        }
        newFrequencyBin = unsigned(weightedBin/wSum);
        newFrequency = fBinWidth * ((double)newFrequencyBin + 0.5);

        frequency = newFrequency;
        frequencyBin = newFrequencyBin;
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
        newFrequency = fBinWidth * ((double)newFrequencyBin + 0.5);

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
                lineIt->LineTrimming(fTrimmingFactor, fMinPoints);

                if (lineIt->fNPoints >= fMinPoints and lineIt->fSlope > fMinSlope)
                {
                    this->EmitPreCandidate(*lineIt);
                }
            }
            lineIt = fActiveLines.erase(lineIt);
        }
        KTDEBUG(stflog, "Now there should be no lines left over " << fActiveLines.empty());
    }
} /* namespace Katydid */
