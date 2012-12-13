/*
 * KTBasicROOTFileWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTFileWriter.hh"

#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include "KTSlidingWindowFSData.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTGainVariationData.hh"

#include "TH1.h"
#include "TH2.h"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedRegistrar< KTWriter, KTBasicROOTFileWriter > sBRFWriterRegistrar("basic-root-writer");
    static KTDerivedRegistrar< KTProcessor, KTBasicROOTFileWriter > sBRFWProcRegistrar("basic-root-writer");

    KTBasicROOTFileWriter::KTBasicROOTFileWriter() :
            KTWriter(),
            fFilename("basic_output.root"),
            fFileFlag("recreate"),
            fFile(NULL)
    {
        fConfigName = "basic-root-writer";

        RegisterSlot("write-data", this, &KTBasicROOTFileWriter::Publish);

        RegisterSlot("write-frequency-spectrum", this, &KTBasicROOTFileWriter::WriteFrequencySpectrumData);
        RegisterSlot("write-frequency-spectrum-fftw", this, &KTBasicROOTFileWriter::WriteFrequencySpectrumDataFFTW);
        RegisterSlot("write-correlation", this, &KTBasicROOTFileWriter::WriteCorrelationData);
        RegisterSlot("write-sliding-window-fs", this, &KTBasicROOTFileWriter::WriteSlidingWindowFSData);
        RegisterSlot("write-sliding-window-fs-fftw", this, &KTBasicROOTFileWriter::WriteSlidingWindowFSDataFFTW);
        RegisterSlot("write-gain-variation", this, &KTBasicROOTFileWriter::WriteGainVariationData);
    }

    KTBasicROOTFileWriter::~KTBasicROOTFileWriter()
    {
        if (fFile != NULL)
        {
            fFile->Close();
        }
        delete fFile;
    }

    Bool_t KTBasicROOTFileWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileFlag(node->GetData<string>("file-flag", fFileFlag));
        }

        // Command-line settings
        //SetFilename(fCLHandler->GetCommandLineValue< string >("broot-output-file", fTransformFlag));

        return true;
    }

    Bool_t KTBasicROOTFileWriter::OpenAndVerifyFile()
    {
        if (fFile == NULL)
        {
            fFile = new TFile(fFilename.c_str(), fFileFlag.c_str());
        }
        if (! fFile->IsOpen())
        {
            delete fFile;
            fFile = NULL;
            KTERROR(publog, "Output file <" << fFilename << "> did not open!");
            return false;
        }
        return true;
    }

    void KTBasicROOTFileWriter::Publish(const KTWriteableData* data)
    {
        data->Accept(this);
        return;
    }

    void KTBasicROOTFileWriter::Write(const KTWriteableData* data)
    {
        KTWARN(publog, "Generic Write function called; no data written");
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTFileWriter::Write(const KTTimeSeriesData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nChannels = data->GetNTimeSeries();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTTimeSeries* spectrum = data->GetTimeSeries(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTS_" << eventNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreateHistogram(histName);
                powerSpectrum->SetDirectory(fFile);
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTFileWriter::Write(const KTFrequencySpectrumData* data)
    {
        return WriteFrequencySpectrumData(data);
    }

    void KTBasicROOTFileWriter::WriteFrequencySpectrumData(const KTFrequencySpectrumData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nChannels = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrum* spectrum = data->GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPS_" << eventNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                powerSpectrum->SetDirectory(fFile);
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    void KTBasicROOTFileWriter::Write(const KTFrequencySpectrumDataFFTW* data)
    {
        return WriteFrequencySpectrumDataFFTW(data);
    }

    void KTBasicROOTFileWriter::WriteFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nChannels = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = data->GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPS_" << eventNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreatePowerHistogram(histName);
                powerSpectrum->SetDirectory(fFile);
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    //************************
    // Correlation Data
    //************************

    void KTBasicROOTFileWriter::Write(const KTCorrelationData* data)
    {
        return WriteCorrelationData(data);
    }

    void KTBasicROOTFileWriter::WriteCorrelationData(const KTCorrelationData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nPairs = data->GetNPairs();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iPair=0; iPair<nPairs; iPair++)
        {
            const KTFrequencySpectrum* spectrum = data->GetCorrelation(iPair);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histCorr_" << eventNumber << "_" << iPair;
                string histName;
                conv >> histName;
                TH1D* corrHist = spectrum->CreateMagnitudeHistogram(histName);
                stringstream titleStream;
                titleStream << "Event " << eventNumber << ", Correlation " << iPair << ", "
                        "Channels (" << data->GetFirstChannel(iPair) << ", " << data->GetSecondChannel(iPair) << ")";
                corrHist->SetTitle(titleStream.str().c_str());
                corrHist->SetDirectory(fFile);
                corrHist->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

    //************************
    // Sliding Window Data
    //************************

    void KTBasicROOTFileWriter::Write(const KTSlidingWindowFSData* data)
    {
        return WriteSlidingWindowFSData(data);
    }

    void KTBasicROOTFileWriter::WriteSlidingWindowFSData(const KTSlidingWindowFSData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nPlots = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histSW_" << eventNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = data->CreateMagnitudeHistogram(iPlot, histName);
            swHist->SetDirectory(fFile);
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

    void KTBasicROOTFileWriter::Write(const KTSlidingWindowFSDataFFTW* data)
    {
        return WriteSlidingWindowFSDataFFTW(data);
    }

    void KTBasicROOTFileWriter::WriteSlidingWindowFSDataFFTW(const KTSlidingWindowFSDataFFTW* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nPlots = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histSW_" << eventNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = data->CreateMagnitudeHistogram(iPlot, histName);
            swHist->SetDirectory(fFile);
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

    //************************
    // Hough Transform Data
    //************************

    void KTBasicROOTFileWriter::Write(const KTHoughData* data)
    {
        return WriteHoughData(data);
    }

    void KTBasicROOTFileWriter::WriteHoughData(const KTHoughData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nPlots = data->GetNTransforms();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histHT_" << eventNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH2D* swHist = data->CreateHistogram(iPlot, histName);
            swHist->SetDirectory(fFile);
            swHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
        }
        return;
    }

    //************************
    // Gain Variation Data
    //************************

    void KTBasicROOTFileWriter::Write(const KTGainVariationData* data)
    {
        return WriteGainVariationData(data);
    }

    void KTBasicROOTFileWriter::WriteGainVariationData(const KTGainVariationData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nPlots = data->GetNChannels();

        if (! OpenAndVerifyFile()) return;

        for (unsigned iPlot=0; iPlot<nPlots; iPlot++)
        {
            stringstream conv;
            conv << "histGV_" << eventNumber << "_" << iPlot;
            string histName;
            conv >> histName;
            TH1D* gvHist = data->CreateGainVariationHistogram(iPlot, histName);
            gvHist->SetDirectory(fFile);
            gvHist->Write();
            KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");

            stringstream conv2;
            string splineName;
            conv2 << "splineGV_" << eventNumber << "_" << iPlot;
            conv2 >> splineName;
            const TSpline* spline = data->GetSpline(iPlot);
            if (spline == NULL)
            {
                KTDEBUG(publog, "No spline in the data");
                continue;
            }
            TSpline* splineClone = (TSpline*)spline->Clone();
            splineClone->SetName(splineName.c_str());
            splineClone->Write();
            KTDEBUG(publog, "Spline <" << splineName << "> written to ROOT file");
        }
        return;
    }

} /* namespace Katydid */
