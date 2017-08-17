/*
 * TestSpectrogramStriper.cc
 *
 *  Created on: Aug 16, 2017
 *      Author: N.S. Oblath
 */

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTSliceHeader.hh"
#include "KTSpectrogramStriper.hh"

#include "KTLogger.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH2.h"
#endif

KTLOGGER(testlog, "TestSpectrogramStriper");

namespace Katydid
{
    class HistogramPrinter : public Nymph::KTProcessor
    {
        public:
            HistogramPrinter() : Nymph::KTProcessor()
            {
                this->RegisterSlot("print-hist", this, &HistogramPrinter::PrintHistogram);
            }
            virtual ~HistogramPrinter() {}

            bool Configure(const scarab::param_node*)
            {
                return true;
            }

            void PrintHistogram(Nymph::KTDataPtr dataPtr)
            {
                KTINFO(testlog, "Printing a histogram");
#ifdef ROOT_FOUND
                TFile* file = new TFile("TestSpectrogramStriper.root", "update");
                KTMultiFSDataFFTW& data = dataPtr->Of< KTMultiFSDataFFTW >();
                TH2D* tsHist = data.CreateMagnitudeHistogram(0);
                tsHist->SetDirectory(file);
                tsHist->Write();
                file->Close();
#endif
                return;
            }
    };
}


using namespace Katydid;


int main()
{
    KTINFO(testlog, "Preparing");

    Nymph::KTDataPtr dp;
    KTMultiFSFFTWData& md = dp->Of< KTMultiFSFFTWData >();

    // Test parameters
    unsigned nFreqBins = 10;
    unsigned stripeSize = 8;
    unsigned stripeOverlap = 3;
    unsigned nAcquisitions = 3;
    unsigned slicesPerAcq = 15;

    // Set the necessary parameters in the header; time-in-run will be updated for each slice
    KTSliceHeader header;
    header.SetTimeInRun(0.);
    header.SetSliceLength(1.);
    header.SetIsNewAcquisition(true);

    // Create a spectrum object and corresponding data object
    KTFrequencySpectrumFFTW* spectrum = new KTFrequencySpectrumFFTW(nFreqBins, 0., 10.);
    spectrum->operator*=(0.);
    KTFrequencySpectrumDataFFTW spectrumData;
    spectrumData.SetSpectrum(spectrum);

    // Create and setup the striper
    KTSpectrogramStriper striper;
    striper.SetStripeSize(stripeSize);
    striper.SetStripeOverlap(stripeOverlap);
    striper.CalculateSwaps();

    // Create the histogram printer
    HistogramPrinter printer;

    // Connect the strip signal to the print-hist slot
    striper.ConnectASlot("stripe", &printer, "print-hist");

    KTINFO(testlog, "Everything is prepared; starting the action");

    unsigned peakPos = 0;
    for (unsigned iAcq = 0; iAcq < nAcquisitions; ++iAcq)
    {
        KTINFO(testlog, "New acquisition");
        header.SetIsNewAcquisition(true);
        for (unsigned iSlice = 0; iSlice < slicesPerAcq; ++iSlice)
        {
            KTDEBUG(testlog, "New slice");
            (*spectrum)(peakPos)[0] = 10.;

            striper.AddData(header, spectrumData);

            (*spectrum)(peakPos)[0] = 0.;
            peakPos++;
            if (peakPos == nFreqBins) peakPos = 0;

            header.SetIsNewAcquisition(false);
        }
    }

    KTINFO(testlog, "Testing complete");

    return 0;
}

