/*
 * KTScoredSpectrum.cc
 *
 *  Created on: Mar 10, 2016
 *      Author: Christine
 */

#include <KTSeqLine.hh>

#include "KTPowerSpectrum.hh"
#include "KTScoredSpectrum.hh"

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

	KTSeqLine::KTSeqLine(unsigned LineID, double StartTime, double StartFreq, double Score)

						{
								SetLineID(LineID);
								SetStartFreq(StartFreq);
								SetStartTime(StartTime);
						}

	KTSeqLine::KTSeqLine(const KTSeqLine& orig)
						{
						}

	KTSeqLine::~KTSeqLine()
	{
	}





	void KTSeqLine::InvestigatePoint(int& MatchFlag, KTSeqTrackCreator::Parameters& p, KTScoredSpectrum& slice, int point)
	{


			double* NewFreq = slice.GetBinCenter(point);
			double* NewScore = slice(point)->Evaluate(slice.GetBinCenter(point));
			double* new_trimming_limits;

			for (unsigned iBin = BinDelta; iBin < nBins-BinDelta; ++iBin)
					{
						(*new_trimming_limits) += slice(iBin)/nBins->Evaluate(slice.GetBinCenter(iBin));

					}
			trimming_limits.push_back(*new_trimming_limits);


			if (time-timelist.back() > p.DeltaT)
			{
				//std::cout << identifier << " time distance" << std::endl;
				this-> TrimEdges(p);
			}
			else if (int(timelist.size()) > p.lan &&  LineScore < p.line_thresh)
			{
				//std::cout << identifier << " low score" << std::endl;
				this -> TrimEdges(p);
			}
			else if (abs(freq-(freqlist[0]+slope*(time-timelist[0]))) < p.DeltaF)
			{
				match = 1;
				slope = (freq-freqlist[0])/(time-timelist[0]);

				timelist.push_back(time);
				freqlist.push_back(freq);
				scorelist.push_back(new_score);
				LineScore += slice[point];

				slice[point] = p.sigma;

			}

		}
	}


	KTScoredSpectrum* KTScoredSpectrum::CreateScoredSpectrum(KTPowerSpectrum& component, KTGainVariationData& gvData)
	{

		double maxFreq = std::max(fabs(component.GetRangeMin()), fabs(component.GetRangeMax()));

		double minFreq = -0.5 * component.GetBinWidth();
		unsigned nBins = (maxFreq - minFreq) / component.GetBinWidth();
		if (component.GetRangeMax() < 0. || component.GetRangeMin() > 0.)
		{
			minFreq = std::min(fabs(component.GetRangeMin()), fabs(component.GetRangeMax()));
			nBins = component.size();
		}

		KTScoredSpectrum* newRatios = new KTScoredSpectrum(nBins, component.GetRangeMin(), component.GetRangeMax());
		KTScoredSpectrum* newScores = new KTScoredSpectrum(nBins, component.GetRangeMin(), component.GetRangeMax());


		for (unsigned iBin = BinDelta; iBin < nBins-BinDelta; ++iBin)
		{
			(*newRatios)(iBin) = component(iBin)/(gvData.GetSpline(0)->Evaluate(component.GetBinCenter(iBin)));


			(*newScores)(iBin) = 0.;

		}


		for (unsigned iBin = 0; iBin < nBins; ++iBin)
		{

			if ((*newRatios)(iBin) > Threshhold) (*newScores)(iBin) = aplus*pow(((*newRatios)(iBin)-Threshhold),bplus);
			else (*newScores)(iBin) = aminus*pow(((*newRatios)(iBin)-Threshhold),bminus);


		}
		delete newRatios;
		return newScores;
	}


} /* namespace Katydid */

