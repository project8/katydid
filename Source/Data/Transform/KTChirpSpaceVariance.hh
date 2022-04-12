/*
 * KTChirpSpaceVariance.hh
 *
 *  Created on: Apr 5, 2022
 *      Author: jkgaison
 */

#ifndef KTCHIRPSPACEVARIANCE_HH_
#define KTCHIRPSPACEVARIANCE_HH_

#include "KTChirpDomainArray.hh"

#include "KTPhysicalArray.hh"

namespace Katydid
{
    class KTChirpSpaceVariance : public KTPhysicalArray< 2, double >, public KTChirpDomainArray
    {
        public:
            KTChirpSpaceVariance(size_t nBins0=1, double rangeMin0=0., double rangeMax0=1., size_t nBins1=1, double rangeMin1=0., double rangeMax1=1.);
            KTChirpSpaceVariance(double value, size_t nBins0, double rangeMin0=0., double rangeMax0=1., size_t nBins1=1, double rangeMin1=0., double rangeMax1=1.);
            KTChirpSpaceVariance(const KTChirpSpaceVariance& orig);
            virtual ~KTChirpSpaceVariance();

            KTChirpSpaceVariance& operator=(const KTChirpSpaceVariance& orig);

            virtual unsigned GetNInterceptBins() const;
            virtual double GetInterceptBinWidth() const;
            virtual unsigned GetNSlopeBins() const;
            virtual double GetSlopeBinWidth() const;
            KTChirpSpaceVariance& Scale(double scale);

//            virtual const KTAxisProperties< 2 >& GetAxis() const;
//            virtual KTAxisProperties< 2 >& GetAxis();

            virtual const std::string& GetOrdinateLabel() const;
    };

/*
    inline const KTAxisProperties< 2 >& KTChirpSpaceVariance::GetAxis() const
    {
        return *this;
    }

    inline KTAxisProperties< 2 >& KTChirpSpaceVariance::GetAxis()
    {
        return *this;
    }
*/
    inline const std::string& KTChirpSpaceVariance::GetOrdinateLabel() const
    {
        return GetDataLabel();
    }

    inline unsigned KTChirpSpaceVariance::GetNSlopeBins() const
    {
        return size(0);
    }

    inline double KTChirpSpaceVariance::GetSlopeBinWidth() const
    {
        return GetBinWidth(0);
    }

    inline unsigned KTChirpSpaceVariance::GetNInterceptBins() const
    {
        return size(1);
    }

    inline double KTChirpSpaceVariance::GetInterceptBinWidth() const
    {
        return GetBinWidth(1);
    }


} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMVARIANCE_HH_ */
