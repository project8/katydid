/*
 * KTBasicROOTFilePublisher.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTFilePublisher.hh"

#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include "TH1.h"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    KTBasicROOTFilePublisher::KTBasicROOTFilePublisher() :
            KTPublisher(),
            fFilename("basic_output.root"),
            fFileFlag("recreate"),
            fFile(NULL)
    {
        fConfigName = "basic-root-publisher";

        RegisterSlot("write-data", this, &KTBasicROOTFilePublisher::Publish);
    }

    KTBasicROOTFilePublisher::~KTBasicROOTFilePublisher()
    {
        if (fFile != NULL)
        {
            fFile->Close();
        }
        delete fFile;
    }

    Bool_t KTBasicROOTFilePublisher::Configure(const KTPStoreNode* node)
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

    Bool_t KTBasicROOTFilePublisher::OpenAndVerifyFile()
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

    void KTBasicROOTFilePublisher::Publish(const KTWriteableData* data)
    {
        data->Accept(this);
        return;
    }

    void KTBasicROOTFilePublisher::Write(const KTWriteableData* data)
    {
        KTWARN(publog, "Generic Write function called; no data written");
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTFilePublisher::Write(const KTFrequencySpectrumData* data)
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

    //************************
    // Correlation Data
    //************************

    void KTBasicROOTFilePublisher::Write(const KTCorrelationData* data)
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
                titleStream << "Event << " << eventNumber << ", Correlation " << iPair << ", "
                        "Channels (" << data->GetFirstChannel(iPair) << ", " << data->GetSecondChannel(iPair) << ")";
                corrHist->SetTitle(titleStream.str().c_str());
                corrHist->SetDirectory(fFile);
                corrHist->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }


} /* namespace Katydid */
