/*
 * KTScoredSpectrum.cc
 *
 *  Created on: Mar 10, 2016
 *      Author: Christine
 */

#include <KTSeqLine.hh>


#include "KTDiscriminatedPoints1DData.hh"

#include "KTLogger.hh"

#include <iostream>
#include <cmath>


using std::max;

namespace
{
	const int BinDelta = 10;
	const double bplus = 1.5;
	const double bminus = 1.0;
	const double Threshhold = 1.5;
	const double aminus = -0.3;
	const double aplus = 1.0;
}

namespace Katydid
{
	KTLOGGER(sclog, "KTSeqLine");

	KTSeqLine::KTSeqLine(unsigned LineID, KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, double& TimeInAcq, double* new_trimming_limits)

						{
								SetLineID(LineID);
								this->CollectPoint(Point, TimeInAcq, *new_trimming_limits);
						}

	KTSeqLine::KTSeqLine(const KTSeqLine& orig)
						{
						}

	KTSeqLine::~KTSeqLine()
	{
	}




	bool KTSeqLine::CollectPoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, double& TimeInAcq, double* new_trimming_limits)
	{
		scorelist.push_back(Point->second.fOrdinate);
		timelist.push_back(TimeInAcq);
		freqlist.push_back(Point->second.fAbscissa);
		trimming_limits.push_back(*new_trimming_limits);
		return true;
	}

	bool KTSeqLine::InvestigatePoint(KTDiscriminatedPoints1DData::SetOfPoints::const_iterator Point, double& TimeInAcq, double* new_trimming_limits)
	{


			double* NewFreq = Point->second.fAbscissa;
			double* NewScore = Point->second.fOrdinate;






			if (TimeInAcq-timelist.back() > DeltaT)
			{
				//std::cout << identifier << " time distance" << std::endl;
				this-> TrimEdges();
				return false;
			}
			else if (int(timelist.size()) > Lambda &&  this->GetLineScore < LineThreshold)
			{
				//std::cout << identifier << " low score" << std::endl;
				this -> TrimEdges();
				return false;
			}
			else if (abs(NewFreq-(freqlist[0]+LineSlope*(TimeInAcq-timelist[0]))) < DeltaF)
			{
				if(this->CollectPoint(Point, TimeInAcq, *new_trimming_limits))
				{
				LineSlope = (NewFreq-freqlist[0])/(TimeInAcq-timelist[0]);
				LineScore += NewScore;



				return true;
				}
				else {return false;}

			}
			else
			{
				return false;
			}

		}

	}

} /* namespace Katydid */

