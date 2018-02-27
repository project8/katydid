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
#include "KTGainNormalization.hh"
#include "KTGainVariationProcessor.hh"
#include "KTGainVariationData.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTRandom.hh"
#include "KTSpectrumDiscriminator.hh"

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

    double meanValueLow = 8.;
    double meanValueHigh = 32.;
    double noiseSigmaLow = 5.;
    double noiseSigmaHigh = 15.;

    double meanValueSlope = (meanValueHigh - meanValueLow) / (double)nBins; // upward sloping
    double noiseSigmaSlope = (noiseSigmaHigh - noiseSigmaLow) / (double)nBins; // growing

    KTRNGEngine* engine = KTGlobalRNGEngine::get_instance();
    engine->SetSeed(20398);
    KTRNGGaussian<> rand;

    KTINFO(testlog, "Initializing data accumulator");

    KTDataAccumulator acc;
    acc.SetAccumulatorSize( nSpectra );
    acc.SetSignalInterval( 0 );

    //std::vector< KTFrequencySpectrumDataPolar > data;

    // Fill in the noise
    KTINFO(testlog, "Creating the baseline and noise");
    
    double meanValue = 0.;
    double noiseSigma = 0.;
    double value = 0.0;
    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; iSpectrum++ )
    {
        KTDEBUG(testlog, "iSpectrum: " << iSpectrum);
        
        KTFrequencySpectrumDataPolar newData;
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            meanValue = meanValueLow + (double)iBin * meanValueSlope;
            noiseSigma = noiseSigmaLow + (double)iBin * noiseSigmaSlope;
            if (iBin < 10)
                KTWARN(testlog, "meanValue and noiseSigma: " << meanValue << "  " << noiseSigma);
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

    KTFrequencySpectrumDataPolar& accData = accResults.fData->Of< KTFrequencySpectrumDataPolar >();
    KTFrequencySpectrumVarianceDataPolar& accVarData = accResults.fData->Of< KTFrequencySpectrumVarianceDataPolar >();
/**/
    for( unsigned iBin = 0; iBin < 20; iBin++ )
    {
        KTDEBUG(testlog, (*(accVarData.GetSpectrum(0)))(iBin));
    }
/**/
    KTINFO(testlog, "Initializing gain variation");

    KTGainVariationProcessor gvProc;
    gvProc.SetMinFrequency( minFreq );
    gvProc.SetMaxFrequency( maxFreq);
    gvProc.SetNFitPoints( 10 );
    gvProc.SetNormalize( false );

    KTINFO(testlog, "Calculating gain variation");

    gvProc.CalculateGainVariation( accData, accVarData );

    KTINFO(testlog, "Grabbing results from gain variation");

    KTGainVariationData& gvData = accData.Of< KTGainVariationData >();

    KTINFO(testlog, "Initializing gain normalization");

    KTGainNormalization gainNorm;
    gainNorm.SetMinFrequency( minFreq );
    gainNorm.SetMaxFrequency( maxFreq );

    KTINFO(testlog, "Initializing discriminator");

    KTSpectrumDiscriminator discrim;
    discrim.SetMinBin( 0 );
    discrim.SetMaxBin( nBins - 1 );
    discrim.SetSigmaThreshold( 3 );
    
    vector<double> xPoints;
    vector<double> yPoints;

    KTINFO(testlog, "Performing discrimination");

    KTFrequencySpectrumPolar* lastFS = nullptr;
    KTFrequencySpectrumPolar* lastNormFS = nullptr;
    for( unsigned iSpectrum = 0; iSpectrum < 1; ++iSpectrum )
    {
        KTFrequencySpectrumDataPolar newData;
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            meanValue = meanValueLow + (double)iBin * meanValueSlope;
            noiseSigma = noiseSigmaLow + (double)iBin * noiseSigmaSlope;
            if (iBin < 10)
                KTWARN(testlog, "meanValue and noiseSigma: " << meanValue << "  " << noiseSigma);
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

        gainNorm.Normalize(newData, gvData);

        KTNormalizedFSDataPolar& normData = newData.Of< KTNormalizedFSDataPolar >();

        if (iSpectrum == 0 )
        {
            //for (unsigned iBin = 0; iBin<20; ++iBin)
            //{
            //    KTWARN(testlog, (*newFS)(iBin) << "  " << (*(normData.GetSpectrumPolar(0)))(iBin) );
            //}
            lastFS = new KTFrequencySpectrumPolar(*newFS);
            lastNormFS = new KTFrequencySpectrumPolar(*(normData.GetSpectrumPolar(0)));
            KTWARN(testlog, lastFS << "  " << lastNormFS);
        }

        discrim.Discriminate( normData );

        for( KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = newData.Of< KTDiscriminatedPoints1DData >().GetSetOfPoints(0).begin(); it != newData.Of< KTDiscriminatedPoints1DData >().GetSetOfPoints(0).end(); ++it )
        {
            xPoints.push_back( (double)iSpectrum );
            yPoints.push_back( it->second.fAbscissa );
        }
    }

    KTINFO(testlog, "Discriminator threshold is at " << discrim.GetSigmaThreshold() << " sigma");
    KTINFO(testlog, "Total number of points tested: " << nSpectra * nBins);
    KTINFO(testlog, "Points above threshold: " << xPoints.size() << " (" << (double)xPoints.size() / (double)(nSpectra * nBins) * 100. << "%)");

#ifdef ROOT_FOUND
    KTINFO(testlog, "Writing to ROOT file");

    KTDEBUG(testlog, "Creating TFile");
    TFile* file = new TFile("background_flattening_test.root", "recreate");

    KTDEBUG(testlog, "Getting spectra");
    KTFrequencySpectrumPolar* meanSpectrum = accData.GetSpectrumPolar(0);
    KTFrequencySpectrumVariance* varianceSpectrum = accVarData.GetSpectrum(0);

    KTDEBUG(testlog, "Creating histograms");
    TH1D* histFreqSpec = KT2ROOT::CreateMagnitudeHistogram(meanSpectrum, "hMeanSpectrum");
    histFreqSpec->SetDirectory( file );
    histFreqSpec->SetLineColor(kMagenta+2);

    TH1D* histFreqVarSpec = KT2ROOT::CreateHistogram(varianceSpectrum, "hVarianceSpectrum");
    histFreqVarSpec->SetDirectory( file );
    histFreqVarSpec->SetLineColor(kMagenta+2);

    TH1D* histLastFS = KT2ROOT::CreateMagnitudeHistogram(lastFS, "hPreNormFS");
    histLastFS->SetDirectory( file );
    histLastFS->SetLineColor(kBlue-4);

    TH1D* histLastNormFS = KT2ROOT::CreateMagnitudeHistogram(lastNormFS, "hNormFS");
    histLastNormFS->SetDirectory( file );
    histLastNormFS->SetLineColor(kGreen+2);
    
    KTDEBUG( testlog, "Converting discriminated points to TGraph");
    TGraph* plot = new TGraph( xPoints.size(), xPoints.data(), yPoints.data() );

    KTDEBUG(testlog, "Appending TGraph to TFile");
    //file->Append( plot );

    KTDEBUG(testlog, "Writing objects");
    histFreqSpec->Write();
    histFreqVarSpec->Write();
    histLastFS->Write();
    histLastNormFS->Write();
    plot->Write( "thresholded_points" );
    
    KTDEBUG(testlog, "Closing file");
    file->Close();
#endif

    return 0;
}
