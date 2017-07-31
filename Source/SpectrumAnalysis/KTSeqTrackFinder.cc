/*
 * KTSeqTrackFinder.cc
 *
 *  Created on: Sep 15, 2016
 *      Author: Christine
 */

#include "KTSeqTrackFinder.hh"

#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTScoredSpectrum.hh"
#include "KTScoredSpectrumData.hh"
//#include "KTSliceHeader.hh"


#include "KTKDTreeData.hh"
#include "KTLogger.hh"

//#include "KTParam.hh"

#include <numeric>
#include <cmath>;

using std::list;
using std::set;
using std::vector;


namespace Katydid
{
	KTLOGGER(stflog, "DansTrackFinder");


    KT_REGISTER_PROCESSOR(KTSeqTrackFinder, "dans-track-finding-algorithm");

    KTSeqTrackFinder::KTSeqTrackFinder(const std::string& name) :
            KTProcessor(name),
			fMode('SNR'),
			fTrimmingFactor(8.8),
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
			fMinFrequency(0.),
			fMaxFrequency(1.),
			fCalculateMinBin(true),
			fCalculateMaxBin(true),
			fActiveLines(),
			fNLines(0),
			//fnew_Lines(),
			fTrackSignal("pre-candidate", this),
			//fDoneSignal("seq-track-finding-done", this),
			fSeqTrackSlot("ps_in", this, &KTSeqTrackFinder::PointLineAssignment)
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


    bool KTSeqTrackFinder::PointLineAssignment(KTSliceHeader& slHeader, const KTPowerSpectrumData& spectrum)
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


            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                uint64_t AcqID = slHeader.GetAcquisitionID(iComponent);
                std::vector<Point> Points;
                std::vector<Point> sorted_Points;
                KTPowerSpectrum slice=spectrum.GetSpectrum(iComponent);
                //list< LineRef > activeLines;

                double BinWidth = slice.GetBinWidth();
                double new_TimeInAcq = slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength();
                double new_TimeInRunC = slHeader.GetTimeInRun() + 0.5 * slHeader.GetSliceLength();

                if (slice == NULL)
                {
                    KTERROR(stflog, "Frequency spectrum pointer (component " << slice << ") is NULL!");
                    return false;
                }

                unsigned nBins = fMaxBin - fMinBin + 1;

                double new_TrimmingLimits;
                double value;


                // loop over bins, checking against the threshold
                for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
                {
                    new_TrimmingLimits += slice(iBin);
                    value = (slice)(iBin);

                    if (fMode == eSNR_Power and value >= fSNRPowerThreshold)
                    {
                        double PointFreq = BinWidth * ((double)iBin + 0.5);
                        Point new_Point(iBin, PointFreq, new_TimeInAcq, new_TimeInRunC, value, AcqID, iComponent);
                        Points.push_back(new_Point);
                    }
                }

                new_TrimmingLimits = new_TrimmingLimits/nBins;

                // sort points by power
                //sorted_Points = std::sort(Points.begin(), Points.end(),std::greater<KTSeqLine::Point>());

                // Loop over the high power points
                this->LoopOverHighPowerPoints(slice, Points, new_TrimmingLimits, iComponent);



                /*list< LineRef >::iterator LineIt = fActiveLines.begin();
                while (LineIt != fActiveLines.end())
                {
                    if (LineIt->LineTrimming)
                    {
                    fLines[iComponent].insert(*LineIt);
                    LineIt = fActiveLines.erase(LineIt);
                    }
                }
                this->TurnLinesintoTracks(fLines);*/
            }
            return true;
        }


    bool KTSeqTrackFinder::LoopOverHighPowerPoints(KTPowerSpectrum& slice, std::vector<Point>& Points, double& new_TrimmingLimits, unsigned component)
     {
    	double oldFreq = 0.0, Freq = 0.0, newFreq = 0.0, Amplitude=0.0;
    	unsigned PointBin = 0;
    	double BinWidth = slice.GetBinWidth();
    	bool match;

    	//loop in reverse order (by power)
    	for(std::vector<Point>::iterator PointIt = Points.rbegin(); PointIt != Points.rend(); ++PointIt)
    	{
    	    newFreq = PointIt->fPointFreq;
    	    PointBin = PointIt->fBinInSlice;
    	    Amplitude = PointIt->fAmplitude;

    	    if (Amplitude == 0.0)
    	    {
    	        continue;
    	    }
    	    else if (std::abs(newFreq - oldFreq) < fMinFreqBinDistance*BinWidth)
    	    {
    	        continue;
    	    }
    	    else if (std::abs(newFreq - oldFreq) >= fMinFreqBinDistance*BinWidth)
    	    {
    	        oldFreq = Freq;
    	        Freq = newFreq;
    	        match = false;

    	        this->SearchTrueLinePoint(*PointIt, slice);
    	        Freq = PointIt->fPointFreq;

    	    }
    	    else
            {
                KTERROR(stflog, "Unexpected error while looping over high snr points");
                throw;
            }

            if (Freq == 0.0 or PointIt->fAmplitude==0.0)
            {
                continue;
            }
            else
            {
                // loop over active lines, sorted by start time

                std::vector< LineRef >::iterator LineIt = fActiveLines.begin();
                while( LineIt != fActiveLines.end())
         		{

                    if (LineIt->fEndTimeInRunC>Points[PointBin].fTimeInRunC+fTimeGapTolerance)
                    {
                        LineIt->LineTrimming(fTrimmingFactor, fMinPoints);

                        if (LineIt->fNPoints > fMinPoints and LineIt->fSlope > fMinSlope)
                        {
                            //fLines.push_back(*LineIt);
                            this->EmitPreCandidate(*LineIt, component);
                        }
                        LineIt = fActiveLines.erase(LineIt);
                    }
                    else if (std::abs(PointIt->fPointFreq - (LineIt->fEndFrequency + LineIt->fSlope*(PointIt->fTimeInRunC - LineIt->fEndTimeInRunC))) < fFrequencyAcceptance)
                    {
                        LineIt->InsertPoint(*PointIt, new_TrimmingLimits);
                        match = true;
                        LineIt++;
                    }

                    else
                    {
                        LineRef new_Line;
                        new_Line.InsertPoint(*PointIt, new_TrimmingLimits);
                        fActiveLines.push_back(new_Line);
                        match = true;
                        LineIt++;
                    }
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
        newTrack.SetEndTimeInRunC( Line.fStartTimeInRunC );
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

    void KTSeqTrackFinder::SearchTrueLinePoint(Point& Point, KTPowerSpectrum& slice)
    {
        double Delta = fConvergeDelta + 1.0;
        unsigned loop_counter = 0;
        double dF = slice.GetBinWidth();
        int max_iterations = 10;

        double frequency = Point.fPointFreq;
        double amplitude = Point.fAmplitude;
        unsigned frequencybin = Point.fBinInSlice;
        double old_frequency;


        while(std::abs(Delta) > fConvergeDelta and loop_counter < max_iterations)
        {
            loop_counter++;

            old_frequency = frequency;

            if (frequencybin > fMinBin + fSearchRadius and frequencybin < fMaxBin - fSearchRadius)
            {
                this-> WeightedAverage(slice, frequencybin, frequency);
            }
            else
            {
                Delta = 0.0;
                amplitude = 0.0;
            }

            Delta = std::abs(frequency - old_frequency);
        }

        // Calculate "true" line amplitude in slice by taking mean of neighboring points
        // and set slice amplitude to zero
        if (frequencybin > fMinBin + fLinePowerWidth and frequencybin < fMaxBin - fLinePowerWidth)
        {
            amplitude = 0;
            for (int iBin = frequencybin - fLinePowerWidth; iBin < frequencybin + fLinePowerWidth + 1; iBin++)
            {
                amplitude += slice(iBin);
                slice(iBin)=0.0;
            }
        }
        // Set larger area to zero if possible
        if (frequencybin > fMinBin + fSearchRadius and frequencybin < fMaxBin - fSearchRadius)
                {
                    for (int iBin = frequencybin - fSearchRadius; iBin < frequencybin + fSearchRadius + 1; iBin++)
                    {
                        slice(iBin)=0.0;
                    }
                }
        // Correct values currently still stored in Point
        Point.fBinInSlice = frequencybin;
        Point.fPointFreq = frequency;
        Point.fAmplitude = amplitude;

    }



    inline void KTSeqTrackFinder::WeightedAverage(const KTPowerSpectrum& slice, unsigned& FrequencyBin, double& Frequency)
    {
        double BinWidth = slice.GetBinWidth();

        unsigned new_FrequencyBin = 0;
        double new_Frequency = 0.0;
        double weightedBin = 0.0;
        double wSum = 0.0;

        for (int iBin = -fSearchRadius; iBin <= fSearchRadius; iBin++)
        {
            weightedBin += double(FrequencyBin+iBin)*slice(FrequencyBin+iBin);
            wSum +=slice(FrequencyBin+iBin);
        }
        new_FrequencyBin = unsigned(weightedBin/wSum);
        new_Frequency = BinWidth * ((double)new_FrequencyBin + 0.5);

        Frequency = new_Frequency;
        FrequencyBin = new_FrequencyBin;
    }








} /* namespace Katydid */
