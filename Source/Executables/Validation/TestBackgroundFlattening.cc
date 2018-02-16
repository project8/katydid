/*
 * TestBackgroundFlattening.cc
 *
 *  Created on: Feb 15, 2018
 *      Author: ezayas
 */

#include "KT2ROOT.hh"
#include "KTDataAccumulator.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTGainVariationProcessor.hh"
#include "KTLogger.hh"
#include "KTSpectrumVarianceData.hh"
#include "KTVariableSpectrumDiscriminator.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"
#include "TRandom3.h"
#endif

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestBackgroundFlattening");

int main()
{
    unsigned nBins = 8192;
    unsigned nSpectra = 200;
    double minFreq = 0., maxFreq = 100.;

    double meanValue = 10.;
    double noiseSigmaLow = 5.;
    double noiseSigmaHigh = 15.;
    double noiseSigma = 0.;

#ifdef ROOT_FOUND
    TRandom3 rand(0);
#endif

    KTINFO(testlog, "Initializing data accumulator");

    KTDataAccumulator acc;
    KTGainVariationProcessor gvProc;
    KTVariableSpectrumDiscriminator discrim;

    acc.SetAccumulatorSize( nSpectra );
    acc.SetSignalInterval( 0 );

    //std::vector< KTFrequencySpectrumDataPolar > data;

    // Fill in the noise
    KTINFO(testlog, "Creating the baseline and noise");
    
    double value = 0.0;
    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; iSpectrum++ )
    {
        KTDEBUG(testlog, "iSpectrum: " << iSpectrum);
        
        KTFrequencySpectrumDataPolar newData;
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            noiseSigma = noiseSigmaLow + (double)(iBin)/nBins * (noiseSigmaHigh - noiseSigmaLow);
        #ifdef ROOT_FOUND
            value = rand.Gaus( meanValue, noiseSigma );
            if( value < 0.0 )
            {
                value = 0.0;
            }
            (*newFS)(iBin).set_polar( value, 0. );
        #else
            (*newFS)(iBin).set_polar( 0., 0. );
        #endif
        }

        newData.SetNComponents( 1 );
        newData.SetSpectrum( newFS, 0 );

        KTDEBUG(testlog, "Adding data to accumulator");
        acc.AddData( newData );
    }

    KTINFO(testlog, "Grabbing results from accumulator");

    const Katydid::KTDataAccumulator::Accumulator& accResults = acc.GetAccumulator< KTFrequencySpectrumDataPolar >();
    KTFrequencySpectrumDataPolar mean = accResults.fData->Of< KTFrequencySpectrumDataPolar >();
    KTFrequencySpectrumVarianceDataPolar variance = accResults.fData->Of< KTFrequencySpectrumVarianceDataPolar >();
/*
    for( unsigned iBin = 0; iBin < nBins; iBin++ )
    {
        KTDEBUG(testlog, (*variance.GetSpectrumPolar(0))(iBin));
    }
*/
    KTINFO(testlog, "Initializing gain variation");

    gvProc.SetMinFrequency( minFreq );
    gvProc.SetMaxFrequency( maxFreq);
    gvProc.SetNFitPoints( 10 );

    KTINFO(testlog, "Calculating gain variation");

    gvProc.CalculateGainVariation( mean );

    KTINFO(testlog, "Grabbing results from gain variation");

    KTGainVariationData& gvData = mean.Of< KTGainVariationData >();

    KTINFO(testlog, "Initializing discriminator");

    discrim.SetMinBin( 0 );
    discrim.SetMaxBin( nBins - 1 );
    discrim.SetSigmaThreshold( 2 );
    discrim.SetNormalize( true );
    
    vector< double > xx;
    vector< double > yy;
    int n = 0;

    KTINFO(testlog, "Performing discrimination");

    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; ++iSpectrum )
    {
        KTFrequencySpectrumDataPolar newData;
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            noiseSigma = noiseSigmaLow + (double)(iBin)/nBins * (noiseSigmaHigh - noiseSigmaLow);
        #ifdef ROOT_FOUND
            value = rand.Gaus( meanValue, noiseSigma );
            if( value < 0.0 )
            {
                value = 0.0;
            }
            (*newFS)(iBin).set_polar( value, 0. );
        #else
            (*newFS)(iBin).set_polar( 0., 0. );
        #endif
        }

        newData.SetNComponents( 1 );
        newData.SetSpectrum( newFS, 0 );

        discrim.Discriminate( newData, gvData );

        for( KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = newData.Of< KTDiscriminatedPoints1DData >().GetSetOfPoints(0).begin(); it != newData.Of< KTDiscriminatedPoints1DData >().GetSetOfPoints(0).end(); ++it )
        {
            xx.push_back( (double)iSpectrum );
            yy.push_back( it->second.fAbscissa );
            n++;
        }        
        
    }

    KTINFO( testlog, "Converting discriminated points to TGraph");

    // Actually we need physical arrays lol
    double* xArray = &xx[0];
    double* yArray = &yy[0];

    KTINFO(testlog, "Writing to file");

#ifdef ROOT_FOUND
    KTDEBUG(testlog, "Creating TFile");
    TFile* file = new TFile("background_flattening_test.root", "recreate");

    KTDEBUG(testlog, "Getting spectra");
    KTFrequencySpectrumPolar* meanSpectrum = mean.GetSpectrumPolar(0);
    KTFrequencySpectrumPolar* varianceSpectrum = variance.GetSpectrumPolar(0);

    KTDEBUG(testlog, "Creating histograms");
    TH1D* histFreqSpec = KT2ROOT::CreateMagnitudeHistogram(meanSpectrum, "hFreqSpectrum");
    TH1D* histFreqVarSpec = KT2ROOT::CreateMagnitudeHistogram(varianceSpectrum, "hFreqVarSpectrum");
    
    KTDEBUG(testlog, "Appending histograms to TFile");
    //file->Append( histFreqSpec );
    //file->Append( histFreqVarSpec );
    histFreqSpec->SetDirectory( file );
    histFreqVarSpec->SetDirectory( file );

    KTDEBUG(testlog, "Creating TGraph");
    TGraph* plot;
    plot = new TGraph( n, xArray, yArray );

    KTDEBUG(testlog, "Appending TGraph to TFile");
    file->Append( plot );

    KTDEBUG(testlog, "Writing objects");
    histFreqSpec->Write();
    histFreqVarSpec->Write();
    plot->Write( "thresholded-points" );
    
    KTDEBUG(testlog, "Closing file");
    file->Close();
#endif

}
