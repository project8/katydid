/*
 * MakeSimpleElectronHuntTree.C
 *
 *  Created on: Feb 7, 2012
 *      Author: nsoblath
 *
 *      Parameter 1: Output ROOT filename
 *      Parameter 2: First input filename prefix
 *      Parameter 3: Second input filename prefix (use "" to skip the second filename prefix)
 *      Parameter 4: Directory in which the input files reside (use "", ".", or "./" to use the current directory)
 *
 *      Expected input filename pattern: [filename prefix]_tmXpY.txt
 *      Second input filename prefix is optional
 *
 *      Formatting of the input files:
 *          * Can have any number of header lines before an event-divider line.
 *          * Event divider lines should start and end with hyphens (e.g. "-[whatever]-")
 *          * There should be an event divider line between the header and the first event, and between each event thereafter.
 *          * There can be an event divider line at the end of the file.
 *          * Each event can list the number of candidates found in a line ending in the word "found" and with no other numbers on the line.
 *          * There may be a line starting with "Event" that gives the event number.
 *          * After all events there can be a footer with lines beginning with "Events" or "Total".
 *          * Any other lines will be assumed to contain the mean frequencies (in MHz) of the candidates.
 *          * Frequencies can be provided in Hz or MHz. If the value is greater than 1000, then it's assumed to be Hz.
 *
 *
 */


#include "TFile.h"
#include "TList.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TTree.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

enum LineType {
    ltEventDivider,
    ltNumberOfCandidates,
    ltEventNumber,
    ltFooter,
    ltOther
};
enum ProcState {
    psExpectNewEvent,
    psInEvent
};

Double_t ExtractThresholdMultiplier(TString* fileName);
void ProcessFile(TString* fileName, TTree* tree);
LineType EvaluateLine(TString* line);
Int_t ExtractNumberOfCandidates(TString* line);
vector< Double_t >* ExtractFrequencies(TString* line);

void MakeSimpleElectronHuntTree(TString outFileName, TString inFileNamePrefix1, TString inFileNamePrefix2, TString inDirectoryName)
{
    Int_t fileLimit = 999999;

    TString inFileExtension(".txt");

    // Deal with the input file directory; get pwd() if using the default; make sure it's a directory
    if (inDirectoryName == TString("") || inDirectoryName == TString(".") || inDirectoryName == TString("./"))
    {
        inDirectoryName = TString(gSystem->pwd());
    }
    if (! inDirectoryName.EndsWith("/")) inDirectoryName.Append("/");
    cout << "inFileDirectory is: " << inDirectoryName << endl;
    TSystemDirectory inDirectory(inDirectoryName.Data(), inDirectoryName.Data());
    if (! inDirectory.IsDirectory())
    {
        cout << "Given input file directory is not a directory: " << inDirectoryName << endl;
        return;
    }

    // Get the list of files in the input directory
    TList* inFileList = inDirectory.GetListOfFiles();
    if (inFileList == NULL)
    {
        cout << "No input files found" << endl;
        return;
    }
    cout << "Number of files found in input directory: " << inFileList->GetEntries() << endl;

    // Determine if we'll be using the second input filename prefix
    Bool_t useSecondInput = kFALSE;
    if (! inFileNamePrefix2.IsNull())
    {
        cout << "Using two input files" << endl;
        useSecondInput = kTRUE;
    }

    // Create the output file
    TFile outFile(outFileName, "recreate");
    if (! outFile.IsOpen())
    {
        cout << "Output file did not open (" << outFileName << ")" << endl;
        inFileList->Delete();
        delete inFileList;
        return;
    }

    // Create the output trees
    Int_t eventNum, candidateNum;
    Double_t threshMult, meanFreq;
    TTree* outTree1 = new TTree("SimpleEHunt1", "Simple Electron Hunt Tree 1");
    outTree1->Branch("ThreshMult", &threshMult);
    outTree1->Branch("Event", &eventNum);
    outTree1->Branch("Candidate", &candidateNum);
    outTree1->Branch("MeanFreq", &meanFreq);
    TTree* outTree2 = NULL;
    if (useSecondInput)
    {
        outTree2 = new TTree("SimpleEHunt2", "Simple Electron Hunt Tree 2");
        outTree2->Branch("ThreshMult", &threshMult);
        outTree2->Branch("Event", &eventNum);
        outTree2->Branch("Candidate", &candidateNum);
        outTree2->Branch("MeanFreq", &meanFreq);
    }

    TSystemFile* inFile;
    TIter fileIter(inFileList);
    // Loop over all of the files in the input directory
    Int_t fileCount = 0;
    while ((inFile = (TSystemFile*)fileIter.Next()) && fileCount < fileLimit)
    {
        TString *inFileName = new TString(inFile->GetName());
        if (! inFile->IsDirectory() && inFileName->EndsWith(inFileExtension.Data()))
        {
            if (inFileName->BeginsWith(inFileNamePrefix1.Data()))
            {
                fileCount++;
                cout << *inFileName << endl;
                ProcessFile(inFileName, outTree1);
            }
            else if (useSecondInput && inFileName->BeginsWith(inFileNamePrefix2.Data()))
            {
                fileCount++;
                cout << *inFileName << endl;
                ProcessFile(inFileName, outTree2);
            }
        }
        delete inFileName;
    }
    cout << fileCount << " files processed" << endl;

    outTree1->Write();
    if (useSecondInput) outTree2->Write();

    outFile.Close();
    inFileList->Delete();
    delete inFileList;

    return;
}


Double_t ExtractThresholdMultiplier(TString* fileName)
{
    TObjArray* fileNameTokens = fileName->Tokenize("_");
    if (! fileNameTokens)
    {
        cout << "No tokens returned: " << *fileName << endl;
        return -1;
    }

    Int_t nTokens = fileNameTokens->GetEntries();
    if (nTokens < 2)
    {
        cout << "Too few tokens in the file name; no threhsold multiplier found" << endl;
        return -1.;
    }

    TString lastToken = ((TObjString*)((*fileNameTokens)[fileNameTokens->GetLast()]))->GetString();
    lastToken.ReplaceAll("tm", "");
    lastToken.ReplaceAll(".txt", "");
    lastToken.ReplaceAll("p",".");
    Double_t threshMult = lastToken.Atof();

    fileNameTokens->Delete();
    delete fileNameTokens;
    return threshMult;
}


void ProcessFile(TString* fileName, TTree* tree)
{
    if (tree == NULL)
    {
        cout << "No tree was given" << endl;
        return;
    }

    if (fileName == NULL)
    {
        cout << "No filename given to process" << endl;
        return;
    }

    Double_t threshMult = ExtractThresholdMultiplier(fileName);
    if (threshMult == -1.)
    {
        cout << "Invalid threshold multiplier; something went wrong with the filename" << endl;
        return;
    }

    ifstream txtFile(fileName->Data());
    if (! txtFile.is_open())
    {
        cout << "Could not open file for processing: " << *fileName << endl;
        return;
    }

    string line;
    // process header
    while (txtFile.good())
    {
        getline(txtFile, line);
        if (EvaluateLine(&TString(line)) == ltEventDivider) break;
    }

    // Variables for filling the tree
    Int_t eventNum, candidateNum;
    Double_t meanFreq;
    tree->SetBranchAddress("ThreshMult", &threshMult);
    tree->SetBranchAddress("Event", &eventNum);
    tree->SetBranchAddress("Candidate", &candidateNum);
    tree->SetBranchAddress("MeanFreq", &meanFreq);

    // process rest of file
    eventNum = -1;
    Int_t candidateCounter = 0, nCandidatesFound = 0;
    vector< Double_t >* candidateFrequencies = NULL;
    ProcState state = psExpectNewEvent;
    while (txtFile.good())
    {
        getline(txtFile, line);
        TString thisLine(line.c_str());
        LineType lineType = EvaluateLine(&thisLine);
        if (lineType == ltEventDivider)
        {
            if (state == psInEvent)
            {
                // In this case we've finished an event
                if (nCandidatesFound != -1 && candidateCounter != -1 && candidateCounter != nCandidatesFound)
                {
                    cout << "Warning: Mismatch between the number of frequencies recorded and the stated number of candidates: nfreqs: " << candidateCounter << "; nfound: " << nCandidatesFound << endl;
                }
                candidateCounter = 0;
                nCandidatesFound = 0;
                delete candidateFrequencies;
                candidateFrequencies = NULL;
                //cout << "------------" << endl;
            }
            state = psExpectNewEvent;
            continue;
        }
        if (state == psExpectNewEvent && lineType != ltEventDivider)
        {
            // In this case we've started a new event
            eventNum++;
            state = psInEvent;
        }
        if (state == psInEvent)
        {
            if (lineType == ltEventNumber || lineType == ltFooter)
            {
                // The event number and footer are not used right now
                continue;
            }
            if (lineType == ltNumberOfCandidates)
            {
                nCandidatesFound = ExtractNumberOfCandidates(&thisLine);
                //cout << "found number of candidates: " << nCandidatesFound << endl;
                continue;
            }
            if (lineType == ltOther)
            {
                vector< Double_t >* frequencies = ExtractFrequencies(&thisLine);
                if (frequencies == NULL) continue;
                Int_t additionalCandidates = (Int_t)frequencies->size();
                if (additionalCandidates > 0)
                {
                    candidateCounter += additionalCandidates;
                    // Add data to tree for each candidate
                    //cout << "candidates at: ";
                    for (candidateNum=0; candidateNum<additionalCandidates; candidateNum++)
                    {
                        meanFreq = (*frequencies)[candidateNum];
                        //cout << meanFreq << " ";
                        tree->Fill();
                    }
                    //cout << endl;
                }
                delete frequencies;
                continue;
            }
            cout << "Warning: unknown line type: " << lineType << endl;
            continue;
        }
    }

    return;
}


LineType EvaluateLine(TString* line)
{
    if (line->BeginsWith("Events") || line->BeginsWith("Total")) return ltFooter;
    if (line->BeginsWith("-") && line->EndsWith("-")) return ltEventDivider;
    if (line->EndsWith(" found")) return ltNumberOfCandidates;
    if (line->BeginsWith("Event")) return ltEventNumber;
    return ltOther;
}


Int_t ExtractNumberOfCandidates(TString* line)
{
    TObjArray* tokens = line->Tokenize(" ");
    TObjString* objToken;
    TIter tokenIter(tokens);
    Int_t nCandidates = -1;
    // Loop over all tokens
    while ((objToken = (TObjString*)tokenIter.Next()))
    {
        TString token = objToken->String();
        if (token.IsDigit())
        {
            nCandidates = token.Atoi();
            break;
        }
    }
    tokens->Delete();
    delete tokens;
    return nCandidates;
}


vector< Double_t >* ExtractFrequencies(TString* line)
{
    TObjArray* tokens = line->Tokenize(" ");
    TObjString* objToken;
    TIter tokenIter(tokens);
    vector< Double_t >* candidates = new vector< Double_t >();
    // Loop over all tokens
    while ((objToken = (TObjString*)tokenIter.Next()))
    {
        TString token = objToken->String();
        token.ReplaceAll("+", "");
        if (token.IsFloat())
        {
            Double_t freq = token.Atof();
            if (freq > 1.e3) freq /= 1.e6;
            // if we need to apply the cut around the noise peaks from the september 2011 data
            if (!(freq < 0.2) && !(freq>99.8 && freq<100.2) && !(freq>199.5&&freq<200.2))
                candidates->push_back(freq);
        }
    }
    tokens->Delete();
    delete tokens;
    return candidates;
}

