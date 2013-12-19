/*
 * KTPhysicalArrayFFTW.hh
 *
 *  Created on: Oct 11, 2012
 *      Author: nsoblath
 */

#ifndef KTPHYSICALARRAYFFTW_HH_
#define KTPHYSICALARRAYFFTW_HH_

#include "KTPhysicalArray.hh"

#ifdef FFTW_FOUND
#include <fftw3.h>
#else
#include "FFTWStandIn.hh"
#endif

namespace Katydid
{

    //*************************
    // 1-D array implementation; specialization for fftw_complex
    //*************************

    template <>
    class KTPhysicalArray< 1, fftw_complex > : public KTAxisProperties< 1 >
    {
        public:
            typedef fftw_complex value_type;
            typedef fftw_complex* array_type;
            typedef const fftw_complex* const_iterator;
            typedef fftw_complex* iterator;
            typedef const fftw_complex* const_reverse_iterator;
            typedef fftw_complex* reverse_iterator;

        private:
            typedef KTNBinsInArray< 1, FixedSize > XNBinsFunctor;

        public:
            KTPhysicalArray();
            explicit KTPhysicalArray(size_t nBins, double rangeMin=0., double rangeMax=1.);
            KTPhysicalArray(const KTPhysicalArray< 1, fftw_complex >& orig);
            virtual ~KTPhysicalArray();

        public:
            const array_type& GetData() const;
            array_type& GetData();

        protected:
            array_type fData;
            fftw_complex fTempCache;

        public:
            const fftw_complex& operator()(unsigned i) const;
            fftw_complex& operator()(unsigned i);

        public:
            bool IsCompatibleWith(const KTPhysicalArray< 1, fftw_complex >& rhs) const;

            KTPhysicalArray< 1, fftw_complex >& operator=(const KTPhysicalArray< 1, fftw_complex >& rhs);

            KTPhysicalArray< 1, fftw_complex >& operator+=(const KTPhysicalArray< 1, fftw_complex >& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator-=(const KTPhysicalArray< 1, fftw_complex >& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator*=(const KTPhysicalArray< 1, fftw_complex >& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator/=(const KTPhysicalArray< 1, fftw_complex >& rhs);

            KTPhysicalArray< 1, fftw_complex >& operator+=(const fftw_complex& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator-=(const fftw_complex& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator*=(const fftw_complex& rhs);
            KTPhysicalArray< 1, fftw_complex >& operator/=(const fftw_complex& rhs);

            KTPhysicalArray< 1, fftw_complex >& operator*=(double rhs);

        public:
            const_iterator begin() const;
            const_iterator end() const;
            iterator begin();
            iterator end();

            const_reverse_iterator rbegin() const;
            const_reverse_iterator rend() const;
            reverse_iterator rbegin();
            reverse_iterator rend();

    };


    //****************************************
    // Operator definitions for fftw_complex
    //****************************************

    std::ostream& operator<< (std::ostream& ostr, const fftw_complex& rhs);



} /* namespace Katydid */
#endif /* KTPHYSICALARRAYFFTW_HH_ */
