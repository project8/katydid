/*
 * TestWignerVille.cc
 *
 *  Created on: Nov 6, 2012
 *      Author: nsoblath
 *
 *  Usage:
 *      > TestWignerVille
 */

#include "KTAnalyticAssociator.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTWignerVille.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH2.h"
#include "TH1.h"
#endif


using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "katydid.applications.validation");

int main()
{
    UInt_t nTimeBins = 32768;

    Double_t amplitude = 1.;
    Double_t startFreq = 2000.; // Hz
    Double_t deltaFreq = -10.; // Hz
    Double_t twoPi = 2. * KTMath::Pi();

    KTTimeSeriesFFTW* ts1 = new KTTimeSeriesFFTW(nTimeBins, 0., 1.);
    KTTimeSeriesFFTW* ts2 = new KTTimeSeriesFFTW(nTimeBins, 0., 1.);
    for (UInt_t iBin=0; iBin<nTimeBins; iBin++)
    {
        Double_t freq = startFreq + Double_t(iBin)/1000. * deltaFreq;
        Double_t binCent = ts1->GetBinCenter(iBin);
        ts1->SetValue(iBin, amplitude * sin(twoPi * freq * binCent));
        ts2->SetValue(iBin, amplitude * sin(twoPi * freq * binCent));
    }

#ifdef ROOT_FOUND
    TFile* file = new TFile("testWignerVille.root", "recreate");

    TH1D* hTS = ts1->CreateHistogram();
    hTS->Write();
#endif


    UInt_t wvSize = 512;

    KTAnalyticAssociator aAssociator;
    aAssociator.GetFullFFT()->SetTransformFlag("ESTIMATE");
    aAssociator.GetFullFFT()->SetSize(wvSize);
    aAssociator.GetFullFFT()->InitializeFFT();

    KTWignerVille wvTransform;
    wvTransform.GetFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetFFT()->SetSize(/*2 */ wvSize);
    wvTransform.GetFFT()->InitializeFFT();
    wvTransform.AddPair(KTWVPair(0, 1));

    UInt_t nWindows = nTimeBins / wvSize;

    vector< KTWignerVilleData* > allOutput(nWindows);
    KTPhysicalArray< 1, KTFrequencySpectrumFFTW* > spectra(nWindows, 0., 1.);

    KTINFO(testlog, nWindows << " will be used");

    UInt_t iWindow = 0;
    for (UInt_t windowStart = 0; windowStart < wvSize * nWindows; windowStart += wvSize)
    {
        KTINFO(testlog, "window: " << iWindow);
        //KTBasicTimeSeriesData windowData(2);
        KTTimeSeriesFFTW* windowTS1 = new KTTimeSeriesFFTW(wvSize, ts1->GetBinLowEdge(windowStart), ts1->GetBinLowEdge(windowStart) + ts1->GetBinWidth() * (Double_t)wvSize);
        KTTimeSeriesFFTW* windowTS2 = new KTTimeSeriesFFTW(wvSize, ts2->GetBinLowEdge(windowStart), ts2->GetBinLowEdge(windowStart) + ts2->GetBinWidth() * (Double_t)wvSize);

        for (UInt_t iBin=windowStart; iBin < windowStart+wvSize; iBin++)
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

        if (! wvTransform.TransformData(aaData))
        {
            KTERROR(testlog, "Something went wrong while computing the Wigner-Ville transform");
        }
        KTWignerVilleData& output = aaData.Of< KTWignerVilleData >();

        allOutput[iWindow] = &output;
        spectra(iWindow) = output.GetSpectrumFFTW(0);

        delete windowTS1;
        delete windowTS2;

        iWindow++;
    }

#ifdef ROOT_FOUND
    UInt_t nBinsX = spectra.size();
    UInt_t nBinsY = spectra(0)->size();
    TH2D* histOut = new TH2D("wv", "Wigner-Ville", nBinsX, spectra.GetRangeMin(), spectra.GetRangeMax(), nBinsY, spectra(0)->GetRangeMin(), spectra(0)->GetRangeMax());
    Double_t value;
    for (UInt_t iX=0; iX<nBinsX; iX++)
    {
        KTFrequencySpectrumFFTW* spectrum = spectra(iX);
        for (UInt_t iY=0; iY<nBinsY; iY++)
        {
            value = sqrt((*spectrum)(iY)[0] * (*spectrum)(iY)[0] + (*spectrum)(iY)[1] * (*spectrum)(iY)[1]);
            histOut->SetBinContent(iX+1, iY+1, value);
        }
    }
    histOut->Write();
#endif

    for (UInt_t iWindow=0; iWindow < nWindows; iWindow++)
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
