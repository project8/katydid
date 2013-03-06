/*
 * TestMultiSliceClustering.cc
 *
 *  Created on: Mar 4, 2013
 *      Author: nsoblath
 */


#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTLogger.hh"
#include "KTMultiSliceClustering.hh"
#include "KTSliceHeader.hh"
#include "KTTimeFrequencyPolar.hh"
#include "KTWaterfallCandidateData.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#include "TFile.h"
#endif

#include <vector>

using namespace Katydid;
using namespace std;

KTLOGGER(vallog, "katydid.applications.validation")

int main()
{
    UInt_t freqBins = 51;
    Double_t freqBW = 100.; // Hz

    KTSliceHeader masterHeader;
    masterHeader.SetNComponents(1);
    masterHeader.SetSliceSize(100);
    masterHeader.SetSampleRate(1.);
    masterHeader.CalculateBinWidthAndSliceLength();
    masterHeader.SetAcquisitionID(42, 0);
    // time in run and slice number set for each slice


    // Simulated points
    vector< KTDiscriminatedPoints1DData::SetOfPoints > allPoints;
    KTDiscriminatedPoints1DData::SetOfPoints freqPoints;
    // assumed clustering rules: sequential time bins are clustered; frequency bin separation = 1; no threshold on the cluster size
    // time bin 0
    freqPoints.insert(pair< UInt_t, Double_t >(5, 2.));  // this should be in a cluster by itself
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 1
    freqPoints.insert(pair< UInt_t, Double_t >(8, 2.));  // this point and the next should form a cluster
    freqPoints.insert(pair< UInt_t, Double_t >(9, 2.));
    freqPoints.insert(pair< UInt_t, Double_t >(11, 2.)); // this point should be clustered with the first three in the next two time bins
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 2
    freqPoints.insert(pair< UInt_t, Double_t >(11, 2.)); // these three should be clustered with the last from the previous time bin,
    freqPoints.insert(pair< UInt_t, Double_t >(12, 2.)); // and the first three in the next time bin
    freqPoints.insert(pair< UInt_t, Double_t >(13, 2.));
    freqPoints.insert(pair< UInt_t, Double_t >(5, 2.));  // this should cluster with the last from the next time bin
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 3
    freqPoints.insert(pair< UInt_t, Double_t >(11, 2.)); // these three should cluster with the first three from the previous time bin
    freqPoints.insert(pair< UInt_t, Double_t >(12, 2.));
    freqPoints.insert(pair< UInt_t, Double_t >(13, 2.));
    freqPoints.insert(pair< UInt_t, Double_t >(6, 2.));  // this should cluster with the last from the previous time bin
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 4
    freqPoints.insert(pair< UInt_t, Double_t >(20, 2.)); // these two should cluster with the two in the next two time bins
    freqPoints.insert(pair< UInt_t, Double_t >(21, 2.));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 5
    freqPoints.insert(pair< UInt_t, Double_t >(20, 2.)); // these two should cluster with the two in the previous and next time bins
    freqPoints.insert(pair< UInt_t, Double_t >(19, 2.));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 6
    freqPoints.insert(pair< UInt_t, Double_t >(19, 2.)); // these two should cluster with the two in the previous two time bins
    freqPoints.insert(pair< UInt_t, Double_t >(18, 2.));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 7
    freqPoints.insert(pair< UInt_t, Double_t >(30, 2.)); // these two should not be clustered until the next time bin is read, at which point
    freqPoints.insert(pair< UInt_t, Double_t >(35, 2.)); // they should be merged together into one cluster
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 8
    freqPoints.insert(pair< UInt_t, Double_t >(31, 2.)); // these four should cause the merged clustering of the two points in the previous time bin
    freqPoints.insert(pair< UInt_t, Double_t >(32, 2.));
    freqPoints.insert(pair< UInt_t, Double_t >(34, 2.));
    freqPoints.insert(pair< UInt_t, Double_t >(33, 2.));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 9
    freqPoints.insert(pair< UInt_t, Double_t >(2, 2.)); // this point should be clustered with the point in the next time bin
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 10
    freqPoints.insert(pair< UInt_t, Double_t >(2, 2.)); // this point should be clustered with the point in the previous time bin
    allPoints.push_back(freqPoints);
    // total number of "truth" clusters: 7

    KTINFO(vallog, "Discriminated points have been simulated.");
    KTINFO(vallog, "There should be 7 clusters");

    KTMultiSliceClustering clustering;
    clustering.SetFrequencyBinWidth(freqBW);
    clustering.SetMaxFrequencySeparationBins(1);
    clustering.SetMinTimeBins(1);

    KTMultiSliceClustering::DataList allNewData;

    KTINFO(vallog, "Commencing with the clustering process");
    ULong64_t iSlice = 0;
    for (vector< KTDiscriminatedPoints1DData::SetOfPoints >::const_iterator setIt = allPoints.begin(); setIt != allPoints.end(); setIt++)
    {
        KTINFO(vallog, "Creating time bin " << iSlice);
        // Setup this time bin's input data
        KTSliceHeader header(masterHeader);
        header.SetSliceNumber(iSlice);
        header.SetTimeInRun(Double_t(iSlice));
        KTDiscriminatedPoints1DData dpDataIn;
        dpDataIn.SetNComponents(1);
        dpDataIn.SetBinWidth(freqBW);
        dpDataIn.SetThreshold(1., 0);
        KTFrequencySpectrumPolar* freqSpec = new KTFrequencySpectrumPolar(freqBins, -0.5*freqBW, (Double_t(freqBins) - 0.5) * freqBW);
        KTFrequencySpectrumDataPolar fsDataIn;
        fsDataIn.SetNComponents(1);
        fsDataIn.SetSpectrum(freqSpec, 0);

        for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pointIt = setIt->begin(); pointIt != setIt->end(); pointIt++)
        {
            KTDEBUG(vallog, "    adding point at " << pointIt->first);
            dpDataIn.AddPoint(pointIt->first, pointIt->second, 0);
            (*freqSpec)(pointIt->first).set_polar(pointIt->second, 0.);
        }
        KTINFO(vallog, "Time bin created");

        KTINFO(vallog, "Adding points to clusters");
        KTMultiSliceClustering::DataList* newData = clustering.FindClusters(dpDataIn, fsDataIn, header);
        KTINFO(vallog, "New data produced: " << newData->size());

        allNewData.splice(allNewData.end(), *newData);
        delete newData;

        iSlice++;
    }

    KTINFO(vallog, "Cleaning up remaining active clusters");
    KTMultiSliceClustering::DataList* newData = clustering.CompleteAllClusters();
    KTINFO(vallog, "New data produced: " << newData->size());

    allNewData.splice(allNewData.end(), *newData);
    delete newData;


#ifdef ROOT_FOUND
    string rootFilename("waterfall_test.root");

    KTINFO(vallog, "Printing waterfall plots to " << rootFilename);

    TFile file(rootFilename.c_str(), "recreate");

    UInt_t iCandidate = 0;
    for (KTMultiSliceClustering::DataList::const_iterator it = allNewData.begin(); it != allNewData.end(); it++)
    {
        stringstream conv;
        conv << "hCandidate" << iCandidate;
        string histName(conv.str());
        if (! (*it)->Has< KTWaterfallCandidateData >())
        {
            KTERROR(vallog, "Waterfall candidate data is not present!");
            iCandidate++;
            continue;
        }
        KTWaterfallCandidateData& wfData = (*it)->Of< KTWaterfallCandidateData >();
        TH2D* wfHist = wfData.GetCandidate()->CreatePowerHistogram(histName.c_str());
        wfHist->SetDirectory(&file);
        wfHist->Write();
        iCandidate++;
    }

    file.Close();
#endif

    KTINFO(vallog, "Test complete; " << allNewData.size() << " new data objects were created.");

    return 0;
}

