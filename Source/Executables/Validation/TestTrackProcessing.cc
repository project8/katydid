/**
@file TestTrackProcessing.cc
@brief Test executable of the TrackProcessing Processor
@author M. Guigue
@date May 27, 2018
@details
This executable tests the Track processing processor by faking a SparseWaterfallCandidateData 
and tests the behavior of the algorithms in this processor.
*/

#include "KTLogger.hh"

#include "KTTrackProcessing.hh"
#include "KTProcessedTrackData.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTDiscriminatedPoint.hh"
#include "KTRandom.hh"

#include <cmath>        // std::abs

// Define the parameters of the fake track to generate
double trackSlope = 100e6; // [Hz/s]
double trackIntercept = 1e5; // [Hz]
double trackStart = 0.1; //[s] 
double trackLength = 0.1; //[s]
double trackSigma = 20000.; // [Hz]
double trackPowerMean = 1e-10;
double trackPowerStd = 1e-11;
int nSlices = 20;
int avgPointsPerSlice = 1;

using namespace Katydid;

KTLOGGER(testlog, "TestTrackProcessing");

KTSparseWaterfallCandidateData createFakeData(){

    KTSparseWaterfallCandidateData sftData;
    
    KTRNGGaussian<> noiseDistribution(0,trackSigma);
    KTRNGGaussian<> powerDistribution(trackPowerMean,trackPowerStd);
    KTRNGPoisson<> numberPointsDistribution(avgPointsPerSlice);

    typedef KTDiscriminatedPoint Point;
    
    if (nSlices<=1){
        KTERROR( testlog, "Number of slices <" << nSlices <<"> should be larger than 1!");
        return sftData;
    }
    double timeStep = (trackLength)/(nSlices-1);
    double sliceTime = trackStart;
    for (unsigned iSlice=0; iSlice <nSlices; ++iSlice){
        int nPoints = numberPointsDistribution();
        for (unsigned iPoint = 0; iPoint<nPoints; ++iPoint){
            double yPoint = trackIntercept + trackSlope*sliceTime + noiseDistribution();
            double power = powerDistribution();
            Point aPoint(sliceTime,yPoint,power,sliceTime,1.,1.,1.);
            sftData.AddPoint(aPoint);
        }
        sliceTime +=timeStep;
    }
    return sftData;
}

int main()
{

    KTINFO(testlog, "Finally, a customer!");

    // Processor definition
    KTTrackProcessing trackProc;
    trackProc.SetTrackProcAlgorithm("weighted-slope");
    trackProc.SetSlopeMinimum(0);
    trackProc.SetProcTrackMinPoints(1);
    trackProc.SetProcTrackAssignedError(12000);

    // Execute the Processing step
    KTSparseWaterfallCandidateData swfData = createFakeData();
    trackProc.ProcessTrackSWF(swfData);

    KTProcessedTrackData& procTrack = swfData.Of< KTProcessedTrackData >();

    // Check the results of the processing
    double foundFrequency = procTrack.GetStartFrequency();
    double toBeFoundFrequency = trackIntercept + trackSlope*trackStart;
    double diff = foundFrequency - toBeFoundFrequency;
    KTINFO(testlog, "Found a track with start frequency: " << foundFrequency << "; should be close to " << toBeFoundFrequency );
    if (std::abs(diff)>1.e5){
        KTERROR(testlog, "The difference seems too large (>1e5 Hz)! " );
        return -1;
    }
    return 0;
}