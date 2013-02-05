/*
 * KTROOTImageWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTROOTImageWriter.hh"

#include "KTBundle.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include "KTSlidingWindowFSData.hh"
#include "KTSlidingWindowFSDataFFTW.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedRegistrar< KTWriter, KTROOTImageWriter > sBRFWriterRegistrar("root-image");
    static KTDerivedRegistrar< KTProcessor, KTROOTImageWriter > sBRFWProcRegistrar("root-image");

    KTROOTImageWriter::KTROOTImageWriter() :
            KTWriter(),
            fFilenameBaseFrequencySpectrum("frequency_spectrum_")
    {
        fConfigName = "root-image";

        RegisterSlot("write-data", this, &KTROOTImageWriter::Publish);
    }

    KTROOTImageWriter::~KTROOTImageWriter()
    {
    }

    Bool_t KTROOTImageWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilenameBaseFrequencySpectrum(node->GetData<string>("filename-base-frequency-spectrum", fFilenameBaseFrequencySpectrum));
        }

        // Command-line settings
        //SetFilename(fCLHandler->GetCommandLineValue< string >("broot-output-file", fTransformFlag));

        return true;
    }

    void KTROOTImageWriter::Publish(const KTWriteableData* data)
    {
        data->Accept(this);
        return;
    }

    void KTROOTImageWriter::Write(const KTWriteableData* data)
    {
        KTWARN(publog, "Generic Write function called; no data written");
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTROOTImageWriter::Write(const KTFrequencySpectrumData* data)
    {
        /*
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nChannels = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrum* spectrum = data->GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPS_" << bundleNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                powerSpectrum->SetDirectory(fFile);
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        */
        return;
    }

    void KTROOTImageWriter::Write(const KTFrequencySpectrumDataFFTW* data)
    {
        /*
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nChannels = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = data->GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPS_" << bundleNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                powerSpectrum->SetDirectory(fFile);
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        */
        return;
    }

    //************************
    // Correlation Data
    //************************

    void KTROOTImageWriter::Write(const KTCorrelationData* data)
    {
        /*
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nPairs = data->GetNPairs();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nPairs; iPair++)
        {
            const KTFrequencySpectrum* spectrum = data->GetCorrelation(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histCorr_" << bundleNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* corrHist = spectrum->CreateMagnitudeHistogram(histName);
                stringstream titleStream;
                titleStream << "Bundle " << bundleNumber << ", Correlation " << iPair << ", "
                        "Channels (" << data->GetFirstChannel(iPair) << ", " << data->GetSecondChannel(iPair) << ")";
                corrHist->SetTitle(titleStream.str().c_str());
                corrHist->SetDirectory(fFile);
                corrHist->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        */
        return;
    }

    //************************
    // Sliding Window Data
    //************************

    void KTROOTImageWriter::Write(const KTSlidingWindowFSData* data)
    {
        /*
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nPlots = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histSW_" << bundleNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = data->CreateMagnitudeHistogram(iPlot, histName);
            swHist->SetDirectory(fFile);
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        */
        return;
    }

    void KTROOTImageWriter::Write(const KTSlidingWindowFSDataFFTW* data)
    {
        /*
        KTBundle* bundle = data->GetBundle();
        UInt_t bundleNumber = 0;
        if (bundle != NULL) bundleNumber = bundle->GetBundleNumber();
        UInt_t nPlots = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histSW_" << bundleNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = data->CreateMagnitudeHistogram(iPlot, histName);
            swHist->SetDirectory(fFile);
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        */
        return;
    }



} /* namespace Katydid */
