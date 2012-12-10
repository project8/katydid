/*
 * TestWignerVille.cc
 *
 *  Created on: Nov 6, 2012
 *      Author: nsoblath
 *
 *  Usage:
 *      > TestWignerVille filename.egg
 */

#include "KTComplexFFTW.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTRectangularWindow.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTTimeSeriesChannelData.hh"
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
    Double_t startFreq = 5000.; // Hz
    Double_t deltaFreq = -10.; // Hz
    Double_t twoPi = 2. * KTMath::Pi();

    KTBasicTimeSeriesData tsData(2);

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

    tsData.SetTimeSeries(ts1, 0);
    tsData.SetTimeSeries(ts2, 1);


    KTWignerVille wvTransform;
    wvTransform.GetFullFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetFullFFT()->SetSize(tsData.GetTimeSeries(0)->GetNTimeBins());
    wvTransform.SetTransformFlag("ESTIMATE");
    KTEventWindowFunction* windowFunc = new KTRectangularWindow(&tsData);
    windowFunc->SetSize(512);
    wvTransform.SetWindowFunction(windowFunc);
    wvTransform.AddPair(KTWVPair(0, 1));

    wvTransform.RecreateFFT();
    wvTransform.InitializeFFT();

    //KTSlidingWindowFSData* output = wvTransform.TransformData(&tsData);
    KTSlidingWindowFSDataFFTW* output = wvTransform.TransformData(&tsData);

#ifdef ROOT_FOUND
    //KTPhysicalArray< 1, KTFrequencySpectrum* >* spectra = output->GetSpectra(0);
    KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra = output->GetSpectra(0);
    UInt_t nBinsX = spectra->size();
    UInt_t nBinsY = (*spectra)(0)->size();
    TH2D* histOut = new TH2D("wv", "Wigner-Ville", nBinsX, spectra->GetRangeMin(), spectra->GetRangeMax(), nBinsY, (*spectra)(0)->GetRangeMin(), (*spectra)(0)->GetRangeMax());
    Double_t value;
    for (UInt_t iX=0; iX<nBinsX; iX++)
    {
        //KTFrequencySpectrum* spectrum = (*spectra)(iX);
        KTFrequencySpectrumFFTW* spectrum = (*spectra)(iX);
        for (UInt_t iY=0; iY<nBinsY; iY++)
        {
            //value = (*spectrum)(iY).abs();
            value = sqrt((*spectrum)(iY)[0] * (*spectrum)(iY)[0] + (*spectrum)(iY)[1] * (*spectrum)(iY)[1]);
            histOut->SetBinContent(iX+1, iY+1, value);
        }
    }
    histOut->Write();
#endif

    delete output;

#ifdef ROOT_FOUND
    file->Close();
    delete file;
#endif

    return 0;

}










/*

#include "KTComplexFFTW.hh"
#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTEggReaderMonarch.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTRectangularWindow.hh"
#include "KTSlidingWindowFFTW.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTWignerVille.hh"

#ifdef ROOT_FOUND
#include "KTBasicROOTFileWriter.hh"
#endif

#include <boost/shared_ptr.hpp>


using namespace std;
using namespace Katydid;


KTLOGGER(testwv, "katydid.validation");


int main(int argc, char** argv)
{

    if (argc < 2)
    {
        KTERROR(testwv, "No filename supplied");
        return 0;
    }
    string filename(argv[1]);

    KTINFO(testwv, "Test of hatching egg file <" << filename << ">");

    KTEgg egg;
    UInt_t recordSize = 0;
    KTINFO(testwv, "Record size will be " << recordSize << " (if 0, it should be the same as the Monarch record size)");
    KTEggReaderMonarch* reader = new KTEggReaderMonarch();
    reader->SetTimeSeriesSizeRequest(recordSize);
    reader->SetTimeSeriesType(KTEggReaderMonarch::kFFTWTimeSeries);
    egg.SetReader(reader);

    KTINFO(testwv, "Opening file");
    if (egg.BreakEgg(filename))
    {
        KTINFO(testwv, "Egg opened successfully");
    }
    else
    {
        KTERROR(testwv, "Egg file was not opened");
        return -1;
    }

    const KTEggHeader* header = egg.GetHeader();
    if (header == NULL)
    {
        KTERROR(testwv, "No header received");
        egg.CloseEgg();
        return -1;
    }

    KTINFO(testwv, "Hatching event");
    boost::shared_ptr<KTEvent> event = egg.HatchNextEvent();
    if (event == NULL)
    {
        KTERROR(testwv, "Event did not hatch");
        egg.CloseEgg();
        return -1;
    }

    // Get the time-series data from the event.
    // The data is still owned by the event.
    KTTimeSeriesData* tsData = event->GetData<KTProgenitorTimeSeriesData>("time-series");
    if (tsData == NULL)
    {
        KTWARN(testwv, "No time-series data present in event");
        egg.CloseEgg();
        return -1;
    }

    // Create the transform, and manually configure it.
    KTINFO(testwv, "Creating and configuring WV transform");
    KTWignerVille wvTransform;
    wvTransform.GetFullFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetFullFFT()->SetSize(tsData->GetTimeSeries(0)->GetNTimeBins());
    KTEventWindowFunction* windowFunc = new KTRectangularWindow(tsData);
    windowFunc->SetSize(5000);
    wvTransform.GetWindowedFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetWindowedFFT()->SetWindowFunction(windowFunc);
    wvTransform.GetWindowedFFT()->SetOverlap((UInt_t)0);
    wvTransform.AddPair(KTWVPair(0, 1));

    // Transform the data.
    // The data is not owned by the event because TransformData was used, not ProcessEvent.
    KTINFO(testwv, "Transforming data");
    KTSlidingWindowFSDataFFTW* wvData = wvTransform.TransformData(tsData);

    if (wvData == NULL)
    {
        KTWARN(testwv, "No data was returned by the WV transform; test failed");
    }
    else
    {
#ifdef ROOT_FOUND
        KTBasicROOTFileWriter writer;
        writer.SetFilename("WVTransformTest.root");
        writer.SetFileFlag("recreate");

        KTINFO(testwv, "Writing data to file");
        writer.Publish(wvData);
#endif
    }

    KTINFO(testwv, "Test complete; cleaning up");
    egg.CloseEgg();
    delete wvData;

    return 0;


}
*/
