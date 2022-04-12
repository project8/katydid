/*
 * KTChirpDomainArray.hh
 *
 *  Created on: Apr 5, 2022
 *      Author: jkgaison
 */

#ifndef KTCHIRPDOMAINARRAY_HH_
#define KTCHIRPDOMAINARRAY_HH_

#include <string>
#include <sys/types.h>  // for size_t

namespace Katydid
{
    template< size_t NDims >
    class KTAxisProperties;

    template<>
    class KTAxisProperties< 1 >;

    class KTChirpDomainArray
    {
        public:
            KTChirpDomainArray();
            virtual ~KTChirpDomainArray();

            /// Get the size of the array using the KTChirpSpace interface
            virtual unsigned GetNInterceptBins() const = 0;
	    virtual unsigned GetNSlopeBins() const = 0;

            /// Get the bin width of the array using the KTChirpSpace interface
            virtual double GetInterceptBinWidth() const = 0;
            virtual double GetSlopeBinWidth() const = 0;


            /// Access the axis information
   //         virtual const KTAxisProperties< 1 >& GetAxis() const = 0;
            /// Access and use the axis information
   //         virtual KTAxisProperties< 1 >& GetAxis() = 0;

            /// Access the label for the data in the array
            virtual const std::string& GetOrdinateLabel() const = 0;
    };

    class KTChirpDomainArrayData
    {
        public:
            KTChirpDomainArrayData();
            virtual ~KTChirpDomainArrayData();

            virtual const KTChirpDomainArray* GetArray(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) const = 0;
            virtual KTChirpDomainArray* GetArray(unsigned SlopeComponent = 0, unsigned InterceptComponent = 0) = 0;

    };


} /* namespace Katydid */
#endif /* KTCHIRPDOMAINARRAY_HH_ */
