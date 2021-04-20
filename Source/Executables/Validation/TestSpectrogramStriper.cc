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
            HistogramPrinter() :
                    Nymph::KTProcessor(),
                    fFilename("TestSpectrogramStriper.root"),
                    fHistCounter(0)
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
                TFile* file = new TFile(fFilename.c_str(), "update");
                KTMultiFSDataFFTW& data = dataPtr->Of< KTMultiFSDataFFTW >();
                std::stringstream str;
                str << "hMultiFSFFTW_" << fHistCounter++;
                TH2D* tsHist = data.CreateMagnitudeHistogram(0, str.str());
                tsHist->SetDirectory(file);
                tsHist->Write();
                file->Close();
#endif
                return;
            }

            MEMBERVARIABLEREF(std::string, Filename);

        private:
            unsigned fHistCounter;
    };
}


using namespace Katydid;


int main()
{
    KTINFO(testlog, "Preparing");

    // Test parameters
    unsigned nFreqBins = 10;
    unsigned stripeSize = 8;
    unsigned stripeOverlap = 3;
    unsigned nAcquisitions = 3;
    unsigned slicesPerAcq = 15;
    std::string filename("TestSpectrogramStriper.root");

    // Set the necessary parameters in the header; time-in-run will be updated for each slice
    KTSliceHeader header;
    header.SetTimeInRun(0.);
    header.SetTimeInAcq(0.);
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
    printer.SetFilename(filename);

    // Connect the strip signal to the print-hist slot
    striper.ConnectASlot("str-fs-fftw", &printer, "print-hist");

    KTINFO(testlog, "Deleting the output file if it exists");
    boost::filesystem::remove(filename);

    KTINFO(testlog, "Everything is prepared; starting the action");

    unsigned peakPos = 0;
    for (unsigned iAcq = 0; iAcq < nAcquisitions; ++iAcq)
    {
        KTINFO(testlog, "New acquisition");
        header.SetIsNewAcquisition(true);
        header.SetTimeInAcq(0.);
        for (unsigned iSlice = 0; iSlice < slicesPerAcq; ++iSlice)
        {
            KTDEBUG(testlog, "New slice");
            spectrum->SetRect(peakPos, 10., spectrum->GetImag(peakPos));

            striper.AddData(header, spectrumData);

            // update data for the next slice
            spectrum->SetRect(peakPos, 0.0, spectrum->GetImag(peakPos));
            peakPos++;
            if (peakPos == nFreqBins) peakPos = 0;

            header.SetIsNewAcquisition(false);
            header.SetTimeInRun(header.GetTimeInRun() + header.GetSliceLength());
            header.SetTimeInAcq(header.GetTimeInAcq() + header.GetSliceLength());
        }
    }

    striper.OutputStripes();

    KTINFO(testlog, "Testing complete");

    return 0;
}

