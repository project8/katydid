/*
 * KTROOTTreeTypeWriterAnalysis.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterAnalysis.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTWaterfallCandidateData.hh"

#include "TFile.h"
#include "TTree.h"

#include <sstream>

using boost::shared_ptr;

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterAnalysis > sRTTWCRegistrar;

    KTROOTTreeTypeWriterAnalysis::KTROOTTreeTypeWriterAnalysis() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterAnalysis()
            fDiscPoints1DTree(NULL),
            fDiscPoints1DData()
    {
    }

    KTROOTTreeTypeWriterAnalysis::~KTROOTTreeTypeWriterAnalysis()
    {
        //delete fDiscPoints1DTree;
        //delete fDiscPoints1DData;
    }


    void KTROOTTreeTypeWriterAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("disc-1d", this, &KTROOTTreeTypeWriterAnalysis::WriteDiscriminatedPoints1D);
        return;
    }


    //*********************
    // Frequency Analysis
    //*********************

    void KTROOTTreeTypeWriterAnalysis::WriteDiscriminatedPoints1D(shared_ptr< KTData > data)
    {
        KTDiscriminatedPoints1DData& fcData = data->Of< KTDiscriminatedPoints1DData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fDiscPoints1DTree == NULL)
        {
            if (! SetupDiscriminatedPoints1DTree())
            {
                KTERROR(publog, "Something went wrong while setting up the frequency candidate tree! Nothing was written.");
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

    Bool_t KTROOTTreeTypeWriterAnalysis::SetupDiscriminatedPoints1DTree()
    {
        fDiscPoints1DTree = new TTree("discPoints1D", "Discriminated Points 1D");
        if (fDiscPoints1DTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fDiscPoints1DTree);

        //fDiscPoints1DData = new TDiscriminatedPoints1DData();

        fDiscPoints1DTree->Branch("Component", &fDiscPoints1DData.fComponent, "fComponent/s");
        fDiscPoints1DTree->Branch("Point", &fDiscPoints1DData.fPoint, "fSlice/i");
        fDiscPoints1DTree->Branch("Value", &fDiscPoints1DData.fValue, "fTimeInRun/d");
        fDiscPoints1DTree->Branch("Threshold", &fDiscPoints1DData.fThreshold, "fThreshold/d");
        //fDiscPoints1DTree->Branch("freqAnalysis", &fDiscPoints1DData.fComponent, "fComponent/s:fSlice/l:fTimeInRun/d:fThreshold/d:fFirstBin/i:fLastBin/i:fMeanFrequency/d:fPeakAmplitude/d");

        return true;
    }

} /* namespace Katydid */



