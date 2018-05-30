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
#include "KTRandom.hh"

// Define the parameters of the fake track to generate
double trackSlope = 100e6; // [Hz/s]
double trackIntercept = 1e5; // [Hz]
double trackStart = 1.; //[s]
double trackLength = 1.; //[s]
double trackSigma = 20000.; // [Hz]
double trackPowerMean = 1e-10;
double trackPowerStd = 1e-11;

int nSlices = 100;
int avgPointsPerSlice = 20;
double freqBinWidth = 24000.;
double timeBinWidth = 4.096e-5;
double pointPowerMean = 166.e-6;
double pointPowerStd = 166.e-6;
unsigned minBin = 0;
unsigned maxBin = 4096;
double threshold = 6;
unsigned component = 0;



using namespace Katydid;

KTLOGGER(testlog, "TestSequentialTrackFinder");

KTSliceHeader createFakeHeader( unsigned sliceNumber ){
    KTSliceHeader header;
    header.SetBinWidth(timeBinWidth);
    header.SetTimeInAcq(timeBinWidth*sliceNumber);
    header.SetSampleRate(100.e6);
    header.SetRawSliceSize(4096);
    return header;
}

KTDiscriminatedPoints1DData createFakeData(){

    KTDiscriminatedPoints1DData disc1d;

    KTRNGUniform<> yBin(minBin, maxBin);
    KTRNGGaussian<> powerDistribution(pointPowerMean,pointPowerStd);
    KTRNGPoisson<> numberPointsDistribution(avgPointsPerSlice);

    typedef KTDiscriminatedPoints1DData::Point Point;

    int nPoints = numberPointsDistribution();
    for (unsigned iPoint = 0; iPoint<nPoints; ++iPoint)
    {
        double power = powerDistribution();
        unsigned iBin = yBin();
        disc1d.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(freqBinWidth * ((double)iBin + 0.5), power, pointPowerMean/threshold), component);
    }
    return disc1d;
}

int main()
{

    KTINFO(testlog, "Testing STF!");

    KTSequentialTrackFinder stf;
    stf.SetTrimmingThreshold(7);
    stf.SetApplyTotalPowerCut(true);
    stf.SetTotalPowerThreshold(166e-6);
    stf.SetMinFrequency(0.);
    stf.SetMaxFrequency(100.e6);
    stf.SetMinPoints(2);
    stf.SetFrequencyAcceptance(1.e8);
    stf.SetTimeGapTolerance(1.e-3);

    KTDiscriminatedPoints1DData disc1d;

    for (unsigned iSlice = 0; iSlice <= nSlices; ++iSlice )
    {
        KTSliceHeader header = createFakeHeader(iSlice);
        KTINFO(testlog, "Sample rate is "<<header.GetSampleRate());

        disc1d = createFakeData();
        stf.CollectDiscrimPointsFromSlice(header, disc1d);
    }

    stf.AcquisitionIsOver();
    const std::set< Nymph::KTDataPtr >& candidates = stf.GetCandidates();
    KTINFO(testlog, "Candidates found: " << candidates.size())

    return 0;
}
