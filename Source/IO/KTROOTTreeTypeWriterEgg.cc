/*
 * KTROOTTreeTypeWriterEgg.cc
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterEgg.hh"

#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"

#include "TFile.h"
#include "TTree.h"

#include <cstring>
#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTTreeTypeWriterEgg");

    static KTTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterEgg > sRTTWCRegistrar;

    KTROOTTreeTypeWriterEgg::KTROOTTreeTypeWriterEgg() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterAnalysis()
            fEggHeaderTree(NULL),
            fEggHeaderData()
    {
        fEggHeaderData.fFilename = new TString();
        fEggHeaderData.fTimestamp = new TString();
        fEggHeaderData.fDescription = new TString();
    }

    KTROOTTreeTypeWriterEgg::~KTROOTTreeTypeWriterEgg()
    {
        delete fEggHeaderData.fFilename;
        delete fEggHeaderData.fTimestamp;
        delete fEggHeaderData.fDescription;
    }


    void KTROOTTreeTypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("header", this, &KTROOTTreeTypeWriterEgg::WriteEggHeader);
        return;
    }


    //*********************
    // Egg Header
    //*********************

    void KTROOTTreeTypeWriterEgg::WriteEggHeader(KTEggHeader* header)
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        if (fEggHeaderTree == NULL)
        {
            if (! SetupEggHeaderTree())
            {
                KTERROR(publog, "Something went wrong while setting up the egg header tree! Nothing was written.");
                return;
            }
        }

        *fEggHeaderData.fFilename = header->GetFilename();
        KTDEBUG(publog, "Writing egg header with filename <" << fEggHeaderData.fFilename << ">");
        fEggHeaderData.fAcquisitionMode = header->GetAcquisitionMode();
        fEggHeaderData.fNChannels = header->GetNChannels();
        fEggHeaderData.fRawSliceSize = header->GetRawSliceSize();
        fEggHeaderData.fSliceSize = header->GetSliceSize();
        fEggHeaderData.fSliceStride = header->GetSliceStride();
        fEggHeaderData.fRecordSize = header->GetRecordSize();
        fEggHeaderData.fRunDuration = header->GetRunDuration();
        fEggHeaderData.fAcquisitionRate = header->GetAcquisitionRate();
        *fEggHeaderData.fTimestamp = header->GetTimestamp();
        *fEggHeaderData.fDescription = header->GetDescription();
        fEggHeaderData.fRunType = header->GetRunType();
        fEggHeaderData.fRunSource = header->GetRunSource();
        fEggHeaderData.fFormatMode = header->GetFormatMode();
        fEggHeaderData.fDataTypeSize = header->GetDataTypeSize();
        fEggHeaderData.fBitDepth = header->GetBitDepth();
        fEggHeaderData.fVoltageMin = header->GetVoltageMin();
        fEggHeaderData.fVoltageRange = header->GetVoltageRange();

        fEggHeaderTree->Fill();

        return;
    }

    bool KTROOTTreeTypeWriterEgg::SetupEggHeaderTree()
    {
        fEggHeaderTree = new TTree("eggHeader", "Egg Header");
        if (fEggHeaderTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fEggHeaderTree);

        fEggHeaderTree->Branch("Filename", "TString", &fEggHeaderData.fFilename);
        fEggHeaderTree->Branch("AcquisitionMode", &fEggHeaderData.fAcquisitionMode, "fAcquisitionMode/i");
        fEggHeaderTree->Branch("NChannels", &fEggHeaderData.fNChannels, "fNChannels/i");
        fEggHeaderTree->Branch("RawSliceSize", &fEggHeaderData.fRawSliceSize, "fRawSliceSize/i");
        fEggHeaderTree->Branch("SliceSize", &fEggHeaderData.fSliceSize, "fSliceSize/i");
        fEggHeaderTree->Branch("SliceStride", &fEggHeaderData.fSliceStride, "fSliceStride/i");
        fEggHeaderTree->Branch("RecordSize", &fEggHeaderData.fRecordSize, "fRecordSize/i");
        fEggHeaderTree->Branch("RunDuration", &fEggHeaderData.fRunDuration, "fRunDuration/i");
        fEggHeaderTree->Branch("AcquisitionRate", &fEggHeaderData.fAcquisitionRate, "fAcquisitionRate/d");
        fEggHeaderTree->Branch("Timestamp", "TString", &fEggHeaderData.fTimestamp);
        fEggHeaderTree->Branch("Description", "TString", &fEggHeaderData.fDescription);
        fEggHeaderTree->Branch("RunType", &fEggHeaderData.fRunType, "fRunType/i");
        fEggHeaderTree->Branch("RunSource", &fEggHeaderData.fRunSource, "fRunSource/i");
        fEggHeaderTree->Branch("FormatMode", &fEggHeaderData.fFormatMode, "fFormatMode/i");
        fEggHeaderTree->Branch("DataTypeSize", &fEggHeaderData.fDataTypeSize, "fDataTypeSize/i");
        fEggHeaderTree->Branch("BitDepth", &fEggHeaderData.fBitDepth, "fBitDepth/i");
        fEggHeaderTree->Branch("VoltageMin", &fEggHeaderData.fVoltageMin, "fVoltageMin/d");
        fEggHeaderTree->Branch("VoltageRange", &fEggHeaderData.fVoltageRange, "fVoltageRange/d");

        return true;
    }

} /* namespace Katydid */



