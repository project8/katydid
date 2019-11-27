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
#include "KTLogger.hh"
#include "KTPowerSpectrumData.hh"
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
    
    const std::string KTROOTTreeTypeWriterSpectrumAnalysis::Result::sName = "root-writer-spectrum-analysis";

    static Nymph::KTTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterSpectrumAnalysis > sRTTWCRegistrar;

    KTROOTTreeTypeWriterSpectrumAnalysis::KTROOTTreeTypeWriterSpectrumAnalysis(const std::string& name) :
                    KTROOTTreeTypeWriter(),
                    //KTTypeWriterSpectrumAnalysis()
                    fDiscPoints1DTree(NULL),
                    fKDTreeTree(NULL),
                    fAmpDistTree(NULL),
                    fHoughTree(NULL),
                    fFlattenedPSDTree(NULL),
                    fFlattenedLabelMaskTree(NULL),
                    fDiscPoints1DData(),
                    fKDTreePointData(),
                    fAmpDistData(),
                    fHoughData(),
                    fPowerValue(0.0),
                    fLabel(0),
                    fMaskThreshold(1.0e-17)
    {
    }

    KTROOTTreeTypeWriterSpectrumAnalysis::~KTROOTTreeTypeWriterSpectrumAnalysis()
    {
    }


    void KTROOTTreeTypeWriterSpectrumAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("disc-1d", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteDiscriminatedPoints1D);
        fWriter->RegisterSlot("kd-tree", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteKDTree);
        fWriter->RegisterSlot("kd-tree-scaled", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteKDTreeScaled);
        fWriter->RegisterSlot("amp-dist", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteAmplitudeDistributions);
        fWriter->RegisterSlot("hough", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteHoughData);
        fWriter->RegisterSlot("ps-flat", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteFlattenedPSDData);
        fWriter->RegisterSlot("ps-mask", this, &KTROOTTreeTypeWriterSpectrumAnalysis::WriteFlattenedLabelMask);
        return;
    }


    //*********************
    // Discriminated Points
    //*********************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteDiscriminatedPoints1D(Nymph::KTDataPtr data)
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
                fDiscPoints1DData.fMean = it->second.fMean;
                fDiscPoints1DData.fVariance = it->second.fVariance;
                fDiscPoints1DData.fNeighborhoodAmplitude = it->second.fNeighborhoodAmplitude;

                fDiscPoints1DTree->Fill();
            }
        }

        return;
    }

    bool KTROOTTreeTypeWriterSpectrumAnalysis::SetupDiscriminatedPoints1DTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "discPoints1D", fDiscPoints1DTree );

            if( fDiscPoints1DTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fDiscPoints1DTree );

                fDiscPoints1DTree->SetBranchAddress("Slice", &fDiscPoints1DData.fSlice);
                fDiscPoints1DTree->SetBranchAddress("TimeInRunC", &fDiscPoints1DData.fTimeInRunC);
                fDiscPoints1DTree->SetBranchAddress("Component", &fDiscPoints1DData.fComponent);
                fDiscPoints1DTree->SetBranchAddress("Bin", &fDiscPoints1DData.fBin);
                fDiscPoints1DTree->SetBranchAddress("Abscissa", &fDiscPoints1DData.fAbscissa);
                fDiscPoints1DTree->SetBranchAddress("Ordinate", &fDiscPoints1DData.fOrdinate);
                fDiscPoints1DTree->SetBranchAddress("Threshold", &fDiscPoints1DData.fThreshold);
                fDiscPoints1DTree->SetBranchAddress("Mean", &fDiscPoints1DData.fMean);
                fDiscPoints1DTree->SetBranchAddress("Variance", &fDiscPoints1DData.fVariance);
                fDiscPoints1DTree->SetBranchAddress("NeighborhoodAmplitude", &fDiscPoints1DData.fNeighborhoodAmplitude);

                return true;
            }
        }

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
        fDiscPoints1DTree->Branch("Mean", &fDiscPoints1DData.fMean, "fMean/d");
        fDiscPoints1DTree->Branch("Variance", &fDiscPoints1DData.fVariance, "fVariance/d");
        fDiscPoints1DTree->Branch("NeighborhoodAmplitude", &fDiscPoints1DData.fNeighborhoodAmplitude, "fNeighborhoodAmplitude/d");
        //fDiscPoints1DTree->Branch("freqAnalysis", &fDiscPoints1DData.fComponent, "fComponent/s:fSlice/l:fTimeInRun/d:f/d:fFirstBin/i:fLastBin/i:fMeanFrequency/d:fPeakAmplitude/d");

        return true;
    }

    //*********************
    // K-D Tree
    //*********************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteKDTree(Nymph::KTDataPtr data)
    {
        KTKDTreeData& kdtData = data->Of< KTKDTreeData >();

        DoWriteKDTree(kdtData, 1., 1.);

        return;
    }

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteKDTreeScaled(Nymph::KTDataPtr data)
    {
        KTKDTreeData& kdtData = data->Of< KTKDTreeData >();

        DoWriteKDTree(kdtData, kdtData.GetXScaling(), kdtData.GetYScaling());

        return;
    }

    void KTROOTTreeTypeWriterSpectrumAnalysis::DoWriteKDTree(KTKDTreeData& kdtData, double xScaling, double yScaling)
    {
        static Long64_t lastSlice = -1;

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
                    fKDTreePointData.fTimeInRunC = it->fCoords[0] * xScaling;
                    fKDTreePointData.fFrequency = it->fCoords[1] * yScaling;
                    fKDTreePointData.fAmplitude = it->fAmplitude;
                    fKDTreePointData.fMean = it->fMean;
                    fKDTreePointData.fVariance = it->fVariance;
                    fKDTreePointData.fNeighborhoodAmplitude = it->fNeighborhoodAmplitude;
                    fKDTreePointData.fNoiseFlag = it->fNoiseFlag;
                    fKDTreePointData.fBinInSlice = it->fBinInSlice;
                    KTKDTreeData::TreeIndex::Neighbors neighbors = index->NearestNeighborsByNumber(pid, 2);
                    fKDTreePointData.fNNDistance = neighbors.dist(1);
                    //KTWARN(publog, "ne to " << pid << ": " << neighbors[0] << " @ " << neighbors.dist(0) << "\t" << neighbors[1] << " @ " << neighbors.dist(1) << '\n'
                    //       << '\t' << neighbors[0] << ": " << points[neighbors[0]].fCoords[0] << ", " << points[neighbors[0]].fCoords[1] << '\n'
                    //       << '\t' << neighbors[1] << ": " << points[neighbors[1]].fCoords[0] << ", " << points[neighbors[1]].fCoords[1]);
                    KTKDTreeData::TreeIndex::Neighbors neighbors2 = index->NearestNeighborsByRadius(pid, 1.);
                    fKDTreePointData.fKNNWithin1 = neighbors2.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors3 = index->NearestNeighborsByRadius(pid, 2.);
                    fKDTreePointData.fKNNWithin2 = neighbors3.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors4 = index->NearestNeighborsByRadius(pid, 3.);
                    fKDTreePointData.fKNNWithin3 = neighbors4.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors5 = index->NearestNeighborsByRadius(pid, 4.);
                    fKDTreePointData.fKNNWithin4 = neighbors5.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors6 = index->NearestNeighborsByRadius(pid, 5.);
                    fKDTreePointData.fKNNWithin5 = neighbors6.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors7 = index->NearestNeighborsByRadius(pid, 6.);
                    fKDTreePointData.fKNNWithin6 = neighbors7.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors8 = index->NearestNeighborsByRadius(pid, 7.);
                    fKDTreePointData.fKNNWithin7 = neighbors8.size();
                    KTKDTreeData::TreeIndex::Neighbors neighbors9 = index->NearestNeighborsByRadius(pid, 8.);
                    fKDTreePointData.fKNNWithin8 = neighbors9.size();

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
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "kdTree", fKDTreeTree );

            if( fKDTreeTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fKDTreeTree );

                fKDTreeTree->SetBranchAddress("Slice", &fKDTreePointData.fSlice);
                fKDTreeTree->SetBranchAddress("TimeInRunC", &fKDTreePointData.fTimeInRunC);
                fKDTreeTree->SetBranchAddress("Frequency", &fKDTreePointData.fFrequency);
                fKDTreeTree->SetBranchAddress("Amplitude", &fKDTreePointData.fAmplitude);
                fKDTreeTree->SetBranchAddress("NoiseFlag", &fKDTreePointData.fNoiseFlag);
                fKDTreeTree->SetBranchAddress("BinInSlice", &fKDTreePointData.fBinInSlice);
                fKDTreeTree->SetBranchAddress("NNDistance", &fKDTreePointData.fNNDistance);
                fKDTreeTree->SetBranchAddress("KNNWithin1", &fKDTreePointData.fKNNWithin1);
                fKDTreeTree->SetBranchAddress("KNNWithin2", &fKDTreePointData.fKNNWithin2);
                fKDTreeTree->SetBranchAddress("KNNWithin3", &fKDTreePointData.fKNNWithin3);
                fKDTreeTree->SetBranchAddress("KNNWithin4", &fKDTreePointData.fKNNWithin4);
                fKDTreeTree->SetBranchAddress("KNNWithin5", &fKDTreePointData.fKNNWithin5);
                fKDTreeTree->SetBranchAddress("KNNWithin6", &fKDTreePointData.fKNNWithin6);
                fKDTreeTree->SetBranchAddress("KNNWithin7", &fKDTreePointData.fKNNWithin7);
                fKDTreeTree->SetBranchAddress("KNNWithin8", &fKDTreePointData.fKNNWithin8);

                return true;
            }
        }

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
        fKDTreeTree->Branch("NoiseFlag", &fKDTreePointData.fNoiseFlag, "fNoiseFlag/O");
        fKDTreeTree->Branch("BinInSlice", &fKDTreePointData.fBinInSlice, "fBinInSlice/i");
        fKDTreeTree->Branch("NNDistance", &fKDTreePointData.fNNDistance, "fNNDistance/d");
        fKDTreeTree->Branch("KNNWithin1", &fKDTreePointData.fKNNWithin1, "fKNNWithin1/i");
        fKDTreeTree->Branch("KNNWithin2", &fKDTreePointData.fKNNWithin2, "fKNNWithin2/i");
        fKDTreeTree->Branch("KNNWithin3", &fKDTreePointData.fKNNWithin3, "fKNNWithin3/i");
        fKDTreeTree->Branch("KNNWithin4", &fKDTreePointData.fKNNWithin4, "fKNNWithin4/i");
        fKDTreeTree->Branch("KNNWithin5", &fKDTreePointData.fKNNWithin5, "fKNNWithin5/i");
        fKDTreeTree->Branch("KNNWithin6", &fKDTreePointData.fKNNWithin6, "fKNNWithin6/i");
        fKDTreeTree->Branch("KNNWithin7", &fKDTreePointData.fKNNWithin7, "fKNNWithin7/i");
        fKDTreeTree->Branch("KNNWithin8", &fKDTreePointData.fKNNWithin8, "fKNNWithin8/i");

        return true;
    }

    //**************************
    // Amplitude Distribution
    //**************************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteAmplitudeDistributions(Nymph::KTDataPtr data)
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
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "freqCand", fAmpDistTree );

            if( fAmpDistTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fAmpDistTree );

                fAmpDistTree->SetBranchAddress("Component", &fAmpDistData.fComponent);
                fAmpDistTree->SetBranchAddress("FreqBin", &fAmpDistData.fFreqBin);
                fAmpDistTree->SetBranchAddress("Distribution", &fAmpDistData.fDistribution);

                return true;
            }
        }

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

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteHoughData(Nymph::KTDataPtr data)
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
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "hough", fHoughTree );

            if( fHoughTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fHoughTree );

                fHoughTree->SetBranchAddress("Component", &fHoughData.fComponent);
                fHoughTree->SetBranchAddress("Transform", &fHoughData.fTransform);
                fHoughTree->SetBranchAddress("XOffset", &fHoughData.fXOffset);
                fHoughTree->SetBranchAddress("XScale", &fHoughData.fXScale);
                fHoughTree->SetBranchAddress("YOffset", &fHoughData.fYOffset);
                fHoughTree->SetBranchAddress("YScale", &fHoughData.fYScale);

                return true;
            }
        }

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

    //*************************
    // Flattened PS Data
    //*************************

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteFlattenedPSDData(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write psd data root tree");
        KTPowerSpectrumData& psData = data->Of< KTPowerSpectrumData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fFlattenedPSDTree == NULL)
        {
            if (! SetupFlattenedPSDTree())
            {
                KTERROR(publog, "Something went wrong while setting up the psd tree! Nothing was written.");
                return;
            }
        }

        KTPowerSpectrum* spectrum = psData.GetSpectrum(0);

        for( unsigned iFreqBin = 0; iFreqBin < spectrum->GetNFrequencyBins(); ++iFreqBin )
        {
            fPowerValue = (*spectrum)(iFreqBin);
            fFlattenedPSDTree->Fill();
        }

        return;
    }

    bool KTROOTTreeTypeWriterSpectrumAnalysis::SetupFlattenedPSDTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "flattenedPSD", fFlattenedPSDTree );

            if( fFlattenedPSDTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fFlattenedPSDTree );

                fFlattenedPSDTree->SetBranchAddress( "Power", &fPowerValue );

                return true;
            }
        }

        fFlattenedPSDTree = new TTree("flattenedPSD", "Flattened Spectrum");
        if (fFlattenedPSDTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fFlattenedPSDTree);


        fFlattenedPSDTree->Branch( "Power", &fPowerValue, "fPower/d" );

        return true;
    }

    void KTROOTTreeTypeWriterSpectrumAnalysis::WriteFlattenedLabelMask(Nymph::KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write psd data root tree");
        KTPowerSpectrumData& psData = data->Of< KTPowerSpectrumData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fFlattenedLabelMaskTree == NULL)
        {
            if (! SetupFlattenedLabelMaskTree())
            {
                KTERROR(publog, "Something went wrong while setting up the psd tree! Nothing was written.");
                return;
            }
        }

        KTPowerSpectrum* spectrum = psData.GetSpectrum(0);

        SetMaskThreshold( node->get_value< double >( "mask-threshold", GetMaskThreshold() ) );
        for( unsigned iFreqBin = 0; iFreqBin < spectrum->GetNFrequencyBins(); ++iFreqBin )
        {
            if( (*spectrum)(iFreqBin) > fMaskThreshold )
            {
                KTDEBUG( publog, "Nonzero power = " << (*spectrum)(iFreqBin) );
                fLabel = 1;
            }
            else
            {
                fLabel = 0;
            }

            fFlattenedLabelMaskTree->Fill();
        }

        return;
    }

    bool KTROOTTreeTypeWriterSpectrumAnalysis::SetupFlattenedLabelMaskTree()
    {
        if( fWriter->GetAccumulate() )
        {
            fWriter->GetFile()->GetObject( "flattenedPSDLabels", fFlattenedLabelMaskTree );

            if( fFlattenedLabelMaskTree != NULL )
            {
                KTINFO( publog, "Tree already exists; will add to it" );
                fWriter->AddTree( fFlattenedLabelMaskTree );

                fFlattenedLabelMaskTree->SetBranchAddress( "Label", &fLabel );

                return true;
            }
        }

        fFlattenedLabelMaskTree = new TTree("flattenedPSDLabels", "Flattened Spectrum Labels");
        if (fFlattenedLabelMaskTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fFlattenedLabelMaskTree);


        fFlattenedLabelMaskTree->Branch( "Label", &fLabel, "fLabel/i" );

        return true;
    }


} /* namespace Katydid */

