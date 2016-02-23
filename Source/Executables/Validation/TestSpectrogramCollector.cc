/*
 * TestSpectrogramCollector.cc
 *
 *  Created on: Feb 23, 2016
 *      Author: ezayas
 */

#include "KTSpectrogramCollector.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTLogger.hh"

#include <vector>

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TRandom3.h"
#endif

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestSpectrogramCollector");

int main()
{
	double t_bin = 10e-6;
	std::vector< KTPowerSpectrumData > psArray;
	std::vector< KTProcessedTrackData > trackArray;
	KTPowerSpectrum ps;
	KTPowerSpectrumData psData;

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

	        if( (t >= tr1.GetStartTimeInRunC() && t <= tr1.GetEndTimeInRunC() && iBin * 1e6 + 50e6 >= tr1.GetStartFrequency() && iBin * 1e6 + 50e6 <= tr1.GetEndFrequency()) ||
	        	(t >= tr2.GetStartTimeInRunC() && t <= tr2.GetEndTimeInRunC() && iBin * 1e6 + 50e6 >= tr2.GetStartFrequency() && iBin * 1e6 + 50e6 <= tr2.GetEndFrequency()) ||
	        	(t >= tr3.GetStartTimeInRunC() && t <= tr3.GetEndTimeInRunC() && iBin * 1e6 + 50e6 >= tr3.GetStartFrequency() && iBin * 1e6 + 50e6 <= tr3.GetEndFrequency()) ||
	        	(t >= tr4.GetStartTimeInRunC() && t <= tr4.GetEndTimeInRunC() && iBin * 1e6 + 50e6 >= tr4.GetStartFrequency() && iBin * 1e6 + 50e6 <= tr4.GetEndFrequency()) ||
	        	(t >= tr5.GetStartTimeInRunC() && t <= tr5.GetEndTimeInRunC() && iBin * 1e6 + 50e6 >= tr5.GetStartFrequency() && iBin * 1e6 + 50e6 <= tr5.GetEndFrequency()) )
	        {
		#ifdef ROOT_FOUND
		        ps(iBin).set_polar(rand.Gaus(1e-11, 0.2e-11), 0.);
		#else
		        ps(iBin).set_polar(1e-11, 0.);
		#endif
	        }
	    }
	
		psData.SetSpectrum( ps );
		psArray.push_back( psData );
	}
	
}