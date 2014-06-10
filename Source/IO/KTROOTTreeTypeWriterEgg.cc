/*
 * KTROOTTreeTypeWriterEgg.cc
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterEgg.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTAmplitudeDistribution.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

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
    }

    KTROOTTreeTypeWriterEgg::~KTROOTTreeTypeWriterEgg()
    {
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

        fEggHeaderData.fFilename = header->fFilename;
        fEggHeaderData.fAcquisitionMode = header->fAcquisitionMode;
        fEggHeaderData.fNChannels = header->fNChannels;
        fEggHeaderData.fRawSliceSize = header->fRawSliceSize;
        fEggHeaderData.fSliceSize = header->fSliceSize;
        fEggHeaderData.fRecordSize = header->fRecordSize;
        fEggHeaderData.fRunDuration = header->fRunDuration;
        fEggHeaderData.fAcquisitionRate = header->fAcquisitionRate;
        fEggHeaderData.fTimestamp = header->fTimestamp;
        fEggHeaderData.fDescription = header->fDescription;
        fEggHeaderData.fRunType = header->fRunType;
        fEggHeaderData.fRunSource = header->fRunSource;
        fEggHeaderData.fFormatMode = header->fFormatMode;
        fEggHeaderData.fDataTypeSize = header->fDataTypeSize;
        fEggHeaderData.fBitDepth = header->fBitDepth;
        fEggHeaderData.fVoltageMin = header->fVoltageMin;
        fEggHeaderData.fVoltageRange = header->fVoltageRange;

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

        fEggHeaderTree->Branch("Filename", &fEggHeaderData.fFilename, "fFilename/s");
        fEggHeaderTree->Branch("AcquisitionMode", &fEggHeaderData.fAcquisitionMode, "fAcquisitionMode/i");
        fEggHeaderTree->Branch("NChannels", &fEggHeaderData.fNChannels, "fNChannels/i");
        fEggHeaderTree->Branch("RawSliceSize", &fEggHeaderData.fRawSliceSize, "fRawSliceSize/i");
        fEggHeaderTree->Branch("SliceSize", &fEggHeaderData.fSliceSize, "fSliceSize/i");
        fEggHeaderTree->Branch("RecordSize", &fEggHeaderData.fRecordSize, "fRecordSize/i");
        fEggHeaderTree->Branch("RunDuration", &fEggHeaderData.fRunDuration, "fRunDuration/i");
        fEggHeaderTree->Branch("AcquisitionRate", &fEggHeaderData.fAcquisitionRate, "fAcquisitionRate/d");
        fEggHeaderTree->Branch("Timestamp", &fEggHeaderData.fTimestamp, "fTimestamp/s");
        fEggHeaderTree->Branch("Description", &fEggHeaderData.fDescription, "fDescription/s");
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



