/*
 * KTSeqTrackFinder.cc
 *
 *  Created on: Sep 15, 2016
 *      Author: Christine
 */

#include "KTSeqTrackFinder.hh"
#include "KTLogger.hh"


#include <numeric>
#include <cmath>

using std::list;
using std::set;
using std::vector;


namespace Katydid
{
    KTLOGGER(stflog, "DansTrackFinder");


    KT_REGISTER_PROCESSOR(KTSeqTrackFinder, "seq-clustering");

    KTSeqTrackFinder::KTSeqTrackFinder(const std::string& name) :
            KTProcessor(name),
            fMode(eSNR_Power),
            fTrimmingFactor(19.8),
            fLinePowerWidth(4),
            fPointAmplitudeAfterVisit(0),
            fMinFreqBinDistance(10),
            fTimeGapTolerance(0.5*pow(10,-3)),
            fFrequencyAcceptance(56166.0528183),
            fSearchRadius(6),
            fConvergeDelta(1.5),
            fSNRPowerThreshold(6.),
            fMinPoints(2),
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
            fGainVarSlot("gv", this, &KTSeqTrackFinder::SetPreCalcGainVar),
            //fPSPreCalcSlot("ps-pre", this, &KTSeqTrack::PointLineAssignment),
            fPSSlot("ps-in", this, &KTSeqTrackFinder::RunSequentialTrackFinding),
            fDoneSlot("done", this, &KTSeqTrackFinder::AcquisitionIsOver, &fClusterDoneSignal)
    {
    }

    KTSeqTrackFinder::~KTSeqTrackFinder()
    {
    }

    bool KTSeqTrackFinder::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("snr-threshold-power"))
        {
            SetSNRPowerThreshold(node->get_value< double >("snr-threshold-power"));
        }

        if (node->has("min-frequency"))
        {
        	SetMinFrequency(node->get_value< double >("min-frequency"));
        }
        if (node->has("max-frequency"))
        {
        	SetMaxFrequency(node->get_value< double >("max-frequency"));
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
        if (node->has("trimming-factor"))
        {
            SetTrimmingFactor(node->get_value<double>("trimming-factor"));
        }
        if (node->has("half-line-width"))
        {
            SetLinePowerWidth(node->get_value<unsigned>("half-line-width"));
        }
        if (node->has("time-gap-tolerance"))
        {
            SetTimeGapTolerance(node->get_value<double>("time-gap-tolerance"));
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
        if (node->has("frequency-acceptance"))
        {
            SetFrequencyAcceptance(node->get_value<double>("frequency-acceptance"));
        }
        if (node->has("initial-slope"))
        {
            SetInitialSlope(node->get_value<double>("initial-slope"));
        }
        if (node->has("min-points"))
        {
            SetMinPoints(node->get_value<int>("min-points"));
        }
        if (node->has("min-slope"))
        {
            SetMinSlope(node->get_value<int>("min-slope"));
        }
        if (node->has("apply-power-cut"))
        {
            SetApplyPowerCut(node->get_value<bool>("apply-power-cut"));
        }
        if (node->has("apply-point-density-cut"))
        {
            SetApplyDensityCut(node->get_value<bool>("apply-point-density-cut"));
        }
        if (node->has("power-threshold"))
        {
            SetPowerThreshold(node->get_value<double>("power-threshold"));
        }
        if (node->has("point-density-threshold"))
        {
            SetDensityThreshold(node->get_value<double>("point-density-threshold"));
        }


        return true;
    }

    bool KTSeqTrackFinder::SetPreCalcGainVar(KTGainVariationData& gvData)
    {
        fGVData = gvData;
        return true;
    }

    bool KTSeqTrackFinder::RunSequentialTrackFinding(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum)
    {
        this->PointLineAssignment(slHeader, spectrum, fGVData);
        return true;
    }

    bool KTSeqTrackFinder::PointLineAssignment(KTSliceHeader& slHeader, KTPowerSpectrumData& spectrum, KTGainVariationData& gvData)
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
            uint64_t AcqID = slHeader.GetAcquisitionID(iComponent);
            const KTPowerSpectrum* power_spectrum=spectrum.GetSpectrum(iComponent);
            KTSpline* spline = fGVData.GetSpline(iComponent);

            unsigned nBins = fMaxBin - fMinBin + 1;
            double freqMin = power_spectrum->GetBinLowEdge(fMinBin);
            double freqMax = power_spectrum->GetBinLowEdge(fMaxBin) + power_spectrum->GetBinWidth();
            KTSpline::Implementation* splineImp = spline->Implement(nBins, freqMin, freqMax);


            fBinWidth = power_spectrum->GetBinWidth();

            // I need a deepcopy of the amplitudes of the power spectrum
            // As I am looping over each bin anyway I will fill the vector "slice" at the same time
            std::vector< double > slice(fMaxBin +1);
            
            // filling it with zeros first shouldn't be necessary
            //std::fill(slice.begin(), slice.end(), 0.0);

            double new_TimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double new_TimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(stflog, "new_TimeInAcq is " << new_TimeInAcq);


            // this vector will collect the high power bins in this power slice
            std::vector<Point> Points;

            // the trimming limits are the average of the entire slice power
            // trimming_factor x trimming_limits are the threshold for the line edges
            // this is part of dans algorithm but seems to only make sense with a flat background
            // ->replaced by point threshold
            // double new_TrimmingLimits;
            double value;


            // loop over bins, checking against the threshold
            KTDEBUG(stflog, "Collecting high power bins");
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = (*power_spectrum)(iBin);
                slice.at(iBin) = value;
                //new_TrimmingLimits += value;
                double threshold = fSNRPowerThreshold * (*splineImp)(iBin - fMinBin);

                // currently this is the only implemented mode
                if (fMode == eSNR_Power and value >= threshold)
                {
                    double PointFreq = fBinWidth * ((double)iBin + 0.5);
                    Point new_Point(iBin, PointFreq, new_TimeInAcq, new_TimeInRunC, value, threshold, AcqID, iComponent);
                    Points.push_back(new_Point);
                    //KTDEBUG(stflog, "Collected track point candidate");
                }
            }

            //new_TrimmingLimits = new_TrimmingLimits/nBins;

            // I want to iterate over the collected points in order of descending power
            // sort points by power
            std::sort(Points.begin(), Points.end(),std::less<Point>());

            // Loop over the high power points
            this->LoopOverHighPowerPoints(slice, Points, iComponent);

        }
        return true;
    }


    bool KTSeqTrackFinder::LoopOverHighPowerPoints(std::vector<double>& slice, std::vector<Point>& Points, unsigned component)
     {
        KTDEBUG(stflog, "Time and Frequency tolerances are "<<fTimeGapTolerance<<" "<<fFrequencyAcceptance);

    	double oldFreq = 0.0, Freq = 0.0, newFreq = 0.0, Amplitude=0.0;
    	bool match;

    	//loop in reverse order (by power)
    	for(std::vector<Point>::reverse_iterator PointIt = Points.rbegin(); PointIt != Points.rend(); ++PointIt)
    	{
    	    newFreq = PointIt->fPointFreq;

    	    // The amplitude of the bin the in the slice at the position of the point in the power spectrum gets set to zero after a visit (in SearchTrueLinePoint)
            // To prevent that in the next iteration the point gets re-found and added to another line the amplitude of the point is reassigned here
            PointIt->fAmplitude = slice[PointIt->fBinInSlice];

    	    if (PointIt->fAmplitude == 0.0)
    	    {
                KTDEBUG(stflog, "Point amplitude is 0, skipping point");
    	        continue;
    	    }

    	    else
    	    {
                oldFreq = Freq;
    	        Freq = newFreq;

    	        this->SearchTrueLinePoint(*PointIt, slice);
    	        Freq = PointIt->fPointFreq;
    	    }


            if (Freq == 0.0 or PointIt->fAmplitude==0.0)
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

                std::vector< LineRef >::iterator LineIt = fActiveLines.begin();
                while( LineIt != fActiveLines.end())
                {
                    // Under these conditions a point will be added to a line
                    bool condition1 = PointIt->fTimeInRunC > LineIt->fEndTimeInRunC;
                    bool condition2 = std::abs(PointIt->fPointFreq - (LineIt->fEndFrequency + LineIt->fSlope*(PointIt->fTimeInAcq - LineIt->fEndTimeInAcq))) < fFrequencyAcceptance;

                    // Check whether line should still be active. If not then check whether the line is a valid new track candidate.
                    if (LineIt->fEndTimeInRunC<PointIt->fTimeInRunC-fTimeGapTolerance)
                    {
                        if (LineIt->fNPoints > fMinPoints)
                        {
                            LineIt->LineTrimming(fTrimmingFactor, fMinPoints);

                            if (LineIt->fNPoints > fMinPoints and LineIt->fSlope > fMinSlope)
                            {
                                KTINFO(stflog, "Found track candidate");
                                this->EmitPreCandidate(*LineIt);
                            }
                        }
                        // in any case, this line should be removed from the vector with active lines
                        LineIt = fActiveLines.erase(LineIt);
                    }
                    // if point matches this line: attach
                    else if (condition1 and condition2)
                    {
                        LineIt->InsertPoint(*PointIt);
                        match = true;
                        LineIt++;
                    }
                    // if not try next line
                    else
                    {
                        LineIt++;
                    }
                }

                // if point was not picked up
                if (match == false)
                {
                    //KTDEBUG(stflog, "Starting new line");

                    LineRef new_Line(fInitialSlope);
                    new_Line.InsertPoint(*PointIt);
                    fActiveLines.push_back(new_Line);
                    match = true;
                }
            }
         }
         return true;
     }

    bool KTSeqTrackFinder::EmitPreCandidate(LineRef Line)
    {
        bool LineIsTrack = true;

        if (fApplyPowerCut)
        {
            if (Line.fAmplitudeSum <= fPowerThreshold)
            {
                LineIsTrack = false;
            }
        }
        if (fApplyDensityCut)
        {
            if ((double)Line.fNPoints/(Line.fEndTimeInRunC-Line.fStartTimeInRunC)*1000.0 <= fDensityThreshold)
            {
                LineIsTrack = false;
            }
        }

        if (LineIsTrack == true)
        {
            //KTDEBUG(stflog, "Line slope is "<<Line.fSlope);
            //Line.FinishTrack();
            //KTDEBUG(stflog, "Track slope is "<<Line.fSlope);


            // Set up new data object
            Nymph::KTDataPtr data( new Nymph::KTData() );
            KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
            newTrack.SetComponent( Line.fComponent );
            newTrack.SetTrackID(fNLines);
            fNLines++;

            newTrack.SetStartTimeInRunC( Line.fStartTimeInRunC );
            newTrack.SetEndTimeInRunC( Line.fEndTimeInRunC );
            newTrack.SetStartTimeInAcq( Line.fStartTimeInAcq);
            newTrack.SetStartFrequency( Line.fStartFrequency );
            newTrack.SetEndFrequency( Line.fEndFrequency );
            newTrack.SetSlope(Line.fSlope);
            newTrack.SetSlopeSigma(Line.fSlopeSigma);
            newTrack.SetInterceptSigma(Line.fInterceptSigma);
            newTrack.SetStartFrequencySigma(Line.fStartFrequencySigma);
            newTrack.SetEndFrequencySigma(Line.fEndFrequencySigma);
            newTrack.SetTotalPower(Line.fAmplitudeSum);

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

    void KTSeqTrackFinder::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );

        // the slope is already calculated
        //myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );

    }

    void KTSeqTrackFinder::SearchTrueLinePoint(Point& Point, std::vector<double>& slice)
    {
        double Delta = fConvergeDelta + 1.0;
        unsigned loop_counter = 0;
        double dF = fBinWidth;
        int max_iterations = 10;

        double frequency = Point.fPointFreq;
        double amplitude = Point.fAmplitude;
        unsigned frequencybin = Point.fBinInSlice;
        double old_frequencybin;
        


        while(std::abs(Delta) > fConvergeDelta and loop_counter < max_iterations)
        {
            loop_counter++;

            old_frequencybin = frequencybin;

            if (frequencybin > fMinBin + fSearchRadius and frequencybin < fMaxBin - fSearchRadius)
            {
                this-> WeightedAverage(slice, frequencybin, frequency);
                Delta = std::abs(frequencybin - old_frequencybin);
            }
            else
            {
                KTDEBUG(stflog, "frequencybin was not in correct range ");
                Delta = 0.0;
                amplitude = 0.0;
            }
        }

        // Calculate "true" line amplitude in slice by summing neighboring point amplitudes
        // and set slice amplitude to zero
        if (frequencybin > fMinBin + fLinePowerWidth and frequencybin < fMaxBin - fLinePowerWidth)
        {
            amplitude = 0;
            for (int iBin = frequencybin - fLinePowerWidth; iBin <= frequencybin + fLinePowerWidth; iBin++)
            {
                amplitude += slice[iBin];
                slice.at(iBin)=fPointAmplitudeAfterVisit;
            }
        }
        else
        {
            amplitude = slice[frequencybin];
            slice.at(frequencybin)=fPointAmplitudeAfterVisit;
        }
        // Set larger area to zero if possible
        if (frequencybin > fMinBin + fMinFreqBinDistance and frequencybin < fMaxBin - fMinFreqBinDistance)
        {
            for (int iBin = frequencybin - fMinFreqBinDistance; iBin <= frequencybin + fMinFreqBinDistance; iBin++)
            {
                slice.at(iBin)=fPointAmplitudeAfterVisit;
            }
        }
        // if point was to close to edge, try again with smaller range
        else if (frequencybin > fMinBin + fSearchRadius and frequencybin < fMaxBin - fSearchRadius)
        {
            for (int iBin = frequencybin - fSearchRadius; iBin <= frequencybin + fSearchRadius; iBin++)
            {
                slice.at(iBin)=fPointAmplitudeAfterVisit;
            }
        }

        // Correct values currently still stored in Point
        Point.fBinInSlice = frequencybin;
        Point.fPointFreq = frequency;
        Point.fAmplitude = amplitude;
    }



    inline void KTSeqTrackFinder::WeightedAverage(const std::vector<double>& slice, unsigned& FrequencyBin, double& Frequency)
    {
        unsigned new_FrequencyBin = 0;
        double new_Frequency = 0.0;
        double weightedBin = 0.0;
        double wSum = 0.0;

        for (int iBin = -1*fSearchRadius; iBin <= fSearchRadius; iBin++)
        {
            weightedBin += double(FrequencyBin+iBin)*slice[FrequencyBin+iBin];
            wSum +=slice[FrequencyBin+iBin];
        }
        new_FrequencyBin = unsigned(weightedBin/wSum);
        new_Frequency = fBinWidth * ((double)new_FrequencyBin + 0.5);

        Frequency = new_Frequency;
        FrequencyBin = new_FrequencyBin;
    }


    void KTSeqTrackFinder::AcquisitionIsOver()
    {
        KTINFO(stflog, "Got egg-done signal. Checking remaining line candidates");

        std::vector< LineRef >::iterator LineIt = fActiveLines.begin();
        while( LineIt != fActiveLines.end())
        {
            if (LineIt->fNPoints >= fMinPoints)
            {
                LineIt->LineTrimming(fTrimmingFactor, fMinPoints);

                if (LineIt->fNPoints > fMinPoints and LineIt->fSlope > fMinSlope)
                {
                    this->EmitPreCandidate(*LineIt);
                }
            }
            LineIt = fActiveLines.erase(LineIt);
        }
        KTDEBUG(stflog, "Now there should be no lines left over " << fActiveLines.empty());
    }
} /* namespace Katydid */
