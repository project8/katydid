/*
 * ProfileManualAnalysis.cc
 *
 *  Created on: Feb 4, 2013
 *      Author: nsoblath
 */

#include "KTEgg.hh"
#include "KTEggReaderMonarch.hh"
#include "KTComplexFFTW.hh"
#include "KTGainVariationProcessor.hh"
#include "KTGainNormalization.hh"
#include "KTCorrelator.hh"
#include "KTSpectrumDiscriminator.hh"
#include "KTDistanceClustering.hh"
#include "KTFrequencyCandidateIdentifier.hh"
#include "KTROOTTreeWriter.hh"
#include "KTROOTTreeTypeWriterCandidates.hh"
#include "KTThroughputProfiler.hh"

#include "KTTimeSeriesData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTCluster1DData.hh"
#include "KTFrequencyCandidateData.hh"

#include "KTLogger.hh"

#include <boost/shared_ptr.hpp>


using namespace Katydid;
using namespace std;

using std::string;
//using boost::shared_ptr;

KTLOGGER(proflog, "katydid.applications.profiling");

int main()
{
    Double_t minAnalysisFreq = 6.e6;
    Double_t maxAnalysisFreq = 95.e6;

    //***********************************
    // Create and configure processors
    //***********************************

    string filename("/Users/nsoblath/My_Documents/Project_8/DataAnalysis/data/mc_file_20s_p1e-15_1hz.egg");
    UInt_t nSlices = 50;
    UInt_t recordSize = 32768;
    KTEggReaderMonarch::TimeSeriesType tsType = KTEggReaderMonarch::kFFTWTimeSeries;

    KTComplexFFTW compFFT;
    compFFT.SetTransformFlag("ESTIMATE");

    KTGainVariationProcessor gainVar;
    gainVar.SetMinFrequency(minAnalysisFreq);
    gainVar.SetMaxFrequency(maxAnalysisFreq);
    gainVar.SetNFitPoints(10);

    KTGainNormalization gainNorm;
    gainNorm.SetMinFrequency(minAnalysisFreq);
    gainNorm.SetMaxFrequency(maxAnalysisFreq);

    KTCorrelator corr;
    corr.AddPair(KTCorrelationPair(0, 1));

    KTSpectrumDiscriminator spectDisc;
    spectDisc.SetMinFrequency(minAnalysisFreq);
    spectDisc.SetMaxFrequency(maxAnalysisFreq);
    spectDisc.SetSNRThreshold(20.);

    KTDistanceClustering distClust;
    distClust.SetMaxFrequencyDistance(2000.);

    KTFrequencyCandidateIdentifier candIdent;

    KTROOTTreeWriter treeWriter;
    treeWriter.SetFilename("candidates_manual.root");
    treeWriter.SetFileFlag("recreate");
    KTROOTTreeTypeWriterCandidates* typeWriter = treeWriter.GetTypeWriter< KTROOTTreeTypeWriterCandidates >();

    KTThroughputProfiler prof;

    //******************************
    // Do the pre-processing work
    //******************************

    // Prepare the egg reader
    KTEggReaderMonarch* eggReader = new KTEggReaderMonarch();
    eggReader->SetTimeSeriesSizeRequest(recordSize);
    eggReader->SetTimeSeriesType(tsType);

    // Prepare and break the egg
    KTEgg egg;
    egg.SetReader(eggReader);

    if (! egg.BreakEgg(filename))
    {
        KTERROR(proflog, "Egg did not break");
        return -1;
    }

    // Configure the FFT with the egg header
    compFFT.ProcessHeader(egg.GetHeader());

    // Start the profiler
    prof.Start();


    //**************************
    // Do the processing work
    //**************************

    UInt_t iSlice = 0;
    while (kTRUE)
    {
        if (iSlice >= nSlices) break;

        KTINFO(proflog, "Slice " << iSlice);

        // Hatch the bundle
        boost::shared_ptr<KTData> data = egg.HatchNextSlice();
        if (data.get() == NULL) break;

        if (iSlice == nSlices - 1) data->fLastData = true;

        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(proflog, "No time-series data is present");
            continue;
        }
        KTTimeSeriesData& tsData = data->Of< KTTimeSeriesData >();

        // Mark the time of this slice
        prof.ProcessData(data);

        // Calcualte the FFT
        if (! compFFT.TransformData(tsData))
        {
            KTERROR(proflog, "A problem occurred while performing the FFT");
            continue;
        }
        KTFrequencySpectrumDataFFTW& fsData = data->Of< KTFrequencySpectrumDataFFTW >();

        // Calculate the gain variation
        if (! gainVar.CalculateGainVariation(fsData))
        {
            KTERROR(proflog, "A problem occurred while calculating the gain variation");
            continue;
        }
        KTGainVariationData& gainVarData = data->Of< KTGainVariationData >();

        // Normalize the spectra
        if (! gainNorm.Normalize(fsData, gainVarData))
        {
            KTERROR(proflog, "A problem occurred while normalizing the spectra");
            continue;
        }
        KTNormalizedFSDataFFTW& normFSData = data->Of< KTNormalizedFSDataFFTW >();

        // Correlate the two channels
        if (! corr.Correlate(normFSData))
        {
            KTERROR(proflog, "A problem occurred while correlating");
            continue;
        }
        KTCorrelationData& corrData = data->Of< KTCorrelationData >();

        // Pick out peaks
        if (! spectDisc.Discriminate(corrData))
        {
            KTERROR(proflog, "A problem occurred while discriminating peaks");
            continue;
        }
        KTDiscriminatedPoints1DData& discPointsData = data->Of< KTDiscriminatedPoints1DData >();

        // Find clusters in the peak bins
        if (! distClust.FindClusters(discPointsData))
        {
            KTERROR(proflog, "A problem occurred while finding clusters");
            continue;
        }
        KTCluster1DData& clusterData = data->Of< KTCluster1DData >();

        // Identify the clusters as candidates
        if (! candIdent.IdentifyCandidates(clusterData, corrData))
        {
            KTERROR(proflog, "A problem occurred while identifying candidates");
            continue;
        }
        KTFrequencyCandidateData& freqCandData = data->Of< KTFrequencyCandidateData >();

        // Write out the candidates
        typeWriter->WriteFrequencyCandidates(data);

        iSlice++;
    }

    //*******************************
    // Do the post-processing work
    //*******************************

    // Stop the profiler
    prof.Stop();

    return 0;
}
