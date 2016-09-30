/*
 * KTScoredSpectrum.cc
 *
 *  Created on: Mar 10, 2016
 *      Author: Christine
 */

#include <KTSeqLine.hh>

#include "KTLogger.hh"

#include <iostream>
#include <cmath>


namespace Katydid
{
	KTLOGGER(sclog, "KTSeqLine");

	KTSeqLine::KTSeqLine(unsigned LineID, Point& Point, double& new_trimming_limits):
			Identifier(LineID),
			fActive(true),
			fCollectable(false),
			fLineSlope(0.0),
			fLength(0.0),

			fDeltaT(1e-4),
			fDeltaF(2e5),
			fLambda(10),
			fMu(0.5),
			fNu(1),
			fLineThreshold(15.),
			fLineScore(0.0)
			{
				this->CollectPoint(Point, new_trimming_limits);
			}


	KTSeqLine::~KTSeqLine()
	{
	}


	unsigned KTSeqLine::GetLineID()
	{
		return Identifier;
	}

	bool KTSeqLine::CollectPoint(Point& Point, double& new_trimming_limits)
	{
		fLinePoints.push_back(Point);
		trimming_limits.push_back(new_trimming_limits);
		fLineScore+=Point.fScore;
		fLineSlope = (Point.fPointFreq-fLinePoints[0].fPointFreq)/(Point.fTimeInAcq-fLinePoints[0].fPointFreq);




		/*scorelist.push_back(Point.fScore);
		timelist.push_back(Point.fTimeInAcq);
		freqlist.push_back(Point.fPointFreq);
		trimming_limits.push_back(new_trimming_limits);
		*/
		return true;
	}

	bool KTSeqLine::Configure(double& DeltaT, double& DeltaF)
	{
		fDeltaT(DeltaT);
		fDeltaF(DeltaF);
		return true;
	}

	bool KTSeqLine::InvestigatePoint(Point& Point, double& new_trimming_limits)
	{


			double NewFreq = Point.fPointFreq;
			double NewScore = Point.fScore;





			if (Point.fTimeInAcq-fLinePoints.back().fTimeInAcq > fDeltaT)
			{
				KTINFO(sclog,  "End of Line" << Identifier << " to far in the past");
				this-> TrimEdges();
				return false;
			}
			else if (fLinePoints.size() > fLambda &&  fLineScore < fLineThreshold)
			{
				KTINFO(sclog,  "Line" << Identifier << " has fallen below threshold");
				this -> TrimEdges();
				return false;
			}
			else if (abs(Point.fPointFreq-(fLinePoints[0].fPointFreq+fLineSlope*(Point.fTimeInAcq-fLinePoints[0].fTimeInAcq))) < fDeltaF)
			{
				if(this->CollectPoint(Point, new_trimming_limits))
				{
					return true;
				}
				else {return false;}

			}
			else
			{
				return false;
			}

		}
	bool KTSeqLine::TrimEdges()
	{
		return true;

	}

	bool KTSeqLine::TrimEdges()
	{
		while (fLinePoints.size()>=fLambda && fLinePoints.back().fScore < fMu*trimming_limits.back())
		{
			fLinePoints.erase (fLinePoints.end() - 1);
			trimming_limits.erase (trimming_limits.end() - 1);
		}

		while (fLinePoints.size()>=fLambda && fLinePoints.front().fScore < fMu*trimming_limits.front())
			{
			fLinePoints.erase (fLinePoints.begin());
			trimming_limits.erase (trimming_limits.begin());
			}

		fActive = false;
		if (fLinePoints.size()> fLambda)
			{
			fCollectable = true;
			fLength = fLinePoints.back().fTimeInAcq-fLinePoints.front().fTimeInAcq;
			KTINFO(sclog, "Line" << Identifier << " is ready for collection");
			}
		else
		{
			KTINFO(sclog,  "Line" << Identifier << " was to short and is deleted");

		}
		}
	}

} /* namespace Katydid */

