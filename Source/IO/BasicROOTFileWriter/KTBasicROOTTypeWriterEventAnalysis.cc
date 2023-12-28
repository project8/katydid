/*
 * KTBasicROOTTypeWriterEventAnalysis.cc
 *
 *  Created on: Jul 29, 2014
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterEventAnalysis.hh"

//#include "KT2ROOT.hh"
#include "logger.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTTIFactory.hh"
#include "KTDiscriminatedPoint.hh"

#include "TAxis.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLine.h"

#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    LOGGER(publog, "KTBasicROOTTypeWriterEventAnalysis");


    static Nymph::KTTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterEventAnalysis > sBRTWCandidatesRegistrar;

    KTBasicROOTTypeWriterEventAnalysis::KTBasicROOTTypeWriterEventAnalysis() :
            KTBasicROOTTypeWriter(),
            fTrackList(nullptr)
    {
    }

    KTBasicROOTTypeWriterEventAnalysis::~KTBasicROOTTypeWriterEventAnalysis()
    {
    }


    void KTBasicROOTTypeWriterEventAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("proc-track", this, &KTBasicROOTTypeWriterEventAnalysis::WriteProcTrack);
        fWriter->RegisterSlot("track-and-swfc", this, &KTBasicROOTTypeWriterEventAnalysis::WriteProcTrackAndSWFC);

        return;
    }


    //************************
    // Processed Track
    //************************
    void KTBasicROOTTypeWriterEventAnalysis::WriteProcTrack(Nymph::KTDataPtr data)
    {
        if (! data) return;

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fTrackList == nullptr) fTrackList = new TList();

        KTProcessedTrackData& ptData = data->Of< KTProcessedTrackData >();

        // line for track
        TLine* trackLine = new TLine(ptData.GetStartTimeInRunC(), ptData.GetStartFrequency(), ptData.GetEndTimeInRunC(), ptData.GetEndFrequency());
        trackLine->SetLineColor(2);
        trackLine->SetLineWidth(1);
        LDEBUG(publog, "Line created: (" << trackLine->GetX1() << ", " << trackLine->GetY1() << "), --> (" << trackLine->GetX2() << ", " << trackLine->GetY2() << ")");

        fTrackList->Add(trackLine);

        fWriter->GetFile()->WriteTObject(fTrackList, "proc-tracks", "SingleKey Overwrite");

        return;
    }



    //************************
    // Processed Track & Sparse Waterfall Candidate
    //************************

    void KTBasicROOTTypeWriterEventAnalysis::WriteProcTrackAndSWFC(Nymph::KTDataPtr data)
    {
        if (! data) return;

        static unsigned iCandidate = 0;

        LINFO(publog, "Drawing track for candidate " << iCandidate);

        KTSparseWaterfallCandidateData& swfcData = data->Of< KTSparseWaterfallCandidateData >();
        KTProcessedTrackData& ptData = data->Of< KTProcessedTrackData >();

        if (ptData.GetIsCut())
        {
            LINFO(publog, "Track was cut");
            ++iCandidate;
            return;
        }

        const KTDiscriminatedPoints& points = swfcData.GetPoints();

        if (! fWriter->OpenAndVerifyFile()) return;

        stringstream conv;
        conv << "track_" << iCandidate;

        TCanvas* trackCanv = new TCanvas(conv.str().c_str(), conv.str().c_str());

        // graph of points
        TGraph* grPoints = new TGraph(points.size());
        grPoints->SetName(conv.str().c_str());
        grPoints->SetMarkerStyle(20);
        grPoints->SetMarkerColor(4);

        unsigned iPoint = 0;
        for (KTDiscriminatedPoints::const_iterator pIt = points.begin(); pIt != points.end(); ++pIt)
        {
            grPoints->SetPoint(iPoint, pIt->fTimeInRunC, pIt->fFrequency);
            LDEBUG(publog, "Point " << iPoint << ": (" << pIt->fTimeInRunC << ", " << pIt->fFrequency << ")");
            ++iPoint;
        }
        grPoints->Draw("ap");
        grPoints->GetXaxis()->SetTitle("Time (s)");
        grPoints->GetYaxis()->SetTitle("Frequency (Hz)");

        // line for track
        TLine* trackLine = new TLine(ptData.GetStartTimeInRunC(), ptData.GetStartFrequency(), ptData.GetEndTimeInRunC(), ptData.GetEndFrequency());
        trackLine->SetLineColor(1);
        trackLine->SetLineWidth(1);
        trackLine->Draw();
        LDEBUG(publog, "Line drawn: (" << trackLine->GetX1() << ", " << trackLine->GetY1() << "), --> (" << trackLine->GetX2() << ", " << trackLine->GetY2() << ")");

        trackCanv->Write();

        LDEBUG(publog, "Track drawing complete");

        ++iCandidate;
        return;
    }




} /* namespace Katydid */
