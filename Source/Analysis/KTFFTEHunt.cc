/*
 * KTFFTEHunt.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTFFTEHunt.hh"

#include "KTEggHeader.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTMaskedArray.hh"
#include "KTPhysicalArray.hh"
#include "KTPowerSpectrum.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"
#include "KTSlidingWindowFSData.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#include "TMath.h"
#endif

#include <list>
#include <map>
#include <string>
#include <sstream>

using std::list;
using std::multimap;
using std::string;
using std::stringstream;
using std::vector;

namespace Katydid
{
    KTLOGGER(ehuntlog, "katydid.fftehunt");

    KTFFTEHunt::KTFFTEHunt() :
            KTProcessor(),
            KTConfigurable(),
            fEventPeakBins(),
            fMinimumGroupSize(2),
            fCutRanges(),
            fSimpleFFT(),
            fWindowFFT(),
            fGainNorm(),
            fClustering(),
            fTextFilename("FFTEHuntOutput.txt"),
            fROOTFilename("FFTEHuntOutput.root"),
            fWriteTextFileFlag(kFALSE),
            fWriteROOTFileFlag(kTRUE),
            fTextFile(),
            //fROOTFile(),
            fFrequencyMultiplier(1.e-6),
            fTotalCandidates(0)
    {
        fConfigName = "fft-e-hunt";

        RegisterSlot("header", this, &KTFFTEHunt::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("event", this, &KTFFTEHunt::ProcessEvent, " void (const KTEvent*)");
        RegisterSlot("event_done", this, &KTFFTEHunt::FinishHunt, "void ()");

        fWindowFFT.ConnectASlot("full_fft", &fGainNorm, "freq_spect", 0);
        fWindowFFT.ConnectASlot("full_fft", &fClustering, "freq_spect", 1);

#ifdef ROOT_FOUND
        fROOTFile();
#endif
    }

    KTFFTEHunt::~KTFFTEHunt()
    {
        EmptyEventPeakBins();
#ifdef ROOT_FOUND
        if (fROOTFile.IsOpen()) fROOTFile.Close();
#endif
        if (fTextFile.is_open()) fTextFile.close();
    }

    Bool_t KTFFTEHunt::Configure(const KTPStoreNode* node)
    {
        if (node != NULL)
        {
            string filenameBase = node->GetData< string >("output-filename-base", "FFTEHuntOutput");
            fROOTFilename = filenameBase + string(".root");
            fTextFilename = filenameBase + string(".txt");
            fWriteROOTFileFlag = node->GetData< Bool_t >("write-root-file");
            fWriteTextFileFlag = node->GetData< Bool_t >("write-text-file");
            fFrequencyMultiplier = node->GetData< Double_t >("frequency-multiplier");

            fMinimumGroupSize = node->GetData< UInt_t >("minimum-group-size", 2);
            fClustering.SetMinimumGroupSize(fMinimumGroupSize);

            // TODO: cut range when arrays are ready in the parameter store
            /*
             * From ApplySetting
                if (setting->GetName() == "CutRange")
                {
                    fCutRanges.push_back(CutRange(setting->GetValue< CutRange >()));
                    return kTRUE;
                }
             *
             */

            const KTPStoreNode* clusterNode = node->GetChild("simple-clustering");
            if (clusterNode != NULL)
            {
                if (! fClustering.Configure(clusterNode)) return false;
            }

            const KTPStoreNode* simpleFFTNode = node->GetChild("simple-fft");
            if (simpleFFTNode != NULL)
            {
                if (! fSimpleFFT.Configure(simpleFFTNode)) return false;
            }

            const KTPStoreNode* slidingWindowFFTNode = node->GetChild("sliding-window-fft");
            if (slidingWindowFFTNode != NULL)
            {
                if (! fWindowFFT.Configure(slidingWindowFFTNode)) return false;
            }
        }

        // No CL options

        return true;
    }

    void KTFFTEHunt::ProcessHeader(const KTEggHeader* header)
    {
        // Process the header information
        fSimpleFFT.ProcessHeader(header);
        fWindowFFT.ProcessHeader(header);

        // Set up the bin cuts
        // get the number of frequency bins and the min and max frequencies
        Int_t nFreqBins = fWindowFFT.GetFrequencySize();
        //Double_t freqBinWidth = fWindowFFT.GetFreqBinWidth();
        Double_t freqMin = fWindowFFT.GetFreqMin();
        Double_t freqMax = fWindowFFT.GetFreqMax();
        // create KTFrequencySpectrum w/ number of bins; set bin width
        // binFinder will go out of scope at the end of this function.
        // therefore, from that point until fClusteringProc sets a new array, binCuts should NOT be used to access array values!
        KTFrequencySpectrum binFinder(nFreqBins, freqMin, freqMax);
        // create KTMaskedArray based on power spectrum magnitude array
        KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >* binCuts = new KTMaskedArray< KTFrequencySpectrum::array_type, complexpolar<Double_t> >(binFinder.data(), nFreqBins);
        // convert cut frequency ranges to bins, and cut bins from the masked array
        for (vector< CutRange >::const_iterator itCutRange = fCutRanges.begin(); itCutRange != fCutRanges.end(); itCutRange++)
        {
            Int_t firstBinCut = binFinder.FindBin((*itCutRange).first / fFrequencyMultiplier);
            Int_t nCutBins = binFinder.FindBin((*itCutRange).second / fFrequencyMultiplier) - firstBinCut + 1;
            binCuts->Cut(UInt_t(firstBinCut), UInt_t(nCutBins));
        }
        // give the masked array to fClusteringProc
        fClustering.SetBinCuts(binCuts);

        EmptyEventPeakBins();
        fClustering.SetEventPeakBinsList(&fEventPeakBins);

        if (fWriteTextFileFlag)
        {
            fTextFile.open(fTextFilename.c_str(), std::ios::out | std::ios::app);
            if (! fTextFile.is_open()) fWriteTextFileFlag = kFALSE;
        }
#ifdef ROOT_FOUND
        if (fWriteROOTFileFlag)
        {
            fROOTFile.Open(fROOTFilename.c_str(), "UPDATE");
            if (! fROOTFile.IsOpen()) fWriteROOTFileFlag = kFALSE;
        }
#endif

        fTotalCandidates = 0;

        return;
    }

    void KTFFTEHunt::ProcessEvent(const KTEvent* event)
    {
        UInt_t iEvent = event->GetEventNumber();
        if (fWriteTextFileFlag)
        {
            fTextFile << "Event " << iEvent << '\n';
        }

        const KTTimeSeriesData* tsData = dynamic_cast<const KTTimeSeriesData*>(event->GetData(KTTimeSeriesData::StaticGetName()));

        // Perform a 1-D FFT on the entire event
        const KTTimeSeries* tsDataVect = tsData->GetRecord(0);
        KTFrequencySpectrum* freqSpect = fSimpleFFT.Transform(tsDataVect);

        // Use the data from the full FFT to create a gain normalization
        fGainNorm.PrepareNormalization(freqSpect, (UInt_t)fWindowFFT.GetFrequencySize(), fWindowFFT.GetFreqBinWidth());

        // Prepare to run the windowed FFT
        //list< multimap< Int_t, Int_t >* > eventPeakBins;

        // Run the windowed FFT; the grouping algorithm is triggered at each FFT from fWindowFFTProc.
        KTSlidingWindowFSData* windowedFFTData = fWindowFFT.TransformData(tsData);
        Double_t freqBinWidth = fWindowFFT.GetFreqBinWidth() * fFrequencyMultiplier;

        // Scan through the groups
        // Remove any that are too small
        // Draw histograms and print text if requested

        Int_t iCandidate = 0;
        // when we make the plot of the group we want a frame of a few bins around the actual group
        Int_t frameFFT = 5;
        Int_t frameFreqBin = 5;
        EventPeakBinsList::iterator iEPB = fEventPeakBins.begin();
        while (! fEventPeakBins.empty())
        {
            // for each group we need to get the min and max FFT, and min and max frequency bins.
            // then we will remove the group from the list.
            multimap< Int_t, Int_t >* groupMap = *iEPB;
            Int_t minFFT = 9999999;
            Int_t maxFFT = -1;
            Int_t minFreqBin = 9999999;
            Int_t maxFreqBin = -1;
            //cout << "Group " << iEPB << ":  ";
            for (multimap< Int_t, Int_t >::iterator iGroup=groupMap->begin(); iGroup != groupMap->end(); iGroup++)
            {
                Int_t thisFFT = iGroup->first;
                if (thisFFT < minFFT) minFFT = thisFFT;
                if (thisFFT > maxFFT) maxFFT = thisFFT;
                Int_t thisFreqBin = iGroup->second;
                if (thisFreqBin < minFreqBin) minFreqBin = thisFreqBin;
                if (thisFreqBin > maxFreqBin) maxFreqBin = thisFreqBin;
                //cout << iGroup->first << "-" << iGroup->second << "  ";
            }
            //cout << endl;

            // we're done with this multimap object; all we need is min/maxFFT and min/maxFreqBin
            delete groupMap;
            iEPB = fEventPeakBins.erase(iEPB); // move the iterator back one so we don't skip anything when the for loop advances the iterator

            // check if this group is too small in time
            if (maxFFT - minFFT < 2) continue;

            if (fWriteTextFileFlag)
            {
                Double_t meanFreq = ((Double_t)maxFreqBin - 1 - (Double_t)(maxFreqBin - minFreqBin)/2.) * freqBinWidth;
                fTextFile << meanFreq << "   ";
            }

#ifdef ROOT_FOUND
            if (fWriteROOTFileFlag)
            {
                stringstream conv;
                string histName;
                conv << iEvent;
                conv >> histName;
                histName = string("histWindowedPS") + histName;
                TH2D* histWaterfall = windowedFFTData->CreatePowerHistogram(0, histName);
                Double_t timeBinWidth = histWaterfall->GetXaxis()->GetBinWidth(1);

                Char_t histname[256], histtitle[256];
                sprintf(histname, "hCandidate_%i_%i", iEvent, iCandidate);
                sprintf(histtitle, "Candidate Group - Event %i - Candidate %i", iEvent, iCandidate);
                minFFT = TMath::Max(1, minFFT-frameFFT);
                maxFFT = TMath::Min(histWaterfall->GetNbinsX(), maxFFT+frameFFT);
                Double_t minValFFT = histWaterfall->GetBinLowEdge(minFFT);
                Double_t maxValFFT = histWaterfall->GetBinLowEdge(maxFFT) + timeBinWidth;
                minFreqBin = TMath::Max(1, minFreqBin-frameFreqBin);
                maxFreqBin = TMath::Min(histWaterfall->GetNbinsY(), maxFreqBin+frameFreqBin);
                Double_t minValFreqBin = histWaterfall->GetYaxis()->GetBinLowEdge(minFreqBin);
                Double_t maxValFreqBin = histWaterfall->GetYaxis()->GetBinLowEdge(maxFreqBin) + freqBinWidth;
                //cout << minFFT << "  " << maxFFT << "  " << minValFFT << "  " << maxValFFT << endl;
                //cout << minFreqBin << "  " << maxFreqBin << "  " << minValFreqBin << "  " << maxValFreqBin << endl;
                TH2D* groupHist = new TH2D(histname, histtitle, maxFFT-minFFT+1, minValFFT, maxValFFT, maxFreqBin-minFreqBin+1, minValFreqBin, maxValFreqBin);
                groupHist->SetXTitle("Time (s)");
                groupHist->SetYTitle("Frequency (MHz)");
                for (Int_t iFFTBin=1; iFFTBin<=groupHist->GetNbinsX(); iFFTBin++)
                {
                    for (Int_t iFreqBin=1; iFreqBin<=groupHist->GetNbinsY(); iFreqBin++)
                    {
                        Double_t content = histWaterfall->GetBinContent(iFFTBin+minFFT, iFreqBin+minFreqBin);
                        groupHist->SetBinContent(iFFTBin, iFreqBin, content);
                    }
                }

                //groupHist->Draw("colz");
                //c1->Print(outputFileNamePS.c_str());

                groupHist->Write();
                delete histWaterfall;
            }
#endif

            //candidates->Add(groupHist);
            iCandidate++;
        }

        KTINFO(ehuntlog, "Found " << iCandidate << " candidate groups");
        if (fWriteTextFileFlag)
        {
            fTextFile << '\n';
            fTextFile << "  " << iCandidate << " candidates found" << '\n';
            fTextFile << "------------------------------------" << '\n';
        }

        fTotalCandidates += iCandidate;

        delete freqSpect;
        delete windowedFFTData;

        return;
    }

    void KTFFTEHunt::FinishHunt()
    {
        if (fTextFile.is_open())
        {
            fTextFile << "Total candidates found in this file: " << fTotalCandidates << '\n';
            fTextFile.close();
        }
#ifdef ROOT_FOUND
        if (fROOTFile.IsOpen()) fROOTFile.Close();
#endif
        return;
    }

    void KTFFTEHunt::EmptyEventPeakBins()
    {
        while (! fEventPeakBins.empty())
        {
            delete fEventPeakBins.back();
            fEventPeakBins.pop_back();
        }
        return;
    }

} /* namespace Katydid */
