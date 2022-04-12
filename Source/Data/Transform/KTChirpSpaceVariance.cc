/*
 * KTChirpSpaceVariance.cc
 *
 *  Created on: Feb 23, 2018
 *      Author: nsoblath
 */

#include "KTChirpSpaceVariance.hh"

namespace Katydid
{
    KTChirpSpaceVariance::KTChirpSpaceVariance(size_t nBins0, double rangeMin0, double rangeMax0, size_t nBins1, double rangeMin1, double rangeMax1) :
            KTPhysicalArray< 2, double >(nBins0, rangeMin0, rangeMax0, nBins1, rangeMin1, rangeMax1),
            KTChirpDomainArray()
    {
//	SetAxisLabel("Label Test");
//	SetDataLabel("Data Test");

	const std::string& AxisLabel0 = "Slope (Hz/s)";
        const std::string& DataLabel0 = "Arb 0";
        const std::string& AxisLabel1 = "Intercept (Hz)";
        const std::string& DataLabel1 = "Arb 1";

        SetAxisLabel(0, AxisLabel0);
        SetDataLabel( DataLabel0);
        SetAxisLabel(1, AxisLabel1);
        SetDataLabel( DataLabel1);

    }

    KTChirpSpaceVariance::KTChirpSpaceVariance(double value, size_t nBins0, double rangeMin0, double rangeMax0, size_t nBins1, double rangeMin1, double rangeMax1) :
            KTChirpSpaceVariance(nBins0, rangeMin0, rangeMax0, nBins1, rangeMin1, rangeMax1)
    {
        for (unsigned x_index = 0; x_index < nBins0; ++x_index)
        {
	    for (unsigned y_index = 0; y_index < nBins1; ++y_index)
        	{
            	    fData(x_index,y_index) = value;
		}
        }
    }

    KTChirpSpaceVariance::KTChirpSpaceVariance(const KTChirpSpaceVariance& orig) :
            KTPhysicalArray< 2, double >(orig),
            KTChirpDomainArray(orig)
    {
    }

    KTChirpSpaceVariance::~KTChirpSpaceVariance()
    {
    }


    KTChirpSpaceVariance& KTChirpSpaceVariance::operator=(const KTChirpSpaceVariance& orig)
    {
        KTPhysicalArray< 2, double >::operator=(orig);
        return *this;
    }

    KTChirpSpaceVariance& KTChirpSpaceVariance::Scale(double scale)
    {
        (*this) *= scale;
        return *this;
    }

} /* namespace Katydid */
