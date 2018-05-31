/**
@file TestSequentialTrackFinder.cc
@brief Test executable of the SequentialTrackFinder the OverlapingTrackClustering and the IterativeTrackClustering Processor
@author C. Claessens
@date May 30, 2018
@details
This executable tests the finding of SparseWaterfallCandidates by faking DiscriminatedPoints1DData
and tests the behavior of the algorithms in this processor.
*/


#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTSequentialTrackFinder.hh"
#include "KTOverlappingTrackClustering.hh"
#include "KTIterativeTrackClustering.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTSeqLine.hh"
#include "KTRandom.hh"



int nSlices = 70;


int avgPointsPerSlice = 10;
double freqBinWidth = 24000.;
double timeBinWidth = 4.096e-5;
double pointPowerMean = 166.0e-6;
double pointPowerStd = 166.0e-7;
unsigned minBin = 0;
unsigned maxBin = 4096;
double threshold = 6;
unsigned component = 0;

// Define the parameters of the fake track to generate
double trackSlope = 500e6/freqBinWidth*timeBinWidth; // [Bins/s]
double trackIntercept = 1000; // [Bins]
unsigned trackStart = 20; //[Bin]
unsigned trackLength = 50; //[Bin]

using namespace Katydid;

KTLOGGER(testlog, "TestSequentialTrackFinder");

KTSliceHeader createFakeHeader( unsigned sliceNumber ){
    KTSliceHeader header;
    header.SetBinWidth(timeBinWidth);
    header.SetTimeInAcq(timeBinWidth*sliceNumber);
    header.SetTimeInRun(timeBinWidth*sliceNumber);
    header.SetSampleRate(100.e6);
    header.SetRawSliceSize(4096);
    return header;
}

KTDiscriminatedPoints1DData createFakeData(unsigned sliceNumber){

    KTDiscriminatedPoints1DData disc1d;

    KTRNGUniform<> yBin(minBin, maxBin);
    KTRNGGaussian<> powerDistribution(pointPowerMean, pointPowerStd);
    KTRNGPoisson<> numberPointsDistribution(avgPointsPerSlice);

    typedef KTDiscriminatedPoints1DData::Point Point;

    // random points
    int nPoints = numberPointsDistribution();
    for (unsigned iPoint = 0; iPoint<nPoints; ++iPoint)
    {

        double power = powerDistribution();
        unsigned iBin = yBin();
        disc1d.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(freqBinWidth * ((double)iBin + 0.5), power, pointPowerMean-2*pointPowerStd), component);
        KTINFO(testlog, "Adding point: "<<iBin<<" "<<freqBinWidth* ((double)iBin + 0.5)<<" "<<power);
    }
    // track points
    if (sliceNumber >= trackStart and sliceNumber < trackStart + trackLength)
    {
        double power = powerDistribution();
        unsigned iBin = trackIntercept + trackSlope*(sliceNumber - trackStart);
        disc1d.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(freqBinWidth * ((double)iBin + 0.5), power, pointPowerMean-2*pointPowerStd), component);
        KTINFO(testlog, "Adding track point: "<<iBin<<" "<<freqBinWidth* ((double)iBin + 0.5)<<" "<<power);
    }
    return disc1d;
}

int main()
{

    KTINFO(testlog, "Testing STF!");
    KTINFO(testlog, "test track slope: "<<trackSlope);

    KTSequentialTrackFinder stf;

    // Apply some settings
    stf.SetTrimmingThreshold(1);
    stf.SetApplyTotalPowerCut(false);
    stf.SetTotalPowerThreshold(166e-6);
    stf.SetMinFrequency(0.);
    stf.SetMaxFrequency(100.e6);
    stf.SetMinPoints(3);
    stf.SetFrequencyAcceptance(60.e3);
    stf.SetTimeGapTolerance(1.e-3);
    stf.SetMinSlope(0);



    // Create fake data for every slice and run stf
    for (unsigned iSlice = 0; iSlice <= nSlices; ++iSlice )
    {
        KTSliceHeader header = createFakeHeader(iSlice);
        KTINFO(testlog, "Sample rate is "<<header.GetSampleRate());

        KTDiscriminatedPoints1DData disc1d = createFakeData(iSlice);
        stf.CollectDiscrimPointsFromSlice(header, disc1d);
    }
    stf.AcquisitionIsOver();

    // Get output
    const std::set< Nymph::KTDataPtr >& candidates = stf.GetCandidates();
    KTINFO(testlog, "Candidates found: " << candidates.size());

    unsigned iCand = 0;
    typedef KTSequentialLine::Point Point;

    // Print some output
    for (std::set< Nymph::KTDataPtr >::const_iterator cIt = candidates.begin(); cIt != candidates.end(); ++cIt)
    {
        KTINFO(testlog, "Candidate " << iCand);
        KTSequentialLine& sqlData = (*cIt)->Of< KTSequentialLine >();
        KTINFO(testlog, "Properties (starttime/frequnecy - endtime/frequency - slope) "<<sqlData.GetStartTimeInRunC()<<" / "<<sqlData.GetStartFrequency()<<" - "<<sqlData.GetEndTimeInRunC()<<" / "<<sqlData.GetEndFrequency()<<" - "<<sqlData.GetSlope());

        iCand++;

        const std::vector<Point>& candPoints = sqlData.GetPoints();
        for(std::vector<Point>::const_iterator pointIt = candPoints.begin(); pointIt != candPoints.end(); ++pointIt )
        {
            KTINFO(testlog, "Point: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fNeighborhoodAmplitude);
        }
    }

    return 0;
}
