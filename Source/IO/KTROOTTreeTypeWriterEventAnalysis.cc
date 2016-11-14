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
            fMultiTrackEventTree(NULL),
            fLinearFitResultTree(NULL),
            fPowerFitDataTree(NULL),
            fFreqCandidateData(),
            fWaterfallCandidateData(),
            fSparseWaterfallCandidateData(),
            fProcessedTrackDataPtr(NULL),
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
        fWriter->RegisterSlot("multi-track-event", this, &KTROOTTreeTypeWriterEventAnalysis::WriteMultiTrackEvent);
        fWriter->RegisterSlot("linear-fit", this, &KTROOTTreeTypeWriterEventAnalysis::WriteLinearFitResultData);
        fWriter->RegisterSlot("power-fit", this, &KTROOTTreeTypeWriterEventAnalysis::WritePowerFitData);
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
            fLineFitData.fSlopeSigma = lfData.GetSlopeSigma( fLineFitData.fComponent );
            fLineFitData.fIntercept = lfData.GetIntercept( fLineFitData.fComponent );
            fLineFitData.fIntercept_deviation = lfData.GetIntercept_deviation( fLineFitData.fComponent );
            fLineFitData.fStartingFrequency = lfData.GetStartingFrequency( fLineFitData.fComponent );
            fLineFitData.fTrackDuration = lfData.GetTrackDuration( fLineFitData.fComponent );
            fLineFitData.fSidebandSeparation = lfData.GetSidebandSeparation( fLineFitData.fComponent );
            fLineFitData.fFineProbe_sigma_1 = lfData.GetFineProbe_sigma_1( fLineFitData.fComponent );
            fLineFitData.fFineProbe_sigma_2 = lfData.GetFineProbe_sigma_2( fLineFitData.fComponent );
            fLineFitData.fFineProbe_SNR_1 = lfData.GetFineProbe_SNR_1( fLineFitData.fComponent );
            fLineFitData.fFineProbe_SNR_2 = lfData.GetFineProbe_SNR_2( fLineFitData.fComponent );
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
                fLinearFitResultTree->SetBranchAddress( "SlopeSigma", &fLineFitData.fSlopeSigma );
                fLinearFitResultTree->SetBranchAddress( "Intercept", &fLineFitData.fIntercept );
                fLinearFitResultTree->SetBranchAddress( "InterceptDev", &fLineFitData.fIntercept_deviation );
                fLinearFitResultTree->SetBranchAddress( "StartingFrequency", &fLineFitData.fStartingFrequency );
                fLinearFitResultTree->SetBranchAddress( "TrackDuration", &fLineFitData.fTrackDuration );
                fLinearFitResultTree->SetBranchAddress( "SidebandSeparation", &fLineFitData.fSidebandSeparation );
                fLinearFitResultTree->SetBranchAddress( "Significance1_sigma", &fLineFitData.fFineProbe_sigma_1 );
                fLinearFitResultTree->SetBranchAddress( "Significance2_sigma", &fLineFitData.fFineProbe_sigma_2 );
                fLinearFitResultTree->SetBranchAddress( "Significance1_SNR", &fLineFitData.fFineProbe_SNR_1 );
                fLinearFitResultTree->SetBranchAddress( "Significance2_SNR", &fLineFitData.fFineProbe_SNR_2 );
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
        fLinearFitResultTree->Branch( "SlopeSigma", &fLineFitData.fSlopeSigma, "fSlopeSigma/d" );
        fLinearFitResultTree->Branch( "Intercept", &fLineFitData.fIntercept, "fIntercept/d" );
        fLinearFitResultTree->Branch( "InterceptDev", &fLineFitData.fIntercept_deviation, "fIntercept_deviation/d" );
        fLinearFitResultTree->Branch( "StartingFrequency", &fLineFitData.fStartingFrequency, "fStartingFrequency/d" );
        fLinearFitResultTree->Branch( "TrackDuration", &fLineFitData.fTrackDuration, "fTrackDuration/d" );
        fLinearFitResultTree->Branch( "SidebandSeparation", &fLineFitData.fSidebandSeparation, "fSidebandSeparation/d" );
        fLinearFitResultTree->Branch( "Significance1_sigma", &fLineFitData.fFineProbe_sigma_1, "fFineProbe_sigma_1/d" );
        fLinearFitResultTree->Branch( "Significance2_sigma", &fLineFitData.fFineProbe_sigma_2, "fFineProbe_sigma_2/d" );
        fLinearFitResultTree->Branch( "Significance1_SNR", &fLineFitData.fFineProbe_SNR_1, "fFineProbe_SNR_1/d" );
        fLinearFitResultTree->Branch( "Significance2_SNR", &fLineFitData.fFineProbe_SNR_2, "fFineProbe_SNR_2/d" );
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
            fPowerFitData.fScale = pfData.GetScale( fPowerFitData.fComponent );
            fPowerFitData.fBackground = pfData.GetBackground( fPowerFitData.fComponent );
            fPowerFitData.fCenter = pfData.GetCenter( fPowerFitData.fComponent );
            fPowerFitData.fCurvature = pfData.GetCurvature( fPowerFitData.fComponent );
            fPowerFitData.fWidth = pfData.GetWidth( fPowerFitData.fComponent );

            fPowerFitData.fScaleErr = pfData.GetScaleErr( fPowerFitData.fComponent );
            fPowerFitData.fBackgroundErr = pfData.GetBackgroundErr( fPowerFitData.fComponent );
            fPowerFitData.fCenterErr = pfData.GetCenterErr( fPowerFitData.fComponent );
            fPowerFitData.fCurvatureErr = pfData.GetCurvatureErr( fPowerFitData.fComponent );
            fPowerFitData.fWidthErr = pfData.GetWidthErr( fPowerFitData.fComponent );

            fPowerFitData.fIsValid = pfData.GetIsValid( fPowerFitData.fComponent );

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

                fPowerFitDataTree->SetBranchAddress( "Scale", &fPowerFitData.fScale );
                fPowerFitDataTree->SetBranchAddress( "Background", &fPowerFitData.fBackground );
                fPowerFitDataTree->SetBranchAddress( "Center", &fPowerFitData.fCenter );
                fPowerFitDataTree->SetBranchAddress( "Curvature", &fPowerFitData.fCurvature );
                fPowerFitDataTree->SetBranchAddress( "Width", &fPowerFitData.fWidth );

                fPowerFitDataTree->SetBranchAddress( "ScaleErr", &fPowerFitData.fScaleErr );
                fPowerFitDataTree->SetBranchAddress( "BackgroundErr", &fPowerFitData.fBackgroundErr );
                fPowerFitDataTree->SetBranchAddress( "CenterErr", &fPowerFitData.fCenterErr );
                fPowerFitDataTree->SetBranchAddress( "CurvatureErr", &fPowerFitData.fCurvatureErr );
                fPowerFitDataTree->SetBranchAddress( "WidthErr", &fPowerFitData.fWidthErr );

                fPowerFitDataTree->SetBranchAddress( "IsValid", &fPowerFitData.fIsValid );
                fPowerFitDataTree->SetBranchAddress( "MainPeak", &fPowerFitData.fMainPeak );

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

        fPowerFitDataTree->Branch( "Scale", &fPowerFitData.fScale, "fScale/d" );
        fPowerFitDataTree->Branch( "Background", &fPowerFitData.fBackground, "fBackground/d" );
        fPowerFitDataTree->Branch( "Center", &fPowerFitData.fCenter, "fCenter/d" );
        fPowerFitDataTree->Branch( "Curvature", &fPowerFitData.fCurvature, "fCurvature/d" );
        fPowerFitDataTree->Branch( "Width", &fPowerFitData.fWidth, "fWidth/d" );

        fPowerFitDataTree->Branch( "ScaleErr", &fPowerFitData.fScaleErr, "fScaleErr/d" );
        fPowerFitDataTree->Branch( "BackgroundErr", &fPowerFitData.fBackgroundErr, "fBackgroundErr/d" );
        fPowerFitDataTree->Branch( "CenterErr", &fPowerFitData.fCenterErr, "fCenterErr/d" );
        fPowerFitDataTree->Branch( "CurvatureErr", &fPowerFitData.fCurvatureErr, "fCurvatureErr/d" );
        fPowerFitDataTree->Branch( "WidthErr", &fPowerFitData.fWidthErr, "fWidthErr/d" );

        fPowerFitDataTree->Branch( "IsValid", &fPowerFitData.fIsValid, "fIsValid/i" );
        fPowerFitDataTree->Branch( "MainPeak", &fPowerFitData.fMainPeak, "fMainPeak/i" );

        return true;
    }


} /* namespace Katydid */
