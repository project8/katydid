/*
 * KTConsensusThresholding.cc
 *
 *  Created on: Sep 15, 2016
 *      Author: Christine
 */

#include "KTSeqTrackCreator.hh"

#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTScoredSpectrum.hh"
#include "KTScoredSpectrumData.hh"

#include "KTKDTreeData.hh"
#include "KTLogger.hh"
#include "KTParam.hh"

#include <cmath>;

using std::string;


namespace Katydid
{
    KTLOGGER(ctlog, "KTConsensusThresholding");

    KT_REGISTER_PROCESSOR(KTSeqTrackCreator, "seq-clustering");

    KTSeqTrackCreator::KTSeqTrackCreator(const std::string& name) :
            KTProcessor(name),
            fFDelta(5*pow(10,5)),
            fTimeDistance(2),
            fBinDelta(4),
			fSigma(-3),
            fSeqTrackSignal("lines-out", this),
			fSeqTrackSlot("disc-in", this, &KTSeqTrackCreator::PointLineAssignment, &fSeqTrackSignal)
    {
    }

    KTSeqTrackCreator::~KTSeqTrackCreator()
    {
    }

    bool KTSeqTrackCreator::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetFrequencyRadius(node->GetValue<double>("frequency-radius"));
        SetTimeDistance(node->GetValue<double>("max-component-distance"));
        SetBinDelta(node->GetValue<int>("max-bin-distance"));


        return true;
    }

    bool KTSeqTrackCreator::PointLineAssignment(KTSliceHeader& slHeader, KTPowerSpectrumData& slice,  KTDiscriminatedPoints1DData& discPoints)
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

            if (!KTSeqTrackCreator::LoopOverDiscriminatedPoints(incomingPts, slice, new_TimeInAcq, *new_trimming_limits))
            {
            	KTERROR(ctlog, "Sequential track creation failed");
            	return false;
            }

            KTDEBUG(ctlog, "Currently building" << KTLines.size() << " lines");

        }
        return true;
    }

    bool KTSeqTrackCreator::LoopOverDiscriminatedPoints(const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts, KTPowerSpectrum& slice, double& TimeInAcq, double* new_trimming_limits)
    {
    	double freq = 0.0;


        for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin();
                            pIt != incomingPts.end(); ++pIt)
        {

        	if (KTSeqTrackCreator::VetoPoint(pIt, slice, freq))
        	{
        		bool match = false;
        		for (KTLines::SetOfLines::const_iterator LineIt =fLines.begin(); LineIt != fLines.end(); ++LineIt)
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

    bool KTSeqTrackCreator::VetoPoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, KTPowerSpectrum& slice, double& freq)
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






} /* namespace Katydid */
