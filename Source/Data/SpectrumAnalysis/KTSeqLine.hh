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
#include "KTMemberVariable.hh"
//#include "KTSeqTrackCreator.hh"


#include <string>
#include <vector>

namespace Katydid {


//using namespace Nymph;

class KTSeqLine : public Nymph::KTExtensibleData< KTHoughData >
{
	struct Point
	        {
	        	double fBinInSlice;
	            double fPointFreq;
	            double fTimeInAcq;
	            double fTimeInRunC;
	            double fScore;
	            double fAcquisitionID;
	            double fComponent;

	            Point(unsigned BinInSclice, double PointFreq, double TimeInAcq, double TimeInRunC, double Score, uint64_t AcqID, unsigned iComponent) : fBinInSlice(BinInSclice), fPointFreq(PointFreq),
	            		fTimeInAcq(TimeInAcq), fTimeInRunC(TimeInRunC), fScore(Score), fAcquisitionID(AcqID), fComponent(iComponent) {}
	        };

	public:


		KTSeqLine();
		virtual ~KTSeqLine();


		unsigned GetNPoints();
		//double GetLength();
		double GetStartFreq();
		//double GetSlope();
		double GetStartTime();
		double GetTimeInRunC();
		double GetTotalScore();
		//unsigned GetLineID();
		double GetStopTime();
		double GetEndFreq();
		//unsigned GetComponent();
		//uint64_t GetAcquisitionID();
		double GetAmplitudeSum();


		bool Configure(double& DeltaT, double& DeltaF);
		bool InvestigatePoint(Point& Point, double& new_trimming_limits);
		bool TrimEdges();
		bool CollectPoint(Point&, double& new_trimming_limits);

		double GetLineScore();

	private:
		//investigation parameters
		MEMBERVARIABLE(double, DeltaT);
		MEMBERVARIABLE(double, DeltaF);
		MEMBERVARIABLE(unsigned, Lambda);
		MEMBERVARIABLE(double, LineThreshold);
		MEMBERVARIABLE(double, Mu);
		MEMBERVARIABLE(int, Nu);

	public:
		//line properties
		MEMBERVARIABLE(double, LineSlope);
		MEMBERVARIABLE(double, LineScore);
		MEMBERVARIABLE(double, Length);

		MEMBERVARIABLE(unsigned, Component);
		MEMBERVARIABLE(uint64_t, AcquisitionID);

		MEMBERVARIABLE(bool, Active);
		MEMBERVARIABLE(bool, Collectable);

		MEMBERVARIABLE(unsigned, Identifier);

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
	/*double inline KTSeqLine::GetLength()
	{
		return fLength;
	}*/

	double inline KTSeqLine::GetStartFreq()
	{
		return fLinePoints[0].fPointFreq;
	}

	double inline KTSeqLine::GetStartTime()
	{
		return fLinePoints[0].fTimeInAcq;
	}
	double KTSeqLine::GetStopTime()
	{
		return fLinePoints.back().fTimeInAcq-this->GetStartTime();
	}
	double KTSeqLine::GetTimeInRunC()
	{
		return fLinePoints[0].fTimeInRunC;
	}

	/*double inline KTSeqLine::GetSlope()
	{
		return fLineSlope;
	}

	*/
	double inline KTSeqLine::GetEndFreq()
	{
		return fLinePoints.back().fPointFreq;
	}
	/*unsigned inline KTSeqLine::GetComponent()
	{
		return fComponent;
	}
	uint64_t inline KTSeqLine::GetAcquisitionID()
	{
		return fAcquisitionID;
	}*/
	double inline KTSeqLine::GetAmplitudeSum()
	{
		double sum = 0.0;
		for (unsigned iPoint = 0; iPoint < fLinePoints.size(); iPoint++)
		{
			sum += fLinePoints[iPoint].fPower;
		}
		return sum;
	}




} /* namespace Katydid */

#endif /* SOURCE_DATA_TRANSFORM_KTSEQLINE_HH_ */


