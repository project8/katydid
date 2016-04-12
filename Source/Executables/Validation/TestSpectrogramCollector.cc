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
#include "KTData.hh"

#include <vector>
#include <set>
#include <map>

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
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
	typedef std::map< double, KTPowerSpectrum* > collection;
	TRandom3 r;

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
	        ps(iBin).set_polar(r.Gaus(1e-12, 0.2e-12), 0.);
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
		        ps(iBin).set_polar(r.Gaus(1e-11, 0.2e-11), 0.);
		#else
		        ps(iBin).set_polar(1e-11, 0.);
		#endif
	        }
	    }
	
		psData.SetSpectrum( ps );
		psArray.push_back( psData );
	}

	KTSpectrogramCollector spec;
	spec.SetMinFrequency( 50e6 );
	spec.SetMaxFrequency( 150e6 );
	spec.SetLeadTime( 20e-6 );
	spec.SetTrailTime( 20e-6 );

	for( int i = 0; i < 5; i++ )
		spec.ReceiveTrack( trackArray[i] );

	for( int i = 0; i < 100; i++ )
		spec.ReceiveSpectrum( psArray[i] );

	vector< KTPSCollectionData > result;
	vector< TH2D* > plots;

	for( std::set< std::pair< KTDataPtr, KTPSCollectionData* >, KTTrackCompare >::const_iterator it = spec.fWaterfallSets[component].begin(); it != spec.fWaterfallSets[component].end(); ++it )
  		result.push_back( it->first->Of< KTPSCollectionData > );

	for( int i = 0; i < result.size(); i++ )
	{
		TH2D* plot = new TH2D( "Spectrogram Collection Plot", "Spectrogram Collection Plot", 100, 0, t_bin * 100, 100, 50e6, 150e6 );
		for (collection::const_iterator it = result[i].fSpectra.begin(); it != result[i].fSpectra.end(); ++it)
        {
        	for( int j = 0; j < it->second.GetNFrequencyBins(), j++)
        	{
        		plot->Fill( it->first, j * 1e6 + 50e6, it->second[j] )
            }
        }
        plots.push_back( plot );
	}
}