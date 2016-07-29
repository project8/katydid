/*
  * KTMultiFileROOTTreeReader.cc
 *
 *  Created on: May 31, 2013
 *      Author: nsoblath
 */

#include "KTMultiFileROOTTreeReader.hh"

#include "KTAmplitudeDistribution.hh"
#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "TAxis.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"

#include <sstream>
#include "KTROOTTreeTypeWriterSpectrumAnalysis.hh"



using std::deque;
using std::string;
using std::stringstream;

namespace Katydid
{
    KTLOGGER(inlog, "KTMultiFileROOTTreeReader");

    static KTNORegistrar< KTReader, KTMultiFileROOTTreeReader > sMFRTReaderRegistrar("mf-root-tree-reader");
    static KTNORegistrar< KTProcessor, KTMultiFileROOTTreeReader > sMFRTRProcRegistrar("mf-root-tree-reader");

    KTMultiFileROOTTreeReader::KTMultiFileROOTTreeReader(const std::string& name) :
            KTReader(name),
            fFilenames(),
            fFileIter(fFilenames.end()),
            fDataTypes(),
            fAmpDistSignal("amp-dist", this),
            fDoneSignal("done", this),
            fAppendAmpDistSlot("amp-dist", this, &KTMultiFileROOTTreeReader::Append, &fAmpDistSignal)
    {
    }

    KTMultiFileROOTTreeReader::~KTMultiFileROOTTreeReader()
    {
    }

    bool KTMultiFileROOTTreeReader::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node == NULL) return false;

        const scarab::param_array* inputFileArray = node->array_at("input-files");
        if (inputFileArray != NULL)
        {
            for (scarab::param_array::const_iterator ifIt = inputFileArray->begin(); ifIt != inputFileArray->end(); ++ifIt)
            {
                AddFilename((*ifIt)->as_value().as_string());
                KTDEBUG(inlog, "Added filename <" << fFilenames.back() << ">");
            }
        }

        const scarab::param_array* dataTypeArray = node->array_at("data-types");
        if (inputFileArray != NULL)
        {
            for (scarab::param_array::const_iterator dtIt = dataTypeArray->begin(); dtIt != dataTypeArray->end(); ++dtIt)
            {
                AddDataType((*dtIt)->as_array().get_value(0), (*dtIt)->as_array().get_value(1));
                KTDEBUG(inlog, "Added data type <" << fDataTypes.back().fName << ">, tree name <" << fDataTypes.back().fTreeName << ">");
            }
        }

        return true;
    }

    bool KTMultiFileROOTTreeReader::AddDataType(const std::string& type, const std::string& treeName)
    {
        if (type == "amp-dist")
        {
            fDataTypes.push_back(DataType(type, treeName, &KTMultiFileROOTTreeReader::AppendAmpDistData, &fAmpDistSignal));
        }
        else
        {
            KTERROR(inlog, "Invalid run-data-type: " << type);
            return false;
        }

        return true;
    }

    TFile* KTMultiFileROOTTreeReader::OpenFile(const string& filename) const
    {
        TFile* newFile = TFile::Open(filename.c_str(), "read");
        if (! newFile->IsOpen())
        {
            KTERROR(inlog, "Input file did not open: <" << filename << ">");
            return NULL;
        }

        KTINFO(inlog, "Input file open: <" << filename << ">");

        return newFile;
    }

    TTree* KTMultiFileROOTTreeReader::ExtractTree(TFile* file, const string& treeName) const
    {
        return (TTree*)(file->Get(treeName.c_str()));
    }

    bool KTMultiFileROOTTreeReader::Run()
    {
        for (fFileIter = fFilenames.begin(); fFileIter != fFilenames.end(); fFileIter++)
        {
            TFile* file = OpenFile(*fFileIter);
            if (file == NULL)
            {
                KTERROR(inlog, "A problem occurred while trying to open file <" << *fFileIter << ">");
                return false;
            }

            KTDataPtr newData(new KTData());
            for (deque< DataType >::const_iterator dtIt = fDataTypes.begin(); dtIt != fDataTypes.end(); dtIt++)
            {
                KTDEBUG(inlog, "Appending data of type " << dtIt->fName);

                TTree* tree = ExtractTree(file, dtIt->fTreeName);
                if (tree == NULL)
                {
                    KTERROR(inlog, "Tree <" << dtIt->fTreeName << "> was not extracted from file <" << *fFileIter << ">");
                    return false;
                }

                if (! (this->*(dtIt->fAppendFcn))(tree, *(newData.get())))
                {
                    KTERROR(inlog, "Something went wrong while appending data of type <" << dtIt->fName << "> from tree <" << dtIt->fTreeName << "> from file <" << *fFileIter << ">");
                    return false;
                }
                (*(dtIt->fSignal))(newData);
            }
        }

        fDoneSignal();

        return true;
    }

    bool KTMultiFileROOTTreeReader::Append(KTData& data)
    {
        if (fFileIter == fFilenames.end())
        {
            KTERROR(inlog, "File iterator has already reached the end of the filenames");
            return false;
        }

        TFile* file = OpenFile(*fFileIter);
        if (file == NULL)
        {
            KTERROR(inlog, "A problem occurred while trying to open file <" << *fFileIter << ">");
            return false;
        }

        for (deque< DataType >::const_iterator dtIt = fDataTypes.begin(); dtIt != fDataTypes.end(); dtIt++)
        {
            KTDEBUG(inlog, "Appending data of type " << dtIt->fName);

            TTree* tree = ExtractTree(file, dtIt->fTreeName);
            if (tree == NULL)
            {
                KTERROR(inlog, "Tree <" << dtIt->fTreeName << "> was not extracted from file <" << *fFileIter << ">");
                return false;
            }

            if (! (this->*(dtIt->fAppendFcn))(tree, data))
            {
                KTERROR(inlog, "Something went wrong while appending data of type <" << dtIt->fName << "> from tree <" << dtIt->fTreeName << "> from file <" << *fFileIter << ">");
                return false;
            }
        }

        fFileIter++;

        return true;
    }

    bool KTMultiFileROOTTreeReader::AppendAmpDistData(TTree* tree, KTData& appendToData)
    {
        // Determine the number of components and the number of frequency bins
        unsigned nComponents = 0;
        unsigned nFreqBins = 0;
        while (true)
        {
            stringstream cut;
            cut << "Component == " << nComponents;
            unsigned nEntriesForComponent = (unsigned)tree->GetEntries(cut.str().c_str());
            if (nEntriesForComponent == 0)
            {
                nComponents++; // advance this by 1 to be the number of components, not the last component number
                break;
            }
            if (nEntriesForComponent > nFreqBins)
            {
                nFreqBins = nEntriesForComponent;
            }
        }

        // Create data structures
        TAmplitudeDistributionData ampDistData;
        tree->SetBranchAddress("Component", &ampDistData.fComponent);
        tree->SetBranchAddress("FreqBin", &ampDistData.fFreqBin);
        tree->SetBranchAddress("Distribution", &ampDistData.fDistribution);

        KTDEBUG(inlog, "Initializing new set of amplitude distributions, with " << nComponents << " components and " << nFreqBins << "frequency bins");
        KTAmplitudeDistribution& ampDist = appendToData.Of< KTAmplitudeDistribution >();
        ampDist.InitializeNull(nComponents, nFreqBins);

        // Read in the data
        unsigned nEntries = (unsigned)tree->GetEntries();
        for (unsigned iEntry=0; iEntry < nEntries; iEntry++)
        {
            tree->GetEntry(iEntry);

            unsigned nDistBins = (unsigned)ampDistData.fDistribution->GetNbinsX();
            ampDist.InitializeADistribution(ampDistData.fComponent, ampDistData.fFreqBin, nDistBins, ampDistData.fDistribution->GetXaxis()->GetXmin(), ampDistData.fDistribution->GetXaxis()->GetXmax());
            for (unsigned iDistBin = 0; iDistBin < nDistBins; iDistBin++)
            {
                ampDist.SetDistValue(ampDistData.fDistribution->GetBinContent(iDistBin+1), ampDistData.fFreqBin, iDistBin, ampDistData.fComponent);
            }
        }


        KTDEBUG(inlog, "some success message!");

        return true;
    }

} /* namespace Katydid */
