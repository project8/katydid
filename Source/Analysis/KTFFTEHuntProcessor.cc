/*
 * KTFFTEHuntProcessor.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTFFTEHuntProcessor.hh"

#include "KTPhysicalArray.hh"
#include "KTHannWindow.hh"
#include "KTMaskedArray.hh"
#include "KTPowerSpectrum.hh"

#include "TH2.h"

#include "boost/bind.hpp"

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>

using std::cout;
using std::endl;
using std::list;
using std::multimap;
using std::string;
using std::stringstream;
using std::vector;

namespace Katydid
{

    KTFFTEHuntProcessor::KTFFTEHuntProcessor() :
            fEventPeakBins(),
            fMinimumGroupSize(2),
            fCutRanges(),
            fSimpleFFTProc(),
            fWindowFFTProc(),
            fGainNormProc(),
            fClusteringProc(),
            fTextFilename("FFTEHuntOutput.txt"),
            fROOTFilename("FFTEHuntOutput.root"),
            fWriteTextFileFlag(kFALSE),
            fWriteROOTFileFlag(kTRUE),
            fTextFile(),
            fROOTFile(),
            fFrequencyMultiplier(1.e-6),
            fTotalCandidates(0)
    {
        fGainNormProc.SetPowerSpectrumSlotConnection(fWindowFFTProc.GetFFT()->ConnectToFFTSignal( 0, boost::bind(&KTGainNormalizationProcessor::ProcessPowerSpectrum, boost::ref(fGainNormProc), _1, _2) ));
        fClusteringProc.SetPowerSpectrumSlotConnection(fWindowFFTProc.GetFFT()->ConnectToFFTSignal( 1, boost::bind(&KTSimpleClusteringProcessor::ProcessPowerSpectrum, boost::ref(fClusteringProc), _1, _2) ));
    }

    KTFFTEHuntProcessor::~KTFFTEHuntProcessor()
    {
        EmptyEventPeakBins();
        if (fROOTFile.IsOpen()) fROOTFile.Close();
        if (fTextFile.is_open()) fTextFile.close();
    }

    Bool_t KTFFTEHuntProcessor::ApplySetting(const KTSetting* setting)
    {
        if (setting->GetName() == "ThresholdMult" || setting->GetName() == "GroupBinsMarginHigh" || setting->GetName() == "GroupBinsMarginLow" || setting->GetName() == "GroupBinsMarginSameTime" || setting->GetName() == "FirstBinToUse")
        {
            return fClusteringProc.ApplySetting(setting);
        }
        if (setting->GetName() == "MinimumGroupSize")
        {
            fMinimumGroupSize = setting->GetValue< UInt_t >();
            fClusteringProc.SetMinimumGroupSize(2);
            return kTRUE;
        }
        if (setting->GetName() == "CutRange")
        {
            fCutRanges.push_back(CutRange(setting->GetValue< CutRange >()));
            return kTRUE;
        }
        if (setting->GetName() == "ROOTFilename")
        {
            fROOTFilename = setting->GetValue< string >();
            return kTRUE;
        }
        if (setting->GetName() == "TextFilename")
        {
            fTextFilename = setting->GetValue< string >();
            return kTRUE;
        }
        if (setting->GetName() == "WriteROOTFileFlag")
        {
            fWriteROOTFileFlag = setting->GetValue< Bool_t >();
            return kTRUE;
        }
        if (setting->GetName() == "WriteTextFileFlag")
        {
            fWriteTextFileFlag = setting->GetValue< Bool_t >();
            return kTRUE;
        }
        if (setting->GetName() == "FrequencyMultiplier")
        {
            fFrequencyMultiplier = setting->GetValue< Double_t >();
            return kTRUE;
        }
        return kFALSE;
    }

    void KTFFTEHuntProcessor::ProcessHeader(KTEgg::HeaderInfo headerInfo)
    {
        // Initialize the processors that will be used for each event
        KTSetting settingFFTTransFlag("TransformFlag", string("ES"));
        fSimpleFFTProc.ApplySetting(&settingFFTTransFlag);

        //KTSetting settingFFTTransFlag("TransformFlag", string("ES"));
        fWindowFFTProc.ApplySetting(&settingFFTTransFlag);

        KTWindowFunction* winFunc = new KTHannWindow();
        winFunc->SetLength(1.e-5);
        KTSetting settingWinFunc("WindowFunction", winFunc);
        KTSetting settingFFTOverlap("OverlapFrac", 0.2);
        fWindowFFTProc.ApplySetting(&settingWinFunc);
        fWindowFFTProc.ApplySetting(&settingFFTOverlap);

        // Process the header information
        fSimpleFFTProc.ProcessHeader(headerInfo);
        fWindowFFTProc.ProcessHeader(headerInfo);

        // Set up the bin cuts
        // get the number of frequency bins and the frequency bin width
        Int_t nFreqBins = fWindowFFTProc.GetFFT()->GetFrequencySize();
        Double_t freqBinWidth = fWindowFFTProc.GetFFT()->GetFreqBinWidth();
        // create KTPowerSpectrum w/ number of bins; set bin width
        // binFinder will go out of scope at the end of this function.
        // therefore, from that point until fClusteringProc sets a new array, binCuts should NOT be used to access array values!
        KTPowerSpectrum binFinder(nFreqBins, freqBinWidth);
        // create KTMaskedArray based on power spectrum magnitude array
        KTMaskedArray< Double_t*, Double_t >* binCuts = new KTMaskedArray< Double_t*, Double_t >(binFinder.GetMagnitude().GetMatrixArray(), nFreqBins);
        // convert cut frequency ranges to bins, and cut bins from the masked array
        for (vector< CutRange >::const_iterator itCutRange = fCutRanges.begin(); itCutRange != fCutRanges.end(); itCutRange++)
        {
            Int_t firstBinCut = binFinder.GetBin((*itCutRange).first / fFrequencyMultiplier);
            Int_t nCutBins = binFinder.GetBin((*itCutRange).second / fFrequencyMultiplier) - firstBinCut + 1;
            binCuts->Cut(UInt_t(firstBinCut), UInt_t(nCutBins));
        }
        // give the masked array to fClusteringProc
        fClusteringProc.SetBinCuts(binCuts);

        EmptyEventPeakBins();
        fClusteringProc.SetEventPeakBinsList(&fEventPeakBins);

        if (fWriteTextFileFlag)
        {
            fTextFile.open(fTextFilename.c_str(), std::ios::out | std::ios::app);
            if (! fTextFile.is_open()) fWriteTextFileFlag = kFALSE;
        }
        if (fWriteROOTFileFlag)
        {
            fROOTFile.Open(fROOTFilename.c_str(), "UPDATE");
            if (! fROOTFile.IsOpen()) fWriteROOTFileFlag = kFALSE;
        }

        fTotalCandidates = 0;

        return;
    }

    void KTFFTEHuntProcessor::ProcessEvent(UInt_t iEvent, const KTEvent* event)
    {
        if (fWriteTextFileFlag)
        {
            fTextFile << "Event " << iEvent << endl;
        }

        // Perform a 1-D FFT on the entire event
        fSimpleFFTProc.ProcessEvent(iEvent, event);
        KTPhysicalArray< 1, Double_t >* fullFFT = fSimpleFFTProc.GetFFT()->CreatePowerSpectrumPhysArr();

        // Use the data from the full FFT to create a gain normalization
        fGainNormProc.PrepareNormalization(fullFFT, (UInt_t)fWindowFFTProc.GetFFT()->GetFrequencySize(), fWindowFFTProc.GetFFT()->GetFreqBinWidth());
        delete fullFFT;

        // Prepare to run the windowed FFT
        //list< multimap< Int_t, Int_t >* > eventPeakBins;

        // Run the windowed FFT; the grouping algorithm is triggered at each FFT from fWindowFFTProc.
        fWindowFFTProc.ProcessEvent(iEvent, event);
        Double_t freqBinWidth = fWindowFFTProc.GetFFT()->GetFreqBinWidth() * fFrequencyMultiplier;

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

            if (fWriteROOTFileFlag)
            {
                stringstream conv;
                string histName;
                conv << iEvent;
                conv >> histName;
                histName = string("histWindowedPS") + histName;
                TH2D* histWaterfall = fWindowFFTProc.GetFFT()->CreatePowerSpectrumHistogram(histName);
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
            }

            //candidates->Add(groupHist);
            iCandidate++;
        }

        cout << "Found " << iCandidate << " candidate groups" << endl;
        if (fWriteTextFileFlag)
        {
            fTextFile << endl;
            fTextFile << "  " << iCandidate << " candidates found" << endl;
            fTextFile << "------------------------------------" << endl;
        }

        fTotalCandidates += iCandidate;

        return;
    }

    void KTFFTEHuntProcessor::FinishHunt()
    {
        if (fTextFile.is_open())
        {
            fTextFile << "Total candidates found in this file: " << fTotalCandidates << endl;
            fTextFile.close();
        }
        if (fROOTFile.IsOpen()) fROOTFile.Close();
        return;
    }

    void KTFFTEHuntProcessor::EmptyEventPeakBins()
    {
        while (! fEventPeakBins.empty())
        {
            delete fEventPeakBins.back();
            fEventPeakBins.pop_back();
        }
        return;
    }

} /* namespace Katydid */
