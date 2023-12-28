/*
 * TestWignerVille.cc
 *
 *  Created on: Nov 6, 2012
 *      Author: nsoblath
 *
 *  Usage:
 *      > TestWignerVille
 */

#include "KTAnalyticAssociateData.hh"
#include "KTAnalyticAssociator.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "logger.hh"
#include "KTMath.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"
#include "KTWignerVille.hh"
#include "KTWignerVilleData.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH2.h"
#include "TH1.h"
#endif


using namespace Katydid;
using namespace std;

LOGGER(testlog, "TestWignerVille");

int main()
{
    unsigned nTimeBins = 32768;

    double amplitude = 1.;
    double startFreq = 2000.; // Hz
    double deltaFreq = -10.; // Hz
    double twoPi = 2. * KTMath::Pi();

    KTTimeSeriesReal* ts1 = new KTTimeSeriesReal(nTimeBins, 0., 1.);
    KTTimeSeriesReal* ts2 = new KTTimeSeriesReal(nTimeBins, 0., 1.);
    for (unsigned iBin=0; iBin<nTimeBins; iBin++)
    {
        double freq = startFreq + double(iBin)/1000. * deltaFreq;
        double binCent = ts1->GetBinCenter(iBin);
        ts1->SetValue(iBin, amplitude * sin(twoPi * freq * binCent));
        ts2->SetValue(iBin, amplitude * sin(twoPi * freq * binCent));
    }

#ifdef ROOT_FOUND
    TFile* file = new TFile("testWignerVille.root", "recreate");

    TH1D* hTS = ts1->CreateHistogram();
    hTS->Write();
#endif


    unsigned wvSize = 512;

    KTAnalyticAssociator aAssociator;
    aAssociator.GetForwardFFT()->SetTransformFlag("ESTIMATE");
    aAssociator.GetForwardFFT()->SetTimeSize(wvSize);
    aAssociator.GetForwardFFT()->InitializeForRealTDD();
    aAssociator.GetReverseFFT()->SetTransformFlag("ESTIMATE");
    aAssociator.GetReverseFFT()->SetTimeSize(wvSize);
    aAssociator.GetReverseFFT()->InitializeForComplexTDD();

    KTWignerVille wvTransform;
    wvTransform.GetFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetFFT()->SetTimeSize(/*2 */ wvSize);
    wvTransform.GetFFT()->InitializeForComplexTDD();
    wvTransform.AddPair(KTWignerVille::UIntPair(0, 1));

    unsigned nWindows = nTimeBins / wvSize;

    vector< KTWignerVilleData* > allOutput(nWindows);
    KTPhysicalArray< 1, KTFrequencySpectrumFFTW* > spectra(nWindows, 0., 1.);

    LINFO(testlog, nWindows << " will be used");

    KTSliceHeader header;

    unsigned iWindow = 0;
    for (unsigned windowStart = 0; windowStart < wvSize * nWindows; windowStart += wvSize)
    {
        LINFO(testlog, "window: " << iWindow);
        //KTBasicTimeSeriesData windowData(2);
        KTTimeSeriesReal* windowTS1 = new KTTimeSeriesReal(wvSize, ts1->GetBinLowEdge(windowStart), ts1->GetBinLowEdge(windowStart) + ts1->GetBinWidth() * (double)wvSize);
        KTTimeSeriesReal* windowTS2 = new KTTimeSeriesReal(wvSize, ts2->GetBinLowEdge(windowStart), ts2->GetBinLowEdge(windowStart) + ts2->GetBinWidth() * (double)wvSize);

        for (unsigned iBin=windowStart; iBin < windowStart+wvSize; iBin++)
        {
            windowTS1->SetValue(iBin-windowStart, ts1->GetValue(iBin));
            windowTS2->SetValue(iBin-windowStart, ts2->GetValue(iBin));
        }

        //windowData.SetTimeSeries(windowTS1, 0);
        //windowData.SetTimeSeries(windowTS2, 1);

        //KTTimeSeriesData* aaTSData = aAssociator.CreateAssociateData(&windowData);
        KTTimeSeriesFFTW* aaTS1 = aAssociator.CalculateAnalyticAssociate(windowTS1);
        KTTimeSeriesFFTW* aaTS2 = aAssociator.CalculateAnalyticAssociate(windowTS2);

        KTAnalyticAssociateData aaData;
        aaData.SetNComponents(2);
        aaData.SetTimeSeries(aaTS1, 0);
        aaData.SetTimeSeries(aaTS2, 1);
        //aaTSData->SetTimeSeries(windowTS1, 0);
        //aaTSData->SetTimeSeries(windowTS2, 1);

        if (! wvTransform.TransformData(aaData, header))
        {
            LERROR(testlog, "Something went wrong while computing the Wigner-Ville transform");
        }
        KTWignerVilleData& output = aaData.Of< KTWignerVilleData >();

        allOutput[iWindow] = &output;
        spectra(iWindow) = output.GetSpectrumFFTW(0);

        delete windowTS1;
        delete windowTS2;

        iWindow++;
    }

#ifdef ROOT_FOUND
    unsigned nBinsX = spectra.size();
    unsigned nBinsY = spectra(0)->size();
    TH2D* histOut = new TH2D("wv", "Wigner-Ville", nBinsX, spectra.GetRangeMin(), spectra.GetRangeMax(), nBinsY, spectra(0)->GetRangeMin(), spectra(0)->GetRangeMax());
    double value;
    for (unsigned iX=0; iX<nBinsX; iX++)
    {
        KTFrequencySpectrumFFTW* spectrum = spectra(iX);
        for (unsigned iY=0; iY<nBinsY; iY++)
        {
            value = spectrum->GetAbs(iY);
            histOut->SetBinContent(iX+1, iY+1, value);
        }
    }
    histOut->Write();
#endif

    for (unsigned iWindow=0; iWindow < nWindows; iWindow++)
    {
        delete allOutput[iWindow];
        spectra(iWindow) = NULL;
    }

    delete ts1;
    delete ts2;

#ifdef ROOT_FOUND
    file->Close();
    delete file;
#endif

    return 0;

}
