/*
 * KTROOTTreeTypeWriterTime.cc
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "logger.hh"
#include "KTSliceHeader.hh"

#include "TFile.h"
#include "TTree.h"

#include <cstring>
#include <sstream>
#include "KTROOTTreeTypeWriterTime.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    LOGGER(publog, "KTROOTTreeTypeWriterTime");

    static Nymph::KTTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterTime > sRTTWCRegistrar;

    KTROOTTreeTypeWriterTime::KTROOTTreeTypeWriterTime() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterAnalysis()
            fEggHeaderTree(NULL),
            fChannelHeaderTree(NULL),
            fEggHeaderData(),
            fChannelHeaderData()
    {
        fEggHeaderData.fFilename = new TString();
        fEggHeaderData.fTimestamp = new TString();
        fEggHeaderData.fDescription = new TString();
        fChannelHeaderData.fSource = new TString();
    }

    KTROOTTreeTypeWriterTime::~KTROOTTreeTypeWriterTime()
    {
        delete fEggHeaderData.fFilename;
        delete fEggHeaderData.fTimestamp;
        delete fEggHeaderData.fDescription;
        delete fChannelHeaderData.fSource;
    }


    void KTROOTTreeTypeWriterTime::RegisterSlots()
    {
        fWriter->RegisterSlot("header", this, &KTROOTTreeTypeWriterTime::WriteEggHeader);
        return;
    }


    //*********************
    // Egg Header
    //*********************

    void KTROOTTreeTypeWriterTime::WriteEggHeader(Nymph::KTDataPtr headerPtr)
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        if (fEggHeaderTree == NULL)
        {
            if (! SetupEggHeaderTree())
            {
                LERROR(publog, "Something went wrong while setting up the egg header tree! Nothing was written.");
                return;
            }
        }

        if (fChannelHeaderTree == NULL)
        {
            if (! SetupChannelHeaderTree())
            {
                LERROR(publog, "Something went wrong while setting up the channel header tree! Nothing was written.");
                return;
            }
        }

        KTEggHeader& header = headerPtr->Of< KTEggHeader >();

        *fEggHeaderData.fFilename = header.GetFilename();
        LDEBUG(publog, "Writing egg header with filename <" << fEggHeaderData.fFilename << ">");
        fEggHeaderData.fCenterFrequency = header.GetCenterFrequency();
        fEggHeaderData.fMaximumFrequency = header.GetMaximumFrequency();
        fEggHeaderData.fMinimumFrequency = header.GetMinimumFrequency();
        fEggHeaderData.fAcquisitionMode = header.GetAcquisitionMode();
        fEggHeaderData.fNChannels = header.GetNChannels();
        fEggHeaderData.fRunDuration = header.GetRunDuration();
        fEggHeaderData.fAcquisitionRate = header.GetAcquisitionRate();
        *fEggHeaderData.fTimestamp = header.GetTimestamp();
        *fEggHeaderData.fDescription = header.GetDescription();

        fEggHeaderTree->Fill();

        for (unsigned iChan = 0; iChan < fEggHeaderData.fNChannels; ++iChan)
        {
            KTChannelHeader* chanHeader = header.GetChannelHeader(iChan);
            fChannelHeaderData.fNumber = chanHeader->GetNumber();
            *fChannelHeaderData.fSource = chanHeader->GetSource();
            fChannelHeaderData.fRawSliceSize = chanHeader->GetRawSliceSize();
            fChannelHeaderData.fSliceSize = chanHeader->GetSliceSize();
            fChannelHeaderData.fSliceStride = chanHeader->GetSliceStride();
            fChannelHeaderData.fRecordSize = chanHeader->GetRecordSize();
            fChannelHeaderData.fSampleSize = chanHeader->GetSampleSize();
            fChannelHeaderData.fDataTypeSize = chanHeader->GetDataTypeSize();
            fChannelHeaderData.fBitDepth = chanHeader->GetBitDepth();
            fChannelHeaderData.fVoltageOffset = chanHeader->GetVoltageOffset();
            fChannelHeaderData.fVoltageRange = chanHeader->GetVoltageRange();
            fChannelHeaderData.fDACGain = chanHeader->GetDACGain();

            fChannelHeaderTree->Fill();
        }

        return;
    }

    bool KTROOTTreeTypeWriterTime::SetupEggHeaderTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "eggHeader", fEggHeaderTree );

            if( fEggHeaderTree != NULL )
            {
                LINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fEggHeaderTree );

                fEggHeaderTree->SetBranchAddress("Filename", &fEggHeaderData.fFilename);
                fEggHeaderTree->SetBranchAddress("CenterFrequency", &fEggHeaderData.fCenterFrequency);
                fEggHeaderTree->SetBranchAddress("MaximumFrequency", &fEggHeaderData.fMaximumFrequency);
                fEggHeaderTree->SetBranchAddress("MinimumFrequency", &fEggHeaderData.fMinimumFrequency);
                fEggHeaderTree->SetBranchAddress("AcquisitionMode", &fEggHeaderData.fAcquisitionMode);
                fEggHeaderTree->SetBranchAddress("NChannels", &fEggHeaderData.fNChannels);
                fEggHeaderTree->SetBranchAddress("RunDuration", &fEggHeaderData.fRunDuration); // in ms
                fEggHeaderTree->SetBranchAddress("AcquisitionRate", &fEggHeaderData.fAcquisitionRate); // in Hz
                fEggHeaderTree->SetBranchAddress("Timestamp", &fEggHeaderData.fTimestamp);
                fEggHeaderTree->SetBranchAddress("Description", &fEggHeaderData.fDescription);

                return true;
            }
        }

        fEggHeaderTree = new TTree("eggHeader", "Egg Header");
        if (fEggHeaderTree == NULL)
        {
            LERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fEggHeaderTree);

        fEggHeaderTree->Branch("Filename", "TString", &fEggHeaderData.fFilename);
        fEggHeaderTree->Branch("CenterFrequency", &fEggHeaderData.fCenterFrequency, "fCenterFrequency/d");
        fEggHeaderTree->Branch("MaximumFrequency", &fEggHeaderData.fMaximumFrequency, "fMaximumFrequency/d");
        fEggHeaderTree->Branch("MinimumFrequency", &fEggHeaderData.fMinimumFrequency, "fMinimumFrequency/d");
        fEggHeaderTree->Branch("AcquisitionMode", &fEggHeaderData.fAcquisitionMode, "fAcquisitionMode/i");
        fEggHeaderTree->Branch("NChannels", &fEggHeaderData.fNChannels, "fNChannels/i");
        fEggHeaderTree->Branch("RunDuration", &fEggHeaderData.fRunDuration, "fRunDuration/i"); // in ms
        fEggHeaderTree->Branch("AcquisitionRate", &fEggHeaderData.fAcquisitionRate, "fAcquisitionRate/d"); // in Hz
        fEggHeaderTree->Branch("Timestamp", "TString", &fEggHeaderData.fTimestamp);
        fEggHeaderTree->Branch("Description", "TString", &fEggHeaderData.fDescription);

        return true;
    }

    bool KTROOTTreeTypeWriterTime::SetupChannelHeaderTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "channelHeader", fChannelHeaderTree );

            if( fChannelHeaderTree != NULL )
            {
                LINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fChannelHeaderTree );

                fChannelHeaderTree->SetBranchAddress("Number", &fChannelHeaderData.fNumber);
                fChannelHeaderTree->SetBranchAddress("Source", &fChannelHeaderData.fSource);
                fChannelHeaderTree->SetBranchAddress("RawSliceSize", &fChannelHeaderData.fRawSliceSize);
                fChannelHeaderTree->SetBranchAddress("SliceSize", &fChannelHeaderData.fSliceSize);
                fChannelHeaderTree->SetBranchAddress("SliceStride", &fChannelHeaderData.fSliceStride);
                fChannelHeaderTree->SetBranchAddress("RecordSize", &fChannelHeaderData.fRecordSize);
                fChannelHeaderTree->SetBranchAddress("SampleSize", &fChannelHeaderData.fSampleSize);
                fChannelHeaderTree->SetBranchAddress("DataTypeSize", &fChannelHeaderData.fDataTypeSize);
                fChannelHeaderTree->SetBranchAddress("BitDepth", &fChannelHeaderData.fBitDepth);
                fChannelHeaderTree->SetBranchAddress("VoltageOffset", &fChannelHeaderData.fVoltageOffset);
                fChannelHeaderTree->SetBranchAddress("VoltageRange", &fChannelHeaderData.fVoltageRange);
                fChannelHeaderTree->SetBranchAddress("DACGain", &fChannelHeaderData.fDACGain);

                return true;
            }
        }

        fChannelHeaderTree = new TTree("channelHeader", "Channel Headers");
        if (fChannelHeaderTree == NULL)
        {
            LERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fChannelHeaderTree);

        fChannelHeaderTree->Branch("Number", &fChannelHeaderData.fNumber, "fNumber/i");
        fChannelHeaderTree->Branch("Source", "TString", &fChannelHeaderData.fSource);
        fChannelHeaderTree->Branch("RawSliceSize", &fChannelHeaderData.fRawSliceSize, "fRawSliceSize/i");
        fChannelHeaderTree->Branch("SliceSize", &fChannelHeaderData.fSliceSize, "fSliceSize/i");
        fChannelHeaderTree->Branch("SliceStride", &fChannelHeaderData.fSliceStride, "fSliceStride/i");
        fChannelHeaderTree->Branch("RecordSize", &fChannelHeaderData.fRecordSize, "fRecordSize/i");
        fChannelHeaderTree->Branch("SampleSize", &fChannelHeaderData.fSampleSize, "fSampleSize/i");
        fChannelHeaderTree->Branch("DataTypeSize", &fChannelHeaderData.fDataTypeSize, "fDataTypeSize/i");
        fChannelHeaderTree->Branch("BitDepth", &fChannelHeaderData.fBitDepth, "fBitDepth/i");
        fChannelHeaderTree->Branch("VoltageOffset", &fChannelHeaderData.fVoltageOffset, "fVoltageOffset/d");
        fChannelHeaderTree->Branch("VoltageRange", &fChannelHeaderData.fVoltageRange, "fVoltageRange/d");
        fChannelHeaderTree->Branch("DACGain", &fChannelHeaderData.fDACGain, "fDACGAin/d");

        return true;
    }

} /* namespace Katydid */



