/*
 * KTROOTTreeTypeWriterSpectrumAnalysis.cc
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"
#include "KTAmplitudeDistribution.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTKDTreeData.hh"
#include "KTHoughData.hh"
#include "KTLinearFitResult.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <sstream>
#include "KTROOTTreeTypeWriterSpectrumAnalysis.hh"



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTTreeTypeWriterSpectrumAnalysis");

    static KTTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterSpectrumAnalysis > sRTTWCRegistrar;

    KTROOTTreeTypeWriterSpectrumAnalysis::KTROOTTreeTypeWriterSpectrumAnalysis() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterSpectrumAnalysis()
            fDiscPoints1DTree(NULL),
            fKDTreeTree(NULL),
            fAmpDistTree(NULL),
            fHoughTree(NULL),
            fLinearFitResultTree(NULL),
            fDiscPoints1DData(),
            fKDTreePointData(),
            fAmpDistData(),
            fHoughData(),
            fLineFitData()
    {
    }

    KTROOTTreeTypeWriterSpectrumAnalysis::~KTROOTTreeTypeWriterSpectrumAnalysis()
    {
    }


    void KTROOTTreeTypeWriterSpectrumAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("disc-1d", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteDiscriminatedPoints1D);
        fWriter->RegisterSlot("kd-tree", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteKDTree);
        fWriter->RegisterSlot("amp-dist", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteAmplitudeDistributions);
        fWriter->RegisterSlot("hough", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteHoughData);
        fWriter->RegisterSlot("linear-fit", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteLinearFitResultData);
        return;
    }


    //*********************
    // Discriminated Points
    //*********************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteDiscriminatedPoints1D(KTDataPtr data)
    {
        KTDiscriminatedPoints1DData& fcData = data->Of< KTDiscriminatedPoints1DData >();
        KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fDiscPoints1DTree == NULL)
        {
            if (! SetupDiscriminatedPoints1DTree())
            {
                KTERROR(publog, "Something went wrong while setting up the discriminated points 1D tree! Nothing was written.");
                return;
            }
        }

        fDiscPoints1DData.fSlice = header.GetSliceNumber();
        fDiscPoints1DData.fTimeInRunC = header.GetTimeInRun() + 0.5 * header.GetSliceLength();

        for (fDiscPoints1DData.fComponent = 0; fDiscPoints1DData.fComponent < fcData.GetNComponents(); fDiscPoints1DData.fComponent++)
        {
            const KTDiscriminatedPoints1DData::SetOfPoints& points = fcData.GetSetOfPoints(fDiscPoints1DData.fComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = points.begin(); it != points.end(); ++it)
            {
                fDiscPoints1DData.fBin = it->first;
                fDiscPoints1DData.fAbscissa = it->second.fAbscissa;
                fDiscPoints1DData.fOrdinate = it->second.fOrdinate;
                fDiscPoints1DData.fThreshold = it->second.fThreshold;

                fDiscPoints1DTree->Fill();
           }
        }

        return;
    }

    bool KTROOTTreeTypeWriterSpectrumAnalysis::SetupDiscriminatedPoints1DTree()
    {
        fDiscPoints1DTree = new TTree("discPoints1D", "Discriminated Points 1D");
        if (fDiscPoints1DTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fDiscPoints1DTree);

        //fDiscPoints1DData = new TDiscriminatedPoints1DData();

        fDiscPoints1DTree->Branch("Slice", &fDiscPoints1DData.fSlice, "fSlice/i");
        fDiscPoints1DTree->Branch("TimeInRunC", &fDiscPoints1DData.fTimeInRunC, "fTimeInRunC/d");
        fDiscPoints1DTree->Branch("Component", &fDiscPoints1DData.fComponent, "fComponent/i");
        fDiscPoints1DTree->Branch("Bin", &fDiscPoints1DData.fBin, "fBin/i");
        fDiscPoints1DTree->Branch("Abscissa", &fDiscPoints1DData.fAbscissa, "fAbscissa/d");
        fDiscPoints1DTree->Branch("Ordinate", &fDiscPoints1DData.fOrdinate, "fOrdinate/d");
        fDiscPoints1DTree->Branch("Threshold", &fDiscPoints1DData.fThreshold, "fThreshold/d");
        //fDiscPoints1DTree->Branch("freqAnalysis", &fDiscPoints1DData.fComponent, "fComponent/s:fSlice/l:fTimeInRun/d:fThreshold/d:fFirstBin/i:fLastBin/i:fMeanFrequency/d:fPeakAmplitude/d");

        return true;
    }

    //*********************
    // K-D Tree
    //*********************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteKDTree(KTDataPtr data)
    {
        static Long64_t lastSlice = -1;

        KTKDTreeData& kdtData = data->Of< KTKDTreeData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fKDTreeTree == NULL)
        {
            if (! SetupKDTreeTree())
            {
                KTERROR(publog, "Something went wrong while setting up the k-d tree tree! Nothing was written.");
                return;
            }
        }

        Long64_t lastSliceThisData = lastSlice;
        for (fKDTreePointData.fComponent = 0; fKDTreePointData.fComponent < kdtData.GetNComponents(); fKDTreePointData.fComponent++)
        {
            const KTKDTreeData::SetOfPoints& points = kdtData.GetSetOfPoints(fKDTreePointData.fComponent);
            const KTKDTreeData::TreeIndex* index = kdtData.GetTreeIndex(fKDTreePointData.fComponent);
            unsigned pid = 0;
            for (KTKDTreeData::SetOfPoints::const_iterator it = points.begin(); it != points.end(); ++it)
            {
                if ((int64_t)it->fSliceNumber > lastSlice)
                {
                    if ((int64_t)it->fSliceNumber > lastSliceThisData) lastSliceThisData = (int64_t)it->fSliceNumber;
                    fKDTreePointData.fSlice = it->fSliceNumber;
                    fKDTreePointData.fTimeInRunC = it->fCoords[0];
                    fKDTreePointData.fFrequency = it->fCoords[1];
                    fKDTreePointData.fAmplitude = it->fAmplitude;
                    fKDTreePointData.fNoiseFlag = it->fNoiseFlag;
                    KTKDTreeData::TreeIndex::Neighbors neighbors = index->NearestNeighborsByNumber(pid, 2);
                    fKDTreePointData.fNNDistance = neighbors.dist(1);
                    //KTWARN(publog, "ne to " << pid << ": " << neighbors[0] << " @ " << neighbors.dist(0) << "\t" << neighbors[1] << " @ " << neighbors.dist(1) << '\n'
                    //       << '\t' << neighbors[0] << ": " << points[neighbors[0]].fCoords[0] << ", " << points[neighbors[0]].fCoords[1] << '\n'
                    //       << '\t' << neighbors[1] << ": " << points[neighbors[1]].fCoords[0] << ", " << points[neighbors[1]].fCoords[1]);
                    KTKDTreeData::TreeIndex::Neighbors neighbors2 = index->NearestNeighborsByRadius(pid, 0.22);
                    fKDTreePointData.fKNNWithin0p22 = neighbors2.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors3 = index->NearestNeighborsByRadius(pid, 0.32);
                    fKDTreePointData.fKNNWithin0p32 = neighbors3.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors4 = index->NearestNeighborsByRadius(pid, 0.45);
                    fKDTreePointData.fKNNWithin0p45 = neighbors4.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors5 = index->NearestNeighborsByRadius(pid, 0.7);
                    fKDTreePointData.fKNNWithin0p7 = neighbors5.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors6 = index->NearestNeighborsByRadius(pid, 1.);
                    fKDTreePointData.fKNNWithin1p0 = neighbors6.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors7 = index->NearestNeighborsByRadius(pid, 1.4);
                    fKDTreePointData.fKNNWithin1p4 = neighbors7.size();

                    fKDTreeTree->Fill();
                    ++pid;
                }
           }
        }
        lastSlice = lastSliceThisData;

        return;
    }

    bool KTROOTTreeTypeWriterSpectrumAnalysis::SetupKDTreeTree()
    {
        fKDTreeTree = new TTree("kdTree", "K-D Tree");
        if (fKDTreeTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fKDTreeTree);

        //fDiscPoints1DData = new TDiscriminatedPoints1DData();

        fKDTreeTree->Branch("Slice", &fKDTreePointData.fSlice, "fSlice/l");
        fKDTreeTree->Branch("TimeInRunC", &fKDTreePointData.fTimeInRunC, "fTimeInRunC/d");
        fKDTreeTree->Branch("Frequency", &fKDTreePointData.fFrequency, "fFrequency/d");
        fKDTreeTree->Branch("Amplitude", &fKDTreePointData.fAmplitude, "fAmplitude/d");
        fKDTreeTree->Branch("NoiseFlag", &fKDTreePointData.fNoiseFlag, "fNoiseFlag/d");
        fKDTreeTree->Branch("NNDistance", &fKDTreePointData.fNNDistance, "fNNDistance/d");
        fKDTreeTree->Branch("KNNWithin0p22", &fKDTreePointData.fKNNWithin0p22, "fKNNWithin0p22/i");
        fKDTreeTree->Branch("KNNWithin0p32", &fKDTreePointData.fKNNWithin0p32, "fKNNWithin0p32/i");
        fKDTreeTree->Branch("KNNWithin0p45", &fKDTreePointData.fKNNWithin0p45, "fKNNWithin0p45/i");
        fKDTreeTree->Branch("KNNWithin0p7", &fKDTreePointData.fKNNWithin0p7, "fKNNWithin0p7/i");
        fKDTreeTree->Branch("KNNWithin1p0", &fKDTreePointData.fKNNWithin1p0, "fKNNWithin1p0/i");
        fKDTreeTree->Branch("KNNWithin1p4", &fKDTreePointData.fKNNWithin1p4, "fKNNWithin1p4/i");

        return true;
    }

    //**************************
    // Amplitude Distribution
    //**************************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteAmplitudeDistributions(KTDataPtr data)
    {
        KTAmplitudeDistribution& adData = data->Of< KTAmplitudeDistribution >();
        //KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fAmpDistTree == NULL)
        {
            if (! SetupAmplitudeDistributionTree())
            {
                KTERROR(publog, "Something went wrong while setting up the amplitude distribution tree! Nothing was written.");
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
                unsigned nBins = dist.size();
                fAmpDistData.fDistribution = new TH1D(name.str().c_str(), "Amplitude Distribution", (int)nBins, dist.GetRangeMin(), dist.GetRangeMax());
                for (unsigned iBin=0; iBin<nBins; iBin++)
                {
                    fAmpDistData.fDistribution->SetBinContent((int)iBin+1, dist(iBin));
                }
                fAmpDistData.fDistribution->SetXTitle("Amplitude");
                fAmpDistData.fDistribution->SetYTitle("Slices");
                fAmpDistData.fDistribution->SetDirectory(NULL);

                fAmpDistTree->Fill();
           }
        }

        return;
    }

    bool KTROOTTreeTypeWriterSpectrumAnalysis::SetupAmplitudeDistributionTree()
    {
        fAmpDistTree = new TTree("freqCand", "Frequency Analysis");
        if (fAmpDistTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fAmpDistTree);

        fAmpDistTree->Branch("Component", &fAmpDistData.fComponent, "fComponent/i");
        fAmpDistTree->Branch("FreqBin", &fAmpDistData.fFreqBin, "fFreqBin/i");
        fAmpDistTree->Branch("Distribution", &fAmpDistData.fDistribution, 32000, 0);

        return true;
    }


    //*************************
    // Hough Transform Data
    //*************************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteHoughData(KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to hough data root tree");
        KTHoughData& htData = data->Of< KTHoughData >();
        //KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fHoughTree == NULL)
        {
            if (! SetupHoughTree())
            {
                KTERROR(publog, "Something went wrong while setting up the Hough tree! Nothing was written.");
                return;
            }
        }

        for (fHoughData.fComponent = 0; fHoughData.fComponent < htData.GetNComponents(); fHoughData.fComponent++)
        {
            fHoughData.fTransform = KT2ROOT::CreateHistogram(htData.GetTransform(fHoughData.fComponent));
            fHoughData.fTransform->SetDirectory(NULL);
            fHoughData.fTransform->SetTitle("Hough Space");
            fHoughData.fTransform->SetXTitle("Angle");
            fHoughData.fTransform->SetYTitle("Radius");
            KTINFO(publog, "Angle axis: " << fHoughData.fTransform->GetNbinsX() << " bins; range: " << fHoughData.fTransform->GetXaxis()->GetXmin() << " - " << fHoughData.fTransform->GetXaxis()->GetXmax());
            KTINFO(publog, "Radius axis: " << fHoughData.fTransform->GetNbinsY() << " bins; range: " << fHoughData.fTransform->GetYaxis()->GetXmin() << " - " << fHoughData.fTransform->GetYaxis()->GetXmax());

            fHoughData.fXOffset = htData.GetXOffset(fHoughData.fComponent);
            fHoughData.fXScale = htData.GetXScale(fHoughData.fComponent);
            fHoughData.fYOffset = htData.GetYOffset(fHoughData.fComponent);
            fHoughData.fYScale = htData.GetYScale(fHoughData.fComponent);

            fHoughTree->Fill();
        }

        return;
    }

    bool KTROOTTreeTypeWriterSpectrumAnalysis::SetupHoughTree()
    {
        fHoughTree = new TTree("hough", "Hough Transform");
        if (fHoughTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fHoughTree);

        fHoughTree->Branch("Component", &fHoughData.fComponent, "fComponent/i");
        fHoughTree->Branch("Transform", &fHoughData.fTransform, 32000, 0);
        fHoughTree->Branch("XOffset", &fHoughData.fXOffset, "fXOffset/d");
        fHoughTree->Branch("XScale", &fHoughData.fXScale, "fXScale/d");
        fHoughTree->Branch("YOffset", &fHoughData.fYOffset, "fYOffset/d");
        fHoughTree->Branch("YScale", &fHoughData.fYScale, "fYScale/d");

        return true;
    }


    //**************************
    // Linear Fit Result Data
    //**************************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteLinearFitResultData(KTDataPtr data)
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

    bool KTROOTTreeTypeWriterSpectrumAnalysis::SetupLinearFitResultTree()
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

} /* namespace Katydid */



