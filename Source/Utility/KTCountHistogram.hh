/*
 * KTCountHistogram.hh
 *
 *  Created on: Dec 20, 2013
 *      Author: nsoblath
 */

#ifndef KTCOUNTHISTOGRAM_HH_
#define KTCOUNTHISTOGRAM_HH_

#include "KTPhysicalArray.hh"

namespace Katydid
{
    

    class KTCountHistogram : public KTPhysicalArray< 1, unsigned >
    {
        public:
            KTCountHistogram();
            KTCountHistogram(size_t nBins, double rangeMin=0., double rangeMax=1.);
            KTCountHistogram(const KTCountHistogram& orig);
            virtual ~KTCountHistogram();

            KTCountHistogram& operator=(const KTCountHistogram& rhs);

            void Increment(unsigned bin, int delta = 1);
            void Increment(double value, int delta = 1);

    };

} /* namespace Katydid */
#endif /* KTCOUNTHISTOGRAM_HH_ */
