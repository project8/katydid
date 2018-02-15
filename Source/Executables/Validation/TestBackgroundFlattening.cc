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

    std::vector< KTFrequencySpectrumDataPolar* > data;
    KTFrequencySpectrumDataPolar* datum;
    KTFrequencySpectrumPolar* spectrum;

    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; ++iSpectrum )
    {
        spectrum = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );

        // Fill in the noise
        KTINFO(testlog, "Creating the baseline and noise");
        for (unsigned iBin=0; iBin<nBins; iBin++)
        {
            noiseSigma = noiseSigmaLow + (double)(iBin)/nBins * (noiseSigmaHigh - noiseSigmaLow);
    #ifdef ROOT_FOUND
            (*spectrum)(iBin).set_polar(rand.Gaus(meanValue, noiseSigma), 0.);
    #else
            (*spectrum)(iBin).set_polar(meanValue, 0.);
    #endif
        }

        datum->SetSpectrum( spectrum, 0 );
        data.push_back( datum );
    }

    KTDataAccumulator acc;
    KTGainVariationProcessor gvProc;
    KTVariableSpectrumDiscriminator discrim;

    acc.SetAccumulatorSize( 0 );
    acc.SetSignalInterval( 0 );

    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; ++iSpectrum )
    {
        acc.AddData( *(data[iSpectrum]) );
    }

    const Katydid::KTDataAccumulator::Accumulator& accResults = acc.GetAccumulator< KTFrequencySpectrumDataPolar >();
    KTFrequencySpectrumDataPolar mean = accResults.fData->Of< KTFrequencySpectrumDataPolar >();
    KTFrequencySpectrumVarianceDataPolar variance = accResults.fData->Of< KTFrequencySpectrumVarianceDataPolar >();

    gvProc.SetMinFrequency( minFreq );
    gvProc.SetMaxFrequency( maxFreq);
    gvProc.SetNFitPoints( 10 );

    gvProc.CalculateGainVariation( mean );

    KTGainVariationData& gvData = mean.Of< KTGainVariationData >();

    discrim.SetMinFrequency( minFreq );
    discrim.SetMaxFrequency( maxFreq );
    discrim.SetNormalize( true );
    
    std::vector< KTDiscriminatedPoints1DData* > pts;

    for( unsigned iSpectrum = 0; iSpectrum < nSpectra; ++iSpectrum )
    {
        discrim.Discriminate( *(data[iSpectrum]), gvData );
        pts.push_back( &(data[iSpectrum]->Of< KTDiscriminatedPoints1DData >()) );
    }

#ifdef ROOT_FOUND
    TFile* file = new TFile("background_flattening_test.root", "recreate");

    KTFrequencySpectrumPolar* meanSpectrum = mean.GetSpectrumPolar(0);
    KTFrequencySpectrumPolar* varianceSpectrum = variance.GetSpectrumPolar(0);

    TH1D* histFreqSpec = KT2ROOT::CreateMagnitudeHistogram(meanSpectrum, "hFreqSpectrum");
    TH1D* histFreqVarSpec = KT2ROOT::CreateMagnitudeHistogram(varianceSpectrum, "hFreqVarSpectrum");

    histFreqSpec->Write();
    histFreqVarSpec->Write();

    file->Close();
    delete file;
#endif

}
