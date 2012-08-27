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

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    KTBasicROOTFilePublisher::KTBasicROOTFilePublisher() :
            KTPublisher(),
            fFilename("output.root"),
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
        //SetTransformFlag(fCLHandler->GetCommandLineValue< string >("transform-flag", fTransformFlag));

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

    void KTBasicROOTFilePublisher::Publish(KTWriteableData* data)
    {
        std::cout << "publisher's publish" << std::endl;
        data->Accept(this);
        return;
    }

    void KTBasicROOTFilePublisher::Write(KTWriteableData* data)
    {
        std::cout << "basicrootfilepublisher's generic write" << std::endl;
        return;
    }


    //************************
    // Frequency Spectrum Data
    //************************

    void KTBasicROOTFilePublisher::Write(KTFrequencySpectrumData* data)
    {
        std::cout << "basicrootfilepublisher's write for frequencyspectrumdata" << std::endl;
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nChannels = data->GetNChannels();

        if (! OpenAndVerifyFile())
        {
            return;
        }

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrum* spectrum = data->GetSpectrum(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histPS_" << eventNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                std::cout << "hist name: <" << histName << ">" << std::endl;
                TH1D* powerSpectrum = CreatePowerSpectrumHistFromFreqSpect(histName, spectrum);
                powerSpectrum->SetDirectory(fFile);
                powerSpectrum->Write();
            }
        }
        return;
    }



} /* namespace Katydid */
