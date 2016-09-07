/*
 * KTScoredSpectrum.hh
 *
 *  Created on: Mar 10, 2016
 *      Author: Christine
 */

#ifndef SOURCE_DATA_TRANSFORM_KTSCOREDSPECTRUM_HH_
#define SOURCE_DATA_TRANSFORM_KTSCOREDSPECTRUM_HH_


#include "KTFrequencyDomainArray.hh"
#include "KTPhysicalArray.hh"


#include <string>

namespace Katydid {

class KTPowerSpectrum;
class KTGainVariationData;

using namespace Nymph;

class KTScoredSpectrum : public KTPhysicalArray< 1, double >, public KTFrequencyDomainArray
{

public:
	KTScoredSpectrum(size_t nBins=1, double rangeMin=0., double rangeMax=1.);
	KTScoredSpectrum(const KTScoredSpectrum& orig);
	virtual ~KTScoredSpectrum();

	unsigned GetNFrequencyBins() const;
	double GetFrequencyBinWidth() const;

	const KTAxisProperties< 1 >& GetAxis() const;
	KTAxisProperties< 1 >& GetAxis();

	const std::string& GetOrdinateLabel() const;


	void ConvertToScoredSpectrum();
	//void CreateScoredSpectrum();


	static KTScoredSpectrum* CreateScoredSpectrum(KTPowerSpectrum& component, KTGainVariationData& gvData);


public:
	KTScoredSpectrum& operator=(const KTScoredSpectrum& rhs);

	KTScoredSpectrum& Scale(double scale);

//private:

//	const int BinDelta = 10;
//	const double bplus = 1.5;
//	const double bminus = 1.0;
//	const double Threshhold = 1.5;
//	const double aminus = -0.3;
//	const double aplus = 1.0;



};

inline const KTAxisProperties< 1 >& KTScoredSpectrum::GetAxis() const
{
	return *this;
}

inline KTAxisProperties< 1 >& KTScoredSpectrum::GetAxis()
{
	return *this;
}

inline const std::string& KTScoredSpectrum::GetOrdinateLabel() const
{
	return GetDataLabel();
}

inline unsigned KTScoredSpectrum::GetNFrequencyBins() const
{
	return size();
}

inline double KTScoredSpectrum::GetFrequencyBinWidth() const
{
	return GetBinWidth();
}







} /* namespace Katydid */

#endif /* SOURCE_DATA_TRANSFORM_KTSCOREDSPECTRUM_HH_ */


