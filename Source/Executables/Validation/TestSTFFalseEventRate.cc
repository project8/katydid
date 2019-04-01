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

#include <random>

using namespace Katydid;

KTLOGGER(testlog, "TestSTFFalseEventRate");

int main()
{
    // Simulation parameters
    double runTime = 10.0;
    int sliceSize = 4096;
    double acquisitionRate = 100.0e6;
    double SNRthreshold = 6.0;
    int progInterval = 100000;

    int nSlices = runTime * acquisitionRate / (double)sliceSize;
    double timeBinWidth = (double)sliceSize / (double)acquisitionRate;
    double freqBinWidth = acquisitionRate / (double)sliceSize;

    KTPROG( testlog, "Running false event simulation for " << runTime << " seconds" );
    KTPROG( testlog, "Total slices: " << nSlices );

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
    KTRNGEngine* engine = KTGlobalRNGEngine::get_instance();
    std::random_device rd;
    engine->SetSeed( rd() );

    KTRNGUniform01<> uniformRandom;
    KTRNGPoisson<> poissonRandom;

    // Create fake data for every slice and run stf
    for( unsigned iSlice = 0; iSlice < nSlices; ++iSlice )
    {
        if( iSlice % progInterval == 0 )
        {
            KTPROG( testlog, "Processing slice: " << iSlice << " / " << nSlices );
        }
        // Slice header
        KTSliceHeader header;
        header.SetBinWidth( timeBinWidth );
        header.SetTimeInAcq( timeBinWidth * (double)iSlice );
        header.SetTimeInRun( timeBinWidth * (double)iSlice);
        header.SetSampleRate( acquisitionRate );
        header.SetRawSliceSize( sliceSize );
        header.SetAcquisitionID( 5 );

        // Points
        KTDiscriminatedPoints1DData disc1d;
        int nPoints = poissonRandom( exp( -1.0 * SNRthreshold ) * (double)sliceSize );
        for( unsigned iPoint = 0; iPoint < nPoints; ++iPoint )
        {
            double power = -1.0 * log( uniformRandom() ) + SNRthreshold;
            double iBin = uniformRandom() * (double)sliceSize;

            disc1d.AddPoint( (int)iBin, KTDiscriminatedPoints1DData::Point( freqBinWidth * (iBin + 0.5), power, SNRthreshold, 1.0, 1.0, 1.0 ), 0 );
        }

        // Send points to STF
        stf.CollectDiscrimPointsFromSlice( header, disc1d );
    }

    // Finish STF
    stf.AcquisitionIsOver();

    KTPROG( testlog, "Finished!" );

    // Get STF output
    const std::set< Nymph::KTDataPtr >& candidates = stf.GetCandidates();
    KTPROG( testlog, "Candidates found: " << candidates.size() );

    // Run OTC
    for( std::set< Nymph::KTDataPtr >::const_iterator cIt = candidates.begin(); cIt != candidates.end(); ++cIt )
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        otc.TakeSeqLineCandidate( sqlData );
    }
    otc.Run();

    // Get OTC output
    std::set< Nymph::KTDataPtr > otccandidates = otc.GetCandidates();
    KTPROG( testlog, "OTC Candidates found: " << otccandidates.size() );

    // Run ITC
    for( std::set< Nymph::KTDataPtr >::const_iterator cIt = otccandidates.begin(); cIt != otccandidates.end(); ++cIt )
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();
        itc.TakeSeqLineCandidate( sqlData );
    }
    itc.Run();

    // Get ITC output
    std::set< Nymph::KTDataPtr > itccandidates = itc.GetCandidates();
    KTPROG( testlog, "ITC Candidates found: " << itccandidates.size() );

    // Apply cuts
    std::set< Nymph::KTDataPtr >::const_iterator cIt = itccandidates.begin();
    while( cIt != itccandidates.end() )
    {
        KTSequentialLineData& sqlData = (*cIt)->Of< KTSequentialLineData >();

        Nymph::KTData data = (*cIt)->Of< Nymph::KTData >();
        nupcut.Apply( data, sqlData );
        densitycut.Apply( data, sqlData );

        if( data.GetCutStatus().IsCut() )
        {
            cIt = itccandidates.erase( cIt );
        }
        else
        {
            ++cIt;
        }
    }

    KTPROG( testlog, "ITC Candidates after Cut: " << itccandidates.size() );

    if( itccandidates.size() < 1 )
    {
        return 0;
    }

    // Write file
    
#ifdef ROOT_FOUND
    KTROOTTreeWriter writer;
    writer.SetFilename( "TestSequentialLineData_output.root" );
    writer.SetFileFlag( "recreate" );

    KTROOTTreeTypeWriterEventAnalysis treeTypeWriter;
    treeTypeWriter.SetWriter( &writer );
    treeTypeWriter.WriteSequentialLine( *itccandidates.begin() );
#endif

    return 0;
}
