/*
 * KTROOTTreeTypeWriterCandidates.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterCandidates.hh"

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTWaterfallCandidateData.hh"

//#include "TFrequencyCandidateData.hh"

#include "TFile.h"
#include "TH2.h"
#include "TTree.h"

#include <sstream>

using boost::shared_ptr;

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterCandidates > sRTTWCRegistrar;

    KTROOTTreeTypeWriterCandidates::KTROOTTreeTypeWriterCandidates() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterCandidates()
            fFreqCandidateTree(NULL),
            fWaterfallCandidateTree(NULL),
            fFreqCandidateData(),
            fWaterfallCandidateData()
    {
    }

    KTROOTTreeTypeWriterCandidates::~KTROOTTreeTypeWriterCandidates()
    {
        //delete fFreqCandidateTree;
        //delete fFreqCandidateData;
    }


    void KTROOTTreeTypeWriterCandidates::RegisterSlots()
    {
        fWriter->RegisterSlot("frequency-candidates", this, &KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates);
        fWriter->RegisterSlot("waterfall-candidates", this, &KTROOTTreeTypeWriterCandidates::WriteWaterfallCandidate);
        return;
    }


    //*********************
    // Frequency Candidates
    //*********************

    void KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates(shared_ptr< KTData > data)
    {
        KTFrequencyCandidateData& fcData = data->Of< KTFrequencyCandidateData >();
        KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fFreqCandidateTree == NULL) SetupFrequencyCandidateTree();

        // Load() also clears any existing data
        //fFreqCandidateData->Load(*data);
        fFreqCandidateData.fSlice = header.GetSliceNumber();
        fFreqCandidateData.fTimeInRun = header.GetTimeInRun();
        for (fFreqCandidateData.fComponent = 0; fFreqCandidateData.fComponent < fcData.GetNComponents(); fFreqCandidateData.fComponent++)
        {
            fFreqCandidateData.fThreshold = fcData.GetThreshold(fFreqCandidateData.fComponent);
            const KTFrequencyCandidateData::Candidates& candidates = fcData.GetCandidates(fFreqCandidateData.fComponent);
            for (KTFrequencyCandidateData::Candidates::const_iterator it = candidates.begin(); it != candidates.end(); it++)
            {
                fFreqCandidateData.fFirstBin = it->GetFirstBin();
                fFreqCandidateData.fLastBin = it->GetLastBin();
                fFreqCandidateData.fMeanFrequency = it->GetMeanFrequency();
                fFreqCandidateData.fPeakAmplitude = it->GetPeakAmplitude();
                fFreqCandidateData.fAmplitudeSum = it->GetAmplitudeSum();

                fFreqCandidateTree->Fill();
           }
        }

        return;
    }

    void KTROOTTreeTypeWriterCandidates::SetupFrequencyCandidateTree()
    {
        fFreqCandidateTree = new TTree("freqCand", "Frequency Candidates");
        fWriter->AddTree(fFreqCandidateTree);

        //fFreqCandidateData = new TFrequencyCandidateData();

        //fFreqCandidateTree->Branch("freqCandidates", "Katydid::TFrequencyCandidateData", &fFreqCandidateData);
        fFreqCandidateTree->Branch("Component", &fFreqCandidateData.fComponent, "fComponent/s");
        fFreqCandidateTree->Branch("Slice", &fFreqCandidateData.fSlice, "fSlice/l");
        fFreqCandidateTree->Branch("TimeInRun", &fFreqCandidateData.fTimeInRun, "fTimeInRun/d");
        fFreqCandidateTree->Branch("Threshold", &fFreqCandidateData.fThreshold, "fThreshold/d");
        fFreqCandidateTree->Branch("FirstBin", &fFreqCandidateData.fFirstBin, "fFirstBin/i");
        fFreqCandidateTree->Branch("LastBin", &fFreqCandidateData.fLastBin, "fLastBin/i");
        fFreqCandidateTree->Branch("MeanFrequency", &fFreqCandidateData.fMeanFrequency, "fMeanFrequency/d");
        fFreqCandidateTree->Branch("PeakAmplitude", &fFreqCandidateData.fPeakAmplitude, "fPeakAmplitude/d");
        fFreqCandidateTree->Branch("AmplitudeSum", &fFreqCandidateData.fAmplitudeSum, "fAmplitudeSum/d");
        //fFreqCandidateTree->Branch("freqCandidates", &fFreqCandidateData.fComponent, "fComponent/s:fSlice/l:fTimeInRun/d:fThreshold/d:fFirstBin/i:fLastBin/i:fMeanFrequency/d:fPeakAmplitude/d");

        return;
    }

    //*********************
    // Waterfall Candidates
    //*********************

    void KTROOTTreeTypeWriterCandidates::WriteWaterfallCandidate(shared_ptr< KTData > data)
    {
        KTDEBUG(publog, "attempting to write to root tree");
        KTWaterfallCandidateData& wcData = data->Of< KTWaterfallCandidateData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fWaterfallCandidateTree == NULL) SetupWaterfallCandidateTree();

        // Load() also clears any existing data
        //fFreqCandidateData->Load(*data);
        fWaterfallCandidateData.fComponent = wcData.GetComponent();
        fWaterfallCandidateData.fTimeInRun = wcData.GetTimeInRun();
        fWaterfallCandidateData.fTimeLength = wcData.GetTimeLength();
        fWaterfallCandidateData.fFirstSliceNumber = wcData.GetFirstSliceNumber();
        fWaterfallCandidateData.fLastSliceNumber = wcData.GetLastSliceNumber();
        fWaterfallCandidateData.fFrequencyWidth = wcData.GetFrequencyWidth();
        fWaterfallCandidateData.fCandidate = wcData.GetCandidate()->CreatePowerHistogram();

        return;
    }

    void KTROOTTreeTypeWriterCandidates::SetupWaterfallCandidateTree()
    {
        fWaterfallCandidateTree = new TTree("freqCand", "Frequency Candidates");
        fWriter->AddTree(fFreqCandidateTree);

        fWaterfallCandidateTree->Branch("Component", &fWaterfallCandidateData.fComponent, "fComponent/s");
        fWaterfallCandidateTree->Branch("TimeInRun", &fWaterfallCandidateData.fTimeInRun, "fTimeInRun/d");
        fWaterfallCandidateTree->Branch("TimeLength", &fWaterfallCandidateData.fTimeLength, "fTimeLength/d");
        fWaterfallCandidateTree->Branch("FirstSlice", &fWaterfallCandidateData.fFirstSliceNumber, "fFirstSliceNumber/l");
        fWaterfallCandidateTree->Branch("LastSlice", &fWaterfallCandidateData.fLastSliceNumber, "fLastSliceNumber/l");
        fWaterfallCandidateTree->Branch("FrequencyWidth", &fWaterfallCandidateData.fFrequencyWidth, "fFrequencyWidth/d");
        fWaterfallCandidateTree->Branch("Candidate", &fWaterfallCandidateData.fCandidate, 32000, 0);

        return;
    }


} /* namespace Katydid */



