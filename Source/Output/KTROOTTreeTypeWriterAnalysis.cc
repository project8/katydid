/*
 * KTROOTTreeTypeWriterAnalysis.cc
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterAnalysis.hh"

#include "KTAmplitudeDistribution.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"

#include "TFile.h"
#include "TH1.h"
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
            fAmpDistTree(NULL),
            fAmpDistData()
    {
    }

    KTROOTTreeTypeWriterAnalysis::~KTROOTTreeTypeWriterAnalysis()
    {
    }


    void KTROOTTreeTypeWriterAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("amp-dist", this, &KTROOTTreeTypeWriterAnalysis::WriteAmplitudeDistributions);
        return;
    }


    //**************************
    // Amplitude Distribution
    //**************************

    void KTROOTTreeTypeWriterAnalysis::WriteAmplitudeDistributions(shared_ptr< KTData > data)
    {
        KTAmplitudeDistribution& fcData = data->Of< KTAmplitudeDistribution >();
        KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fAmpDistTree == NULL)
        {
            if (! SetupAmplitudeDistributionTree())
            {
                KTERROR(publog, "Something went wrong while setting up the frequency candidate tree! Nothing was written.");
                return;
            }
        }

        // Load() also clears any existing data
        //fAmpDistData->Load(*data);
        fAmpDistData.fSlice = header.GetSliceNumber();
        fAmpDistData.fTimeInRun = header.GetTimeInRun();
        for (fAmpDistData.fComponent = 0; fAmpDistData.fComponent < fcData.GetNComponents(); fAmpDistData.fComponent++)
        {
            fAmpDistData.fThreshold = fcData.GetThreshold(fAmpDistData.fComponent);
            const KTAmplitudeDistributionData::Analysis& candidates = fcData.GetAnalysis(fAmpDistData.fComponent);
            for (KTAmplitudeDistributionData::Analysis::const_iterator it = candidates.begin(); it != candidates.end(); it++)
            {
                fAmpDistData.fFirstBin = it->GetFirstBin();
                fAmpDistData.fLastBin = it->GetLastBin();
                fAmpDistData.fMeanFrequency = it->GetMeanFrequency();
                fAmpDistData.fPeakAmplitude = it->GetPeakAmplitude();
                fAmpDistData.fAmplitudeSum = it->GetAmplitudeSum();

                fAmpDistTree->Fill();
           }
        }

        return;
    }

    Bool_t KTROOTTreeTypeWriterAnalysis::SetupAmplitudeDistributionTree()
    {
        fAmpDistTree = new TTree("freqCand", "Frequency Analysis");
        if (fAmpDistTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fAmpDistTree);

        fAmpDistTree->Branch("Component", &fAmpDistData.fComponent, "fComponent/s");
        fAmpDistTree->Branch("FreqBin", &fAmpDistData.fFreqBin, "fFreqBin/l");
        fAmpDistTree->Branch("Distribution", &fAmpDistTree.fDistribution, 32000, 0);

        return true;
    }

} /* namespace Katydid */



