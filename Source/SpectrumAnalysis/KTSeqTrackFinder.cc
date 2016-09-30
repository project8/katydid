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


#include "KTKDTreeData.hh"
#include "KTLogger.hh"
//#include "KTParam.hh"

#include <numeric>
#include <cmath>;

using std::string;


namespace Katydid
{
	KTLOGGER(sdlog, "KTSeqTrackFinder");

/* KTLOGGER(ctlog, "KTConsensusThresholding");
*/
    KT_REGISTER_PROCESSOR(KTSeqTrackFinder, "seq-clustering");

    KTSeqTrackFinder::KTSeqTrackFinder(const std::string& name) :
            KTProcessor(name),
            fFDelta(5*pow(10,5)),
            fTimeDistance(2),
            fBinDelta(4),
			fSigma(-3),
			fScoreThreshold(5.),
			fMinBin(0),
			fMaxBin(1),
			fMinFrequency(0.),
			fMaxFrequency(1.),
			fCalculateMinBin(true),
			fCalculateMaxBin(true),
            fSeqTrackSignal("lines-out", this),
	//		fSeqTrackSlot("disc-in", this, &KTSeqTrackFinder::PointLineAssignment, &fSeqTrackSignal),
    		fSeqTrackSlot("scores_in", this, &KTSeqTrackFinder::PointLineAssignment, &fSeqTrackSignal)
    {
    }

    KTSeqTrackFinder::~KTSeqTrackFinder()
    {
    }

    bool KTSeqTrackFinder::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

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
        }
        if (node->has("max-bin"))
        {
            SetMaxBin(node->get_value< unsigned >("max-bin"));
        }
        if (node->has("minimum-line-frequency-distance"))
		{
        	SetFrequencyRadius(node->get_value<double>("minimum-line-frequency-distance"));
		}
        if (node->has("max-time-gap-in-track"))
        {
        	SetTimeDistance(node->get_value<double>("max-time-gap-in-track"));
        }
        if (node->has("searched-bin-radius"))
        {
        	SetBinDelta(node->get_value<int>("searched-bin-radius"));
        }


        return true;
    }


  /*  bool KTSeqTrackFinder::PointLineAssignment(KTSliceHeader& slHeader, KTPowerSpectrumData& slice,  KTDiscriminatedPoints1DData& discPoints)
    {
    	KTDEBUG(kdlog, "Is this a new acquisition? fHaveNewData=" << fHaveNewData << " and GetIsNewAcquisition=" << slHeader.GetIsNewAcquisition());
    	// first check to see if this is a new acquisition; if so, run clustering on the previous acquistion's data
    	if (fHaveNewData && slHeader.GetIsNewAcquisition())
    	{'do track post processing and hand them over to the event clustering'}
        KTINFO(ctlog, "Performing point line assignment on discriminated points");

        double* new_trimming_limits;
        unsigned nComponents = slHeader.GetNComponents();


        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
        	KTPowerSpectrum* islice=slice.GetSpectrum(iComponent);
    		double maxFreq = std::max(fabs(slice.GetRangeMin()), fabs(slice.GetRangeMax()));
    		double minFreq = -0.5 * slice.GetBinWidth();
    		unsigned nBins = (maxFreq - minFreq) / slice.GetBinWidth();
    		if (slice.GetRangeMax() < 0. || slice.GetRangeMin() > 0.)
    		{
    			minFreq = std::min(fabs(slice.GetRangeMin()), fabs(slice.GetRangeMax()));
    			nBins = slice.size();
    		}

        	for (unsigned iBin = 0; iBin < nBins; ++iBin)
        	{
        			(*new_trimming_limits) += slice(iBin)/nBins;

        	}

        	const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discPoints.GetSetOfPoints(iComponent);
        	double new_TimeInAcq = (slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength());

            if (!KTSeqTrackFinder::LoopOverDiscriminatedPoints(incomingPts, slice, new_TimeInAcq, *new_trimming_limits))
            {
            	KTERROR(ctlog, "Sequential track creation failed");
            	return false;
            }

            KTDEBUG(ctlog, "Currently building" << KTSeqLines.size() << " lines");

        }
        return true;
    }
*/

    bool KTSeqTrackFinder::PointLineAssignment(KTSliceHeader& slHeader, KTScoredSpectrumData& spectrum)
        {
    		KTLines& new_Lines;

            unsigned nComponents = slHeader.GetNComponents();

            if (fCalculateMinBin)
            {
            	SetMinBin(spectrum.GetSpectrum(0)->FindBin(fMinFrequency));
            	KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
            }
            if (fCalculateMaxBin)
            {
            	SetMaxBin(spectrum.GetSpectrum(0)->FindBin(fMaxFrequency));
            	KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
            }


            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
            	std::vector<KTSeqLine::Point> CandidatePoints;
            	KTScoredSpectrum slice=spectrum.GetSpectrum(iComponent);
            	double BinWidth = slice.GetBinWidth();
            	double new_trimming_limits = 0.0;
            	double new_TimeInAcq = (slHeader.GetTimeInAcq() + 0.5 * slHeader.GetSliceLength());

            	if (slice == NULL)
            	{
            		KTERROR(sdlog, "Frequency spectrum pointer (component " << slice << ") is NULL!");
            	    return false;
            	}

            	unsigned nBins = fMaxBin - fMinBin + 1;

          	    // loop over bins, checking against the threshold
           	    double threshold, value;
           	    for (unsigned iBin=fMinBin; iBin<=fMaxBin; ++iBin)
            	{
            	 	(new_trimming_limits) += slice(iBin)/nBins;
            	   	value = (slice)(iBin);
            	    threshold = fScoreThreshold;

            	    if (value >= threshold)
           	        {
            	    	double PointFreq = BinWidth * ((double)iBin + 0.5);
            	    	KTSeqLine::Point new_Point(iBin, PointFreq, new_TimeInAcq, value);
           	        	CandidatePoints.push_back(new_Point);
           	        }
            	}

            	this->LoopOverHighPowerPoints(slice, CandidatePoints, new_Lines, new_trimming_limits);
            }
            return true;
        }

    bool KTSeqTrackFinder::LoopOverHighPowerPoints(KTScoredSpectrum& slice, std::vector<KTSeqLine::Point>& Points, KTLines& new_Lines, double& new_trimming_limits)
     {
    	double freq = 0.0, PointFreq= 0.0, new_score=0.0;
    	unsigned PointBin = 0;

         for (unsigned iPoint=0; iPoint<Points.size(); iPoint++)
         {
        	 PointFreq = Points[iPoint].fPointFreq;
        	 PointBin = Points[iPoint].fPointFreq;

         	if (PointFreq - freq > fFDelta)
         	{
         		bool match = false;

         		this->SearchAreaForBetterPoints(PointBin, PointFreq, slice);

         		freq = PointFreq;
         		slice(PointBin) = fSigma; //should be PointBin-BinDelta:PointBin+BinDelta

         		unsigned int nLines = new_Lines.GetNLines();
         		std::vector<KTSeqLine>& fLines = new_Lines.GetLines();

         		for (unsigned iLines=0; iLines>nLines; iLines++)
         		{
         			if (fLines[iLines].fActive)
					{
         				if (fLines[iLines].InvestigatePoint(Points[PointBin], new_trimming_limits))
         				{
         					match = true;
         					break;
         				}
					}
         		}
         		if (!match)
         		{
         			KTSeqLine new_Line(nLines, Points(iPoint), new_trimming_limits);
         			new_Lines.AddLine(new_Line);

         		 }

         	}
         }
     }

 /*   bool KTSeqTrackFinder::LoopOverDiscriminatedPoints(const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts, KTPowerSpectrum& slice, double& TimeInAcq, double& TrimmingLimits)
    {
    	double freq = 0.0;


        for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin();
                            pIt != incomingPts.end(); ++pIt)
        {

        	if (KTSeqTrackFinder::VetoPoint(pIt, slice, freq))
        	{
        		bool match = false;
        		for (KTSeqLines::SetOfLines::const_iterator LineIt =fLines.begin(); LineIt != fLines.end(); ++LineIt)
        			{
        			if (LineIt.InvestigatePoint(pIt, TimeInAcq, *new_trimming_limits))
        				{
        				match = true;
        				break;
        				}
        			}
        		if (!match)
        		{
        			nLines=fLines.GetNLines()+1;
        			fLines.SetNComponents(nLines);
        			fLines.Line(nLines, pIt, TimeInAcq, *new_trimming_limits);
        		 }

        	}
        }
    }
*/

    void KTSeqTrackFinder::SearchAreaForBetterPoints(unsigned& PointBin, double& PointFreq, KTScoredSpectrum& slice)
    {
    	double dF = slice.GetBinWidth();
    	double LocalDelta = fFDelta+dF; //make sure initially LocalDelta is greater than dF

    	int loop_counter = 0;
    	double OldFreq = 0.0;


    	while(LocalDelta > 2*dF && loop_counter <10)
    	{
    		loop_counter++;
    		OldFreq = PointFreq;

    		if (PointBin > fMinBin && PointBin <fMaxBin)
    		{
    			this-> WeightedAverage(slice, PointFreq, PointBin);
    		}

    		LocalDelta = abs(OldFreq-PointFreq);
    	}
    }

    inline void KTSeqTrackFinder::WeightedAverage(KTScoredSpectrum& slice, double& PointFreq, unsigned& PointBin)
    {
    	double BinWidth = slice.GetBinWidth();

    	double new_Point = 0.0;
    	double f_average = 0.0;
    	double weighted_Bin = slice(PointBin)*PointFreq;
    	double wsum = 0.0;

    	for (int iBin=-fBinDelta/2; iBin<fBinDelta/2; iBin++)
    	{
    		weighted_Bin = double(PointBin+iBin)*slice(PointBin+iBin);
    		wsum +=slice(PointBin+iBin);
    		new_Point += weighted_Bin;
    	}
    	new_Point = unsigned(new_Point/wsum);
    	f_average = BinWidth * ((double)new_Point + 0.5);

    	PointBin = unsigned(new_Point);
    	PointFreq = f_average;
    }

/*    bool KTSeqTrackFinder::VetoPoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, KTPowerSpectrum& slice, double& freq)
    {
    	double new_freq = Point->second.fAbscissa;
    	double new_Amp = Point->second.fOrdinate;
    	double weighed_freq = 0;
    	double weighed_sum = 0;
    	int PointBin = Point->first.Bin;
    	double new_point = 0;
    	double df = slice(1)-slice(0);


    	if (abs(freq-new_freq)<fFDelta)
    	{
    		slice.Override(PointBin)=fSigma;

    		return false;
    	}
    	else
		{
    		freq = new_freq;


    		for (unsigned iBin = PointBin-fBinDelta; iBin < PointBin+fBinDelta; ++iBin)
    		{
    			weighed_freq = freq*slice(iBin);
    			weighed_sum +=slice(iBin);
    			new_point += weighed_freq;
    		}
    		new_point = (new_point/weighed_sum);
    		if (abs(freq-new_point)<df)
    		{
    			slice.Override(PointBin)=fSigma;
    			return true;
    		}
    		else {return false;}
		}
    }

*/




} /* namespace Katydid */
