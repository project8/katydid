/*
 * KTSeqLine.hh
 *
 *  Created on: Sep 8, 2016
 *      Author: Christine
 */

#ifndef KTSEQLINE_HH_
#define KTSEQLINE_HH_


#include "KTFrequencyDomainArray.hh"
#include "KTPhysicalArray.hh"
//#include "KTSeqTrackCreator.hh"


#include <string>
#include <vector>

namespace Katydid {


//using namespace Nymph;

class KTSeqLine
{
	struct Point
	        {
	        	double fBinInSlice;
	            double fPointFreq;
	            double fTimeInAcq;
	            double fScore;
	            Point(unsigned BinInSclice, double PointFreq, double TimeInAcq, double Score) : fBinInSlice(BinInSclice), fPointFreq(PointFreq), fTimeInAcq(TimeInAcq), fScore(Score) {}
	        };

	public:


		KTSeqLine(unsigned LineID, Point& Point, double& new_trimming_limits);
		virtual ~KTSeqLine();


		unsigned GetNPoints();
		double GetLength();
		double GetStartFreq();
		double GetSlope();
		double GetStartTime();
		double GetTotalScore();
		unsigned GetLineID();



		bool Configure(double& DeltaT, double& DeltaF);
		bool InvestigatePoint(Point& Point, double& new_trimming_limits);
		bool TrimEdges();
		bool CollectPoint(Point&, double& new_trimming_limits);

		double GetLineScore();

		//investigation parameters
		double fDeltaT;
		double fDeltaF;
		unsigned fLambda;
		double fLineThreshold;
		double fMu;
		int fNu;


		//line properties
		double fLineSlope;
		double fLineScore;
		double fLength;

		bool fActive;
		bool fCollectable;

		unsigned Identifier;

		//point lists

		std::vector<double> trimming_limits;
		std::vector<Point> fLinePoints;

/*		public:
			KTScoredSpectrum& operator=(const KTScoredSpectrum& rhs);

			KTScoredSpectrum& Scale(double scale);
*/
};


	double KTSeqLine::GetTotalScore()
	{
		double score;
		unsigned nPoints = this->GetNPoints;
		if (nPoints>0)
		{
			for(unsigned i=0; i<nPoints;i++)
			{
				score+=this->fLinePoints[i].fScore;
			}
			score=score/nPoints;
		}
		return score;
	}

	unsigned inline KTSeqLine::GetNPoints()
	{
		return fLinePoints.size();
	}
	double inline KTSeqLine::GetLength()
	{
		return fLength;
	}

	double inline KTSeqLine::GetStartFreq()
	{
		return fLinePoints[0].fPointFreq;
	}

	double inline KTSeqLine::GetStartTime()
	{
		return fLinePoints[0].fTimeInAcq;
	}
	double inline KTSeqLine::GetSlope()
	{
		return fLineSlope;
	}





} /* namespace Katydid */

#endif /* SOURCE_DATA_TRANSFORM_KTSEQLINE_HH_ */


