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
#include "KTSequentialLineData.hh"
#include "KTSequentialLineSNRCut.hh"
#include "KTRandom.hh"



using namespace Katydid;

KTLOGGER(testlog, "TestSequentialTrackFinder");

KTSliceHeader createFakeHeader( unsigned sliceNumber, double timeBinWidth ){


    KTSliceHeader header;
    header.SetBinWidth(timeBinWidth);
    header.SetTimeInAcq(timeBinWidth*sliceNumber);
    header.SetTimeInRun(timeBinWidth*sliceNumber);
    header.SetSampleRate(100.e6);
    header.SetRawSliceSize(4096);
    header.SetAcquisitionID(5);
    return header;
}

KTDiscriminatedPoints1DData createFakeData(unsigned sliceNumber, double timeBinWidth, double freqBinWidth){

    int avgPointsPerSlice = 10;


    double pointPowerMean = 1e-6;
    double pointPowerStd = 1e-7;
    unsigned minBin = 0;
    unsigned maxBin = 4096;
    double threshold = pointPowerMean - 2* pointPowerStd;
    unsigned component = 0;

    // Define the parameters of the fake track to generate
    double trackSlope = 500e6/freqBinWidth*timeBinWidth; // [Bins/s]
    double trackIntercept = 1000; // [Bins]
    unsigned trackStart = 20; //[Bin]
    unsigned trackStart2 = 100;
    unsigned trackLength = 50; //[Bin]

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
        disc1d.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(freqBinWidth*((double)iBin + 0.5), power, threshold, pointPowerMean, pointPowerStd, 2*power), component);
        KTINFO(testlog, "Adding point: "<<iBin<<" "<<freqBinWidth* ((double)iBin + 0.5)<<" "<<power);
    }
    // track points
    if (sliceNumber >= trackStart and sliceNumber < trackStart + trackLength)
    {
        double power = powerDistribution();
        unsigned iBin = trackIntercept + trackSlope*(sliceNumber - trackStart);
        disc1d.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(freqBinWidth*((double)iBin + 0.5), power, threshold, pointPowerMean,pointPowerStd, 2*power), component);
        KTINFO(testlog, "Adding track point: "<<iBin<<" "<<freqBinWidth* ((double)iBin + 0.5)<<" "<<power);
    }
    if (sliceNumber >= trackStart2 and sliceNumber < trackStart2 + trackLength)
        {
            double power = powerDistribution();
            unsigned iBin = trackIntercept + trackSlope*(sliceNumber - trackStart);
            disc1d.AddPoint(iBin, KTDiscriminatedPoints1DData::Point(freqBinWidth * ((double)iBin + 0.5), power, threshold, pointPowerMean, pointPowerStd, 2*power), component);
            KTINFO(testlog, "Adding track point: "<<iBin<<" "<<freqBinWidth* ((double)iBin + 0.5)<<" "<<power);
        }
    return disc1d;
}

int main()
{
    int nSlices = 200;
    double timeBinWidth = 4.096e-5;
    double freqBinWidth = 24000.;


    KTINFO(testlog, "Testing STF!");

    KTSequentialTrackFinder stf;
    KTSequentialLineSNRCut snrcut;

    // Apply some settings
    stf.SetTrimmingThreshold(0.9);
    stf.SetApplyTotalSNRCut(true);
    stf.SetTotalSNRThreshold(8);
    stf.SetMinFrequency(0.);
    stf.SetMaxFrequency(100.e6);
    stf.SetMinPoints(2);
    stf.SetFrequencyAcceptance(60.e3);
    stf.SetTimeGapTolerance(1.e-3);
    stf.SetMinSlope(0);

    snrcut.SetMinAverageSNR(1e4);

    // Create fake data for every slice and run stf
    for (unsigned iSlice = 0; iSlice <= nSlices; ++iSlice )
    {
        KTSliceHeader header = createFakeHeader(iSlice, timeBinWidth);
        KTINFO(testlog, "Sample rate is "<<header.GetSampleRate());

        KTDiscriminatedPoints1DData disc1d = createFakeData(iSlice, timeBinWidth, freqBinWidth);
        stf.CollectDiscrimPointsFromSlice(header, disc1d);
    }
    stf.AcquisitionIsOver();

    // Get output
    const std::set< Nymph::KTDataPtr >& candidates = stf.GetCandidates();
    KTINFO(testlog, "Candidates found: " << candidates.size());


    // Print some output
    unsigned STFPoints = 0;
    for (std::set< Nymph::KTDataPtr >::const_iterator cIt = candidates.begin(); cIt != candidates.end(); ++cIt)
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        KTINFO(testlog, "Candidate " << sqlData.GetCandidateID());
        KTINFO(testlog, "Properties (starttime/frequnecy - endtime/frequency - slope) "<<sqlData.GetStartTimeInRunC()<<" / "<<sqlData.GetStartFrequency()<<" - "<<sqlData.GetEndTimeInRunC()<<" / "<<sqlData.GetEndFrequency()<<" - "<<sqlData.GetSlope());
        //KTINFO(testlog, "Length (end time - start time [bins]): "<<(sqlData.GetEndTimeInRunC() - sqlData.GetStartTimeInRunC()) / timeBinWidth);

        const KTDiscriminatedPoints& candPoints = sqlData.GetPoints();
        KTINFO(testlog, "Length [bins]: "<<candPoints.size());
        STFPoints += candPoints.size();
        //for(KTDiscriminatedPoints::const_iterator pointIt = candPoints.begin(); pointIt != candPoints.end(); ++pointIt )
        //{
        //    KTINFO(testlog, "Point: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fNeighborhoodAmplitude);
        //}
    }
    KTINFO(testlog, "Total STF Points: "<<STFPoints);



    // Test OTC
    KTOverlappingTrackClustering otc;

    for (std::set< Nymph::KTDataPtr >::const_iterator cIt = candidates.begin(); cIt != candidates.end(); ++cIt)
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        otc.TakeSeqLineCandidate(sqlData);
    }
    otc.Run();


    // Get output
    std::set< Nymph::KTDataPtr > otccandidates = otc.GetCandidates();

    KTINFO(testlog, "OTC Candidates found: " << otccandidates.size());

    // Print some output
    unsigned OTCPoints = 0;
    for (std::set< Nymph::KTDataPtr >::const_iterator cIt = otccandidates.begin(); cIt != otccandidates.end(); ++cIt)
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        KTINFO(testlog, "OTC Candidate " << sqlData.GetCandidateID());
        KTINFO(testlog, "Properties (starttime/frequnecy - endtime/frequency - slope) "<<sqlData.GetStartTimeInRunC()<<" / "<<sqlData.GetStartFrequency()<<" - "<<sqlData.GetEndTimeInRunC()<<" / "<<sqlData.GetEndFrequency()<<" - "<<sqlData.GetSlope());
        //KTINFO(testlog, "Length (end time - start time [bins]): "<<(sqlData.GetEndTimeInRunC() - sqlData.GetStartTimeInRunC()) / timeBinWidth);

        Nymph::KTData data = (*cIt)->Of< Nymph::KTData >();
        snrcut.Apply(data, sqlData);
        KTINFO(testlog, "Cut status: "<<data.GetCutStatus().IsCut());
        if (data.GetCutStatus().IsCut() == true)
        {
            otccandidates.erase(*cIt);
        }
        else
        {
            const KTDiscriminatedPoints& candPoints = sqlData.GetPoints();
            KTINFO(testlog, "Length [bins]: "<<candPoints.size());
            OTCPoints += candPoints.size();
        }



        //for(KTDiscriminatedPoints::const_iterator pointIt = candPoints.begin(); pointIt != candPoints.end(); ++pointIt )
        //{
        //    KTINFO(testlog, "Point: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fNeighborhoodAmplitude);
        //}

    }
    KTINFO(testlog, "OTC Candidates after Cut: " << otccandidates.size());
    KTINFO(testlog, "Total OTC Points: "<<OTCPoints);


    // Test ITC
    KTIterativeTrackClustering itc;

    for (std::set< Nymph::KTDataPtr >::const_iterator cIt = otccandidates.begin(); cIt != otccandidates.end(); ++cIt)
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        itc.TakeSeqLineCandidate(sqlData);
    }
    itc.Run();

    // Get output
    const std::set< Nymph::KTDataPtr >& itccandidates = itc.GetCandidates();
    KTINFO(testlog, "ITC Candidates found: " << itccandidates.size());

    // Print some output
    unsigned ITCPoints = 0;
    for (std::set< Nymph::KTDataPtr >::const_iterator cIt = itccandidates.begin(); cIt != itccandidates.end(); ++cIt)
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        KTINFO(testlog, "ITCCandidate " << sqlData.GetCandidateID());
        KTINFO(testlog, "Properties (starttime/frequnecy - endtime/frequency - slope) "<<sqlData.GetStartTimeInRunC()<<" / "<<sqlData.GetStartFrequency()<<" - "<<sqlData.GetEndTimeInRunC()<<" / "<<sqlData.GetEndFrequency()<<" - "<<sqlData.GetSlope());
        //KTINFO(testlog, "Length (end time - start time [bins]): "<<(sqlData.GetEndTimeInRunC() - sqlData.GetStartTimeInRunC()) / timeBinWidth);
        //KTINFO(testlog, "AcquisitionsID - CandidateID: "<<sqlData.GetAcquisitionID()<<" - "<<sqlData.GetCandidateID());

        const KTDiscriminatedPoints& candPoints = sqlData.GetPoints();
        KTINFO(testlog, "Length [bins]: "<<candPoints.size());
        ITCPoints += candPoints.size();

        for(KTDiscriminatedPoints::const_iterator pointIt = candPoints.begin(); pointIt != candPoints.end(); ++pointIt )
        {
           KTINFO(testlog, "Point: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fNeighborhoodAmplitude);
        }
    }
    KTINFO(testlog, "Total ITC Points: "<<OTCPoints);


    return 0;
}
