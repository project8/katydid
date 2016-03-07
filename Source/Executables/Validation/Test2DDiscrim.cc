/*
 * Test2DDiscrim.cc
 *
 *  Created on: Mar 7, 2016
 *      Author: ezayas 
 */

#include "KTSpectrumCollectionData.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTProcessedTrackData.hh"
#include "KTLogger.hh"
#include "KTVariableSpectrumDiscriminator.hh"

#include <vector>

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TRandom3.h"
#endif

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestSpectrogramCollector");

bool lineIntersects( double x1, double y1, double x2, double y2, double xx1, double yy1, double xx2, double yy2 )
{
	if( x2 <=  xx1 || x1 >= xx2 )
		return false;

	double m = (y2 - y1) / (x2 - x1);
	double yLeft = y1 + (y2 - y1) * (xx1 - x1) / (x2 - x1);
	double yRight = y1 + (y2 - y1) * (xx2 - x1) / (x2 - x1);

	if( m > 0 )
	{
		if( yLeft >= yy2 || yRight <= yy1 )
			return false;
		else
			return true;
	}
	else
	{
		if( yLeft <= yy1 || yRight >= yy2 )
			return false;
		else
			return true;
	}
}

int main()
{
	double t_bin = 10e-6;
	KTPSCollectionData psColl;
	std::vector< KTProcessedTrackData > trackArray;
	KTPowerSpectrum ps;

	KTProcessedTrackData tr1 = new KTProcessedTrackData();
	tr1.SetComponent( 1 );
	tr1.SetStartTimeInRunC( 5e-5 );
	tr1.SetEndTimeInRunC( 15e-5 );
	tr1.SetStartFrequency( 80e6 );
	tr1.SetEndFrequency( 85e6 );
	trackArray.push_back( tr1 );

	KTProcessedTrackData tr2 = new KTProcessedTrackData();
	tr2.SetComponent( 1 );
	tr2.SetStartTimeInRunC( 20e-5 );
	tr2.SetEndTimeInRunC( 45e-5 );
	tr2.SetStartFrequency( 60e6 );
	tr2.SetEndFrequency( 65e6 );
	trackArray.push_back( tr2 );
	
	KTProcessedTrackData tr3 = new KTProcessedTrackData();
	tr3.SetComponent( 1 );
	tr3.SetStartTimeInRunC( 45e-5 );
	tr3.SetEndTimeInRunC( 55e-5 );
	tr3.SetStartFrequency( 90e6 );
	tr3.SetEndFrequency( 91e6 );
	trackArray.push_back( tr3 );
	
	KTProcessedTrackData tr4 = new KTProcessedTrackData();
	tr4.SetComponent( 1 );
	tr4.SetStartTimeInRunC( 50e-5 );
	tr4.SetEndTimeInRunC( 85e-5 );
	tr4.SetStartFrequency( 125e6 );
	tr4.SetEndFrequency( 135e6 );
	trackArray.push_back( tr4 );
	
	KTProcessedTrackData tr5 = new KTProcessedTrackData();
	tr5.SetComponent( 1 );
	tr5.SetStartTimeInRunC( 80e-5 );
	tr5.SetEndTimeInRunC( 90e-5 );
	tr5.SetStartFrequency( 105e6 );
	tr5.SetEndFrequency( 106e6 );
	trackArray.push_back( tr5 );

	psColl.SetStartTime( 0 );
	psColl.SetEndTime( t_bin * 100 );
	psColl.SetDeltaT( t_bin );

	for( double t = 0; t < t_bin * 100; t += t_bin )
	{
		ps = new KTPowerSpectrum( 100, 50e6, 150e6 );

		// Fill Spectrum
		KTINFO(testlog, "Creating the power spectra");
	    for (unsigned iBin=0; iBin<100; iBin++)
	    {
	#ifdef ROOT_FOUND
	        ps(iBin).set_polar(rand.Gaus(1e-12, 0.2e-12), 0.);
	#else
	        ps(iBin).set_polar(1e-12, 0.);
	#endif

	        if( lineIntersects( tr1.GetStartTimeInRunC(), tr1.GetStartFrequency(), tr1.GetEndTimeInRunC(), tr1.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) ||
	        	lineIntersects( tr2.GetStartTimeInRunC(), tr2.GetStartFrequency(), tr2.GetEndTimeInRunC(), tr2.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) ||
	        	lineIntersects( tr3.GetStartTimeInRunC(), tr3.GetStartFrequency(), tr3.GetEndTimeInRunC(), tr3.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) ||
	        	lineIntersects( tr4.GetStartTimeInRunC(), tr4.GetStartFrequency(), tr4.GetEndTimeInRunC(), tr4.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) ||
	        	lineIntersects( tr5.GetStartTimeInRunC(), tr5.GetStartFrequency(), tr5.GetEndTimeInRunC(), tr5.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) )
	        {
		#ifdef ROOT_FOUND
		        ps(iBin).set_polar(rand.Gaus(1e-11, 0.2e-11), 0.);
		#else
		        ps(iBin).set_polar(1e-11, 0.);
		#endif
	        }
	    }

	    psColl.AddSpectrum( t, ps );

	}
	
	KTVariableSpectrumDiscriminator discrim;
	
}