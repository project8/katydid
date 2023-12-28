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
#include "logger.hh"
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

LOGGER(testlog, "TestBackgroundFlattening");

int main()
{
    unsigned nBins = 8192;
    unsigned nSpectra = 200;
    double minFreq = 0., maxFreq = 100.;

    double meanValueLow = 800.;
    double meanValueHigh = 3200.;
    double noiseSigmaLow = 5.;
    double noiseSigmaHigh = 15.;

    double meanValueSlope = (meanValueHigh - meanValueLow) / (double)nBins; // upward sloping
    double noiseSigmaSlope = (noiseSigmaHigh - noiseSigmaLow) / (double)nBins; // growing

    KTRNGEngine* engine = KTGlobalRNGEngine::get_instance();
    engine->SetSeed(20398);
    KTRNGGaussian<> rand;

    LINFO(testlog, "Initializing data accumulator");

    KTDataAccumulator acc;
    acc.SetAccumulatorSize( nSpectra );
    acc.SetSignalInterval( 0 );

    //std::vector< KTFrequencySpectrumDataPolar > data;

    // Fill in the noise
    LINFO(testlog, "Creating the baseline and noise");
    
    double meanValue = 0.;
    double noiseSigma = 0.;
    double value = 0.0;
    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; iSpectrum++ )
    {
        LDEBUG(testlog, "iSpectrum: " << iSpectrum);
        
        KTFrequencySpectrumDataPolar newData;
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            meanValue = meanValueLow + (double)iBin * meanValueSlope;
            noiseSigma = noiseSigmaLow + (double)iBin * noiseSigmaSlope;
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

        LDEBUG(testlog, "Adding data to accumulator");
        acc.AddData( newData );
    }

    LINFO(testlog, "Grabbing results from accumulator");

    KTDataAccumulator::Accumulator& accResults = acc.GetAccumulatorNonConst< KTFrequencySpectrumDataPolar >();
    accResults.Finalize();

    KTFrequencySpectrumDataPolar& accData = accResults.fData->Of< KTFrequencySpectrumDataPolar >();
    KTFrequencySpectrumVarianceDataPolar& accVarData = accResults.fData->Of< KTFrequencySpectrumVarianceDataPolar >();
/*
    for( unsigned iBin = 0; iBin < 20; iBin++ )
    {
        LDEBUG(testlog, (*(accVarData.GetSpectrum(0)))(iBin));
    }
*/
    LINFO(testlog, "Initializing gain variation");

    KTGainVariationProcessor gvProc;
    gvProc.SetMinFrequency( minFreq );
    gvProc.SetMaxFrequency( maxFreq);
    gvProc.SetNFitPoints( 10 );
    gvProc.SetNormalize( false );

    LINFO(testlog, "Calculating gain variation");

    gvProc.CalculateGainVariation( accData, accVarData );

    LINFO(testlog, "Grabbing results from gain variation");

    KTGainVariationData& gvData = accData.Of< KTGainVariationData >();

    LINFO(testlog, "Initializing gain normalization");

    KTGainNormalization gainNorm;
    gainNorm.SetMinFrequency( minFreq );
    gainNorm.SetMaxFrequency( maxFreq );

    LINFO(testlog, "Initializing discriminator");

    KTSpectrumDiscriminator discrim;
    discrim.SetMinBin( 0 );
    discrim.SetMaxBin( nBins - 1 );
    discrim.SetSigmaThreshold( 3 );
    
    vector<double> xPoints;
    vector<double> yPoints;

    LINFO(testlog, "Performing discrimination");

    KTFrequencySpectrumPolar* firstFS = nullptr;
    KTFrequencySpectrumPolar* firstNormFS = nullptr;
    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; ++iSpectrum )
    {
        KTFrequencySpectrumDataPolar newData;
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            meanValue = meanValueLow + (double)iBin * meanValueSlope;
            noiseSigma = noiseSigmaLow + (double)iBin * noiseSigmaSlope;
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
            //    LWARN(testlog, (*newFS)(iBin) << "  " << (*(normData.GetSpectrumPolar(0)))(iBin) );
            //}
            firstFS = new KTFrequencySpectrumPolar(*newFS);
            firstNormFS = new KTFrequencySpectrumPolar(*(normData.GetSpectrumPolar(0)));
            //LWARN(testlog, firstFS << "  " << firstNormFS);
        }

        discrim.Discriminate( normData );

        for( KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = newData.Of< KTDiscriminatedPoints1DData >().GetSetOfPoints(0).begin(); it != newData.Of< KTDiscriminatedPoints1DData >().GetSetOfPoints(0).end(); ++it )
        {
            xPoints.push_back( (double)iSpectrum );
            yPoints.push_back( it->second.fAbscissa );
        }
    }

    unsigned nDiscPoints = xPoints.size();
    LINFO(testlog, "Discriminator threshold is at " << discrim.GetSigmaThreshold() << " sigma");
    LINFO(testlog, "Total number of points tested: " << nSpectra * nBins);
    LINFO(testlog, "Points above threshold: " << nDiscPoints << " (" << (double)nDiscPoints/ (double)(nSpectra * nBins) * 100. << "%)");

#ifdef ROOT_FOUND
    LINFO(testlog, "Writing to ROOT file");

    LDEBUG(testlog, "Creating TFile");
    TFile* file = new TFile("background_flattening_test.root", "recreate");

    LDEBUG(testlog, "Getting spectra");
    KTFrequencySpectrumPolar* meanSpectrum = accData.GetSpectrumPolar(0);
    KTFrequencySpectrumVariance* varianceSpectrum = accVarData.GetSpectrum(0);

    LDEBUG(testlog, "Creating histograms");
    TH1D* histFreqSpec = KT2ROOT::CreateMagnitudeHistogram(meanSpectrum, "hMeanSpectrum");
    histFreqSpec->SetDirectory( file );
    histFreqSpec->SetLineColor(kMagenta+2);

    TH1D* histFreqVarSpec = KT2ROOT::CreateHistogram(varianceSpectrum, "hVarianceSpectrum");
    histFreqVarSpec->SetDirectory( file );
    histFreqVarSpec->SetLineColor(kMagenta+2);

    TH1D* histLastFS = KT2ROOT::CreateMagnitudeHistogram(firstFS, "hPreNormFS");
    histLastFS->SetDirectory( file );
    histLastFS->SetLineColor(kBlue-4);

    TH1D* histLastNormFS = KT2ROOT::CreateMagnitudeHistogram(firstNormFS, "hNormFS");
    histLastNormFS->SetDirectory( file );
    histLastNormFS->SetLineColor(kGreen+2);
    
    LDEBUG( testlog, "Converting discriminated points to TGraph");
    TGraph* plot = new TGraph( xPoints.size(), xPoints.data(), yPoints.data() );

    TH1I* histDiscPointDist = new TH1I("hDiscPointDist", "Discriminated Points", 100, minFreq, maxFreq);
    for (const double& value : yPoints)
    {
        histDiscPointDist->Fill(value);
    }

    LDEBUG(testlog, "Appending TGraph to TFile");
    //file->Append( plot );

    LDEBUG(testlog, "Writing objects");
    histFreqSpec->Write();
    histFreqVarSpec->Write();
    histLastFS->Write();
    histLastNormFS->Write();
    plot->Write( "thresholded_points" );
    histDiscPointDist->Write();
    
    LDEBUG(testlog, "Closing file");
    file->Close();
#endif

    return 0;
}
