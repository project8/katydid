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


unsigned nTimeBins = 100;
double timeBinWidth = 1e-6;   // Length of a time bin
unsigned nFreqBins = 100;
double freqBinWidth = 1.e6;
double freqMin = 50.e6;
double freqMax = 150.e6;



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

double BinToFreq(unsigned iBin)
{
    return (double)iBin * freqBinWidth + freqMin;
}

double BinToTime(unsigned iBin)
{
    return (double)iBin * timeBinWidth;
}

int main()
{
    KTINFO(testlog, "Hi!");
    typedef KTMultiPS collection;
#ifdef ROOT_FOUND
    TRandom3 rand(0);
#endif

    KTINFO(testlog, "Hi!");

    // Declare all required data objects
    std::vector< KTPowerSpectrumData > psArray(nTimeBins);
    std::vector< KTSliceHeader > sArray(nTimeBins);

    std::vector< KTProcessedTrackData > trackArray(5);

    // Next we create 5 tracks with specific start and end points
    // Some tracks overlap in the time domain

    trackArray[0].SetComponent( 0 );
    trackArray[0].SetStartTimeInRunC( 5e-5 );
    trackArray[0].SetEndTimeInRunC( 15e-5 );
    trackArray[0].SetStartFrequency( 80e6 );
    trackArray[0].SetEndFrequency( 85e6 );

    trackArray[1].SetComponent( 0 );
    trackArray[1].SetStartTimeInRunC( 20e-5 );
    trackArray[1].SetEndTimeInRunC( 45e-5 );
    trackArray[1].SetStartFrequency( 60e6 );
    trackArray[1].SetEndFrequency( 65e6 );
    
    trackArray[2].SetComponent( 0 );
    trackArray[2].SetStartTimeInRunC( 45e-5 );
    trackArray[2].SetEndTimeInRunC( 55e-5 );
    trackArray[2].SetStartFrequency( 90e6 );
    trackArray[2].SetEndFrequency( 91e6 );
    
    trackArray[3].SetComponent( 0 );
    trackArray[3].SetStartTimeInRunC( 50e-5 );
    trackArray[3].SetEndTimeInRunC( 85e-5 );
    trackArray[3].SetStartFrequency( 125e6 );
    trackArray[3].SetEndFrequency( 135e6 );
    
    trackArray[4].SetComponent( 0 );
    trackArray[4].SetStartTimeInRunC( 80e-5 );
    trackArray[4].SetEndTimeInRunC( 90e-5 );
    trackArray[4].SetStartFrequency( 105e6 );
    trackArray[4].SetEndFrequency( 106e6 );

    // Create power spectra to send to the spectrogram collector
    KTINFO(testlog, "Creating Power Spectra");

    for( unsigned tBin = 0; tBin < nTimeBins; ++tBin )
    {
        KTDEBUG(testlog, "Creating spectrum " << tBin);
        // Create new frequency spectrum
        KTFrequencySpectrumPolar* fftw = new KTFrequencySpectrumPolar( nFreqBins, freqMin, freqMax );

        // Fill Spectrum
        for (unsigned fBin=0; fBin<nFreqBins; fBin++)
        {
    #ifdef ROOT_FOUND
            (*fftw)(fBin).set_polar(rand.Gaus(1e-5, 0.2e-5), 0.);
    #else
            (*fftw)(fBin).set_polar(1e-5, 0.);
    #endif

            // If any track intersects the bin, fill by a factor of 3 greater magnitude
            if( lineIntersects( trackArray[0].GetStartTimeInRunC(), trackArray[0].GetStartFrequency(), trackArray[0].GetEndTimeInRunC(), trackArray[0].GetEndFrequency(), BinToTime(tBin), BinToFreq(fBin), BinToTime(tBin+1), BinToFreq(fBin + 1) ) ||
                lineIntersects( trackArray[1].GetStartTimeInRunC(), trackArray[1].GetStartFrequency(), trackArray[1].GetEndTimeInRunC(), trackArray[1].GetEndFrequency(), BinToTime(tBin), BinToFreq(fBin), BinToTime(tBin+1), BinToFreq(fBin + 1) ) ||
                lineIntersects( trackArray[2].GetStartTimeInRunC(), trackArray[2].GetStartFrequency(), trackArray[2].GetEndTimeInRunC(), trackArray[2].GetEndFrequency(), BinToTime(tBin), BinToFreq(fBin), BinToTime(tBin+1), BinToFreq(fBin + 1) ) ||
                lineIntersects( trackArray[3].GetStartTimeInRunC(), trackArray[3].GetStartFrequency(), trackArray[3].GetEndTimeInRunC(), trackArray[3].GetEndFrequency(), BinToTime(tBin), BinToFreq(fBin), BinToTime(tBin+1), BinToFreq(fBin + 1) ) ||
                lineIntersects( trackArray[4].GetStartTimeInRunC(), trackArray[4].GetStartFrequency(), trackArray[4].GetEndTimeInRunC(), trackArray[4].GetEndFrequency(), BinToTime(tBin), BinToFreq(fBin), BinToTime(tBin+1), BinToFreq(fBin + 1) ) )
            {
        #ifdef ROOT_FOUND
                (*fftw)(fBin).set_polar(rand.Gaus(3e-5, 0.6e-5), 0.);
        #else
                (*fftw)(fBin).set_polar(3e-5, 0.);
        #endif
            }
        }
    
        fftw->SetNTimeBins( 1 );    // Default is zero, causing a divergence with the scaling

        // Send a log message just the first time
        if( tBin == 0 )
        {
            KTINFO(testlog, "Finished filling first spectrum. " << nTimeBins-1 << " to go");
        }

        KTDEBUG(testlog, "Adding spectrum to ps array");
        // Initialize KTPowerSpectrumData object and slice header
        psArray[tBin].SetNComponents( 1 );

        KTDEBUG(testlog, "Creating power spectrum");
        // Configure psData and s
        KTPowerSpectrum* ps = fftw->CreatePowerSpectrum();
        ps->ConvertToPowerSpectrum();
        KTDEBUG(testlog, "power spectrum: " << ps);
        psArray[tBin].SetSpectrum( ps, 0 );
        KTWARN(testlog, "power spectrum: " << psArray[tBin].GetSpectrum(0));

        KTDEBUG(testlog, "Creating slice header");
        sArray[tBin].SetTimeInRun( (double)tBin*timeBinWidth );
        sArray[tBin].SetTimeInAcq( (double)tBin*timeBinWidth );
        sArray[tBin].SetSliceLength( timeBinWidth );

        // Log
        if( tBin == 0 )
        {
            KTINFO(testlog, "Finished processing first spectrum. " << nTimeBins-1 << " to go");
        }
        KTWARN(testlog, psArray.size() << "  " << sArray.size() )
    }

    KTWARN(testlog, "A" << psArray[0].GetSpectrum(0));

    // Now we create and configure the spectrogram collector
    KTSpectrogramCollector spec;
    spec.SetMinFrequency( freqMin );
    spec.SetMaxFrequency( freqMax );
    spec.SetLeadTime( 20e-6 );
    spec.SetTrailTime( 20e-6 );

    KTWARN(testlog, "B" << psArray[0].GetSpectrum(0));

    // Add tracks to listen
    KTINFO(testlog, "Adding tracks to the spectrogram collector");
    for( int i = 0; i < 5; i++ )
    {
        KTDEBUG(testlog, "Adding track " << i);
        if( ! spec.ReceiveTrack( trackArray[i] ) )
        {
            KTERROR(testlog, "Something went wrong adding track" << i);
        }
    }

    KTWARN(testlog, "C" << psArray[0].GetSpectrum(0));

    // Add spectra
    KTINFO(testlog, "Adding spectra to the spectrogram collector");
    for( int i = 0; i < nTimeBins; i++ )
    {
        KTDEBUG(testlog, "Adding spectrum " << i);
        if( ! spec.ReceiveSpectrum( psArray[i], sArray[i] ) )
        {
            KTERROR(testlog, "Something went wrong adding spectrum" << i);
        }
    }

    // The result is a KTPSCollectionData for each track
    //std::vector< KTPSCollectionData > result;

    KTINFO(testlog, "Finished receiving spectra. Begin retrieving produced spectrograms");

    const KTSpectrogramCollector::WaterfallSet& spectrograms = spec.WaterfallSets()[0];

    // Fill the result vector and count the number of plots
    //int nPlots = spectrograms.size();
    /*
    for( KTSpectrogramCollector::WaterfallSet::const_iterator it = spec.WaterfallSets()[0].begin(); it != spec.WaterfallSets()[0].end(); ++it )
    {
        result.push_back( it->first->Of< KTPSCollectionData >() );
        nPlots++;
    }
*/

    if( spectrograms.size() == 0 )
    {
        KTERROR(testlog, "No spectrograms were produced!");
    }
    else
    {
        KTINFO(testlog, "Produced " << spectrograms.size() << " spectrograms");
    }

    // Fill a TH2D for each spectrogram and write to a file

#ifdef ROOT_FOUND
    TFile* file = new TFile( "spectrogram-collector-test.root", "recreate" );
    unsigned iSpect = 0;
    for (auto spectIter = spectrograms.begin(); spectIter != spectrograms.end(); ++spectIter)
    {
        KTWARN(testlog, spectIter->second << "  " <<
                spectIter->second->GetSpectra() << "  " <<
                spectIter->second->GetSpectra()->size() << "  " <<
                spectIter->second->GetSpectra()->operator()(0) << "  " <<
                spectIter->second->GetSpectra()->operator()(1) );
        TH2D* hist = spectIter->second->CreatePowerHistogram(0);
        if (hist) hist->Write(TString::Format("track-%i", iSpect));
        else KTERROR(testlog, "Empty histogram");
        ++iSpect;
    }

    // Cleanup
    file->Close();
    delete file;
#endif

    KTINFO(testlog, "Finished; exiting script");
    return 0;
}
