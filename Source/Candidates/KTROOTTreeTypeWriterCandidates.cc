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

//#include "TFrequencyCandidateData.hh"

#include "TFile.h"
#include "TTree.h"

#include <sstream>

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
            fFreqCandidateData()
    {
    }

    KTROOTTreeTypeWriterCandidates::~KTROOTTreeTypeWriterCandidates()
    {
        //delete fFreqCandidateTree;
        //delete fFreqCandidateData;
    }


    void KTROOTTreeTypeWriterCandidates::RegisterSlots()
    {
        fWriter->RegisterSlot("frequency-candidates", this, &KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates, "void (const KTFrequencyCandidateData*)");
        return;
    }


    //*********************
    // Frequency Candidates
    //*********************

    void KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates(const KTFrequencyCandidateData* data)
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        if (fFreqCandidateTree == NULL) SetupFrequencyCandidateTree();

        // Load() also clears any existing data
        //fFreqCandidateData->Load(*data);
        fFreqCandidateData.fSlice = data->GetSliceNumber();
        fFreqCandidateData.fTimeInRun = data->GetTimeInRun();
        for (fFreqCandidateData.fComponent = 0; fFreqCandidateData.fComponent < data->GetNGroups(); fFreqCandidateData.fComponent++)
        {
            fFreqCandidateData.fThreshold = data->GetThreshold(fFreqCandidateData.fComponent);
            const KTFrequencyCandidateData::Candidates& candidates = data->GetCandidates(fFreqCandidateData.fComponent);
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

} /* namespace Katydid */



