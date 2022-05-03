/*
 * KTCooleyTukey.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTCooleyTukey.hh"

#include "KTLogger.hh"
#include "KTChirpSpaceDataFFT.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTChirpSpaceDataFFT.hh"
#include "KTTimeSeriesFFTW.hh"

#include <sstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::stringstream;

namespace Katydid
{
    KTLOGGER(fslog, "KTCooleyTukey");

    KTCooleyTukey::KTCooleyTukey()
    {
    }


    void CT_FFT(const KTTimeSeriesFFTW* tsIn, KTChirpSpaceFFT* fsOut)
    {
	
    }

    void KTCooleyTukey::CT_FFT(const KTTimeSeriesFFTW* tsIn, KTChirpSpaceFFT* fsOut, double alpha)
    {

                KTTimeSeriesFFTW* tsIn_even = new KTTimeSeriesFFTW( tsIn->GetNTimeBins()/2, tsIn->GetRangeMin(), tsIn->GetRangeMax());
                KTTimeSeriesFFTW* tsIn_odd= new KTTimeSeriesFFTW( tsIn->GetNTimeBins()/2, tsIn->GetRangeMin(), tsIn->GetRangeMax());


                KTChirpSpaceFFT* fsOut_even = new KTChirpSpaceFFT( 1, 0., 1., nInterceptBins/2, fsOut->GetRangeMin(2), fsOut->GetRangeMax(2), false);
                KTChirpSpaceFFT* fsOut_odd = new KTChirpSpaceFFT( 1, 0., 1., nInterceptBins/2, fsOut->GetRangeMin(2), fsOut->GetRangeMax(2), false);

	if(fIsInterceptEven and (tsIn->GetNTimeBins() != 2))
	{
		
		UnzipEvenTimeSeries(tsIn,tsIn_even,tsIn_odd);

		CT_FFT(tsIn_even,fsOut_even,alpha);
		CT_FFT(tsIn_odd,fsOut_odd,alpha);

		ZipResult(fsOut_even,fsOut_odd,fsOut, alpha);		

	}

	else
	{
		if( ! nTimeBins==2 ) 
		{
			KTDEBUG(fslog, "Must use Time Series with N = power of 2 for this algorithm, will likely end in error otherwise");
		}


                CT_Even_transform(tsIn_even, fsOut_even, alpha);
                CT_Odd_transform(tsIn_odd, fsOut_odd, alpha);

		fsOut->SetRect(0,0,tsIn->GetReal(0),tsIn->GetImag(0));	
	}


    }    

    bool KTCooleyTukey::Configure(const KTTimeSeriesFFTW* tsIn, KTChirpSpaceFFT* fsOut)
    {
            this->nTimeBins = tsIn->GetNTimeBins();
            this->nSlopeBins = fsOut->GetNSlopeBins();
            this->nInterceptBins = fsOut->GetNInterceptBins();
            this->fIsInterceptEven = nInterceptBins%2 == 0 ? true : false;

	    if(! nTimeBins==nInterceptBins)
	    {
		KTDEBUG(fslog, "nInterceptBins  and nTimeBins don't match, so FFT may fail");
	    }

            return true;
    }

    void KTCooleyTukey::UnzipEvenTimeSeries(const KTTimeSeriesFFTW* tsIn, KTTimeSeriesFFTW* tsOutEven, KTTimeSeriesFFTW* tsOutOdd)
    {
	unsigned N = tsIn->GetNTimeBins();
	for(int i=0; i<tsIn->GetNTimeBins()/2; i++)
    	{
		tsOutEven->SetRect(i, tsIn->GetReal(2*i), tsIn->GetImag(2*i));
                tsOutOdd->SetRect(i, tsIn->GetReal(2*i+1), tsIn->GetImag(2*i+1));
	}
    }

    void KTCooleyTukey::CT_Even_transform(const KTTimeSeriesFFTW* tsIn,  KTChirpSpaceFFT* fsOut, double alpha)
    {
	unsigned N = tsIn->GetNTimeBins();
	std::complex<double> I(0,1);
        double PI = 3.14159;
	std::complex<double> t(0.,0.);
	for( int i=0; i<N; i++) //indexing over time bins
	{
		t = {tsIn->GetReal(i),tsIn->GetImag(i)};
		for( int b_i=0; b_i<N; b_i++) //indexing over intercept bins
		{
			fsOut->SetRect(0,i,(t*exp(-2.*PI*I/(1.*N)*(2*i*alpha + b_i)*(1. * i))).real(), (t*exp(-2.*PI*I/(1.*N)*(2*i*alpha + b_i)*(1. * i))).imag() );
		}
	
	}
    }

    void KTCooleyTukey::CT_Odd_transform(const KTTimeSeriesFFTW* tsIn,  KTChirpSpaceFFT* fsOut, double alpha)
    {
        unsigned N = tsIn->GetNTimeBins();
        std::complex<double> I(0,1);
        double PI = 3.14159;
        std::complex<double> t(0.,0.);
        for( int i=0; i<N; i++) //indexing over time bins
        {
                t = {tsIn->GetReal(i),tsIn->GetImag(i)};
                for( int b_i=0; b_i<N; b_i++) //indexing over intercept bins
                {
                        fsOut->SetRect(0,i,(t*exp(-2.*PI*I/(1.*N)*(2*i*alpha + alpha + b_i)*(1. * i))).real(), (t*exp(-2.*PI*I/(1.*N)*(2*i*alpha + alpha + b_i)*(1. * i))).imag() );
                }

        }
    }

    void KTCooleyTukey::ZipResult(const KTChirpSpaceFFT* fsOut_even, const KTChirpSpaceFFT*fsOut_odd, KTChirpSpaceFFT* fsOut, double alpha)
    {
	unsigned N = fsOut->GetNSlopeBins();
	std::complex<double> I(0,1);
	double PI = 3.14159;
	std::complex<double> EvenComp(0.,0.);
	std::complex<double> OddComp(0.,0.);
	std::complex<double> Twiddle = exp(-2.*PI*I/(1.* N)*(alpha)); 
	for(int i=0; i<N/2; i++)
	{
		EvenComp = {fsOut_even->GetReal(0,i), fsOut_even->GetImag(0,i)};

		OddComp = {fsOut_odd->GetReal(0,i), fsOut_odd->GetImag(0,i)};


		fsOut->SetRect(0, i, (EvenComp + Twiddle*exp(-2.*PI*I/(1. * N)*(1. * i))*OddComp).real() , (EvenComp + Twiddle*exp(-2.*PI*I/(1. * N)*(1. * i))*OddComp).imag());
		fsOut->SetRect(0, i+N/2, (EvenComp - Twiddle*exp(-2.*PI*I/(1. * N)*(1. * i))*OddComp).real() , (EvenComp - Twiddle*exp(-2.*PI*I/(1. * N)*(1. * i))*OddComp).imag());
	}
    }


} /* namespace Katydid */
