/*
 * TestSimpleClustering.cc
 *
 *  Created on: Feb 4, 2013
 *      Author: nsoblath
 */


#include "KTDiscriminatedPoints1DData.hh"
#include "KTLogger.hh"
#include "KTSimpleClustering.hh"

#include <vector>

using namespace Katydid;
using namespace std;

KTLOGGER(vallog, "TestSimpleClustering")

int main()
{
    double freqBW = 100.; // Hz

    // Simulated points
    vector< KTDiscriminatedPoints1DData::SetOfPoints > allPoints;
    KTDiscriminatedPoints1DData::SetOfPoints freqPoints;
    // assumed clustering rules: sequential time bins are clustered; frequency bin separation = 1; no threshold on the cluster size
    freqPoints.insert(pair< unsigned, double >(5, 2.));  // this should be in a cluster by itself
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    freqPoints.insert(pair< unsigned, double >(8, 2.));  // this point and the next should form a cluster
    freqPoints.insert(pair< unsigned, double >(9, 2.));
    freqPoints.insert(pair< unsigned, double >(11, 2.)); // this point should be clustered with the first three in the next two time bins
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    freqPoints.insert(pair< unsigned, double >(11, 2.)); // these three should be clustered with the last from the previous time bin,
    freqPoints.insert(pair< unsigned, double >(12, 2.)); // and the first three in the next time bin
    freqPoints.insert(pair< unsigned, double >(13, 2.));
    freqPoints.insert(pair< unsigned, double >(5, 2.));  // this should cluster with the last from the next time bin
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    freqPoints.insert(pair< unsigned, double >(11, 2.)); // these three should cluster with the first three from the previous time bin
    freqPoints.insert(pair< unsigned, double >(12, 2.));
    freqPoints.insert(pair< unsigned, double >(13, 2.));
    freqPoints.insert(pair< unsigned, double >(6, 2.));  // this should cluster with the last from the previous time bin
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    freqPoints.insert(pair< unsigned, double >(20, 2.)); // these two should cluster with the two in the next two time bins
    freqPoints.insert(pair< unsigned, double >(21, 2.));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    freqPoints.insert(pair< unsigned, double >(20, 2.)); // these two should cluster with the two in the previous and next time bins
    freqPoints.insert(pair< unsigned, double >(19, 2.));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    freqPoints.insert(pair< unsigned, double >(19, 2.)); // these two should cluster with the two in the previous two time bins
    freqPoints.insert(pair< unsigned, double >(18, 2.));
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    freqPoints.insert(pair< unsigned, double >(30, 2.)); // these two should not be clustered until the next time bin is read, at which point
    freqPoints.insert(pair< unsigned, double >(35, 2.)); // they should be merged together into one cluster
    allPoints.push_back(freqPoints);
    freqPoints.clear();
    freqPoints.insert(pair< unsigned, double >(31, 2.)); // these four should cause the merged clustering of the two points in the previous time bin
    freqPoints.insert(pair< unsigned, double >(32, 2.));
    freqPoints.insert(pair< unsigned, double >(34, 2.));
    freqPoints.insert(pair< unsigned, double >(33, 2.));
    allPoints.push_back(freqPoints);
    // total number of "truth" clusters: 6

    KTINFO(vallog, "Discriminated points have been simulated.");
    KTINFO(vallog, "There should be 6 clusters");

    KTSimpleClustering clustering;
    clustering.SetFrequencyBinWidth(freqBW);
    clustering.SetMaxFrequencySeparationBins(1);

    KTSimpleClustering::NewBundleList allNewBundles;

    KTINFO(vallog, "Commencing with the clustering process");
    for (vector< KTDiscriminatedPoints1DData::SetOfPoints >::const_iterator setIt = allPoints.begin(); setIt != allPoints.end(); setIt++)
    {
        KTINFO(vallog, "Creating time bin");
        // Setup this frequency bin's input data
        KTDiscriminatedPoints1DData dataIn(1);
        dataIn.SetBinWidth(freqBW);
        dataIn.SetThreshold(1., 0);
        for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pointIt = setIt->begin(); pointIt != setIt->end(); pointIt++)
        {
            KTDEBUG(vallog, "    adding point");
            dataIn.AddPoint(pointIt->first, pointIt->second, 0);
        }
        KTINFO(vallog, "Time bin created");

        KTINFO(vallog, "Adding points to clusters");
        KTSimpleClustering::NewBundleList* newBundles = clustering.AddPointsToClusters(&dataIn);
        KTINFO(vallog, "New bundles produced: " << newBundles->size());

        allNewBundles.splice(allNewBundles.end(), *newBundles);
        delete newBundles;
    }

    KTINFO(vallog, "Cleaning up remaining active clusters");
    KTSimpleClustering::NewBundleList* newBundles = clustering.CompleteAllClusters(0);
    KTINFO(vallog, "New bundles produced: " << newBundles->size());

    allNewBundles.splice(allNewBundles.end(), *newBundles);
    delete newBundles;

    KTINFO(vallog, "Test complete; " << allNewBundles.size() << " new bundles were created.");

    return 0;
}

