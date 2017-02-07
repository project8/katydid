/*
 * KTROOTTreeTypeWriterEventAnalysis.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterEventAnalysis.hh"

#include "KTFrequencyCandidate.hh"
#include "KTFrequencyCandidateData.hh"
#include "KTTIFactory.hh"
#include "KTLinearFitResult.hh"
#include "KTLogger.hh"
#include "KTMultiTrackEventData.hh"
#include "KTPowerFitData.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTWaterfallCandidateData.hh"

//#include "TFrequencyCandidateData.hh"

#include "TFile.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TH2.h"
#include "TTree.h"

#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTTreeTypeWriterEventAnalysis");

    static Nymph::KTTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterEventAnalysis > sRTTWCRegistrar;

    KTROOTTreeTypeWriterEventAnalysis::KTROOTTreeTypeWriterEventAnalysis() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterEventAnalysis()
            fFreqCandidateTree(NULL),
            fWaterfallCandidateTree(NULL),
            fSparseWaterfallCandidateTree(NULL),
            fProcessedTrackTree(NULL),
            fMultiPeakTrackTree(NULL),
            fMultiTrackEventTree(NULL),
            fLinearFitResultTree(NULL),
            fPowerFitDataTree(NULL),
            fFreqCandidateData(),
            fWaterfallCandidateData(),
            fSparseWaterfallCandidateData(),
            fProcessedTrackDataPtr(NULL),
            fMultiPeakTrackData(),
            fMultiTrackEventDataPtr(NULL),
            fLineFitData(),
            fPowerFitData()
    {
    }

    KTROOTTreeTypeWriterEventAnalysis::~KTROOTTreeTypeWriterEventAnalysis()
    {
        delete fProcessedTrackDataPtr;
        delete fMultiTrackEventDataPtr;
    }


    void KTROOTTreeTypeWriterEventAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("frequency-candidates", this, &KTROOTTreeTypeWriterEventAnalysis::WriteFrequencyCandidates);
        fWriter->RegisterSlot("waterfall-candidates", this, &KTROOTTreeTypeWriterEventAnalysis::WriteWaterfallCandidate);
        fWriter->RegisterSlot("sparse-waterfall-candidates", this, &KTROOTTreeTypeWriterEventAnalysis::WriteSparseWaterfallCandidate);
        fWriter->RegisterSlot("processed-track", this, &KTROOTTreeTypeWriterEventAnalysis::WriteProcessedTrack);
        fWriter->RegisterSlot("multi-peak-track", this, &KTROOTTreeTypeWriterEventAnalysis::WriteMultiPeakTrack);
        fWriter->RegisterSlot("multi-track-event", this, &KTROOTTreeTypeWriterEventAnalysis::WriteMultiTrackEvent);
        fWriter->RegisterSlot("density-fit", this, &KTROOTTreeTypeWriterEventAnalysis::WriteLinearFitResultData);
        fWriter->RegisterSlot("power-fit", this, &KTROOTTreeTypeWriterEventAnalysis::WritePowerFitData);
        fWriter->RegisterSlot("mt-comp", this, &KTROOTTreeTypeWriterEventAnalysis::WriteMultiTrackComparisonData);
        return;
    }


    //*********************
    // Frequency Candidates
    //*********************

    void KTROOTTreeTypeWriterEventAnalysis::WriteFrequencyCandidates(Nymph::KTDataPtr data)
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

    bool KTROOTTreeTypeWriterEventAnalysis::SetupFrequencyCandidateTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "freqCand", fFreqCandidateTree );

            if( fFreqCandidateTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fFreqCandidateTree );

                fFreqCandidateTree->SetBranchAddress("Component", &fFreqCandidateData.fComponent);
                fFreqCandidateTree->SetBranchAddress("Slice", &fFreqCandidateData.fSlice);
                fFreqCandidateTree->SetBranchAddress("TimeInRun", &fFreqCandidateData.fTimeInRun);
                fFreqCandidateTree->SetBranchAddress("Threshold", &fFreqCandidateData.fThreshold);
                fFreqCandidateTree->SetBranchAddress("FirstBin", &fFreqCandidateData.fFirstBin);
                fFreqCandidateTree->SetBranchAddress("LastBin", &fFreqCandidateData.fLastBin);
                fFreqCandidateTree->SetBranchAddress("MeanFrequency", &fFreqCandidateData.fMeanFrequency);
                fFreqCandidateTree->SetBranchAddress("PeakAmplitude", &fFreqCandidateData.fPeakAmplitude);
                fFreqCandidateTree->SetBranchAddress("AmplitudeSum", &fFreqCandidateData.fAmplitudeSum);

                return true;
            }
        }

        fFreqCandidateTree = new TTree("freqCand", "Frequency Candidates");
        if (fFreqCandidateTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fFreqCandidateTree);

        //fFreqCandidateData = new TFrequencyCandidateData();

        //fFreqCandidateTree->Branch("freqCandidates", "Katydid::TFrequencyCandidateData", &fFreqCandidateData);
        fFreqCandidateTree->Branch("Component", &fFreqCandidateData.fComponent, "fComponent/i");
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

    void KTROOTTreeTypeWriterEventAnalysis::WriteWaterfallCandidate(Nymph::KTDataPtr data)
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
        fWaterfallCandidateData.fCandidate = wcData.GetCandidate()->CreatePowerHistogram();
        fWaterfallCandidateData.fCandidate->SetDirectory(NULL);
        KTDEBUG(publog, "Candidate info:\n"
                << "\tTime axis: " << fWaterfallCandidateData.fCandidate->GetNbinsX() << " bins;  bin width: " << fWaterfallCandidateData.fCandidate->GetXaxis()->GetBinWidth(1) << " s;  range: " << fWaterfallCandidateData.fCandidate->GetXaxis()->GetXmin() << " - " << fWaterfallCandidateData.fCandidate->GetXaxis()->GetXmax() << " s\n"
                << "\tFreq axis: " << fWaterfallCandidateData.fCandidate->GetNbinsY() << " bins;  bin width: " << fWaterfallCandidateData.fCandidate->GetYaxis()->GetBinWidth(1) << " Hz;  range: " << fWaterfallCandidateData.fCandidate->GetYaxis()->GetXmin() << " - " << fWaterfallCandidateData.fCandidate->GetYaxis()->GetXmax() << " Hz");

        fWaterfallCandidateTree->Fill();
        KTDEBUG("filled");

        return;
    }

    bool KTROOTTreeTypeWriterEventAnalysis::SetupWaterfallCandidateTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "wfCand", fWaterfallCandidateTree );

            if( fWaterfallCandidateTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fWaterfallCandidateTree );

                fWaterfallCandidateTree->SetBranchAddress("Component", &fWaterfallCandidateData.fComponent);
                fWaterfallCandidateTree->SetBranchAddress("TimeInRun", &fWaterfallCandidateData.fTimeInRun);
                fWaterfallCandidateTree->SetBranchAddress("TimeLength", &fWaterfallCandidateData.fTimeLength);
                fWaterfallCandidateTree->SetBranchAddress("FirstSlice", &fWaterfallCandidateData.fFirstSliceNumber);
                fWaterfallCandidateTree->SetBranchAddress("LastSlice", &fWaterfallCandidateData.fLastSliceNumber);
                fWaterfallCandidateTree->SetBranchAddress("MinFrequency", &fWaterfallCandidateData.fMinFrequency);
                fWaterfallCandidateTree->SetBranchAddress("MaxFrequency", &fWaterfallCandidateData.fMaxFrequency);
                fWaterfallCandidateTree->SetBranchAddress("MeanStartFrequency", &fWaterfallCandidateData.fMeanStartFrequency);
                fWaterfallCandidateTree->SetBranchAddress("MeanEndFrequency", &fWaterfallCandidateData.fMeanEndFrequency);
                fWaterfallCandidateTree->SetBranchAddress("FrequencyWidth", &fWaterfallCandidateData.fFrequencyWidth);
                fWaterfallCandidateTree->SetBranchAddress("Candidate", &fWaterfallCandidateData.fCandidate);

                return true;
            }
        }

        fWaterfallCandidateTree = new TTree("wfCand", "Waterfall Candidates");
        if (fWaterfallCandidateTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fWaterfallCandidateTree);

        fWaterfallCandidateTree->Branch("Component", &fWaterfallCandidateData.fComponent, "fComponent/i");
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

    void KTROOTTreeTypeWriterEventAnalysis::WriteSparseWaterfallCandidate(Nymph::KTDataPtr data)
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
        fSparseWaterfallCandidateData.fAcquisitionID = swcData.GetAcquisitionID();
        //fSparseWaterfallCandidateData.fTimeBinWidth = swcData.GetTimeBinWidth();
        //fSparseWaterfallCandidateData.fFreqBinWidth = swcData.GetFreqBinWidth();
        fSparseWaterfallCandidateData.fTimeInRunC = swcData.GetTimeInRunC();
        fSparseWaterfallCandidateData.fTimeLength = swcData.GetTimeLength();
        fSparseWaterfallCandidateData.fMinFrequency = swcData.GetMinimumFrequency();
        fSparseWaterfallCandidateData.fMaxFrequency = swcData.GetMaximumFrequency();
        fSparseWaterfallCandidateData.fFrequencyWidth = swcData.GetFrequencyWidth();

        const KTSparseWaterfallCandidateData::Points& points = swcData.GetPoints();

        if (points.size() == 0)
        {
            KTWARN(publog, "No points in sparse waterfall candidate; nothing written to ROOT file");
            return;
        }

        fSparseWaterfallCandidateData.fPoints = new TGraph2D(points.size());
        unsigned iPoint = 0;
        for (KTSparseWaterfallCandidateData::Points::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            fSparseWaterfallCandidateData.fPoints->SetPoint(iPoint, pIt->fTimeInRunC, pIt->fFrequency, pIt->fAmplitude);
            ++iPoint;
        }
        fSparseWaterfallCandidateData.fPoints->SetDirectory(NULL);
        KTDEBUG(publog, "Candidate info:\n"
                << "\tNumber of points: " << fSparseWaterfallCandidateData.fPoints->GetN());// << "\n"
                //<< "\tTime axis: bin width: " << fSparseWaterfallCandidateData.fTimeBinWidth << " s\n"
                //<< "\tFreq axis: bin width: " << fSparseWaterfallCandidateData.fFreqBinWidth << " Hz");

        fSparseWaterfallCandidateTree->Fill();

        return;
    }

    bool KTROOTTreeTypeWriterEventAnalysis::SetupSparseWaterfallCandidateTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "swfCand", fSparseWaterfallCandidateTree );

            if( fSparseWaterfallCandidateTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fSparseWaterfallCandidateTree );

                fSparseWaterfallCandidateTree->SetBranchAddress("Component", &fSparseWaterfallCandidateData.fComponent);
                fSparseWaterfallCandidateTree->SetBranchAddress("AcquisitionID", &fSparseWaterfallCandidateData.fAcquisitionID);
                fSparseWaterfallCandidateTree->SetBranchAddress("CandidateID", &fSparseWaterfallCandidateData.fCandidateID);
                //fSparseWaterfallCandidateTree->SetBranchAddress("TimeBinWidth", &fSparseWaterfallCandidateData.fTimeBinWidth);
                //fSparseWaterfallCandidateTree->SetBranchAddress("FreqBinWidth", &fSparseWaterfallCandidateData.fFreqBinWidth);
                fSparseWaterfallCandidateTree->SetBranchAddress("TimeInRunC", &fSparseWaterfallCandidateData.fTimeInRunC);
                fSparseWaterfallCandidateTree->SetBranchAddress("TimeLength", &fSparseWaterfallCandidateData.fTimeLength);
                fSparseWaterfallCandidateTree->SetBranchAddress("MinFrequency", &fSparseWaterfallCandidateData.fMinFrequency);
                fSparseWaterfallCandidateTree->SetBranchAddress("MaxFrequency", &fSparseWaterfallCandidateData.fMaxFrequency);
                fSparseWaterfallCandidateTree->SetBranchAddress("FrequencyWidth", &fSparseWaterfallCandidateData.fFrequencyWidth);
                fSparseWaterfallCandidateTree->SetBranchAddress("Points", &fSparseWaterfallCandidateData.fPoints);

                return true;
            }
        }

        fSparseWaterfallCandidateTree = new TTree("swfCand", "Sparse Waterfall Candidates");
        if (fSparseWaterfallCandidateTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fSparseWaterfallCandidateTree);

        fSparseWaterfallCandidateTree->Branch("Component", &fSparseWaterfallCandidateData.fComponent, "fComponent/i");
        fSparseWaterfallCandidateTree->Branch("AcquisitionID", &fSparseWaterfallCandidateData.fAcquisitionID, "fAcquisitionID/i");
        fSparseWaterfallCandidateTree->Branch("CandidateID", &fSparseWaterfallCandidateData.fCandidateID, "fCandidateID/i");
        //fSparseWaterfallCandidateTree->Branch("TimeBinWidth", &fSparseWaterfallCandidateData.fTimeBinWidth, "fTimeBinWidth/d");
        //fSparseWaterfallCandidateTree->Branch("FreqBinWidth", &fSparseWaterfallCandidateData.fFreqBinWidth, "fFreqBinWidth/d");
        fSparseWaterfallCandidateTree->Branch("TimeInRunC", &fSparseWaterfallCandidateData.fTimeInRunC, "fTimeInRunC/d");
        fSparseWaterfallCandidateTree->Branch("TimeLength", &fSparseWaterfallCandidateData.fTimeLength, "fTimeLength/d");
        fSparseWaterfallCandidateTree->Branch("MinFrequency", &fSparseWaterfallCandidateData.fMinFrequency, "fMinFrequency/d");
        fSparseWaterfallCandidateTree->Branch("MaxFrequency", &fSparseWaterfallCandidateData.fMaxFrequency, "fMaxFrequency/d");
        fSparseWaterfallCandidateTree->Branch("FrequencyWidth", &fSparseWaterfallCandidateData.fFrequencyWidth, "fFrequencyWidth/d");
        fSparseWaterfallCandidateTree->Branch("Points", &fSparseWaterfallCandidateData.fPoints, 32000, 0);

        return true;
    }

    //****************
    // Processed Track
    //****************

    void KTROOTTreeTypeWriterEventAnalysis::WriteProcessedTrack(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to processed track root tree");
        KTProcessedTrackData& ptData = data->Of< KTProcessedTrackData >();

        if (! fWriter->OpenAndVerifyFile()) return;
        fWriter->GetFile()->GetObject( "procTracks", fProcessedTrackTree );

        if (fProcessedTrackTree == NULL)
        {
            if (! SetupProcessedTrackTree())
            {
                KTERROR(publog, "Something went wrong while setting up the processed track tree! Nothing was written.");
                return;
            }
        }
        else
        {
            KTINFO(publog, "Tree already exists!");
            fWriter->AddTree( fProcessedTrackTree );

            fProcessedTrackTree->SetBranchAddress("Track", &fProcessedTrackDataPtr);
        }

        fProcessedTrackDataPtr->Load(ptData);

        fProcessedTrackTree->Fill();

        return;
    }

    bool KTROOTTreeTypeWriterEventAnalysis::SetupProcessedTrackTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "procTracks", fProcessedTrackTree );

            if( fProcessedTrackTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fProcessedTrackTree );

                fProcessedTrackTree->SetBranchAddress("Track", &fProcessedTrackDataPtr);

                return true;
            }
        }

        fProcessedTrackTree = new TTree("procTracks", "Processed Tracks");
        if (fProcessedTrackTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fProcessedTrackTree);

        //fProcessedTrackDataPtr = new TProcessedTrackData();

        fProcessedTrackTree->Branch("Track", "Katydid::TProcessedTrackData", &fProcessedTrackDataPtr);

        return true;
    }

    //**************************
    // Multi-Peak Track
    //**************************

    void KTROOTTreeTypeWriterEventAnalysis::WriteMultiPeakTrack(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to multi-peak track root tree");
        KTMultiPeakTrackData& mptData = data->Of< KTMultiPeakTrackData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fMultiPeakTrackTree == NULL)
        {
            if (! SetupMultiPeakTrackTree())
            {
                KTERROR(publog, "Something went wrong while setting up the multi-peak track tree! Nothing was written.");
                return;
            }
        }

        fMultiPeakTrackData.fComponent = mptData.GetComponent();
        fMultiPeakTrackData.fMultiplicity = mptData.GetMultiplicity();
        fMultiPeakTrackData.fEventSequenceID = mptData.GetEventSequenceID();
        fMultiPeakTrackData.fMeanStartTimeInRunC = mptData.GetMeanStartTimeInRunC();
        fMultiPeakTrackData.fSumStartTimeInRunC = mptData.GetSumStartTimeInRunC();
        fMultiPeakTrackData.fMeanEndTimeInRunC = mptData.GetMeanEndTimeInRunC();
        fMultiPeakTrackData.fSumEndTimeInRunC = mptData.GetSumEndTimeInRunC();
        fMultiPeakTrackData.fAcquisitionID = mptData.GetAcquisitionID();

        if( mptData.GetUnknownEventTopology() )
        {
            fMultiPeakTrackData.fUnknownEventTopology = 1;
        }
        else
        {
            fMultiPeakTrackData.fUnknownEventTopology = 0;
        }

        fMultiPeakTrackTree->Fill();
        
        return;
    }

    bool KTROOTTreeTypeWriterEventAnalysis::SetupMultiPeakTrackTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "mp-track", fMultiPeakTrackTree );

            if (fMultiPeakTrackTree != NULL)
            {
                KTINFO(publog, "Tree already exists; will add to it");
                fWriter->AddTree( fMultiPeakTrackTree );

                fMultiPeakTrackTree->SetBranchAddress( "Component", &fMultiPeakTrackData.fComponent );
                fMultiPeakTrackTree->SetBranchAddress( "Multiplicity", &fMultiPeakTrackData.fMultiplicity );
                fMultiPeakTrackTree->SetBranchAddress( "EventSequenceID", &fMultiPeakTrackData.fEventSequenceID );
                fMultiPeakTrackTree->SetBranchAddress( "MeanStartTimeInRunC", &fMultiPeakTrackData.fMeanStartTimeInRunC );
                fMultiPeakTrackTree->SetBranchAddress( "SumStartTimeInRunC", &fMultiPeakTrackData.fSumStartTimeInRunC );
                fMultiPeakTrackTree->SetBranchAddress( "MeanEndTimeInRunC", &fMultiPeakTrackData.fMeanEndTimeInRunC );
                fMultiPeakTrackTree->SetBranchAddress( "SumEndTimeInRunC", &fMultiPeakTrackData.fSumEndTimeInRunC );
                fMultiPeakTrackTree->SetBranchAddress( "AcquisitionID", &fMultiPeakTrackData.fAcquisitionID );
                fMultiPeakTrackTree->SetBranchAddress( "UnknownEventTopology", &fMultiPeakTrackData.fUnknownEventTopology );

                return true;
            }
        }

        fMultiPeakTrackTree = new TTree("mp-track", "Multi-Peak Track");
        if( fMultiPeakTrackTree == NULL )
        {
            KTERROR( publog, "Tree was not created!" );
            return false;
        }
        fWriter->AddTree( fMultiPeakTrackTree );

        fMultiPeakTrackTree->Branch( "Component", &fMultiPeakTrackData.fComponent, "fComponent/i" );
        fMultiPeakTrackTree->Branch( "Multiplicity", &fMultiPeakTrackData.fMultiplicity, "fMultiplicity/i" );
        fMultiPeakTrackTree->Branch( "EventSequenceID", &fMultiPeakTrackData.fEventSequenceID, "fEventSequenceID/i" );
        fMultiPeakTrackTree->Branch( "MeanStartTimeInRunC", &fMultiPeakTrackData.fMeanStartTimeInRunC, "fMeanStartTimeInRunC/d" );
        fMultiPeakTrackTree->Branch( "SumStartTimeInRunC", &fMultiPeakTrackData.fSumStartTimeInRunC, "fSumStartTimeInRunC/d" );
        fMultiPeakTrackTree->Branch( "MeanEndTimeInRunC", &fMultiPeakTrackData.fMeanEndTimeInRunC, "fMeanEndTimeInRunC/d" );
        fMultiPeakTrackTree->Branch( "SumEndTimeInRunC", &fMultiPeakTrackData.fSumEndTimeInRunC, "fSumEndTimeInRunC/d" );
        fMultiPeakTrackTree->Branch( "AcquisitionID", &fMultiPeakTrackData.fAcquisitionID, "fAcquisitionID/i" );
        fMultiPeakTrackTree->Branch( "UnknownEventTopology", &fMultiPeakTrackData.fUnknownEventTopology, "fUnknownEventTopology/i" );

        return true;
    }

    //******************
    // Multi-Track Event
    //******************

    void KTROOTTreeTypeWriterEventAnalysis::WriteMultiTrackEvent(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to multi-track event root tree");
        KTMultiTrackEventData& mteData = data->Of< KTMultiTrackEventData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fMultiTrackEventTree == NULL)
        {
            if (! SetupMultiTrackEventTree())
            {
                KTERROR(publog, "Something went wrong while setting up the multi-track event tree! Nothing was written.");
                return;
            }
        }

        fMultiTrackEventDataPtr->Load(mteData);

        fMultiTrackEventTree->Fill();

        return;
    }

    bool KTROOTTreeTypeWriterEventAnalysis::SetupMultiTrackEventTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "multiTrackEvents", fMultiTrackEventTree );

            if( fMultiTrackEventTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fMultiTrackEventTree );

                fMultiTrackEventTree->SetBranchAddress("Event", &fMultiTrackEventDataPtr);

                return true;
            }
        }

        fMultiTrackEventTree = new TTree("multiTrackEvents", "Multi-Track Events");
        if (fMultiTrackEventTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fMultiTrackEventTree);

        fMultiTrackEventDataPtr = new TMultiTrackEventData();

        fMultiTrackEventTree->Branch("Event", "Katydid::TMultiTrackEventData", &fMultiTrackEventDataPtr);

        return true;
    }

    //**************************
    // Linear Fit Result Data
    //**************************

    void KTROOTTreeTypeWriterEventAnalysis::WriteLinearFitResultData(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to linear fit result root tree");
        KTLinearFitResult& lfData = data->Of< KTLinearFitResult >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fLinearFitResultTree == NULL)
        {
            if (! SetupLinearFitResultTree())
            {
                KTERROR(publog, "Something went wrong while setting up the Linear Fit tree! Nothing was written.");
                return;
            }
        }

        for (fLineFitData.fComponent = 0; fLineFitData.fComponent < lfData.GetNComponents(); fLineFitData.fComponent++)
        {
            fLineFitData.fSlope = lfData.GetSlope( fLineFitData.fComponent );
            fLineFitData.fIntercept = lfData.GetIntercept( fLineFitData.fComponent );
            fLineFitData.fStartingFrequency = lfData.GetStartingFrequency( fLineFitData.fComponent );
            fLineFitData.fTrackDuration = lfData.GetTrackDuration( fLineFitData.fComponent );
            fLineFitData.fSidebandSeparation = lfData.GetSidebandSeparation( fLineFitData.fComponent );
            //fLineFitData.fFineProbe_sigma_1 = lfData.GetFineProbe_sigma_1( fLineFitData.fComponent );
            //fLineFitData.fFineProbe_sigma_2 = lfData.GetFineProbe_sigma_2( fLineFitData.fComponent );
            //fLineFitData.fFineProbe_SNR_1 = lfData.GetFineProbe_SNR_1( fLineFitData.fComponent );
            //fLineFitData.fFineProbe_SNR_2 = lfData.GetFineProbe_SNR_2( fLineFitData.fComponent );
            fLineFitData.fFFT_peak = lfData.GetFFT_peak( fLineFitData.fComponent );
            fLineFitData.fFFT_SNR = lfData.GetFFT_SNR( fLineFitData.fComponent );
            fLineFitData.fFit_width = lfData.GetFit_width( fLineFitData.fComponent );
            fLineFitData.fNPoints = lfData.GetNPoints( fLineFitData.fComponent );
            fLineFitData.fProbeWidth = lfData.GetProbeWidth( fLineFitData.fComponent );

            fLinearFitResultTree->Fill();
        }

        return;
    }

    bool KTROOTTreeTypeWriterEventAnalysis::SetupLinearFitResultTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "line", fLinearFitResultTree );

            if (fLinearFitResultTree != NULL)
            {
                KTINFO(publog, "Tree already exists; will add to it");
                fWriter->AddTree( fLinearFitResultTree );

                fLinearFitResultTree->SetBranchAddress( "Component", &fLineFitData.fComponent );
                fLinearFitResultTree->SetBranchAddress( "Slope", &fLineFitData.fSlope );
                fLinearFitResultTree->SetBranchAddress( "Intercept", &fLineFitData.fIntercept );
                fLinearFitResultTree->SetBranchAddress( "StartingFrequency", &fLineFitData.fStartingFrequency );
                fLinearFitResultTree->SetBranchAddress( "TrackDuration", &fLineFitData.fTrackDuration );
                fLinearFitResultTree->SetBranchAddress( "SidebandSeparation", &fLineFitData.fSidebandSeparation );
                //fLinearFitResultTree->SetBranchAddress( "Significance1_sigma", &fLineFitData.fFineProbe_sigma_1 );
                //fLinearFitResultTree->SetBranchAddress( "Significance2_sigma", &fLineFitData.fFineProbe_sigma_2 );
                //fLinearFitResultTree->SetBranchAddress( "Significance1_SNR", &fLineFitData.fFineProbe_SNR_1 );
                //fLinearFitResultTree->SetBranchAddress( "Significance2_SNR", &fLineFitData.fFineProbe_SNR_2 );
                fLinearFitResultTree->SetBranchAddress( "FFT_peak", &fLineFitData.fFFT_peak );
                fLinearFitResultTree->SetBranchAddress( "FFT_SNR", &fLineFitData.fFFT_SNR );
                fLinearFitResultTree->SetBranchAddress( "WindowBandwidth", &fLineFitData.fFit_width );
                fLinearFitResultTree->SetBranchAddress( "NPoints", &fLineFitData.fNPoints );
                fLinearFitResultTree->SetBranchAddress( "ProbeWidth", &fLineFitData.fProbeWidth );

                return true;
            }
        }

        fLinearFitResultTree = new TTree("line", "Linear Fit Result");
        if( fLinearFitResultTree == NULL )
        {
            KTERROR( publog, "Tree was not created!" );
            return false;
        }
        fWriter->AddTree( fLinearFitResultTree );

        fLinearFitResultTree->Branch( "Component", &fLineFitData.fComponent, "fComponent/i" );
        fLinearFitResultTree->Branch( "Slope", &fLineFitData.fSlope, "fSlope/d" );
        fLinearFitResultTree->Branch( "Intercept", &fLineFitData.fIntercept, "fIntercept/d" );
        fLinearFitResultTree->Branch( "StartingFrequency", &fLineFitData.fStartingFrequency, "fStartingFrequency/d" );
        fLinearFitResultTree->Branch( "TrackDuration", &fLineFitData.fTrackDuration, "fTrackDuration/d" );
        fLinearFitResultTree->Branch( "SidebandSeparation", &fLineFitData.fSidebandSeparation, "fSidebandSeparation/d" );
        //fLinearFitResultTree->Branch( "Significance1_sigma", &fLineFitData.fFineProbe_sigma_1, "fFineProbe_sigma_1/d" );
        //fLinearFitResultTree->Branch( "Significance2_sigma", &fLineFitData.fFineProbe_sigma_2, "fFineProbe_sigma_2/d" );
        //fLinearFitResultTree->Branch( "Significance1_SNR", &fLineFitData.fFineProbe_SNR_1, "fFineProbe_SNR_1/d" );
        //fLinearFitResultTree->Branch( "Significance2_SNR", &fLineFitData.fFineProbe_SNR_2, "fFineProbe_SNR_2/d" );
        fLinearFitResultTree->Branch( "FFT_peak", &fLineFitData.fFFT_peak, "fFFT_peak/d" );
        fLinearFitResultTree->Branch( "FFT_SNR", &fLineFitData.fFFT_SNR, "fFFT_SNR/d" );
        fLinearFitResultTree->Branch( "WindowBandwidth", &fLineFitData.fFit_width, "fFit_width/d" );
        fLinearFitResultTree->Branch( "NPoints", &fLineFitData.fNPoints, "fNPoints/i" );
        fLinearFitResultTree->Branch( "ProbeWidth", &fLineFitData.fProbeWidth, "fProbeWidth/d" );

        return true;
    }

    //**************************
    // Power Fit Data
    //**************************

    std::vector<double> * pNorm;
    std::vector<double> * pMean;
    std::vector<double> * pSigma;
    std::vector<double> * pMaximum;
    
    std::vector<double> * pNormErr;
    std::vector<double> * pMeanErr;
    std::vector<double> * pSigmaErr;
    std::vector<double> * pMaximumErr;

    void KTROOTTreeTypeWriterEventAnalysis::WritePowerFitData(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to power fit data root tree");
        KTPowerFitData& pfData = data->Of< KTPowerFitData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fPowerFitDataTree == NULL)
        {
            if (! SetupPowerFitDataTree())
            {
                KTERROR(publog, "Something went wrong while setting up the Power Fit Data tree! Nothing was written.");
                return;
            }
        }

        for (fPowerFitData.fComponent = 0; fPowerFitData.fComponent < pfData.GetNComponents(); fPowerFitData.fComponent++)
        {
            fPowerFitData.fNorm = pfData.GetNorm( fPowerFitData.fComponent );
            fPowerFitData.fMean = pfData.GetMean( fPowerFitData.fComponent );
            fPowerFitData.fSigma = pfData.GetSigma( fPowerFitData.fComponent );
            fPowerFitData.fMaximum = pfData.GetMaximum( fPowerFitData.fComponent );

            fPowerFitData.fNormErr = pfData.GetNormErr( fPowerFitData.fComponent );
            fPowerFitData.fMeanErr = pfData.GetMeanErr( fPowerFitData.fComponent );
            fPowerFitData.fSigmaErr = pfData.GetSigmaErr( fPowerFitData.fComponent );
            fPowerFitData.fMaximumErr = pfData.GetMaximumErr( fPowerFitData.fComponent );

            fPowerFitData.fIsValid = pfData.GetIsValid( fPowerFitData.fComponent );
            fPowerFitData.fMainPeak = pfData.GetMainPeak( fPowerFitData.fComponent );
            fPowerFitData.fNPeaks = pfData.GetNPeaks( fPowerFitData.fComponent );

            fPowerFitData.fAverage = pfData.GetAverage( fPowerFitData.fComponent );
            fPowerFitData.fRMS = pfData.GetRMS( fPowerFitData.fComponent );
            fPowerFitData.fSkewness = pfData.GetSkewness( fPowerFitData.fComponent );
            fPowerFitData.fKurtosis = pfData.GetKurtosis( fPowerFitData.fComponent );

            fPowerFitData.fNormCentral = pfData.GetNormCentral( fPowerFitData.fComponent );
            fPowerFitData.fMeanCentral = pfData.GetMeanCentral( fPowerFitData.fComponent );
            fPowerFitData.fSigmaCentral = pfData.GetSigmaCentral( fPowerFitData.fComponent );
            fPowerFitData.fMaximumCentral = pfData.GetMaximumCentral( fPowerFitData.fComponent );

            fPowerFitData.fRMSAwayFromCentral = pfData.GetRMSAwayFromCentral( fPowerFitData.fComponent );
            fPowerFitData.fCentralPowerRatio = pfData.GetCentralPowerRatio( fPowerFitData.fComponent );

            fPowerFitData.fTrackIntercept = pfData.GetTrackIntercept( fPowerFitData.fComponent );

            const KTPowerFitData::SetOfPoints& points = pfData.GetSetOfPoints( fPowerFitData.fComponent );

            if (points.size() == 0)
            {
                KTWARN(publog, "No points in power fit data; nothing written to ROOT file");
                return;
            }

            fPowerFitData.fPoints = new TGraph(points.size());
            unsigned iPoint = 0;
            for (KTPowerFitData::SetOfPoints::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
            {
                fPowerFitData.fPoints->SetPoint(iPoint, pIt->second.fAbscissa, pIt->second.fOrdinate);
                ++iPoint;
            }
            //fPowerFitData.fPoints->SetDirectory(NULL);

            fPowerFitDataTree->Fill();
        }

        return;
    }

    bool KTROOTTreeTypeWriterEventAnalysis::SetupPowerFitDataTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "power-fit", fPowerFitDataTree );

            if (fPowerFitDataTree != NULL)
            {
                KTINFO(publog, "Tree already exists; will add to it");
                fWriter->AddTree( fPowerFitDataTree );

                fPowerFitDataTree->SetBranchAddress( "Component", &fPowerFitData.fComponent );

                fPowerFitData.fNorm.push_back(0.);
                fPowerFitData.fMean.push_back(0.);
                fPowerFitData.fSigma.push_back(0.);
                fPowerFitData.fMaximum.push_back(0.);

                fPowerFitData.fNormErr.push_back(0.);
                fPowerFitData.fMeanErr.push_back(0.);
                fPowerFitData.fSigmaErr.push_back(0.);
                fPowerFitData.fMaximumErr.push_back(0.);

                pNorm = &fPowerFitData.fNorm;
                pMean = &fPowerFitData.fMean;
                pSigma = &fPowerFitData.fSigma;
                pMaximum = &fPowerFitData.fMaximum;
                
                pNormErr = &fPowerFitData.fNormErr;
                pMeanErr = &fPowerFitData.fMeanErr;
                pSigmaErr = &fPowerFitData.fSigmaErr;
                pMaximumErr = &fPowerFitData.fMaximumErr;

                fPowerFitDataTree->SetBranchAddress( "Norm", &pNorm );
                fPowerFitDataTree->SetBranchAddress( "Mean", &pMean );
                fPowerFitDataTree->SetBranchAddress( "Sigma", &pSigma );
                fPowerFitDataTree->SetBranchAddress( "Maximum", &pMaximum );

                fPowerFitDataTree->SetBranchAddress( "NormErr", &pNormErr );
                fPowerFitDataTree->SetBranchAddress( "MeanErr", &pMeanErr );
                fPowerFitDataTree->SetBranchAddress( "SigmaErr", &pSigmaErr );
                fPowerFitDataTree->SetBranchAddress( "MaximumErr", &pMaximumErr);

                fPowerFitDataTree->SetBranchAddress( "IsValid", &fPowerFitData.fIsValid );
                fPowerFitDataTree->SetBranchAddress( "MainPeak", &fPowerFitData.fMainPeak );
                fPowerFitDataTree->SetBranchAddress( "NPeaks", &fPowerFitData.fNPeaks );

                fPowerFitDataTree->SetBranchAddress( "Points", &fPowerFitData.fPoints );

                fPowerFitDataTree->SetBranchAddress( "Average", &fPowerFitData.fAverage );
                fPowerFitDataTree->SetBranchAddress( "RMS", &fPowerFitData.fRMS );
                fPowerFitDataTree->SetBranchAddress( "Skewness", &fPowerFitData.fSkewness );
                fPowerFitDataTree->SetBranchAddress( "Kurtosis", &fPowerFitData.fKurtosis );

                fPowerFitDataTree->SetBranchAddress( "NormCentral", &fPowerFitData.fNormCentral );
                fPowerFitDataTree->SetBranchAddress( "MeanCentral", &fPowerFitData.fMeanCentral );
                fPowerFitDataTree->SetBranchAddress( "SigmaCentral", &fPowerFitData.fSigmaCentral );
                fPowerFitDataTree->SetBranchAddress( "MaximumCentral", &fPowerFitData.fMaximumCentral );

                fPowerFitDataTree->SetBranchAddress( "RMSAwayFromCentral", &fPowerFitData.fRMSAwayFromCentral );
                fPowerFitDataTree->SetBranchAddress( "CentralPowerRatio", &fPowerFitData.fCentralPowerRatio );

                fPowerFitDataTree->SetBranchAddress( "TrackIntercept", &fPowerFitData.fTrackIntercept );

                return true;
            }
        }

        fPowerFitDataTree = new TTree("power-fit", "Power Fit Data");
        if( fPowerFitDataTree == NULL )
        {
            KTERROR( publog, "Tree was not created!" );
            return false;
        }
        fWriter->AddTree( fPowerFitDataTree );

        fPowerFitDataTree->Branch( "Component", &fPowerFitData.fComponent, "fComponent/i" );

        fPowerFitData.fNorm.push_back(0.);
        fPowerFitData.fMean.push_back(0.);
        fPowerFitData.fSigma.push_back(0.);
        fPowerFitData.fMaximum.push_back(0.);

        fPowerFitData.fNormErr.push_back(0.);
        fPowerFitData.fMeanErr.push_back(0.);
        fPowerFitData.fSigmaErr.push_back(0.);
        fPowerFitData.fMaximumErr.push_back(0.);

        pNorm = &fPowerFitData.fNorm;
        pMean = &fPowerFitData.fMean;
        pSigma = &fPowerFitData.fSigma;
        pMaximum = &fPowerFitData.fMaximum;
        
        pNormErr = &fPowerFitData.fNormErr;
        pMeanErr = &fPowerFitData.fMeanErr;
        pSigmaErr = &fPowerFitData.fSigmaErr;
        pMaximumErr = &fPowerFitData.fMaximumErr;

        fPowerFitDataTree->Branch( "Norm", "std::vector<double>", &pNorm );
        fPowerFitDataTree->Branch( "Mean", "std::vector<double>", &pMean );
        fPowerFitDataTree->Branch( "Sigma", "std::vector<double>", &pSigma );
        fPowerFitDataTree->Branch( "Maximum", "std::vector<double>", &pMaximum );

        fPowerFitDataTree->Branch( "NormErr", "std::vector<double>", &pNormErr );
        fPowerFitDataTree->Branch( "MeanErr", "std::vector<double>", &pMeanErr );
        fPowerFitDataTree->Branch( "SigmaErr", "std::vector<double>", &pSigmaErr );
        fPowerFitDataTree->Branch( "MaximumErr", "std::vector<double>", &pMaximumErr );
                
        fPowerFitDataTree->Branch( "IsValid", &fPowerFitData.fIsValid, "fIsValid/i" );
        fPowerFitDataTree->Branch( "MainPeak", &fPowerFitData.fMainPeak, "fMainPeak/i" );
        fPowerFitDataTree->Branch( "NPeaks", &fPowerFitData.fNPeaks, "fNPeaks/i" );

        fPowerFitDataTree->Branch( "Points", &fPowerFitData.fPoints, 32000, 0 );

        fPowerFitDataTree->Branch( "Average", &fPowerFitData.fAverage, "fAverage/d" );
        fPowerFitDataTree->Branch( "RMS", &fPowerFitData.fRMS, "fRMS/d" );
        fPowerFitDataTree->Branch( "Skewness", &fPowerFitData.fSkewness, "fSkewness/d" );
        fPowerFitDataTree->Branch( "Kurtosis", &fPowerFitData.fKurtosis, "fKurtosis/d" );

        fPowerFitDataTree->Branch( "NormCentral", &fPowerFitData.fNormCentral, "fNormCentral/d" );
        fPowerFitDataTree->Branch( "MeanCentral", &fPowerFitData.fMeanCentral, "fMeanCentral/d" );
        fPowerFitDataTree->Branch( "SigmaCentral", &fPowerFitData.fSigmaCentral, "fSigmaCentral/d" );
        fPowerFitDataTree->Branch( "MaximumCentral", &fPowerFitData.fMaximumCentral, "fMaximumCentral/d" );

        fPowerFitDataTree->Branch( "RMSAwayFromCentral", &fPowerFitData.fRMSAwayFromCentral, "fRMSAwayFromCentral/d" );
        fPowerFitDataTree->Branch( "CentralPowerRatio", &fPowerFitData.fCentralPowerRatio, "fCentralPowerRatio/d" );

        fPowerFitDataTree->Branch( "TrackIntercept", &fPowerFitData.fTrackIntercept, "fTrackIntercept/d" );

        return true;
    }

    //*****************************
    // Multi Track Comparison Data
    //*****************************

    void KTROOTTreeTypeWriterEventAnalysis::WriteMultiTrackComparisonData(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to multi-track comparison data to root tree");
        KTMultiTrackComparisonData& mtCompData = data->Of< KTMultiTrackComparisonData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fMultiTrackComparisonTree == NULL)
        {
            if (! SetupMultiTrackComparisonTree())
            {
                KTERROR(publog, "Something went wrong while setting up the track comparison tree! Nothing was written.");
                return;
            }
        }

        fMultiTrackCompData.fComponent = mtCompData.GetComponent();

        fMultiTrackCompData.fMultiplicity = mtCompData.GetMultiplicity();

        fMultiTrackCompData.fMinDeltaFRatio = mtCompData.GetMinDeltaFRatio();
        fMultiTrackCompData.fMaxDeltaFRatio = mtCompData.GetMaxDeltaFRatio();

        fMultiTrackCompData.fMeanPowerOld = mtCompData.GetMeanPowerOld();
        fMultiTrackCompData.fMinPowerOld = mtCompData.GetMinPowerOld();
        fMultiTrackCompData.fMaxPowerOld = mtCompData.GetMaxPowerOld();

        fMultiTrackCompData.fPowerNew = mtCompData.GetPowerNew();

        fMultiTrackCompData.fMeanDeltaQ = mtCompData.GetMeanDeltaQ();
        fMultiTrackCompData.fMinDeltaQ = mtCompData.GetMinDeltaQ();
        fMultiTrackCompData.fMaxDeltaQ = mtCompData.GetMaxDeltaQ();

        fMultiTrackCompData.fMeanDeltaTHead = mtCompData.GetMeanDeltaTHead();
        fMultiTrackCompData.fMinDeltaTHead = mtCompData.GetMinDeltaTHead();
        fMultiTrackCompData.fMaxDeltaTHead = mtCompData.GetMaxDeltaTHead();

        fMultiTrackCompData.fMeanDeltaTTail = mtCompData.GetMeanDeltaTTail();
        fMultiTrackCompData.fMinDeltaTTail = mtCompData.GetMinDeltaTTail();
        fMultiTrackCompData.fMaxDeltaTTail = mtCompData.GetMaxDeltaTTail();

        fMultiTrackComparisonTree->Fill();

        return;
    }

    bool KTROOTTreeTypeWriterEventAnalysis::SetupMultiTrackComparisonTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "mt-comp", fMultiTrackComparisonTree );

            if (fMultiTrackComparisonTree != NULL)
            {
                KTINFO(publog, "Tree already exists; will add to it");
                fWriter->AddTree( fMultiTrackComparisonTree );

                fMultiTrackComparisonTree->SetBranchAddress( "Component", &fMultiTrackCompData.fComponent );

                fMultiTrackComparisonTree->SetBranchAddress( "Multiplicity", &fMultiTrackCompData.fMultiplicity );

                fMultiTrackComparisonTree->SetBranchAddress( "MinDeltaFRatio", &fMultiTrackCompData.fMinDeltaFRatio );
                fMultiTrackComparisonTree->SetBranchAddress( "MaxDeltaFRatio", &fMultiTrackCompData.fMaxDeltaFRatio );

                fMultiTrackComparisonTree->SetBranchAddress( "MeanPowerOld", &fMultiTrackCompData.fMeanPowerOld );
                fMultiTrackComparisonTree->SetBranchAddress( "MinPowerOld", &fMultiTrackCompData.fMinPowerOld );
                fMultiTrackComparisonTree->SetBranchAddress( "MaxPowerOld", &fMultiTrackCompData.fMaxPowerOld );

                fMultiTrackComparisonTree->SetBranchAddress( "PowerNew", &fMultiTrackCompData.fPowerNew );

                fMultiTrackComparisonTree->SetBranchAddress( "MeanDeltaQ", &fMultiTrackCompData.fMeanDeltaQ );
                fMultiTrackComparisonTree->SetBranchAddress( "MinDeltaQ", &fMultiTrackCompData.fMinDeltaQ );
                fMultiTrackComparisonTree->SetBranchAddress( "MaxDeltaQ", &fMultiTrackCompData.fMaxDeltaQ );

                fMultiTrackComparisonTree->SetBranchAddress( "MeanDeltaTHead", &fMultiTrackCompData.fMeanDeltaTHead );
                fMultiTrackComparisonTree->SetBranchAddress( "MinDeltaTHead", &fMultiTrackCompData.fMinDeltaTHead );
                fMultiTrackComparisonTree->SetBranchAddress( "MaxDeltaTHead", &fMultiTrackCompData.fMaxDeltaTHead );

                fMultiTrackComparisonTree->SetBranchAddress( "MeanDeltaTTail", &fMultiTrackCompData.fMeanDeltaTTail );
                fMultiTrackComparisonTree->SetBranchAddress( "MinDeltaTTail", &fMultiTrackCompData.fMinDeltaTTail );
                fMultiTrackComparisonTree->SetBranchAddress( "MaxDeltaTTail", &fMultiTrackCompData.fMaxDeltaTTail );

                return true;
            }
        }

        fMultiTrackComparisonTree = new TTree("mt-comp", "Multi-Track Comparison");
        if( fMultiTrackComparisonTree == NULL )
        {
            KTERROR( publog, "Tree was not created!" );
            return false;
        }
        fWriter->AddTree( fMultiTrackComparisonTree );

        fMultiTrackComparisonTree->Branch( "Component", &fMultiTrackCompData.fComponent, "fComponent/i" );

        fMultiTrackComparisonTree->Branch( "Multiplicity", &fMultiTrackCompData.fMultiplicity, "fMultiplicity/i" );

        fMultiTrackComparisonTree->Branch( "MinDeltaFRatio", &fMultiTrackCompData.fMinDeltaFRatio, "fMinDeltaFRatio/d" );
        fMultiTrackComparisonTree->Branch( "MaxDeltaFRatio", &fMultiTrackCompData.fMaxDeltaFRatio, "fMaxDeltaFRatio/d" );

        fMultiTrackComparisonTree->Branch( "MeanPowerOld", &fMultiTrackCompData.fMeanPowerOld, "fMeanPowerOld/d" );
        fMultiTrackComparisonTree->Branch( "MinPowerOld", &fMultiTrackCompData.fMinPowerOld, "fMinPowerOld/d" );
        fMultiTrackComparisonTree->Branch( "MaxPowerOld", &fMultiTrackCompData.fMaxPowerOld, "fMaxPowerOld/d" );

        fMultiTrackComparisonTree->Branch( "PowerNew", &fMultiTrackCompData.fPowerNew, "fPowerNew/d" );

        fMultiTrackComparisonTree->Branch( "MeanDeltaQ", &fMultiTrackCompData.fMeanDeltaQ, "fMeanDeltaQ/d" );
        fMultiTrackComparisonTree->Branch( "MinDeltaQ", &fMultiTrackCompData.fMinDeltaQ, "fMinDeltaQ/d" );
        fMultiTrackComparisonTree->Branch( "MaxDeltaQ", &fMultiTrackCompData.fMaxDeltaQ, "fMaxDeltaQ/d" );

        fMultiTrackComparisonTree->Branch( "MeanDeltaTHead", &fMultiTrackCompData.fMeanDeltaTHead, "fMeanDeltaTHead/d" );
        fMultiTrackComparisonTree->Branch( "MinDeltaTHead", &fMultiTrackCompData.fMinDeltaTHead, "fMinDeltaTHead/d" );
        fMultiTrackComparisonTree->Branch( "MaxDeltaTHead", &fMultiTrackCompData.fMaxDeltaTHead, "fMaxDeltaTHead/d" );

        fMultiTrackComparisonTree->Branch( "MeanDeltaTTail", &fMultiTrackCompData.fMeanDeltaTTail, "fMeanDeltaTHead/d" );
        fMultiTrackComparisonTree->Branch( "MinDeltaTTail", &fMultiTrackCompData.fMinDeltaTTail, "fMeanDeltaTTail/d" );
        fMultiTrackComparisonTree->Branch( "MaxDeltaTTail", &fMultiTrackCompData.fMaxDeltaTTail, "fMeanDeltaTTail/d" );

        return true;
    }


} /* namespace Katydid */
