/*
 * TestMultiSliceClustering.cc
 *
 *  Created on: Mar 4, 2013
 *      Author: nsoblath
 */


#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "logger.hh"
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

LOGGER(vallog, "TestMultiSliceClustering")

int main()
{
    typedef KTDiscriminatedPoints1DData::Point Point;

    unsigned freqBins = 51;
    double freqBW = 100.; // Hz

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
    freqPoints.insert(pair< unsigned, Point >(5, Point(2., 1.0, 0.5)));  // this should be in a cluster by itself
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 1
    freqPoints.insert(pair< unsigned, Point >(8, Point(2., 1.0, 0.5)));  // this point and the next should form a cluster
    freqPoints.insert(pair< unsigned, Point >(9, Point(2., 1.0, 0.5)));
    freqPoints.insert(pair< unsigned, Point >(11, Point(2., 1.0, 0.5))); // this point should be clustered with the first three in the next two time bins
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 2
    freqPoints.insert(pair< unsigned, Point >(11, Point(2., 1.0, 0.5))); // these three should be clustered with the last from the previous time bin,
    freqPoints.insert(pair< unsigned, Point >(12, Point(2., 1.0, 0.5))); // and the first three in the next time bin
    freqPoints.insert(pair< unsigned, Point >(13, Point(2., 1.0, 0.5)));
    freqPoints.insert(pair< unsigned, Point >(5, Point(2., 1.0, 0.5)));  // this should cluster with the last from the next time bin
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 3
    freqPoints.insert(pair< unsigned, Point >(11, Point(2., 1.0, 0.5))); // these three should cluster with the first three from the previous time bin
    freqPoints.insert(pair< unsigned, Point >(12, Point(2., 1.0, 0.5)));
    freqPoints.insert(pair< unsigned, Point >(13, Point(2., 1.0, 0.5)));
    freqPoints.insert(pair< unsigned, Point >(6, Point(2., 1.0, 0.5)));  // this should cluster with the last from the previous time bin
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 4
    freqPoints.insert(pair< unsigned, Point >(20, Point(2., 1.0, 0.5))); // these two should cluster with the two in the next two time bins
    freqPoints.insert(pair< unsigned, Point >(21, Point(2., 1.0, 0.5)));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 5
    freqPoints.insert(pair< unsigned, Point >(20, Point(2., 1.0, 0.5))); // these two should cluster with the two in the previous and next time bins
    freqPoints.insert(pair< unsigned, Point >(19, Point(2., 1.0, 0.5)));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 6
    freqPoints.insert(pair< unsigned, Point >(19, Point(2., 1.0, 0.5))); // these two should cluster with the two in the previous two time bins
    freqPoints.insert(pair< unsigned, Point >(18, Point(2., 1.0, 0.5)));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 7
    freqPoints.insert(pair< unsigned, Point >(30, Point(2., 1.0, 0.5))); // these two should not be clustered until the next time bin is read, at which point
    freqPoints.insert(pair< unsigned, Point >(35, Point(2., 1.0, 0.5))); // they should be merged together into one cluster
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 8
    freqPoints.insert(pair< unsigned, Point >(31, Point(2., 1.0, 0.5))); // these four should cause the merged clustering of the two points in the previous time bin
    freqPoints.insert(pair< unsigned, Point >(32, Point(2., 1.0, 0.5)));
    freqPoints.insert(pair< unsigned, Point >(34, Point(2., 1.0, 0.5)));
    freqPoints.insert(pair< unsigned, Point >(33, Point(2., 1.0, 0.5)));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 9
    freqPoints.insert(pair< unsigned, Point >(2, Point(2., 1.0, 0.5))); // this point should be clustered with the point in the next time bin
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    // time bin 10
    freqPoints.insert(pair< unsigned, Point >(2, Point(2., 1.0, 0.5))); // this point should be clustered with the point in the previous time bin
    allPoints.push_back(freqPoints);
    // total number of "truth" clusters: 7

    LINFO(vallog, "Discriminated points have been simulated.");
    LINFO(vallog, "There should be 7 clusters");

    KTMultiSliceClustering clustering;
    clustering.SetFrequencyBinWidth(freqBW);
    clustering.SetMaxFrequencySeparationBins(1);
    clustering.SetMinTimeBins(1);

    KTMultiSliceClustering::DataList allNewData;

    LINFO(vallog, "Commencing with the clustering process");
    uint64_t iSlice = 0;
    for (vector< KTDiscriminatedPoints1DData::SetOfPoints >::const_iterator setIt = allPoints.begin(); setIt != allPoints.end(); setIt++)
    {
        LINFO(vallog, "Creating time bin " << iSlice);
        // Setup this time bin's input data
        KTSliceHeader header(masterHeader);
        header.SetSliceNumber(iSlice);
        header.SetTimeInRun(double(iSlice));
        KTDiscriminatedPoints1DData dpDataIn;
        dpDataIn.SetNComponents(1);
        dpDataIn.SetBinWidth(freqBW);
        KTFrequencySpectrumPolar* freqSpec = new KTFrequencySpectrumPolar(freqBins, -0.5*freqBW, (double(freqBins) - 0.5) * freqBW);
        KTFrequencySpectrumDataPolar fsDataIn;
        fsDataIn.SetNComponents(1);
        fsDataIn.SetSpectrum(freqSpec, 0);

        for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pointIt = setIt->begin(); pointIt != setIt->end(); pointIt++)
        {
            LDEBUG(vallog, "    adding point at " << pointIt->first);
            dpDataIn.AddPoint(pointIt->first, Point(pointIt->second.fAbscissa, 1.0, 1.0), 0);
            (*freqSpec)(pointIt->first).set_polar(pointIt->second.fAbscissa, 0.);
        }
        LINFO(vallog, "Time bin created");

        LINFO(vallog, "Adding points to clusters");
        KTMultiSliceClustering::DataList* newData = clustering.FindClusters(dpDataIn, fsDataIn, header);
        LINFO(vallog, "New data produced: " << newData->size());

        allNewData.splice(allNewData.end(), *newData);
        delete newData;

        iSlice++;
    }

    LINFO(vallog, "Cleaning up remaining active clusters");
    KTMultiSliceClustering::DataList* newData = clustering.CompleteAllClusters();
    LINFO(vallog, "New data produced: " << newData->size());

    allNewData.splice(allNewData.end(), *newData);
    delete newData;


#ifdef ROOT_FOUND
    string rootFilename("waterfall_test.root");

    LINFO(vallog, "Printing waterfall plots to " << rootFilename);

    TFile file(rootFilename.c_str(), "recreate");

    unsigned iCandidate = 0;
    for (KTMultiSliceClustering::DataList::const_iterator it = allNewData.begin(); it != allNewData.end(); it++)
    {
        stringstream conv;
        conv << "hCandidate" << iCandidate;
        string histName(conv.str());
        if (! (*it)->Has< KTWaterfallCandidateData >())
        {
            LERROR(vallog, "Waterfall candidate data is not present!");
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

    LINFO(vallog, "Test complete; " << allNewData.size() << " new data objects were created.");

    return 0;
}

