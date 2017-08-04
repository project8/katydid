/*
 * KTSeqTrackFinder.cc
 *
 *  Created on: Sep 15, 2016
 *      Author: Christine
 */

#include "KTSeqTrackFinder.hh"

#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
//#include "KTSliceHeader.hh"


#include "KTKDTreeData.hh"
#include "KTLogger.hh"

//#include "KTParam.hh"

#include <numeric>
#include <cmath>

using std::list;
using std::set;
using std::vector;


namespace Katydid
{
    KTLOGGER(stflog, "DansTrackFinder");


    KT_REGISTER_PROCESSOR(KTSeqTrackFinder, "dans-track-finding-algorithm");

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
            fMinBin(0),
            fMaxBin(1),
            fBinWidth(0.0),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fActiveLines(),
            fNLines(0),
            //fnew_Lines(),
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
        if (node->has("line-width"))
        {
            SetLinePowerWidth(node->get_value<unsigned>("line-width"));
        }
        if (node->has("time-gap-tolerance"))
        {
            SetTimeGapTolerance(node->get_value<double>("time-gap-tolerance"));
        }
        if (node->has("minimum-line-frequency-distance"))
		{
        	SetMinFreqBinDistance(node->get_value<double>("minimum-line-frequency-distance"));
		}
        if (node->has("search-radius"))
        {
        	SetSearchRadius(node->get_value<int>("searched-bin-radius"));
        }
        if (node->has("converge-delta"))
        {
            SetConvergeDelta(node->get_value<int>("converge_delta"));
        }
        if (node->has("frequency-acceptance"))
        {
            SetFrequencyAcceptance(node->get_value<int>("frequency-acceptance"));
        }
        //if (node->has("threshold"))
        //{
        //    SetSNRThreshold(node->get_value<int>("threshold"));
        //}
        if (node->has("min-points"))
        {
            SetMinPoints(node->get_value<int>("min-points"));
        }
        if (node->has("min-slope"))
        {
            SetMinSlope(node->get_value<int>("min-slope"));
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

        KTDEBUG(stflog, "nComponents is " << nComponents);
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
            KTDEBUG(stflog, "fBinWidth is " << fBinWidth);
            std::vector< double > slice(fMaxBin+1);
            std::fill(slice.begin(), slice.end(), 0.0);
            
            double new_TimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
            double new_TimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();
            KTDEBUG(stflog, "new_TimeInAcq is " << new_TimeInAcq);



            std::vector<Point> Points;
            //std::vector<Point> sorted_Points;
            double new_TrimmingLimits;
            double value;


            // loop over bins, checking against the threshold
            for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            {
                value = (*power_spectrum)(iBin);
                slice.at(iBin) = value;
                new_TrimmingLimits += value;
                double threshold = fSNRPowerThreshold * (*splineImp)(iBin - fMinBin);

                if (fMode == eSNR_Power and value >= threshold)
                {
                    double PointFreq = fBinWidth * ((double)iBin + 0.5);
                    Point new_Point(iBin, PointFreq, new_TimeInAcq, new_TimeInRunC, value, AcqID, iComponent);
                    Points.push_back(new_Point);
                    KTDEBUG(stflog, "Collected track point candidate");
                }
            }

            new_TrimmingLimits = new_TrimmingLimits/nBins;

            // sort points by power
            std::sort(Points.begin(), Points.end(),std::less<Point>());

            // Loop over the high power points
            this->LoopOverHighPowerPoints(slice, Points, new_TrimmingLimits, iComponent);

        }
        return true;
    }


    bool KTSeqTrackFinder::LoopOverHighPowerPoints(std::vector<double>& slice, std::vector<Point>& Points, double& new_TrimmingLimits, unsigned component)
     {
    	double oldFreq = 0.0, Freq = 0.0, newFreq = 0.0, Amplitude=0.0;
    	//unsigned PointBin = 0;
    	bool match;

    	//loop in reverse order (by power)
    	for(std::vector<Point>::reverse_iterator PointIt = Points.rbegin(); PointIt != Points.rend(); ++PointIt)
    	{
            KTDEBUG(stflog, "Time and Frequency tolerances are "<<fTimeGapTolerance<<" "<<fFrequencyAcceptance)
    	    newFreq = PointIt->fPointFreq;
    	    //PointBin = PointIt->fBinInSlice;
    	    Amplitude = PointIt->fAmplitude;
            KTDEBUG(stflog, "point amplitude is "<<Amplitude);


    	    if (Amplitude == 0.0)
    	    {
    	        continue;
    	    }
    	    else if (std::abs(newFreq - oldFreq) < fMinFreqBinDistance*fBinWidth)
    	    {
                KTDEBUG(stflog, "frequency distance not big enough, skipping point");
    	        continue;
    	    }
    	    else if (std::abs(newFreq - oldFreq) >= fMinFreqBinDistance*fBinWidth)
    	    {
    	        oldFreq = Freq;
    	        Freq = newFreq;

    	        this->SearchTrueLinePoint(*PointIt, slice);
    	        Freq = PointIt->fPointFreq;

    	    }
    	    else
            {
                KTERROR(stflog, "Unexpected error while looping over high snr points");
                return false;
            }

            if (Freq == 0.0 or PointIt->fAmplitude==0.0)
            {
                KTDEBUG(stflog, "Point discarded after area search. Probably to close to min or max frequency");
                continue;
            }
            else
            {
                match = false;
                // loop over active lines, sorted by start time


                KTDEBUG(stflog, "Currently there are N Lines "<<fActiveLines.size());
                std::vector< LineRef >::iterator LineIt = fActiveLines.begin();
                while( LineIt != fActiveLines.end())
         	{
                    // Under these conditions a point will be added to a line
                    bool condition1 = PointIt->fTimeInRunC > LineIt->fEndTimeInRunC;
                    bool condition2 = std::abs(PointIt->fPointFreq - (LineIt->fEndFrequency + LineIt->fSlope*(PointIt->fTimeInAcq - LineIt->fEndTimeInAcq))) < fFrequencyAcceptance;

                    // Check whether line should still be active. If not then check whether the line is a valid new track candidate.
                    if (LineIt->fEndTimeInRunC<PointIt->fTimeInRunC-fTimeGapTolerance)
                    {
                        KTDEBUG(stflog, "Line with N points should be finished "<< LineIt->fNPoints);
                        if (LineIt->fNPoints >= fMinPoints)
                        {
                            LineIt->LineTrimming(fTrimmingFactor, fMinPoints);

                            if (LineIt->fNPoints > fMinPoints and LineIt->fSlope > fMinSlope)
                            {
                                KTDEBUG(stflog, "going to emit line");

                                //fLines.push_back(*LineIt);
                                this->EmitPreCandidate(*LineIt, component);
                            }
                        }
                        KTDEBUG(stflog, "Erasing this line");
                        LineIt = fActiveLines.erase(LineIt);
                    }
                    else if (condition1 and condition2)
                    {
                        KTDEBUG(stflog, "Going to add point to line");

                        LineIt->InsertPoint(*PointIt, new_TrimmingLimits);
                        match = true;
                        LineIt++;
                    }

                    else
                    {
                        LineIt++;
                    }
                }

                // if point was not picked up
                if (match == false)
                {
                    KTDEBUG(stflog, "creating new line");

                    LineRef new_Line;
                    new_Line.InsertPoint(*PointIt, new_TrimmingLimits);
                    fActiveLines.push_back(new_Line);
                    match = true;
                }
            }
         }
         return true;
     }

    bool KTSeqTrackFinder::EmitPreCandidate(LineRef Line, unsigned component)
    {
        // Set up new data object
        Nymph::KTDataPtr data( new Nymph::KTData() );
        KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
        newTrack.SetComponent( component );
        newTrack.SetTrackID(fNLines);
        fNLines++;

        newTrack.SetStartTimeInRunC( Line.fStartTimeInRunC );
        newTrack.SetEndTimeInRunC( Line.fEndTimeInRunC );
        newTrack.SetStartFrequency( Line.fStartFrequency );
        newTrack.SetEndFrequency( Line.fEndFrequency );
        newTrack.SetSlope(Line.fSlope);


        // Process & emit new track

        KTINFO(stflog, "Now processing PreCandidates");
        ProcessNewTrack( newTrack );

        KTDEBUG(stflog, "Emitting track signal");
        fTrackSignal( data );

        return true;
    }

    void KTSeqTrackFinder::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );
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
        
        KTDEBUG(stflog, "before search, frequency bin, frequency and amplitude are "<<frequencybin<< " "<<frequency<<" "<<amplitude);


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

        // Calculate "true" line amplitude in slice by taking mean of neighboring points
        // and set slice amplitude to zero
        if (frequencybin > fMinBin + fLinePowerWidth and frequencybin < fMaxBin - fLinePowerWidth)
        {
            amplitude = 0;
            for (int iBin = frequencybin - fLinePowerWidth; iBin < frequencybin + fLinePowerWidth + 1; iBin++)
            {
                amplitude += slice[iBin];
                slice.at(iBin)=0.0;
            }
        }
        // Set larger area to zero if possible
        if (frequencybin > fMinBin + fSearchRadius and frequencybin < fMaxBin - fSearchRadius)
                {
                    for (int iBin = frequencybin - fSearchRadius; iBin < frequencybin + fSearchRadius + 1; iBin++)
                    {
                        slice.at(iBin)=0.0;
                    }
                }
        // Correct values currently still stored in Point
        Point.fBinInSlice = frequencybin;
        Point.fPointFreq = frequency;
        Point.fAmplitude = amplitude;
        KTDEBUG(stflog, "after weighted average, frequency bin, frequency and amplitude are "<<Point.fBinInSlice<< " "<<Point.fPointFreq<<" "<<Point.fAmplitude);
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
        KTDEBUG(stflog, "Got egg-done signal. Checking remaining line candidates");
        unsigned component = 0;
        // loop over active lines, sorted by start time

        std::vector< LineRef >::iterator LineIt = fActiveLines.begin();
        while( LineIt != fActiveLines.end())
        {
            if (LineIt->fNPoints >= fMinPoints)
            {
                LineIt->LineTrimming(fTrimmingFactor, fMinPoints);

                if (LineIt->fNPoints > fMinPoints and LineIt->fSlope > fMinSlope)
                {
                    this->EmitPreCandidate(*LineIt, component);
                }
            }
            LineIt = fActiveLines.erase(LineIt);
        }
        KTDEBUG(stflog, "Now there should be no lines left over " << fActiveLines.empty());
    }








} /* namespace Katydid */
