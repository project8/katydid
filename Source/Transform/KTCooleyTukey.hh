/**
 @file KTCooleyTukey.hh
 @brief Contains KTCooleyTukey
 @details 
 @author: J. K. Gaison
 @date: Apr 28, 2022
 */

#ifndef KTCOOLEYTUKEY_HH_
#define KTCOOLEYTUKEY_HH_

#include "KTChirpSpace.hh"
#include "KTPhysicalArrayComplex.hh"


#include <cmath>
#include <string>

namespace Katydid
{    

    class KTChirpSpaceFFT;
    class KTTimeSeriesFFTW;

    class KTCooleyTukey  
    {
        public:
            KTCooleyTukey();

            virtual ~KTCooleyTukey() = default;
	    unsigned nTimeBins;
	    unsigned nSlopeBins;
	    unsigned nInterceptBins;

	    bool Configure(const KTTimeSeriesFFTW* tsIn, KTChirpSpaceFFT* fsOut);

	    void UnzipEvenTimeSeries(const KTTimeSeriesFFTW* tsIn, KTTimeSeriesFFTW* tsOutEven, KTTimeSeriesFFTW* tsOutOdd);

	    void CT_FFT(const KTTimeSeriesFFTW* tsIn, KTChirpSpaceFFT* fsOut, double slope);

	    void CT_Even_transform(const KTTimeSeriesFFTW* tsIn,  KTChirpSpaceFFT* fsOut, double alpha);
	    
	    void CT_Odd_transform(const KTTimeSeriesFFTW* tsIn,  KTChirpSpaceFFT* fsOut, double alpha);

	    void ZipResult(const KTChirpSpaceFFT* fsOut_even, const KTChirpSpaceFFT*fsOut_odd, KTChirpSpaceFFT* fsOut, double alpha);

        protected:
            bool fIsInterceptEven; /// Flag to indicate if the size of the array is even
    };


} /* namespace Katydid */
#endif /* KTCOOLEYTUKEY_HH_ */
