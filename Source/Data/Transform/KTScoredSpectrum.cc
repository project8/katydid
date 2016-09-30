/*
 * KTScoredSpectrum.cc
 *
 *  Created on: Mar 10, 2016
 *      Author: Christine
 */

#include <KTScoredSpectrum.hh>

#include "KTPowerSpectrum.hh"
#include "KTGainVariationData.hh"

#include "KTLogger.hh"

namespace
{
	const int BinDelta = 10;
	const double bplus = 1.5;
	const double bminus = 1.0;
	const double Threshold = 1.5;
	const double aminus = -0.3;
	const double aplus = 1.0;
	const char Mode = 'Scores';
}

namespace Katydid
{
	KTLOGGER(sclog, "KTScoredSpectrum");

	KTScoredSpectrum::KTScoredSpectrum(size_t nBins, double rangeMin, double rangeMax) :
        				KTPhysicalArray< 1, double >(nBins, rangeMin, rangeMax),
						KTFrequencyDomainArray()

						{
								SetAxisLabel("Frequency (Hz)");
								SetDataLabel("Score");
						}
	KTScoredSpectrum::KTScoredSpectrum(const KTScoredSpectrum& orig) :
        				KTPhysicalArray< 1, double >(orig),
						KTFrequencyDomainArray(orig)
						{
						}

	KTScoredSpectrum::~KTScoredSpectrum()
	{
	}


	KTScoredSpectrum& KTScoredSpectrum::operator=(const KTScoredSpectrum& rhs)
	{
		KTPhysicalArray< 1, double >::operator=(rhs);

		return *this;
	}

	KTScoredSpectrum& KTScoredSpectrum::Scale(double scale)
	{
		(*this) *= scale;
		return *this;
	}
	bool KTScoredSpectrum::Configure(char& new_Mode, double new_BinDelta, double new_bplus, double new_bminus, double new_Threshold, double new_aminus, double new_aplus)
	{
		Mode = new_Mode;
		BinDelta = new_BinDelta;
		bplus = new_bplus;
		bminus = new_bminus;
		Threshold = new_Threshold;
		aminus = new_aminus;
		aplus = new_aplus;
		return true;
	}



	void KTScoredSpectrum::ConvertToScoredSpectrum()
	{
		KTDEBUG(sclog, "Converting to Scored Spectrum");
		(*this) *= GetBinWidth();

		SetDataLabel("Scores ");

		return;
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

			if ((*newRatios)(iBin) > Threshold) (*newScores)(iBin) = aplus*pow(((*newRatios)(iBin)-Threshold),bplus);
			else (*newScores)(iBin) = aminus*pow(((*newRatios)(iBin)-Threshold),bminus);


		}
		if (Mode=='SNR')
		{
			delete newScores;
			return newRatios;
		}
		else
		{
		delete newRatios;
		return newScores;
		}
	}



} /* namespace Katydid */

