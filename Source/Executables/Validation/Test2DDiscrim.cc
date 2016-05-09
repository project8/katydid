/*
 * Test2DDiscrim.cc
 *
 *  Created on: Mar 7, 2016
 *      Author: ezayas 
 */

#include "KTSpectrumCollectionData.hh"
#include "KTSpectrogramCollector.hh"
#include "KTSliceHeader.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTProcessedTrackData.hh"
#include "KTLogger.hh"
#include "KTVariableSpectrumDiscriminator.hh"
#include "KTGainVariationData.hh"
#include "KTSpline.hh"
#include "KTDiscriminatedPoints2DData.hh"
#include "KT2ROOT.hh"

#include <vector>

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TFile.h"
#include "TRandom3.h"
#endif

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestSpectrogramCollector");

// Method to determine whether a line intersects a bin with finite size
// The line extends over a finite segment from (x1,y1) to (x2,y2)
// The bin has lower-left corner (xx1,yy1) and upper-right corner (xx2,yy2)
// It follows that the lower-right corner is (xx2,yy1) and the upper-left corner is (xx1,yy2)
bool lineIntersects( double x1, double y1, double x2, double y2, double xx1, double yy1, double xx2, double yy2 )
{
	// If the x or y coordinates of the line are entirely outside the bin range, we can immediately return false
	if( x2 <= xx1 || x1 >= xx2 )
		return false;
	if( y2 <= yy1 || y1 >= yy2 )
		return false;

	double m = (y2 - y1) / (x2 - x1);		// slope
	double yLeft = y1 + m * (xx1 - x1);		// y-position of the line at xx1
	double yRight = y1 + m * (xx2 - x1);	// y-position of the line at xx2

	if( m > 0 )
	{
		// Positive slope
		// We check the upper-left corner and the lower-right corner
		if( yLeft >= yy2 || yRight <= yy1 )
			return false;
		else
			return true;
	}
	else
	{
		// Negative slope
		// We check the lower-left corner and the upper-right corner
		if( yLeft <= yy1 || yRight >= yy2 )
			return false;
		else
			return true;
	}
}

int main()
{
	double t_bin = 10e-6;	// Length of a time bin

	// Declare all required data objects
	std::vector< KTPowerSpectrumData* > psArray;
	std::vector< KTSliceHeader* > sArray;
	KTPowerSpectrum* ps;
	KTPowerSpectrumData* psData;
	KTFrequencySpectrumPolar* fftw;
	KTSliceHeader* s;

#ifdef ROOT_FOUND
	TRandom3 rand(0);
#endif

	// Next we create 5 tracks with specific start and end points
	// Some tracks overlap in the time domain

	KTProcessedTrackData tr1;
	tr1.SetComponent( 0 );
	tr1.SetStartTimeInRunC( 25e-5 );
	tr1.SetEndTimeInRunC( 80e-5 );
	tr1.SetStartFrequency( 100e6 );
	tr1.SetEndFrequency( 118e6 );

	KTGainVariationData gv;

	double* xVals = new double[3];
	double* yVals = new double[3];

	xVals[0] = 50e6;
	xVals[1] = 100e6;
	xVals[2] = 150e6;
	yVals[0] = 1e-12;
	yVals[1] = 2e-12;
	yVals[2] = 1.5e-12;

	KTSpline* sp = new KTSpline( xVals, yVals, 3 );
	gv.SetSpline( sp );

	// Create new power spectra to send to the discriminator
	KTINFO(testlog, "Creating the power spectra");

	double bkgd;
	for( double t = 0; t < t_bin * 100; t += t_bin )
	{
		// Create new frequency spectrum
		fftw = new KTFrequencySpectrumPolar( 100, 50e6, 150e6 );

		// Fill Spectrum
	    for (unsigned iBin=0; iBin<100; iBin++)
	    {
	    	bkgd = pow( 50 * sp->Evaluate( iBin * 1e6 + 50e6 ), 0.5 );

	#ifdef ROOT_FOUND
	        (*fftw)(iBin).set_polar(rand.Gaus(bkgd, 0.2*bkgd), 0.);
	#else
	        (*fftw)(iBin).set_polar(bkgd, 0.);
	#endif

	        // If any track intersects the bin, fill by a factor of 3 greater magnitude
	        if( lineIntersects( tr1.GetStartTimeInRunC(), tr1.GetStartFrequency(), tr1.GetEndTimeInRunC(), tr1.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) )
	        {
		#ifdef ROOT_FOUND
		        (*fftw)(iBin).set_polar(rand.Gaus(1.6*bkgd, 0.2*bkgd), 0.);
		#else
		        (*fftw)(iBin).set_polar(1.6*bkgd, 0.);
		#endif
	        }
	    }
	
		fftw->SetNTimeBins( 1 );	// Default is zero, causing a divergence with the scaling

		psData = new KTPowerSpectrumData();
		psData->SetNComponents( 1 );
		s = new KTSliceHeader();
		
		// Send a log message just the first time
		if( t == 0 )
			KTINFO(testlog, "Finished filling first spectrum. 99 to go");

		// Convert to Power Spectrum
	    ps = fftw->CreatePowerSpectrum();
	    ps->ConvertToPowerSpectrum();
	    psData->SetSpectrum( ps, 0 );
	    s->SetTimeInRun( t );
		s->SetSliceLength( t_bin );

		// Add to vectors
		psArray.push_back( psData );
		sArray.push_back( s );

		// Log
		if( t == 0 )
			KTINFO(testlog, "Finished processing first spectrum. 99 to go");
	}

	// Now we create and configure the spectrogram collector
	KTSpectrogramCollector spec;
	spec.SetMinFrequency( 50e6 );
	spec.SetMaxFrequency( 150e6 );
	spec.SetLeadTime( 50e-6 );
	spec.SetTrailTime( 50e-6 );

	// Add tracks to listen
	KTINFO(testlog, "Adding track to the spectrogram collector");
	if( !spec.ReceiveTrack( tr1 ) )
		KTERROR(testlog, "Something went wrong adding the track");

	// Add spectra
	KTINFO(testlog, "Adding spectra to the spectrogram collector");
	for( int i = 0; i < 100; i++ )
	{
		if( !spec.ReceiveSpectrum( *psArray[i], *sArray[i] ) )
			KTERROR(testlog, "Something went wrong adding spectrum" << i);
	}

	// The result is a KTPSCollectionData
	KTPSCollectionData* psColl;
	psColl = spec.fWaterfallSets[0].begin()->second;
  	
  	KTINFO(testlog, "Retrieved spectrogram. Delta T = " << spec.fWaterfallSets[0].begin()->second->GetDeltaT());
  	KTINFO(testlog, "Retrieved spectrogram. Delta T = " << psColl->GetDeltaT());

	KTVariableSpectrumDiscriminator discrim;
	discrim.SetSNRPowerThreshold( 1.5 );
	discrim.SetMinFrequency( 55e6 );
	discrim.SetMaxFrequency( 145e6 );

	KTINFO(testlog, "Discriminating points");

	if( !discrim.Discriminate( *psColl, gv ) )
		KTERROR(testlog, "Something went wrong discriminating points");

	KTINFO(testlog, "Finished discriminating");

	KTDiscriminatedPoints2DData& result = psColl->Of< KTDiscriminatedPoints2DData >();

	vector< double > xx;
	vector< double > yy;
	int n = 0;
	
	for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = result.GetSetOfPoints(0).begin(); it != result.GetSetOfPoints(0).end(); ++it )
  	{
  		xx.push_back( it->second.fAbscissa );
  		yy.push_back( it->second.fOrdinate );
  		n++;
  	}

  	double* xArray = &xx[0];
  	double* yArray = &yy[0];

  	KTINFO(testlog, "Writing to file");

#ifdef ROOT_FOUND
  	TFile* file = new TFile( "2d-discrim-test.root", "recreate" );
  	TGraph* plot;
  	plot = new TGraph( n, xArray, yArray );
  	file->Append( plot );
  	plot->Write( "thresholded-points" );
  	file->Close();
#endif

  	KTINFO(testlog, "File written successfully. Exiting");

  	return 0;
}