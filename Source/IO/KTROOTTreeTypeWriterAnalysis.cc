/*
 * KTROOTTreeTypeWriterAnalysis.cc
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterAnalysis.hh"

#include "KTAmplitudeDistribution.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

#include <sstream>

using boost::shared_ptr;

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterAnalysis > sRTTWCRegistrar;

    KTROOTTreeTypeWriterAnalysis::KTROOTTreeTypeWriterAnalysis() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterAnalysis()
            fAmpDistTree(NULL),
            fAmpDistData()
    {
    }

    KTROOTTreeTypeWriterAnalysis::~KTROOTTreeTypeWriterAnalysis()
    {
    }


    void KTROOTTreeTypeWriterAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("amp-dist", this, &KTROOTTreeTypeWriterAnalysis::WriteAmplitudeDistributions);
        return;
    }


    //**************************
    // Amplitude Distribution
    //**************************

    void KTROOTTreeTypeWriterAnalysis::WriteAmplitudeDistributions(shared_ptr< KTData > data)
    {
        KTAmplitudeDistribution& adData = data->Of< KTAmplitudeDistribution >();
        KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fAmpDistTree == NULL)
        {
            if (! SetupAmplitudeDistributionTree())
            {
                KTERROR(publog, "Something went wrong while setting up the frequency candidate tree! Nothing was written.");
                return;
            }
        }

        for (fAmpDistData.fComponent = 0; fAmpDistData.fComponent < adData.GetNComponents(); fAmpDistData.fComponent++)
        {
            for (fAmpDistData.fFreqBin = 0; fAmpDistData.fFreqBin < adData.GetNFreqBins(); fAmpDistData.fFreqBin++)
            {
                stringstream name;
                name << "histAmpDist_" << fAmpDistData.fComponent << "_" << fAmpDistData.fFreqBin;
                const KTAmplitudeDistribution::Distribution& dist = adData.GetDistribution(fAmpDistData.fFreqBin, fAmpDistData.fComponent);
                UInt_t nBins = dist.size();
                fAmpDistData.fDistribution = new TH1D(name.str().c_str(), "Amplitude Distribution", (Int_t)nBins, dist.GetRangeMin(), dist.GetRangeMax());
                for (UInt_t iBin=0; iBin<nBins; iBin++)
                {
                    fAmpDistData.fDistribution->SetBinContent((Int_t)iBin+1, dist(iBin));
                }
                fAmpDistData.fDistribution->SetXTitle("Amplitude");
                fAmpDistData.fDistribution->SetYTitle("Slices");
                fAmpDistData.fDistribution->SetDirectory(NULL);

                fAmpDistTree->Fill();
           }
        }

        return;
    }

    Bool_t KTROOTTreeTypeWriterAnalysis::SetupAmplitudeDistributionTree()
    {
        fAmpDistTree = new TTree("freqCand", "Frequency Analysis");
        if (fAmpDistTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fAmpDistTree);

        fAmpDistTree->Branch("Component", &fAmpDistData.fComponent, "fComponent/s");
        fAmpDistTree->Branch("FreqBin", &fAmpDistData.fFreqBin, "fFreqBin/l");
        fAmpDistTree->Branch("Distribution", &fAmpDistData.fDistribution, 32000, 0);

        return true;
    }

} /* namespace Katydid */



