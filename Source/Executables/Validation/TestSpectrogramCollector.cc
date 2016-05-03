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
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTLogger.hh"
#include "KTData.hh"
#include "KT2ROOT.hh"

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
#ifdef ROOT_FOUND
	TRandom3 rand(0);
#endif

	struct KTTrackCompare
    {
        bool operator() (const std::pair< KTDataPtr, KTPSCollectionData* > lhs, const std::pair< KTDataPtr, KTPSCollectionData* > rhs) const
        {
            return lhs.second->GetStartTime() < rhs.second->GetStartTime();
        }
    };

	double t_bin = 10e-6;
	std::vector< KTPowerSpectrumData* > psArray;
	std::vector< KTSliceHeader* > sArray;
	std::vector< KTProcessedTrackData > trackArray;
	KTPowerSpectrum* ps;
	KTPowerSpectrumData* psData;
	KTFrequencySpectrumPolar* fftw;
	KTSliceHeader* s;

	//psData->SetNComponents( 1 );

	KTProcessedTrackData tr1;
	tr1.SetComponent( 0 );
	tr1.SetStartTimeInRunC( 5e-5 );
	tr1.SetEndTimeInRunC( 15e-5 );
	tr1.SetStartFrequency( 80e6 );
	tr1.SetEndFrequency( 85e6 );
	trackArray.push_back( tr1 );

	KTProcessedTrackData tr2;
	tr2.SetComponent( 0 );
	tr2.SetStartTimeInRunC( 20e-5 );
	tr2.SetEndTimeInRunC( 45e-5 );
	tr2.SetStartFrequency( 60e6 );
	tr2.SetEndFrequency( 65e6 );
	trackArray.push_back( tr2 );
	
	KTProcessedTrackData tr3;
	tr3.SetComponent( 0 );
	tr3.SetStartTimeInRunC( 45e-5 );
	tr3.SetEndTimeInRunC( 55e-5 );
	tr3.SetStartFrequency( 90e6 );
	tr3.SetEndFrequency( 91e6 );
	trackArray.push_back( tr3 );
	
	KTProcessedTrackData tr4;
	tr4.SetComponent( 0 );
	tr4.SetStartTimeInRunC( 50e-5 );
	tr4.SetEndTimeInRunC( 85e-5 );
	tr4.SetStartFrequency( 125e6 );
	tr4.SetEndFrequency( 135e6 );
	trackArray.push_back( tr4 );
	
	KTProcessedTrackData tr5;
	tr5.SetComponent( 0 );
	tr5.SetStartTimeInRunC( 80e-5 );
	tr5.SetEndTimeInRunC( 90e-5 );
	tr5.SetStartFrequency( 105e6 );
	tr5.SetEndFrequency( 106e6 );
	trackArray.push_back( tr5 );

	KTINFO(testlog, "Creating Power Spectra");
	for( double t = 0; t < t_bin * 100; t += t_bin )
	{
		fftw = new KTFrequencySpectrumPolar( 100, 50e6, 150e6 );

		// Fill Spectrum
	    for (unsigned iBin=0; iBin<100; iBin++)
	    {
	#ifdef ROOT_FOUND
	        (*fftw)(iBin).set_polar(rand.Gaus(1e-5, 0.2e-5), 0.);
	#else
	        (*fftw)(iBin).set_polar(1e-5, 0.);
	#endif

	        if( lineIntersects( tr1.GetStartTimeInRunC(), tr1.GetStartFrequency(), tr1.GetEndTimeInRunC(), tr1.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) ||
	        	lineIntersects( tr2.GetStartTimeInRunC(), tr2.GetStartFrequency(), tr2.GetEndTimeInRunC(), tr2.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) ||
	        	lineIntersects( tr3.GetStartTimeInRunC(), tr3.GetStartFrequency(), tr3.GetEndTimeInRunC(), tr3.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) ||
	        	lineIntersects( tr4.GetStartTimeInRunC(), tr4.GetStartFrequency(), tr4.GetEndTimeInRunC(), tr4.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) ||
	        	lineIntersects( tr5.GetStartTimeInRunC(), tr5.GetStartFrequency(), tr5.GetEndTimeInRunC(), tr5.GetEndFrequency(), t, iBin * 1e6 + 50e6, t + t_bin, (iBin + 1) * 1e6 + 50e6 ) )
	        {
		#ifdef ROOT_FOUND
		        (*fftw)(iBin).set_polar(rand.Gaus(3e-5, 0.6e-5), 0.);
		#else
		        (*fftw)(iBin).set_polar(3e-5, 0.);
		#endif
	        }

	        //KTINFO(testlog, "Set bin " << iBin << " to " << (*fftw)(iBin));
	    }
	
		fftw->SetNTimeBins( 1 );
		psData = new KTPowerSpectrumData();
		psData->SetNComponents( 1 );
		s = new KTSliceHeader();

		if( t == 0 )
			KTINFO(testlog, "Finished filling first spectrum. 99 to go");
	    ps = fftw->CreatePowerSpectrum();
	    ps->ConvertToPowerSpectrum();
	    psData->SetSpectrum( ps, 0 );
		s->SetTimeInRun( t );
		s->SetSliceLength( t_bin );

		psArray.push_back( psData );
		sArray.push_back( s );
		if( t == 0 )
			KTINFO(testlog, "Finished processing first spectrum. 99 to go");
	}

	KTSpectrogramCollector spec;
	spec.SetMinFrequency( 50e6 );
	spec.SetMaxFrequency( 150e6 );
	spec.SetLeadTime( 20e-6 );
	spec.SetTrailTime( 20e-6 );

	KTINFO(testlog, "Adding tracks to the spectrogram collector");
	for( int i = 0; i < 5; i++ )
		if( !spec.ReceiveTrack( trackArray[i] ) )
			KTERROR(testlog, "Something went wrong adding track" << i);

	KTINFO(testlog, "Adding spectra to the spectrogram collector");
	for( int i = 0; i < 100; i++ )
	{
		if( !spec.ReceiveSpectrum( *psArray[i], *sArray[i] ) )
			KTERROR(testlog, "Something went wrong adding spectrum" << i);
	}

	std::vector< KTPSCollectionData > result;

	KTINFO(testlog, "Finished receiving spectra. Begin retrieving produced spectrograms");

	int nPlots = 0;
	for( std::set< std::pair< KTDataPtr, KTPSCollectionData* >, KTTrackCompare >::const_iterator it = spec.fWaterfallSets[0].begin(); it != spec.fWaterfallSets[0].end(); ++it )
	{
  		result.push_back( it->first->Of< KTPSCollectionData >() );
  		nPlots++;
  	}

  	if( nPlots == 0 )
  	{
  		KTERROR(testlog, "No spectrograms were produced!");
  	}
  	else
  	{
  		KTINFO(testlog, "Produced " << nPlots << " spectrograms");
  	}

#ifdef ROOT_FOUND
  	TFile* file = new TFile( "spectrogram-collector-test.root", "recreate" );
  	std::vector< TH2D* > plots;

  	TH2D* plot = new TH2D( "Spectrogram Collection Plot", "Spectrogram Collection Plot", 100, 0, t_bin * 100, 100, 50e6, 150e6 );
	for( int i = 0; i < nPlots; i++ )
	{
		plot->Reset();
		for (collection::const_iterator it = result[i].GetSpectra().begin(); it != result[i].GetSpectra().end(); ++it)
        {
        	for( int j = 0; j < it->second->GetNFrequencyBins(); j++)
        	{
        		plot->Fill( it->first, j * 1e6 + 50e6, (*it->second)(j) );
            }
        }
        //plot->SetDirectory( file );
        KTINFO(testlog, "Writing spectrogram for track " << i);
        plot->Write(TString::Format("track-%i", i));
        //plots.push_back( plot );
	}
	file->Close();
	delete file;
#endif

	KTINFO(testlog, "Finished; exiting script");
	return 0;
}