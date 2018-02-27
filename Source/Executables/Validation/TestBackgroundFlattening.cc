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
#include "KTGainVariationData.hh"
#include "KTVariableSpectrumDiscriminator.hh"
#include "KTLogger.hh"
#include "KTRandom.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"
#include "TGraph.h"
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

    KTRNGEngine* engine = KTGlobalRNGEngine::get_instance();
    engine->SetSeed(20398);
    KTRNGGaussian<> rand( meanValue, noiseSigma );

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
            noiseSigma = noiseSigmaLow + (double)(iBin)/(double)nBins * (noiseSigmaHigh - noiseSigmaLow);
            rand.param( KTRNGGaussian<>::param_type( meanValue, noiseSigma ) );
            value = rand();
            if( value < 0.0 )
            {
                value = 0.0;
            }
            (*newFS)(iBin).set_polar( value, 0. );
        }

        newData.SetNComponents( 1 );
        newData.SetSpectrum( newFS, 0 );

        KTDEBUG(testlog, "Adding data to accumulator");
        acc.AddData( newData );
    }

    KTINFO(testlog, "Grabbing results from accumulator");

    KTDataAccumulator::Accumulator& accResults = acc.GetAccumulatorNonConst< KTFrequencySpectrumDataPolar >();
    accResults.Finalize();

    KTFrequencySpectrumDataPolar& mean = accResults.fData->Of< KTFrequencySpectrumDataPolar >();
    KTFrequencySpectrumVarianceDataPolar& variance = accResults.fData->Of< KTFrequencySpectrumVarianceDataPolar >();
/*
    for( unsigned iBin = 0; iBin < nBins; iBin++ )
    {
        KTDEBUG(testlog, (*variance.GetSpectrum(0))(iBin));
    }
*/
    KTINFO(testlog, "Initializing gain variation");

    gvProc.SetMinFrequency( minFreq );
    gvProc.SetMaxFrequency( maxFreq);
    gvProc.SetNFitPoints( 10 );
    gvProc.SetNormalize( false );

    KTINFO(testlog, "Calculating gain variation");

    gvProc.CalculateGainVariation( mean, variance );

    KTINFO(testlog, "Grabbing results from gain variation");

    KTGainVariationData& gvData = mean.Of< KTGainVariationData >();

    KTINFO(testlog, "Initializing discriminator");

    discrim.SetMinBin( 0 );
    discrim.SetMaxBin( nBins - 1 );
    discrim.SetSigmaThreshold( 3 );
    discrim.SetNormalize( true );
    
    vector<double> xPoints;
    vector<double> yPoints;

    KTINFO(testlog, "Performing discrimination");

    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; ++iSpectrum )
    {
        KTFrequencySpectrumDataPolar newData;
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            noiseSigma = noiseSigmaLow + (double)(iBin)/nBins * (noiseSigmaHigh - noiseSigmaLow);
            rand.param( KTRNGGaussian<>::param_type( meanValue, noiseSigma ) );
            value = rand();
            if( value < 0.0 )
            {
                value = 0.0;
            }
            (*newFS)(iBin).set_polar( value * 0.005, 0. );
        }

        newData.SetNComponents( 1 );
        newData.SetSpectrum( newFS, 0 );

        discrim.Discriminate( newData, gvData );

        for( KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = newData.Of< KTDiscriminatedPoints1DData >().GetSetOfPoints(0).begin(); it != newData.Of< KTDiscriminatedPoints1DData >().GetSetOfPoints(0).end(); ++it )
        {
            xPoints.push_back( (double)iSpectrum );
            yPoints.push_back( it->second.fAbscissa );
        }
    }



#ifdef ROOT_FOUND
    KTINFO(testlog, "Writing to ROOT file");

    KTDEBUG(testlog, "Creating TFile");
    TFile* file = new TFile("background_flattening_test.root", "recreate");

    KTDEBUG(testlog, "Getting spectra");
    KTFrequencySpectrumPolar* meanSpectrum = mean.GetSpectrumPolar(0);
    KTFrequencySpectrumVariance* varianceSpectrum = variance.GetSpectrum(0);

    KTDEBUG(testlog, "Creating histograms");
    TH1D* histFreqSpec = KT2ROOT::CreateMagnitudeHistogram(meanSpectrum, "hMeanSpectrum");
    TH1D* histFreqVarSpec = KT2ROOT::CreateHistogram(varianceSpectrum, "hVarianceSpectrum");
    
    KTDEBUG(testlog, "Appending histograms to TFile");
    //file->Append( histFreqSpec );
    //file->Append( histFreqVarSpec );
    histFreqSpec->SetDirectory( file );
    histFreqVarSpec->SetDirectory( file );

    KTDEBUG( testlog, "Converting discriminated points to TGraph");
    TGraph* plot = new TGraph( xPoints.size(), xPoints.data(), yPoints.data() );

    KTDEBUG(testlog, "Appending TGraph to TFile");
    file->Append( plot );

    KTDEBUG(testlog, "Writing objects");
    histFreqSpec->Write();
    histFreqVarSpec->Write();
    plot->Write( "thresholded_points" );
    
    KTDEBUG(testlog, "Closing file");
    file->Close();
#endif

    return 0;
}
