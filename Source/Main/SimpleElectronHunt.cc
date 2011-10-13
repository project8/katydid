/*
 * SimpleElectronHunt.cc
 *
 *  Created on: Oct 6, 2011
 *      Author: nsoblath
 *
 *      Search for electrons with very basic peak finding and grouping
 *
 *      Usage: SimpleElectronHunt [-e egg filename] [-p ps filename] [-n # events; -1 for all] [-c control case option]
 *      Command line options
 *       -e: The input data file name
 *       -p: The output ps file name
 *       -n: The number of events to analyze; use -1 for all
 *       -c: Use this to run one of the control setups. -1 reverses the high and low margins; -2 uses large negative margins.
 */

#include "KTEgg.hh"
#include "KTEvent.hh"
#include "KTHannWindow.hh"
#include "KTSlidingWindowFFT.hh"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TMap.h"
#include "TObjArray.h"
#include "TParameter.h"
#include "TROOT.h"
#include "TStyle.h"

#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <string>


using namespace std;
using namespace Katydid;

int main(int argc, char** argv)
{
    string outputFileName("candidates.ps");
    string inputFileName("");
    Int_t numEvents = 1;

    Int_t groupBinsMarginLow = 1;
    Int_t groupBinsMarginHigh = 3;
    Int_t groupBinsMarginSameTime = 1;

    Int_t arg;
    extern char *optarg;
    while ((arg = getopt(argc, argv, "e:p:n:c")) != -1)
        switch (arg)
        {
            case 'e':
                inputFileName = string(optarg);
                break;
            case 'p':
                outputFileName = string(optarg);
                break;
            case 'n':
                numEvents = atoi(optarg);
                break;
            case 'c':
                Int_t controlOpt = atoi(optarg);
                if (controlOpt == 1)
                {
                    groupBinsMarginLow = 3;
                    groupBinsMarginHigh = 1;
                    groupBinsMarginSameTime = 1;
                }
                else if (controlOpt == 2)
                {
                    groupBinsMarginLow = 7;
                    groupBinsMarginHigh = 3;
                    groupBinsMarginSameTime = 1;
                }
                break;
        }

    if (inputFileName.empty())
    {
        cout << "Error: No egg filename given" << endl;
        return -1;
    }

    if (numEvents == -1) numEvents = 999999999;

    KTEgg* egg = new KTEgg();
    egg->SetFileName(inputFileName);
    if (! egg->BreakEgg())
    {
        cout << "Error: Egg did not break" << endl;
        return -1;
    }
    if (! egg->ParseEggHeader())
    {
        cout << "Error: Header did not parse" << endl;
        return -1;
    }

    TApplication* app = new TApplication("", 0, 0);
    TStyle *plain = new TStyle("Plain", "Plain Style");
    plain->SetCanvasBorderMode(0);
    plain->SetPadBorderMode(0);
    plain->SetPadColor(0);
    plain->SetCanvasColor(0);
    plain->SetTitleColor(0);
    plain->SetStatColor(0);
    plain->SetPalette(1);
    plain->SetOptStat(0);
    gROOT->SetStyle("Plain");

    TCanvas *c1 = new TCanvas("c1", "c1");
    Char_t tempFileName[256];
    sprintf(tempFileName, "%s[", outputFileName.c_str());
    c1->Print(tempFileName);
    c1->SetLogz(1);

    Int_t iEvent = 0;
    while (kTRUE)
    {
        if (iEvent >= numEvents) break;

        // Hatch the event
        KTEvent* event = egg->HatchNextEvent();
        if (event == NULL) break;

        TObjArray* candidates = new TObjArray();

        KTWindowFunction* wfunc = new KTHannWindow(event);
        wfunc->SetLength(1.e-5);
        cout << "window length: " << wfunc->GetLength() << " s; bin width: " << wfunc->GetBinWidth() << " s; size: " << wfunc->GetSize() << endl;

        KTSlidingWindowFFT* fft = new KTSlidingWindowFFT();
        fft->SetWindowFunction(wfunc);
        fft->SetOverlap(wfunc->GetSize() / 5);
        fft->SetTransformFlag("ES");
        fft->InitializeFFT();
        fft->TakeData(event);
        fft->Transform();

        // Need to exclude:
        //   0-.2 MHz
        //   100+/-.2 MHz
        //   200+/-.2 MHz

        TH2D* hist = fft->CreatePowerSpectrumHistogram();
        delete fft;
        // Get the first 1D FFT
        string name = string(hist->GetName()) + string("_proj");
        TH1D* histProj = hist->ProjectionY(name.c_str(), 1, 1);

        // use this bin width later:
        TH1D* tempTimeProj = hist->ProjectionX("temp", 1, 1);
        Double_t timeBinWidth = tempTimeProj->GetBinWidth(1);
        delete tempTimeProj;

        // determine search ranges
        Double_t freqBinWidth = histProj->GetBinWidth(1);
        cout << "Frequency bin width: " << freqBinWidth << " MHz" << endl;
        const Int_t nSearchRanges = 3;
        Int_t firstBins[nSearchRanges] = {histProj->FindBin(0.2)+1, histProj->FindBin(100.+0.2)+1, histProj->FindBin(200.+0.2)+1};
        Int_t lastBins[nSearchRanges] = {histProj->FindBin(100.-.2)-1, histProj->FindBin(200.-0.2)-1, histProj->GetNbinsX()-1};
        cout << "Search ranges: [" << firstBins[0] << ", " << lastBins[0] << "], [" << firstBins[1] << ", " << lastBins[1] << "], [" << firstBins[2] << ", " << lastBins[2] << "]" << endl;
        Int_t nBinsInRange[nSearchRanges] = {lastBins[0]-firstBins[0]+1, lastBins[1]-firstBins[1]+1, lastBins[2]-firstBins[2]+1};

        delete histProj;

        TObjArray* eventPeakBins = new TObjArray();
        //for (Int_t ifft=1; ifft<=10; ifft++)
        for (Int_t ifft=1; ifft<=hist->GetNbinsX(); ifft++)
        {
            histProj = hist->ProjectionY(name.c_str(), ifft, ifft);
            TObjArray* peakBins = new TObjArray();

            Double_t* dataArray = histProj->GetArray();
            for (Int_t iRange=0; iRange<nSearchRanges; iRange++)
            {
                // TMath::RMS actually calculates the standard deviation
                //cout << "Search range " << iRange << ": [" << firstBins[iRange] << ", " << lastBins[iRange] << "]" << endl;
                Double_t mean = TMath::Mean(nBinsInRange[iRange], dataArray+firstBins[iRange]);
                //cout << "   Mean: " << mean << endl;
                for (Int_t iBin=firstBins[iRange]; iBin<=lastBins[iRange]; iBin++)
                {
                    if (dataArray[iBin] > 10. * mean)
                    {
                        TParameter<Int_t>* iBinParam = new TParameter<Int_t>("param", iBin);
                        peakBins->Add(iBinParam);
                    }
                }
            }
            //cout << "FFT " << ifft << " -- Peak bins: " << peakBins->GetEntries() << endl;
            delete histProj;

            for (Int_t iPB=0; iPB<peakBins->GetEntriesFast(); iPB++)
            {
                if (peakBins->At(iPB) == NULL) continue;
                Int_t pbVal = ((TParameter<Int_t>*)peakBins->At(iPB))->GetVal();
                Bool_t foundGroup = kFALSE;
                for (Int_t iEPB=0; iEPB<eventPeakBins->GetEntriesFast(); iEPB++)
                {
                    if (eventPeakBins->At(iEPB) == NULL) continue;
                    TObjArray* groupArray = (TObjArray*)eventPeakBins->At(iEPB);
                    TPair* lastInGroup = (TPair*)groupArray->At(groupArray->GetLast());
                    Int_t lastFFT = ((TParameter<Int_t>*)lastInGroup->Key())->GetVal();
                    if (lastFFT < ifft - 1 && groupArray->GetEntries() <= 2)
                    {
                        eventPeakBins->Remove(groupArray);
                        groupArray->Delete();
                        delete groupArray;
                    }
                    else if (lastFFT == ifft - 1)
                    {
                        Int_t lastFreqBin = ((TParameter<Int_t>*)lastInGroup->Value())->GetVal();
                        if (pbVal >= lastFreqBin - groupBinsMarginLow && pbVal <= lastFreqBin + groupBinsMarginHigh)
                        {
                            TObject* key = new TParameter<Int_t>("key", ifft);
                            TObject* val = new TParameter<Int_t>("val", pbVal);
                            TObject* newInGroup = new TPair(key, val);
                            groupArray->Add(newInGroup);
                            foundGroup = kTRUE;
                            break;
                        }
                    }
                    else if (lastFFT == ifft)
                    {
                        Int_t lastFreqBin = ((TParameter<Int_t>*)lastInGroup->Value())->GetVal();
                        if (pbVal >= lastFreqBin - groupBinsMarginSameTime && pbVal <= lastFreqBin + groupBinsMarginSameTime)
                        {
                            TObject* key = new TParameter<Int_t>("key", ifft);
                            TObject* val = new TParameter<Int_t>("val", pbVal);
                            TObject* newInGroup = new TPair(key, val);
                            groupArray->Add(newInGroup);
                            foundGroup = kTRUE;
                            break;
                        }
                    }
                }
                if (foundGroup) continue;
                TObjArray* newGroupArray = new TObjArray();
                TObject* key = new TParameter<Int_t>("key", ifft);
                TObject* val = new TParameter<Int_t>("val", pbVal);
                TObject* newPair = new TPair(key, val);
                newGroupArray->Add(newPair);
                eventPeakBins->Add(newGroupArray);
            } // for loop over peakBins, for grouping purposes

            peakBins->Delete();
            delete peakBins;

        } // for loop over ffts in an event


        for (Int_t iEPB=0; iEPB<eventPeakBins->GetEntriesFast(); iEPB++)
        {
            if (eventPeakBins->At(iEPB) == NULL) continue;
            TObjArray* groupArray = (TObjArray*)eventPeakBins->At(iEPB);
            Int_t minFFT = 9999999;
            Int_t maxFFT = -1;
            for (Int_t iGroup=0; iGroup<groupArray->GetEntriesFast(); iGroup++)
            {
                if (groupArray->At(iGroup) == NULL) continue;
                TPair* thisPair = (TPair*)groupArray->At(iGroup);
                Int_t thisFFT = ((TParameter<Int_t>*)thisPair->Key())->GetVal();
                if (thisFFT < minFFT) minFFT = thisFFT;
                if (thisFFT > maxFFT) maxFFT = thisFFT;
            }
            if (maxFFT - minFFT < 2)
            {
                eventPeakBins->Remove(groupArray);
                groupArray->Delete();
                delete groupArray;
            }
        }

        Int_t nCandidates = eventPeakBins->GetEntries();
        cout << "Found " << nCandidates << " candidate groups" << endl;

        Int_t iCandidate = 0;
        Int_t frameFFT = 5;
        Int_t frameFreqBin = 5;
        for (Int_t iEPB=0; iEPB<eventPeakBins->GetEntriesFast(); iEPB++)
        {
            if (eventPeakBins->At(iEPB) == NULL) continue;
            TObjArray* groupArray = (TObjArray*)eventPeakBins->At(iEPB);
            Int_t minFFT = 9999999;
            Int_t maxFFT = -1;
            Int_t minFreqBin = 9999999;
            Int_t maxFreqBin = -1;
            //cout << "Group " << iEPB << ":  ";
            for (Int_t iGroup=0; iGroup<groupArray->GetEntriesFast(); iGroup++)
            {
                if (groupArray->At(iGroup) == NULL) continue;
                TPair* thisPair = (TPair*)groupArray->At(iGroup);
                Int_t thisFFT = ((TParameter<Int_t>*)thisPair->Key())->GetVal();
                if (thisFFT < minFFT) minFFT = thisFFT;
                if (thisFFT > maxFFT) maxFFT = thisFFT;
                Int_t thisFreqBin = ((TParameter<Int_t>*)thisPair->Value())->GetVal();
                if (thisFreqBin < minFreqBin) minFreqBin = thisFreqBin;
                if (thisFreqBin > maxFreqBin) maxFreqBin = thisFreqBin;
                //cout << ((TParameter<Int_t>*)thisPair->Key())->GetVal() << "-" << ((TParameter<Int_t>*)thisPair->Value())->GetVal() << "  ";
            }
            //cout << endl;
            Char_t histname[256], histtitle[256];
            sprintf(histname, "hCandidate_%i_%i", iEvent, iCandidate);
            sprintf(histtitle, "Candidate Group - Event %i - Candidate %i", iEvent, iCandidate);
            minFFT = TMath::Max(1, minFFT-frameFFT);
            maxFFT = TMath::Min(hist->GetNbinsX(), maxFFT+frameFFT);
            Double_t minValFFT = hist->GetBinLowEdge(minFFT);
            Double_t maxValFFT = hist->GetBinLowEdge(maxFFT) + timeBinWidth;
            minFreqBin = TMath::Max(1, minFreqBin-frameFreqBin);
            maxFreqBin = TMath::Min(hist->GetNbinsY(), maxFreqBin+frameFreqBin);
            Double_t minValFreqBin = hist->GetYaxis()->GetBinLowEdge(minFreqBin);
            Double_t maxValFreqBin = hist->GetYaxis()->GetBinLowEdge(maxFreqBin) + freqBinWidth;
            //cout << minFFT << "  " << maxFFT << "  " << minValFFT << "  " << maxValFFT << endl;
            //cout << minFreqBin << "  " << maxFreqBin << "  " << minValFreqBin << "  " << maxValFreqBin << endl;
            TH2D* groupHist = new TH2D(histname, histtitle, maxFFT-minFFT+1, minValFFT, maxValFFT, maxFreqBin-minFreqBin+1, minValFreqBin, maxValFreqBin);
            groupHist->SetXTitle("Time (s)");
            groupHist->SetYTitle("Frequency (MHz)");
            for (Int_t iFFTBin=1; iFFTBin<=groupHist->GetNbinsX(); iFFTBin++)
            {
                for (Int_t iFreqBin=1; iFreqBin<=groupHist->GetNbinsY(); iFreqBin++)
                {
                    Double_t content = hist->GetBinContent(iFFTBin+minFFT, iFreqBin+minFreqBin);
                    groupHist->SetBinContent(iFFTBin, iFreqBin, content);
                }
            }
            candidates->Add(groupHist);
            iCandidate++;
        }


        //TCanvas *c1 = new TCanvas("c1", "c1");
        //c1->SetLogz(1);
        //hist->Draw("colz");
        //c1->SetLogy(1);
        //histProj->Draw();

        //c1->WaitPrimitive();

        //delete histProj;
        eventPeakBins->Delete();
        delete eventPeakBins;
        delete hist;
        //delete c1;

        iEvent++;

        for (Int_t iCandidate=0; iCandidate<candidates->GetEntriesFast(); iCandidate++)
        {
            if (candidates->At(iCandidate) == NULL) continue;
            TH2D* thisCandidate = (TH2D*)candidates->At(iCandidate);
            thisCandidate->Draw("colz");
            c1->Print(outputFileName.c_str());
            //c1->WaitPrimitive();
        }

        candidates->Delete();
        delete candidates;

    }

    sprintf(tempFileName, "%s]", outputFileName.c_str());
    c1->Print(tempFileName);
    delete c1;

    //cout << endl;
    //cout << "Total candidates: " << candidates->GetEntries() << endl;

    /*
    TCanvas *c1 = new TCanvas("c1", "c1");
    Char_t tempFileName[256];
    sprintf(tempFileName, "%s[", outputFileName.c_str());
    c1->Print(tempFileName);
    c1->SetLogz(1);
    for (Int_t iCandidate=0; iCandidate<candidates->GetEntriesFast(); iCandidate++)
    {
        if (candidates->At(iCandidate) == NULL) continue;
        TH2D* thisCandidate = (TH2D*)candidates->At(iCandidate);
        thisCandidate->Draw("colz");
        c1->Print(outputFileName.c_str());
        //c1->WaitPrimitive();
    }
    sprintf(tempFileName, "%s]", outputFileName.c_str());
    c1->Print(tempFileName);
    delete c1;
    */

    delete egg;

    return 0;
}

