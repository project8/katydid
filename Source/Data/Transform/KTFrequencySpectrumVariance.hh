/*
 * KTFrequencySpectrumVariance.hh
 *
 *  Created on: Feb 23, 2018
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMVARIANCE_HH_
#define KTFREQUENCYSPECTRUMVARIANCE_HH_

#include "KTFrequencyDomainArray.hh"

#include "KTPhysicalArray.hh"

namespace Katydid
{
    class KTFrequencySpectrumVariance : public KTPhysicalArray< 1, double >, public KTFrequencyDomainArray
    {
        public:
            KTFrequencySpectrumVariance(size_t nBins=1, double rangeMin=0., double rangeMax=1.);
            KTFrequencySpectrumVariance(const KTFrequencySpectrumVariance& orig);
            virtual ~KTFrequencySpectrumVariance();

            KTFrequencySpectrumVariance& operator=(const KTFrequencySpectrumVariance& orig);

            virtual unsigned GetNFrequencyBins() const;
            virtual double GetFrequencyBinWidth() const;

            virtual const KTAxisProperties< 1 >& GetAxis() const;
            virtual KTAxisProperties< 1 >& GetAxis();

            virtual const std::string& GetOrdinateLabel() const;
    };


    inline const KTAxisProperties< 1 >& KTFrequencySpectrumVariance::GetAxis() const
    {
        return *this;
    }

    inline KTAxisProperties< 1 >& KTFrequencySpectrumVariance::GetAxis()
    {
        return *this;
    }

    inline const std::string& KTFrequencySpectrumVariance::GetOrdinateLabel() const
    {
        return GetDataLabel();
    }

    inline unsigned KTFrequencySpectrumVariance::GetNFrequencyBins() const
    {
        return size();
    }

    inline double KTFrequencySpectrumVariance::GetFrequencyBinWidth() const
    {
        return GetBinWidth();
    }



} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMVARIANCE_HH_ */
