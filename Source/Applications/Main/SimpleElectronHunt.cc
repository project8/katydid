/*
 * SimpleElectronHunt.cc
 *
 *  Created on: Oct 6, 2011
 *      Author: nsoblath
 *
 *      Search for electrons with very basic peak finding and grouping
 *
 *      Usage: SimpleElectronHunt [-e egg filename] [-p ps filename] [-n # bundles; -1 for all] [-c control case option]
 *      Command line options
 *       -e: The input data file name
 *       -p: The output file name base (there will be .root and .ps files)
 *       -n: The number of bundles to analyze; use -1 for all
 *       -t: Threshold in multiples of the mean (default is 10)
 *       -c: Use this to run one of the control setups. -1 reverses the high and low margins; -2 uses large negative margins.
 *       -d: Include this flag to draw waterfall plots
 */

#include "KTEgg.hh"
#include "KTBundle.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTHannWindow.hh"
#include "KTPowerSpectrum.hh"
#include "KTSimpleFFT.hh"
#include "KTSlidingWindowFFT.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTTimeSeriesChannelData.hh"

#include "TApplication.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"

#include <algorithm>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <list>
#include <map>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>


using namespace std;
using namespace Katydid;

int main(int argc, char** argv)
{
    string outputFileNameBase("candidates");
    string inputFileName("");
    Int_t numEvents = 1;

    Double_t thresholdMult = 10.;

    Bool_t drawWaterfall = kFALSE;

    Int_t groupBinsMarginLow = 1;
    Int_t groupBinsMarginHigh = 3;
    Int_t groupBinsMarginSameTime = 1;

    Int_t arg;
    extern char *optarg;
    while ((arg = getopt(argc, argv, "e:p:n:t:c:d")) != -1)
        switch (arg)
        {
            case 'e':
                inputFileName = string(optarg);
                break;
            case 'p':
                outputFileNameBase = string(optarg);
                break;
            case 'n':
                numEvents = atoi(optarg);
                break;
            case 't':
                thresholdMult = atof(optarg);
                break;
            case 'c':
            {
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
            case 'd':
                drawWaterfall = kTRUE;
                break;
        }

    if (inputFileName.empty())
    {
        cout << "Error: No egg filename given" << endl;
        return -1;
    }

    string outputFileNameRoot = outputFileNameBase + string(".root");
    string outputFileNamePS = outputFileNameBase + string(".ps");
    string outputFileNameText = outputFileNameBase + string(".txt");

    if (numEvents == -1) numEvents = 999999999;

    ofstream txtOutFile(outputFileNameText.c_str());
    txtOutFile << "Egg file: " << inputFileName << endl;
    txtOutFile << "------------------------------------" << endl;

    KTEgg egg;
    if (! egg.BreakEgg(inputFileName))
    {
        cout << "Error: Egg did not break" << endl;
        return -1;
    }

    TFile* outFile = new TFile(outputFileNameRoot.c_str(), "recreate");

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

    TCanvas *c1 = NULL;
    Char_t tempFileName[256];
    if (drawWaterfall)
    {
        c1 = new TCanvas("c1", "c1");
        sprintf(tempFileName, "%s[", outputFileNamePS.c_str());
        c1->Print(tempFileName);
        c1->SetLogz(1);
    }

    // so that these variables exist after the while loop
    Double_t timeBinWidth = 0., freqBinWidth = 0.;
    Double_t freqHistMin = 0., freqHistMax = 0.;
    Int_t freqHistNBins = 0;

    vector< Int_t > globalPeakFreqBins;

    Int_t totalCandidates = 0;
    Int_t iEvent = 0;
    while (kTRUE)
    {
        if (iEvent >= numEvents) break;
        txtOutFile << "Event " << iEvent << endl;
        txtOutFile << "   Candidate bundle frequencies (MHz): ";

        // Hatch the bundle
        KTBundle* bundle = egg.HatchNextEvent();
        if (bundle == NULL) break;
        KTProgenitorTimeSeriesData* data = bundle->GetData< KTProgenitorTimeSeriesData >("time-series");
        if (data == NULL) break;

        // FFT of the entire bundle, which will be used to normalize the gain fluctuations
        KTSimpleFFT fullFFT;
        fullFFT.SetTimeSize(data->GetRecordSize());
        fullFFT.SetTransformFlag("ES");
        fullFFT.InitializeFFT();
        KTFrequencySpectrumData* freqSpectData = fullFFT.TransformData(data);

        TH1D* histFullPS = freqSpectData->GetSpectrum(0)->CreatePowerHistogram();
        /**/// DEBUG
        if (drawWaterfall)
        {
            c1->SetLogy(1);
            histFullPS->Draw();
            //c1->WaitPrimitive();
            c1->Print(outputFileNamePS.c_str());
            c1->SetLogy(0);
        }
        /**/
        delete freqSpectData;
        Double_t fullPSFreqBinWidth = histFullPS->GetBinWidth(1);

        // Now the windowed FFT
        KTBundleWindowFunction* wfunc = new KTHannWindow(data);
        wfunc->SetLength(1.e-5);
        cout << "window length: " << wfunc->GetLength() << " s; bin width: " << wfunc->GetBinWidth() << " s; size: " << wfunc->GetSize() << endl;

        KTSlidingWindowFFT fft;
        fft.SetWindowFunction(wfunc);
        fft.SetOverlap(wfunc->GetSize() / 5);
        fft.SetTransformFlag("ES");
        fft.InitializeFFT();
        KTSlidingWindowFSData* swFSData = fft.TransformData(data);

        // Need to exclude:
        //   0-.2 MHz
        //   100+/-.2 MHz
        //   200+/-.2 MHz

        stringstream conv;
        string histName;
        conv << iEvent;
        conv >> histName;
        histName = string("histWindowedPS") + histName;
        TH2D* hist = swFSData->CreatePowerHistogram(0, histName);
        delete swFSData;
        // DEBUG
        if (drawWaterfall)
        {
            hist->Draw("COLZ");
            //c1->WaitPrimitive();
            c1->Print(outputFileNamePS.c_str());
        }


        // use this bin width later:
        TH1D* tempTimeProj = hist->ProjectionX("temp", 1, 1);
        timeBinWidth = tempTimeProj->GetBinWidth(1);
        delete tempTimeProj;

        // Get the first 1D FFT
        string name = string(hist->GetName()) + string("_proj");
        TH1D* histProj = hist->ProjectionY(name.c_str(), 1, 1);

        // determine search ranges
        freqBinWidth = histProj->GetBinWidth(1);
        freqHistMin = histProj->GetXaxis()->GetXmin();
        freqHistMax = histProj->GetXaxis()->GetXmax();
        freqHistNBins = histProj->GetNbinsX();
        cout << "Frequency bin width: " << freqBinWidth << " MHz" << endl;
        const Int_t nSearchRanges = 3;
        Int_t firstBins[nSearchRanges] = {histProj->FindBin(0.2)+1, histProj->FindBin(100.+0.2)+1, histProj->FindBin(200.+0.2)+1};
        Int_t lastBins[nSearchRanges] = {histProj->FindBin(100.-.2)-1, histProj->FindBin(200.-0.2)-1, histProj->GetNbinsX()-1};
        cout << "Search ranges: [" << firstBins[0] << ", " << lastBins[0] << "], [" << firstBins[1] << ", " << lastBins[1] << "], [" << firstBins[2] << ", " << lastBins[2] << "]" << endl;
        Int_t nBinsInRange[nSearchRanges] = {lastBins[0]-firstBins[0]+1, lastBins[1]-firstBins[1]+1, lastBins[2]-firstBins[2]+1};

        Int_t firstBinsFullPS[nSearchRanges] = {histFullPS->FindBin(0.2)+1, histFullPS->FindBin(100.+0.2)+1, histFullPS->FindBin(200.+0.2)+1};
        Int_t lastBinsFullPS[nSearchRanges] = {histFullPS->FindBin(100.-.2)-1, histFullPS->FindBin(200.-0.2)-1, histFullPS->GetNbinsX()-1};

        // Create a histogram to store the gain normalization
        TH1D* histGainNorm = (TH1D*)(histProj->Clone());
        histGainNorm->Clear();
        histGainNorm->SetNameTitle("GainNorm","GainNorm");
        for (Int_t iBin=1; iBin<=histGainNorm->GetNbinsX(); iBin++)
        {
            Double_t freqBinMin = histGainNorm->GetBinLowEdge(iBin);
            Double_t freqBinMax = histGainNorm->GetBinLowEdge(iBin+1);
            Int_t firstBinFullPS = histFullPS->FindBin(freqBinMin);
            Int_t lastBinFullPS = histFullPS->FindBin(freqBinMax);
            Double_t meanBinContent = 0.;
            Int_t nBinsInSum = 0;
            for (Int_t iSubBin=firstBinFullPS; iSubBin<=lastBinFullPS; iSubBin++)
            {
                if ((iSubBin >= firstBinsFullPS[0] && iSubBin <= lastBinsFullPS[0]) ||
                        (iSubBin >= firstBinsFullPS[1] && iSubBin <= lastBinsFullPS[1]) ||
                        (iSubBin >= firstBinsFullPS[2] && iSubBin <= lastBinsFullPS[2]))
                {
                    meanBinContent += histFullPS->GetBinContent(iSubBin);
                    nBinsInSum++;
                }
            }
            //if (nBinsInSum != 0) meanBinContent /= (Double_t)nBinsInSum;
            histGainNorm->SetBinContent(iBin, meanBinContent);
            //cout << "Gain norm bin " << iBin << "  content: " << meanBinContent << endl;
        }
        cout << "gain normalization calculation complete" << endl;
        /**/// DEBUG
        if (drawWaterfall)
        {
            c1->SetLogy(1);
            histGainNorm->SetTitle("gain normalization");
            histGainNorm->Draw();
            //c1->WaitPrimitive();
            c1->Print(outputFileNamePS.c_str());
            c1->SetLogy(0);
        }
        /**/

        // Rebin the full-bundle power spectrum
        //Int_t rebinFactor = TMath::FloorNint((Double_t)histFullPS->GetNbinsX() / (Double_t)freqHistNBins);
        //histFullPS->Rebin(rebinFactor);
        //histFullPS->Scale(1. / (Double_t)rebinFactor);

        delete histProj;

        list< multimap< Int_t, Int_t >* > bundlePeakBins;
        //list< multimap< Int_t, Int_t >* > bundlePeakBinsActive;

        // Look for the highest-peaked bins
        cout << "selecting peaked bins and grouping them" << endl;
        //for (Int_t ifft=1; ifft<=10; ifft++)
        for (Int_t ifft=1; ifft<=hist->GetNbinsX(); ifft++)
        {
            //cout << "fft " << ifft << flush;
            // Get this fft's histogram
            histProj = hist->ProjectionY(name.c_str(), ifft, ifft);
            // normalize the histogram based on the full-PS normalization values (calcuclated above)
            //cout << "integral before: " << histProj->Integral() << endl;
            histProj->Divide(histGainNorm);
            //cout << "integral after: " << histProj->Integral() << endl;
            /**/// DEBUG
            if (drawWaterfall && ifft < 5)
            {
                c1->SetLogy(1);
                char projnum[30];
                sprintf(projnum, "%s%i", "fft #", ifft);
                histProj->SetTitle(projnum);
                histProj->Draw();
                //c1->WaitPrimitive();
                c1->Print(outputFileNamePS.c_str());
                c1->SetLogy(0);
            }
            /**/

            // this will hold the bin numbers that are above the threshold
            set< Int_t > peakBins;

            Double_t* dataArray = histProj->GetArray();
            Double_t mean = 0.;
            for (Int_t iRange=0; iRange<nSearchRanges; iRange++)
            {
                // TMath::RMS actually calculates the standard deviation
                //cout << "Search range " << iRange << ": [" << firstBins[iRange] << ", " << lastBins[iRange] << "]" << "  nbins: " << nBinsInRange[iRange] << "  mean: " << TMath::Mean(nBinsInRange[iRange], dataArray+firstBins[iRange]) << endl;
                mean += TMath::Mean(nBinsInRange[iRange], dataArray+firstBins[iRange]) * (Double_t)nBinsInRange[iRange];
                //cout << "   Mean: " << mean << endl;
            }
            mean /= (Double_t)(nBinsInRange[0] + nBinsInRange[1] + nBinsInRange[2]);

            Double_t threshold = thresholdMult * mean;
            // at this point histProj's array of data will be directly modified.
            // the histogram should no longer be used as a histogram until it's remade
            Double_t* histProjData = histProj->GetArray();

            //cout << "mean: " << mean << "  threshold: " << threshold << endl;

            // set the bin values outside of the three search ranges to -1
            for (Int_t iRange=0; iRange<nSearchRanges-1; iRange++)
            {
                for(Int_t iBin=lastBins[iRange]+1; iBin<firstBins[iRange+1]; iBin++)
                {
                    histProjData[iBin] = -1.;
                }
            }

            // search for bins above the threshold
            for (Int_t iRange=0; iRange<nSearchRanges; iRange++)
            {
                for (Int_t iBin=firstBins[iRange]; iBin<=lastBins[iRange]; iBin++)
                {
                    Double_t binCenter = freqBinWidth * ((Double_t)iBin-0.5);
                    //Int_t fullPSBin = binCenter / fullPSFreqBinWidth + 0.5;
                    // test the bin agains the threshold
                    if (dataArray[iBin] > threshold)
                    {
                        peakBins.insert(iBin);
                        globalPeakFreqBins.push_back(iBin);
                    }
                }
            }
            //cout << "FFT " << ifft << " -- Peak bins: " << peakBins.size() << endl;
            delete histProj;

            /*
            // Attach peak bins to groups or form new groups
            //cout << "; grouping" << flush;
            for (list< multimap< Int_t, Int_t >* >::iterator iEPBA=bundlePeakBinsActive.begin(); iEPBA!=bundlePeakBinsActive.end(); iEPBA++)
            {
                multimap< Int_t, Int_t >* groupMap = *iEPBA;
                multimap< Int_t, Int_t >::iterator lastGroup = groupMap->end();
                lastGroup--;
                Int_t lastFFT = lastGroup->first;
                // check if we've passed this group
                if (lastFFT < ifft - 1)
                {
                    // if the group is too small, remove it
                    if (groupMap->size() <= 2)
                    {
                        delete groupMap;
                        iEPBA = bundlePeakBinsActive.erase(iEPBA);
                        iEPBA--; // move the iterator back one so we don't skip anything when the for loop advances the iterator
                        continue;
                    }
                    // otherwise, transfer it to completed groups list
                    bundlePeakBins.push_back(groupMap);
                    bundlePeakBinsActive.erase(iEPBA);
                    continue;
                }

                // Find the range of frequency bins covered by the current group
                pair< multimap< Int_t, Int_t >::iterator, multimap< Int_t, Int_t >::iterator > lastFFTRange =
                        groupMap->equal_range(lastFFT);
                multimap< Int_t, Int_t >::iterator firstGroupInRange = lastFFTRange.first;
                Int_t firstGroupFreqBin = firstGroupInRange->second;
                Int_t lastGroupFreqBin = firstGroupFreqBin;
                firstGroupInRange++;
                for (multimap< Int_t, Int_t >::iterator grIt=firstGroupInRange; grIt!=lastFFTRange.second; grIt++)
                {
                    if (grIt->second > lastGroupFreqBin) lastGroupFreqBin = grIt->second;
                    else if (grIt->second < firstGroupFreqBin) firstGroupFreqBin = grIt->second;
                }

                // Loop over the peak bins from the FFT
                for (set< Int_t >::iterator iPB=peakBins.begin(); iPB!=peakBins.end(); iPB++)
                {
                    Int_t pbVal = *iPB;
                    Bool_t foundGroup = kFALSE;

                    // check grouping condition for case where this fft is the next one past the end of the group
                    if (lastFFT == ifft - 1)
                    {
                        if (pbVal >= firstGroupFreqBin - groupBinsMarginLow && pbVal <= lastGroupFreqBin + groupBinsMarginHigh)
                        {
                            groupMap->insert( pair< Int_t, Int_t >(ifft, pbVal) );
                            foundGroup = kTRUE;
                        }
                    }
                    // check the grouping condition for case where this fft is the same as the end of the group
                    else if (lastFFT == ifft)
                    {
                        if (pbVal >= firstGroupFreqBin - groupBinsMarginSameTime && pbVal <= lastGroupFreqBin + groupBinsMarginSameTime)
                        {
                            groupMap->insert( pair< Int_t, Int_t >(ifft, pbVal) );
                            foundGroup = kTRUE;
                        }
                    }

                    // actions if the peak bin is grouped; remove the peak bin from the set and check if the group has increased in frequency bin range
                    if (foundGroup == kTRUE)
                    {
                        set< Int_t >::iterator iPBCopy = iPB;
                        iPB--;
                        peakBins.erase(iPB);
                        if (pbVal < firstGroupFreqBin) firstGroupFreqBin = pbVal;
                        if (pbVal > lastGroupFreqBin) lastGroupFreqBin = pbVal;
                        break;
                    }
                } // end loop over peak bins

            } // end loop over groups

            // loop over the remaining peak bins and add them as new groups
            //cout << "; adding new groups" << flush;
            Int_t lastPBVal = -9999999;
            multimap< Int_t, Int_t >* lastNewGroupMap = NULL;
            for (set< Int_t >::iterator iPB=peakBins.begin(); iPB!=peakBins.end(); iPB++)
            {
                Int_t pbVal = *iPB;
                // see if this peak bin should be attached to the last new group made
                if (lastNewGroupMap != NULL)
                {
                    if (abs(pbVal - lastPBVal) < groupBinsMarginSameTime)
                    {
                        lastNewGroupMap->insert( pair< Int_t, Int_t >(ifft, pbVal) );
                        continue;
                    }
                }
                multimap< Int_t, Int_t >* newGroupMap = new multimap< Int_t, Int_t >();
                newGroupMap->insert( pair< Int_t, Int_t >(ifft, pbVal) );
                bundlePeakBins.push_back(newGroupMap);
                lastNewGroupMap = newGroupMap;
                lastPBVal = pbVal;
            }

            //cout << "; done" << endl;
            */


            /**/
            // Look for groups
            //cout << "; grouping" << flush;
            for (set< Int_t >::iterator iPB=peakBins.begin(); iPB!=peakBins.end(); iPB++)
            {
                Int_t pbVal = *iPB;
                Bool_t foundGroup = kFALSE;
                for (list< multimap< Int_t, Int_t >* >::iterator iEPB=bundlePeakBins.begin(); iEPB!=bundlePeakBins.end(); iEPB++)
                {
                    multimap< Int_t, Int_t >* groupMap = *iEPB;
                    multimap< Int_t, Int_t >::iterator lastGroup = groupMap->end();
                    lastGroup--;
                    Int_t lastFFT = lastGroup->first;
                    // check if we've passed this group, and if so, if the group is too small, remove it
                    if (lastFFT < ifft - 1 && groupMap->size() <= 2)
                    {
                        delete groupMap;
                        iEPB = bundlePeakBins.erase(iEPB);
                        iEPB--; // move the iterator back one so we don't skip anything when the for loop advances the iterator
                        continue;
                    }
                    pair< multimap< Int_t, Int_t >::iterator, multimap< Int_t, Int_t >::iterator > lastFFTRange =
                            groupMap->equal_range(lastFFT);
                    multimap< Int_t, Int_t >::iterator firstGroupInRange = lastFFTRange.first;
                    Int_t firstGroupFreqBin = firstGroupInRange->second;
                    Int_t lastGroupFreqBin = firstGroupFreqBin;
                    firstGroupInRange++;
                    for (multimap< Int_t, Int_t >::iterator grIt=firstGroupInRange; grIt!=lastFFTRange.second; grIt++)
                    {
                        if (grIt->second > lastGroupFreqBin) lastGroupFreqBin = grIt->second;
                        else if (grIt->second < firstGroupFreqBin) firstGroupFreqBin = grIt->second;
                    }

                    if (lastFFT == ifft - 1)
                    {
                        if (pbVal >= firstGroupFreqBin - groupBinsMarginLow && pbVal <= lastGroupFreqBin + groupBinsMarginHigh)
                        {
                            groupMap->insert( pair< Int_t, Int_t >(ifft, pbVal) );
                            foundGroup = kTRUE;
                            break;
                        }
                    }
                    else if (lastFFT == ifft)
                    {
                        if (pbVal >= firstGroupFreqBin - groupBinsMarginSameTime && pbVal <= lastGroupFreqBin + groupBinsMarginSameTime)
                        {
                            groupMap->insert( pair< Int_t, Int_t >(ifft, pbVal) );
                            foundGroup = kTRUE;
                            break;
                        }
                    }
                }
                if (foundGroup) continue;

                // no match to existing groups, so add a new one
                multimap< Int_t, Int_t >* newGroupMap = new multimap< Int_t, Int_t >();
                newGroupMap->insert( pair< Int_t, Int_t >(ifft, pbVal) );
                bundlePeakBins.push_back(newGroupMap);
            } // for loop over peakBins, for grouping purposes

            //cout << "; done" << endl;
            /**/

            // we are now done with this fft.
            // peak bins have been found and checked for inclusion in previous groups.
            // a new group was created if a peak bin did not correspond to a previous group.

        } // for loop over ffts in an bundle

        delete histGainNorm;

        /*
        // do a final loop over the remaining active groups; discard if too small or add to the final list of groups
        for (list< multimap< Int_t, Int_t >* >::iterator iEPBA=bundlePeakBinsActive.begin(); iEPBA!=bundlePeakBinsActive.end(); iEPBA++)
        {
            multimap< Int_t, Int_t >* groupMap = *iEPBA;
            // if the group is too small, remove it
            if (groupMap->size() <= 2)
            {
                delete groupMap;
                iEPBA = bundlePeakBinsActive.erase(iEPBA);
                iEPBA--; // move the iterator back one so we don't skip anything when the for loop advances the iterator
                continue;
            }
            // otherwise, transfer it to completed groups list
            bundlePeakBins.push_back(groupMap);
            bundlePeakBinsActive.erase(iEPBA);
            continue;
        }
        */

        // now we will scan over the groups in the bundle and draw them.
        // there's still a chance that the groups finished in the last fft are too small, so we'll check the group size.

        cout << "scanning over groups in the bundle" << endl;
        Int_t iCandidate = 0;
        // when we make the plot of the group we want a frame of a few bins around the actual group
        Int_t frameFFT = 5;
        Int_t frameFreqBin = 5;
        list< multimap< Int_t, Int_t >* >::iterator iEPB=bundlePeakBins.begin();
        while (! bundlePeakBins.empty())
        {
            // for each group we need to get the min and max FFT, and min and max frequency bins.
            // then we will remove the group from the list.
            multimap< Int_t, Int_t >* groupMap = *iEPB;
            Int_t minFFT = 9999999;
            Int_t maxFFT = -1;
            Int_t minFreqBin = 9999999;
            Int_t maxFreqBin = -1;
            //cout << "Group " << iCandidate << ":  " << flush;
            for (multimap< Int_t, Int_t >::iterator iGroup=groupMap->begin(); iGroup != groupMap->end(); iGroup++)
            {
                Int_t thisFFT = iGroup->first;
                if (thisFFT < minFFT) minFFT = thisFFT;
                if (thisFFT > maxFFT) maxFFT = thisFFT;
                Int_t thisFreqBin = iGroup->second;
                if (thisFreqBin < minFreqBin) minFreqBin = thisFreqBin;
                if (thisFreqBin > maxFreqBin) maxFreqBin = thisFreqBin;
                //cout << iGroup->first << "-" << iGroup->second << "  " << flush;
            }
            //cout << endl;

            // we're done with this multimap object; all we need is min/maxFFT and min/maxFreqBin
            delete groupMap;
            iEPB = bundlePeakBins.erase(iEPB); // move the iterator back one so we don't skip anything when the for loop advances the iterator

            // check if this group is too small in time (this shouldn't happen, but just to be sure . . .)
            if (maxFFT - minFFT < 2) continue;

            Double_t meanFreq = ((Double_t)maxFreqBin - 1 - (Double_t)(maxFreqBin - minFreqBin)/2.) * freqBinWidth;
            txtOutFile << meanFreq << "   ";

            if (drawWaterfall)
            {
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

                groupHist->Draw("colz");
                c1->Print(outputFileNamePS.c_str());

                groupHist->Write();
            }

            //candidates->Add(groupHist);
            iCandidate++;
        }

        cout << "Found " << iCandidate << " candidate groups" << endl;
        txtOutFile << endl;
        txtOutFile << "  " << iCandidate << " candidates found" << endl;
        txtOutFile << "------------------------------------" << endl;

        //TCanvas *c1 = new TCanvas("c1", "c1");
        //c1->SetLogz(1);
        //hist->Draw("colz");
        //c1->SetLogy(1);
        //histProj->Draw();

        //c1->WaitPrimitive();

        //delete histProj;
        //bundlePeakBins->Delete();
        //delete bundlePeakBins;
        //delete hist;
        //delete c1;

        delete histFullPS;
        delete bundle;

        iEvent++;
        totalCandidates += iCandidate;
        /*
        for (Int_t iCandidate=0; iCandidate<candidates->GetEntriesFast(); iCandidate++)
        {
            if (candidates->At(iCandidate) == NULL) continue;
            TH2D* thisCandidate = (TH2D*)candidates->At(iCandidate);
            thisCandidate->Draw("colz");
            c1->Print(outputFileName.c_str());
            //c1->WaitPrimitive();
        }
        */
        //candidates->Delete();
        //delete candidates;

    }

    txtOutFile << "Total candidates found in this file: " << totalCandidates << endl;
    txtOutFile << "Events analyzed: " << iEvent << endl;
    txtOutFile << "------------------------------------" << endl;
    txtOutFile.close();

    if (drawWaterfall)
    {
        TH1I* histGlobalPeakBins = new TH1I("hGlobalPeakBins", "Peak Bins -- Global", freqHistNBins, freqHistMin, freqHistMax);
        for (vector< Int_t >::iterator it = globalPeakFreqBins.begin(); it != globalPeakFreqBins.end(); it++)
        {
            histGlobalPeakBins->Fill(freqBinWidth * ((Double_t)(*it) - 0.5));
        }
        histGlobalPeakBins->Draw("colz");
        c1->Print(outputFileNamePS.c_str());
        histGlobalPeakBins->Write();

        sprintf(tempFileName, "%s]", outputFileNamePS.c_str());
        c1->Print(tempFileName);
        delete c1;
    }

    delete app;

    outFile->Close();

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

    return 0;
}

