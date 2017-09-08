#include "KTConvolution.hh"
#include "KTPowerSpectrum.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KT2ROOT.hh"
#include "KTLogger.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TFile.h"
#endif

#include <cmath>
#include <iostream>

using namespace Katydid;

KTLOGGER(vallog, "TestConvolution1D");

int main()
{
	unsigned nBins = 65536;
	double rangeMin = 50.0e6;
	double rangeMax = 150.0e6;

	double pulseStart = 90.0e6;
	double pulseEnd = 110.0e6;

	unsigned nKernelBins = 1000;
	double blurWidth = 200.0e3;

	// You'll need to change this to your own path
	std::string kernelPath = "/Users/ezayas/Katydid/Examples/CustomApplications/GaussianKernel.json";

	KTPowerSpectrum* powerSpect = new KTPowerSpectrum( nBins, rangeMin, rangeMax );
	KTFrequencySpectrumFFTW* fftwSpect = new KTFrequencySpectrumFFTW( nBins, rangeMin, rangeMax );
	KTFrequencySpectrumPolar* polarSpect = new KTFrequencySpectrumPolar( nBins, rangeMin, rangeMax );

	KTPowerSpectrum* blur = new KTPowerSpectrum( nBins, rangeMin, rangeMax );

	for( unsigned iBin = 0; iBin < nBins; ++iBin )
	{
		if( powerSpect->GetBinCenter( iBin ) >= pulseStart && powerSpect->GetBinCenter( iBin ) <= pulseEnd )
		{
			(*powerSpect)(iBin) = 1.0;
			(*fftwSpect)(iBin)[0] = 1.0;
			(*fftwSpect)(iBin)[1] = 0.0;
			polarSpect->SetRect( iBin, 1.0, 0.0 );
		}
		else
		{
			(*powerSpect)(iBin) = 0.0;
			(*fftwSpect)(iBin)[0] = 0.0;
			(*fftwSpect)(iBin)[1] = 0.0;
			polarSpect->SetRect( iBin, 0.0, 0.0 );
		}

		(*blur)(iBin) = exp( -0.5 * pow( blur->GetBinCenter( iBin ) - 0.5 * (rangeMin + rangeMax), 2 ) * pow( blurWidth, -2 ) );
	}

	KTConvolution1D convProcessor;
	
	std::string kernel = "{\n\t\"kernel\": [";
	for( unsigned iBin = nBins/2 - nKernelBins/2; iBin < nBins/2 + nKernelBins/2; ++iBin )
	{
		if( iBin != nBins/2 - nKernelBins/2 )
		{
			kernel += ", ";
		}

		kernel += std::to_string( (*blur)(iBin) );
	}
	kernel += "]\n}\n";

	//std::cout << kernel << std::endl;

	convProcessor.SetKernel( kernelPath );
	convProcessor.FinishSetup();

    int block = convProcessor.GetBlockSize();
    int overlap = nKernelBins - 1;
    int step = block - overlap;

    convProcessor.Initialize( nBins, block, step, overlap );

    KTPowerSpectrum* convolvedPowerSpect = convProcessor.DoConvolution( powerSpect, block, step, overlap );
    KTFrequencySpectrumFFTW* convolvedFFTWSpect = convProcessor.DoConvolution( fftwSpect, block, step, overlap );
    KTFrequencySpectrumPolar* convolvedPolarSpect = convProcessor.DoConvolution( polarSpect, block, step, overlap );
/*
    for( int i = 0; i < nBins; ++i )
    {
    	std::cout << (*convolvedPowerSpect)(i) << std::endl;
    }
*/
    KTINFO(vallog, "Writing to ROOT file");

    #ifdef ROOT_FOUND
	    TFile* file = new TFile("TestConvolution1D.root", "recreate");
	    TH1D* psInitial = KT2ROOT::CreatePowerHistogram( powerSpect, "hPowerSpectrum" );
	    TH1D* psFinal = KT2ROOT::CreatePowerHistogram( convolvedPowerSpect, "hConvolvedPowerSpectrum" );
	    TH1D* fsfftwInitial = KT2ROOT::CreateMagnitudeHistogram( fftwSpect, "hFFTWSpectrum" );
	    TH1D* fsfftwFinal = KT2ROOT::CreateMagnitudeHistogram( convolvedFFTWSpect, "hConvolvedFFTWSpectrum" );
	    TH1D* fsPolarInitial = KT2ROOT::CreateMagnitudeHistogram( polarSpect, "hPolarSpectrum" );
	    TH1D* fsPolarFinal = KT2ROOT::CreateMagnitudeHistogram( convolvedPolarSpect, "hConvolvedPolarSpectrum" );
	    
	    psInitial->SetDirectory(file);
	    psFinal->SetDirectory(file);
	    fsfftwInitial->SetDirectory(file);
	    fsfftwFinal->SetDirectory(file);
	    fsPolarInitial->SetDirectory(file);
	    fsPolarFinal->SetDirectory(file);
	    
	    psInitial->Write();
	    psFinal->Write();
	    fsfftwInitial->Write();
	    fsfftwFinal->Write();
	    fsPolarInitial->Write();
	    fsPolarFinal->Write();

	    file->Close();
	    delete file;
	#endif

    delete powerSpect;
    delete fftwSpect;
    delete polarSpect;
    delete blur;

    return 0;
}