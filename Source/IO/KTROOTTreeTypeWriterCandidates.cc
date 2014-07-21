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
#include "KTSparseWaterfallCandidateData.hh"
#include "KTWaterfallCandidateData.hh"

//#include "TFrequencyCandidateData.hh"

#include "TFile.h"
#include "TGraph2D.h"
#include "TH2.h"
#include "TTree.h"

#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTTreeTypeWriterCandidates");

    static KTTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterCandidates > sRTTWCRegistrar;

    KTROOTTreeTypeWriterCandidates::KTROOTTreeTypeWriterCandidates() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterCandidates()
            fFreqCandidateTree(NULL),
            fWaterfallCandidateTree(NULL),
            fSparseWaterfallCandidateTree(NULL),
            fFreqCandidateData(),
            fWaterfallCandidateData(),
            fSparseWaterfallCandidateData()
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
        fWriter->RegisterSlot("sparse-waterfall-candidates", this, &KTROOTTreeTypeWriterCandidates::WriteSparseWaterfallCandidate);
        return;
    }


    //*********************
    // Frequency Candidates
    //*********************

    void KTROOTTreeTypeWriterCandidates::WriteFrequencyCandidates(KTDataPtr data)
    {
        KTFrequencyCandidateData& fcData = data->Of< KTFrequencyCandidateData >();
        KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fFreqCandidateTree == NULL)
        {
            if (! SetupFrequencyCandidateTree())
            {
                KTERROR(publog, "Something went wrong while setting up the frequency candidate tree! Nothing was written.");
                return;
            }
        }

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

    bool KTROOTTreeTypeWriterCandidates::SetupFrequencyCandidateTree()
    {
        fFreqCandidateTree = new TTree("freqCand", "Frequency Candidates");
        if (fFreqCandidateTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
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

        return true;
    }

    //*********************
    // Waterfall Candidates
    //*********************

    void KTROOTTreeTypeWriterCandidates::WriteWaterfallCandidate(KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to waterfall candidate root tree");
        KTWaterfallCandidateData& wcData = data->Of< KTWaterfallCandidateData >();

        if (! fWriter->OpenAndVerifyFile())
        {
            KTDEBUG(publog, "unable to verify file");
            return;
        }

        if (fWaterfallCandidateTree == NULL)
        {
            if (! SetupWaterfallCandidateTree())
            {
                KTERROR(publog, "Something went wrong while setting up the waterfall candidate tree! Nothing was written.");
                return;
            } else {
                KTDEBUG(publog, "waterfall candidate tree created");
            }
        }

        // Load() also clears any existing data
        //fFreqCandidateData->Load(*data);
        fWaterfallCandidateData.fComponent = wcData.GetComponent();
        fWaterfallCandidateData.fTimeInRun = wcData.GetTimeInRun();
        fWaterfallCandidateData.fTimeLength = wcData.GetTimeLength();
        fWaterfallCandidateData.fFirstSliceNumber = wcData.GetFirstSliceNumber();
        fWaterfallCandidateData.fLastSliceNumber = wcData.GetLastSliceNumber();
        fWaterfallCandidateData.fMinFrequency = wcData.GetMinimumFrequency();
        fWaterfallCandidateData.fMaxFrequency = wcData.GetMaximumFrequency();
        fWaterfallCandidateData.fMeanStartFrequency = wcData.GetMeanStartFrequency();
        fWaterfallCandidateData.fMeanEndFrequency = wcData.GetMeanEndFrequency();
        fWaterfallCandidateData.fFrequencyWidth = wcData.GetFrequencyWidth();
        KTTimeFrequency* cand = wcData.GetCandidate();
        if (!cand) {
            KTDEBUG("GetCandidate() returned NULL");
        } else {
            KTDEBUG("a cand");
        }
        KTDEBUG(publog, __LINE__ << " in " << __FILE__);
        //fWaterfallCandidateData.fCandidate = wcData.GetCandidate()->CreatePowerHistogram();
        //fWaterfallCandidateData.fCandidate = wcData.GetCandidate();
        KTDEBUG(publog, "set all candidate values");
        //fWaterfallCandidateData.fCandidate->SetDirectory(NULL);
//        KTDEBUG(publog, "Candidate info:\n"
//                << "\tTime axis: " << fWaterfallCandidateData.fCandidate->GetNbinsX() << " bins;  bin width: " << fWaterfallCandidateData.fCandidate->GetXaxis()->GetBinWidth(1) << " s;  range: " << fWaterfallCandidateData.fCandidate->GetXaxis()->GetXmin() << " - " << fWaterfallCandidateData.fCandidate->GetXaxis()->GetXmax() << " s\n"
//                << "\tFreq axis: " << fWaterfallCandidateData.fCandidate->GetNbinsY() << " bins;  bin width: " << fWaterfallCandidateData.fCandidate->GetYaxis()->GetBinWidth(1) << " Hz;  range: " << fWaterfallCandidateData.fCandidate->GetYaxis()->GetXmin() << " - " << fWaterfallCandidateData.fCandidate->GetYaxis()->GetXmax() << " Hz");

        fWaterfallCandidateTree->Fill();
        KTDEBUG("filled");

        return;
    }

    bool KTROOTTreeTypeWriterCandidates::SetupWaterfallCandidateTree()
    {
        fWaterfallCandidateTree = new TTree("wfCand", "Waterfall Candidates");
        if (fWaterfallCandidateTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fWaterfallCandidateTree);

        fWaterfallCandidateTree->Branch("Component", &fWaterfallCandidateData.fComponent, "fComponent/s");
        fWaterfallCandidateTree->Branch("TimeInRun", &fWaterfallCandidateData.fTimeInRun, "fTimeInRun/d");
        fWaterfallCandidateTree->Branch("TimeLength", &fWaterfallCandidateData.fTimeLength, "fTimeLength/d");
        fWaterfallCandidateTree->Branch("FirstSlice", &fWaterfallCandidateData.fFirstSliceNumber, "fFirstSliceNumber/l");
        fWaterfallCandidateTree->Branch("LastSlice", &fWaterfallCandidateData.fLastSliceNumber, "fLastSliceNumber/l");
        fWaterfallCandidateTree->Branch("MinFrequency", &fWaterfallCandidateData.fMinFrequency, "fMinFrequency/d");
        fWaterfallCandidateTree->Branch("MaxFrequency", &fWaterfallCandidateData.fMaxFrequency, "fMaxFrequency/d");
        fWaterfallCandidateTree->Branch("MeanStartFrequency", &fWaterfallCandidateData.fMeanStartFrequency, "fMeanStartFrequency/d");
        fWaterfallCandidateTree->Branch("MeanEndFrequency", &fWaterfallCandidateData.fMeanEndFrequency, "fMeanEndFrequency/d");
        fWaterfallCandidateTree->Branch("FrequencyWidth", &fWaterfallCandidateData.fFrequencyWidth, "fFrequencyWidth/d");
        fWaterfallCandidateTree->Branch("Candidate", &fWaterfallCandidateData.fCandidate, 32000, 0);

        return true;
    }

    //****************************
    // Sparse Waterfall Candidates
    //****************************

    void KTROOTTreeTypeWriterCandidates::WriteSparseWaterfallCandidate(KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to sparse waterfall candidate root tree");
        KTSparseWaterfallCandidateData& swcData = data->Of< KTSparseWaterfallCandidateData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fSparseWaterfallCandidateTree == NULL)
        {
            if (! SetupSparseWaterfallCandidateTree())
            {
                KTERROR(publog, "Something went wrong while setting up the sparse waterfall candidate tree! Nothing was written.");
                return;
            }
        }

        // Load() also clears any existing data
        //fFreqCandidateData->Load(*data);
        fSparseWaterfallCandidateData.fComponent = swcData.GetComponent();
        fSparseWaterfallCandidateData.fTimeBinWidth = swcData.GetTimeBinWidth();
        fSparseWaterfallCandidateData.fFreqBinWidth = swcData.GetFreqBinWidth();
        fSparseWaterfallCandidateData.fTimeInRun = swcData.GetTimeInRun();
        fSparseWaterfallCandidateData.fTimeLength = swcData.GetTimeLength();
        fSparseWaterfallCandidateData.fMinFrequency = swcData.GetMinimumFrequency();
        fSparseWaterfallCandidateData.fMaxFrequency = swcData.GetMaximumFrequency();
        fSparseWaterfallCandidateData.fFrequencyWidth = swcData.GetFrequencyWidth();
        const KTSparseWaterfallCandidateData::Points& points = swcData.GetPoints();
        fSparseWaterfallCandidateData.fPoints = new TGraph2D(points.size());
        unsigned iPoint = 0;
        for (KTSparseWaterfallCandidateData::Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            fSparseWaterfallCandidateData.fPoints->SetPoint(iPoint, pIt->fTimeInRun, pIt->fFrequency, pIt->fAmplitude);
            ++iPoint;
        }
        fSparseWaterfallCandidateData.fPoints->SetDirectory(NULL);
        KTDEBUG(publog, "Candidate info:\n"
                << "\tNumber of points: " << fSparseWaterfallCandidateData.fPoints->GetN() << "\n"
                << "\tTime axis: bin width: " << fSparseWaterfallCandidateData.fTimeBinWidth << " s\n"
                << "\tFreq axis: bin width: " << fSparseWaterfallCandidateData.fFreqBinWidth << " Hz");

        fSparseWaterfallCandidateTree->Fill();

        return;
    }

    bool KTROOTTreeTypeWriterCandidates::SetupSparseWaterfallCandidateTree()
    {
        fSparseWaterfallCandidateTree = new TTree("swfCand", "Sparse Waterfall Candidates");
        if (fSparseWaterfallCandidateTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fSparseWaterfallCandidateTree);

        fSparseWaterfallCandidateTree->Branch("Component", &fSparseWaterfallCandidateData.fComponent, "fComponent/s");
        fSparseWaterfallCandidateTree->Branch("TimeBinWidth", &fSparseWaterfallCandidateData.fTimeBinWidth, "fTimeBinWidth/d");
        fSparseWaterfallCandidateTree->Branch("FreqBinWidth", &fSparseWaterfallCandidateData.fFreqBinWidth, "fFreqBinWidth/d");
        fSparseWaterfallCandidateTree->Branch("TimeInRun", &fSparseWaterfallCandidateData.fTimeInRun, "fTimeInRun/d");
        fSparseWaterfallCandidateTree->Branch("TimeLength", &fSparseWaterfallCandidateData.fTimeLength, "fTimeLength/d");
        fSparseWaterfallCandidateTree->Branch("MinFrequency", &fSparseWaterfallCandidateData.fMinFrequency, "fMinFrequency/d");
        fSparseWaterfallCandidateTree->Branch("MaxFrequency", &fSparseWaterfallCandidateData.fMaxFrequency, "fMaxFrequency/d");
        fSparseWaterfallCandidateTree->Branch("FrequencyWidth", &fSparseWaterfallCandidateData.fFrequencyWidth, "fFrequencyWidth/d");
        fSparseWaterfallCandidateTree->Branch("Points", &fSparseWaterfallCandidateData.fPoints, 32000, 0);

        return true;
    }

} /* namespace Katydid */



