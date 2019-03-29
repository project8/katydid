/**
@file TestSTFFalseEventRate.cc
@brief Simulate STF with probabilistically generated noise
@author E. Zayas
@date Mar 28, 2019
@details
This executable tests the finding of STF candidates by generating uniform spectrogram noise
*/

#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTSequentialTrackFinder.hh"
#include "KTOverlappingTrackClustering.hh"
#include "KTIterativeTrackClustering.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTSequentialLineData.hh"
#include "CutClasses/KTSequentialLineSNRCut.hh"
#include "CutClasses/KTSequentialLineNUPCut.hh"
#include "CutClasses/KTSequentialLinePointDensityCut.hh"
#include "KTROOTTreeTypeWriterEventAnalysis.hh"
#include "KTRandom.hh"

using namespace Katydid;

KTLOGGER(testlog, "TestSTFFalseEventRate");

int main()
{
    // Simulation parameters
    int nSlices = 10;
    int sliceSize = 4096;
    double acquisitionRate = 100.0e6;
    double SNRthreshold = 8.0;

    // Processors and cuts
    KTSequentialTrackFinder stf;
    KTOverlappingTrackClustering otc;
    KTIterativeTrackClustering itc;
    KTSequentialLineSNRCut snrcut;
    KTSequentialLineNUPCut nupcut;
    KTSequentialLinePointDensityCut densitycut;

    // Processor and cut configuration
    stf.SetTrimmingThreshold(0.9);
    stf.SetApplyTotalSNRCut(true);
    stf.SetTotalSNRThreshold(8);
    stf.SetMinFrequency(0.);
    stf.SetMaxFrequency(100.e6);
    stf.SetMinPoints(2);
    stf.SetFrequencyAcceptance(60.e3);
    stf.SetTimeGapTolerance(1.e-3);
    stf.SetMinSlope(0);

    snrcut.SetMinAverageSNR(0.0);
    nupcut.SetMinTotalNUP(0.0);
    snrcut.SetTimeOrBinAverage(KTSequentialLineSNRCut::time_or_bin_average::bin);
    densitycut.SetMinDensity(0.0);

    // Random engines
    KTRNGUniform01<> uniformRandom;
    KTRNGPoisson<> poissonRandom;

    // Create fake data for every slice and run stf
    for (unsigned iSlice = 0; iSlice < nSlices; ++iSlice )
    {
        // Slice header
        KTSliceHeader header;
        header.SetBinWidth(1.0 / (double)acquisitionRate * (double)sliceSize);
        header.SetTimeInAcq(1.0 / (double)acquisitionRate * (double)sliceSize * (double)iSlice);
        header.SetTimeInRun(1.0 / (double)acquisitionRate * (double)sliceSize * (double)iSlice);
        header.SetSampleRate(100.e6);
        header.SetRawSliceSize(4096);
        header.SetAcquisitionID(5);

        // Points
        KTDiscriminatedPoints1DData disc1d;
        int nPoints = poissonRandom( exp( -1.0 * SNRthreshold ) * (double)sliceSize );
        KTPROG( testlog, nPoints );
        for( unsigned iPoint = 0; iPoint < nPoints; ++iPoint )
        {
            double power = -1.0 * log( uniformRandom() ) + SNRthreshold;
            double iBin = uniformRandom() * (double)sliceSize;

            disc1d.AddPoint( (int)iBin, KTDiscriminatedPoints1DData::Point( acquisitionRate / (double)sliceSize * (iBin + 0.5), power, SNRthreshold, 1.0, 1.0, 1.0 ), 0 );
        }

        // Run stf
        stf.CollectDiscrimPointsFromSlice( header, disc1d );
    }
    stf.AcquisitionIsOver();

    // Get STF output
    const std::set< Nymph::KTDataPtr >& candidates = stf.GetCandidates();
    KTPROG(testlog, "Candidates found: " << candidates.size());

    // Run OTC
    for (std::set< Nymph::KTDataPtr >::const_iterator cIt = candidates.begin(); cIt != candidates.end(); ++cIt)
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        otc.TakeSeqLineCandidate(sqlData);
    }
    otc.Run();

    // Get OTC output
    std::set< Nymph::KTDataPtr > otccandidates = otc.GetCandidates();
    KTPROG(testlog, "OTC Candidates found: " << otccandidates.size());

    // Run ITC
    for (std::set< Nymph::KTDataPtr >::const_iterator cIt = otccandidates.begin(); cIt != otccandidates.end(); ++cIt)
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        itc.TakeSeqLineCandidate(sqlData);
    }
    itc.Run();

    // Get ITC output
    std::set< Nymph::KTDataPtr > itccandidates = itc.GetCandidates();
    KTPROG(testlog, "ITC Candidates found: " << itccandidates.size());

    // Apply cuts
    std::set< Nymph::KTDataPtr >::const_iterator cIt = itccandidates.begin();
    while(cIt != itccandidates.end())
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();

        Nymph::KTData data = (*cIt)->Of< Nymph::KTData >();
        nupcut.Apply(data, sqlData);
        densitycut.Apply(data, sqlData);

        if (data.GetCutStatus().IsCut() == true)
        {
            cIt = itccandidates.erase(cIt);
        }
        else
        {
            ++cIt;
        }
    }

    KTPROG(testlog, "ITC Candidates after Cut: " << itccandidates.size());

    if( itccandidates.size() < 1 )
    {
        return 0;
    }

    // Write file
    
#ifdef ROOT_FOUND
    KTROOTTreeWriter writer;
    writer.SetFilename("TestSequentialLineData_output.root");
    writer.SetFileFlag("recreate");

    KTROOTTreeTypeWriterEventAnalysis treeTypeWriter;
    treeTypeWriter.SetWriter(&writer);
    treeTypeWriter.WriteSequentialLine(*itccandidates.begin());
    KTINFO(testlog, "Processed track saved in file");
#endif

    return 0;
}
