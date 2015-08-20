/*
 * KTFrequencyDomainArray.hh
 *
 *  Created on: Jul 27, 2015
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYDOMAINARRAY_HH_
#define KTFREQUENCYDOMAINARRAY_HH_

#include <string>
#include <sys/types.h>  // for size_t

namespace Katydid
{
    template< size_t NDims >
    class KTAxisProperties;

    template<>
    class KTAxisProperties< 1 >;

    class KTFrequencyDomainArray
    {
        public:
            KTFrequencyDomainArray();
            virtual ~KTFrequencyDomainArray();

            /// Get the size of the array using the KTFrequencySpectrum interface
            virtual unsigned GetNFrequencyBins() const = 0;

            /// Get the bin width of the array using the KTFrequencySpectrum interface
            virtual double GetFrequencyBinWidth() const = 0;


            /// Access the axis information
            virtual const KTAxisProperties< 1 >& GetAxis() const = 0;
            /// Access and use the axis information
            virtual KTAxisProperties< 1 >& GetAxis() = 0;

            /// Access the label for the data in the array
            virtual const std::string& GetOrdinateLabel() const = 0;
    };

    class KTFrequencyDomainArrayData
    {
        public:
            KTFrequencyDomainArrayData();
            virtual ~KTFrequencyDomainArrayData();

            virtual const KTFrequencyDomainArray* GetArray(unsigned component = 0) const = 0;
            virtual KTFrequencyDomainArray* GetArray(unsigned component = 0) = 0;

    };


} /* namespace Katydid */
#endif /* KTFREQUENCYDOMAINARRAY_HH_ */
