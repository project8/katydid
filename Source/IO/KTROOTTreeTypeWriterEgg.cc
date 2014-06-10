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

        for (fDiscPoints1DData.fComponent = 0; fDiscPoints1DData.fComponent < fcData.GetNComponents(); fDiscPoints1DData.fComponent++)
        {
            fDiscPoints1DData.fThreshold = fcData.GetThreshold(fDiscPoints1DData.fComponent);
            const KTDiscriminatedPoints1DData::SetOfPoints& points = fcData.GetSetOfPoints(fDiscPoints1DData.fComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = points.begin(); it != points.end(); it++)
            {
                fDiscPoints1DData.fPoint = it->first;
                fDiscPoints1DData.fValue = it->second;

                fDiscPoints1DTree->Fill();
           }
        }

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

        //fDiscPoints1DData = new TDiscriminatedPoints1DData();

        fDiscPoints1DTree->Branch("Component", &fDiscPoints1DData.fComponent, "fComponent/s");
        fDiscPoints1DTree->Branch("Point", &fDiscPoints1DData.fPoint, "fSlice/i");
        fDiscPoints1DTree->Branch("Value", &fDiscPoints1DData.fValue, "fTimeInRun/d");
        fDiscPoints1DTree->Branch("Threshold", &fDiscPoints1DData.fThreshold, "fThreshold/d");
        //fDiscPoints1DTree->Branch("freqAnalysis", &fDiscPoints1DData.fComponent, "fComponent/s:fSlice/l:fTimeInRun/d:fThreshold/d:fFirstBin/i:fLastBin/i:fMeanFrequency/d:fPeakAmplitude/d");

        return true;
    }

} /* namespace Katydid */



