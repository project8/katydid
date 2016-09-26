/*
 * KTSeqLine.hh
 *
 *  Created on: Sep 8, 2016
 *      Author: Christine
 */

#ifndef SOURCE_DATA_TRANSFORM_KTSEQLINE_HH_
#define SOURCE_DATA_TRANSFORM_KTSEQLINE_HH_


#include "KTFrequencyDomainArray.hh"
#include "KTPhysicalArray.hh"
#include "KTSeqTrackCreator.hh"


#include <string>
#include <vector>

namespace Katydid {

class KTPowerSpectrum;
class KTGainVariationData;
class KTScoredSpectrum;
class KTSeqTrackCreator;

using namespace Nymph;

class KTSeqLine
{

public:
	KTSeqLine(unsigned LineID, double StartTime, double StartFreq, double Score);
	KTSeqLine(const KTSeqLine& orig);
	virtual ~KTSeqLine();

	double SetLineID(unsigned) const;
	double SetStartFreq(double) const;
	double SetStartTime(double) const;


	unsigned GetNPoints();
	double GetStartFreq() const;
	double GetSlope();
	double GetStartTime() const;
	double GetTotalScore();




	void InvestigatePoint(int& MatchFlag, KTSeqTrackCreator::Parameters& p, KTScoredSpectrum& slice, int point);
	void TrimEdges(KTSeqTrackCreator::Parameters& p);

	//line properties
	double slope;
	double startfreq;
	double LineScore;

	int active;
	int collectable;
	int length;
	int identifier;

	//point lists
	std::vector<double> scorelist;
	std::vector<double> timelist;
	std::vector<double> freqlist;
	std::vector<double> trimming_limits;

public:
	KTScoredSpectrum& operator=(const KTScoredSpectrum& rhs);

	KTScoredSpectrum& Scale(double scale);

};









} /* namespace Katydid */

#endif /* SOURCE_DATA_TRANSFORM_KTSEQLINE_HH_ */


