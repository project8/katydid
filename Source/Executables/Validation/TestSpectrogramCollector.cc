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

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TH2.h"
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

    double m = (y2 - y1) / (x2 - x1);       // slope
    double yLeft = y1 + m * (xx1 - x1);     // y-position of the line at xx1
    double yRight = y1 + m * (xx2 - x1);    // y-position of the line at xx2

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
    KTINFO(testlog, "Hi!");
    typedef KTMultiPS collection;
#ifdef ROOT_FOUND
    TRandom3 rand(0);
#endif

    KTINFO(testlog, "Hi!");

    double t_bin = 10e-6;   // Length of a time bin

    // Declare all required data objects
    std::vector< KTPowerSpectrumData* > psArray;
    std::vector< KTSliceHeader* > sArray;
    std::vector< KTProcessedTrackData > trackArray;
    KTPowerSpectrum* ps;
    KTPowerSpectrumData* psData;
    KTFrequencySpectrumPolar* fftw;
    KTSliceHeader* s;

    // Next we create 5 tracks with specific start and end points
    // Some tracks overlap in the time domain

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

    // Create power spectra to send to the spectrogram collector
    KTINFO(testlog, "Creating Power Spectra");

    for( double t = 0; t < t_bin * 100; t += t_bin )
    {
        // Create new frequency spectrum
        fftw = new KTFrequencySpectrumPolar( 100, 50e6, 150e6 );

        // Fill Spectrum
        for (unsigned iBin=0; iBin<100; iBin++)
        {
    #ifdef ROOT_FOUND
            (*fftw)(iBin).set_polar(rand.Gaus(1e-5, 0.2e-5), 0.);
    #else
            (*fftw)(iBin).set_polar(1e-5, 0.);
    #endif

            // If any track intersects the bin, fill by a factor of 3 greater magnitude
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
        }
    
        fftw->SetNTimeBins( 1 );    // Default is zero, causing a divergence with the scaling

        // Initialize KTPowerSpectrumData object and slice header
        psData = new KTPowerSpectrumData();
        psData->SetNComponents( 1 );
        s = new KTSliceHeader();

        // Send a log message just the first time
        if( t == 0 )
            KTINFO(testlog, "Finished filling first spectrum. 99 to go");

        // Configure psData and s
        ps = fftw->CreatePowerSpectrum();
        ps->ConvertToPowerSpectrum();
        psData->SetSpectrum( ps, 0 );
        s->SetTimeInRun( t );
        s->SetTimeInAcq( t );
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
    spec.SetLeadTime( 20e-6 );
    spec.SetTrailTime( 20e-6 );

    // Add tracks to listen
    KTINFO(testlog, "Adding tracks to the spectrogram collector");
    for( int i = 0; i < 5; i++ )
        if( !spec.ReceiveTrack( trackArray[i] ) )
            KTERROR(testlog, "Something went wrong adding track" << i);

    // Add spectra
    KTINFO(testlog, "Adding spectra to the spectrogram collector");
    for( int i = 0; i < 100; i++ )
    {
        if( !spec.ReceiveSpectrum( *psArray[i], *sArray[i] ) )
            KTERROR(testlog, "Something went wrong adding spectrum" << i);
    }

    // The result is a KTPSCollectionData for each track
    std::vector< KTPSCollectionData > result;

    KTINFO(testlog, "Finished receiving spectra. Begin retrieving produced spectrograms");

    // Fill the result vector and count the number of plots
    int nPlots = 0;
    for( KTSpectrogramCollector::WaterfallSet::const_iterator it = spec.WaterfallSets()[0].begin(); it != spec.WaterfallSets()[0].end(); ++it )
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

    // Fill a TH2D for each spectrogram and write to a file

#ifdef ROOT_FOUND
    TFile* file = new TFile( "spectrogram-collector-test.root", "recreate" );
    std::vector< TH2D* > plots;

    TH2D* plot = new TH2D( "Spectrogram Collection Plot", "Spectrogram Collection Plot", 100, 0, t_bin * 100, 100, 50e6, 150e6 );
    for( int i = 0; i < nPlots; i++ )
    {
        plot->Reset();  // Clear histogram
        double timeStamp = result[i].GetStartTime();
        double timeStep = result[i].GetDeltaT();
        for (collection::const_iterator it = result[i].GetSpectra()->begin(); it != result[i].GetSpectra()->end(); ++it)
        {
            for( int j = 0; j < (*it)->GetNFrequencyBins(); j++)
            {
                plot->Fill( timeStamp, j * (*it)->GetFrequencyBinWidth() + (*it)->GetRangeMin(), (**it)(j) );
            }

            timeStamp += timeStep;
        }

        KTINFO(testlog, "Writing spectrogram for track " << i);
        plot->Write( TString::Format("track-%i", i) );
    }

    // Cleanup
    file->Close();
    delete file;
#endif

    KTINFO(testlog, "Finished; exiting script");
    return 0;
}
