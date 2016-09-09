/*
 * ProfileManualAnalysis.cc
 *
 *  Created on: Feb 4, 2013
 *      Author: nsoblath
 */

#include "KTCorrelator.hh"
#include "KTCorrelationData.hh"
#include "KTDAC.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTDistanceClustering.hh"
#include "KTCluster1DData.hh"
#include "KTEggHeader.hh"
#include "KTEgg2Reader.hh"
#include "KTForwardFFTW.hh"
#include "KTFrequencyCandidateData.hh"
#include "KTFrequencyCandidateIdentifier.hh"
#include "KTFrequencySpectrumDataFFTW.hh"

#ifdef ROOT_FOUND
#include "KTGainVariationData.hh"
#include "KTGainVariationProcessor.hh"
#include "KTGainNormalization.hh"
#endif

#include "KTData.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTRawTimeSeriesData.hh"

#ifdef ROOT_FOUND
#include "KTROOTTreeWriter.hh"
#include "KTROOTTreeTypeWriterCandidates.hh"
#else
#include "KTJSONWriter.hh"
#include "KTJSONTypeWriterCandidates.hh"
#endif

#include "KTSliceHeader.hh"
#include "KTSpectrumDiscriminator.hh"
#include "KTThroughputProfiler.hh"
#include "KTTimeSeriesData.hh"



using namespace Katydid;
using namespace std;

using std::string;
//

KTLOGGER(proflog, "ProfileManualAnalysis");

int main()
{
    double minAnalysisFreq = 6.e6;
    double maxAnalysisFreq = 95.e6;

    //***********************************
    // Create and configure processors
    //***********************************

    string filename("/Users/nsoblath/My_Documents/Project_8/DataAnalysis/data/mc_file_20s_p1e-15_1hz.egg");
    unsigned nSlices = 50;
    unsigned recordSize = 32768;
    KTSingleChannelDAC::TimeSeriesType tsType = KTSingleChannelDAC::kFFTWTimeSeries;

    KTForwardFFTW compFFT;
    compFFT.SetTransformFlag("ESTIMATE");

#ifdef ROOT_FOUND
    KTGainVariationProcessor gainVar;
    gainVar.SetMinFrequency(minAnalysisFreq);
    gainVar.SetMaxFrequency(maxAnalysisFreq);
    gainVar.SetNFitPoints(10);

    KTGainNormalization gainNorm;
    gainNorm.SetMinFrequency(minAnalysisFreq);
    gainNorm.SetMaxFrequency(maxAnalysisFreq);
#endif

    KTCorrelator corr;
    corr.AddPair(KTCorrelator::UIntPair(0, 1));

    KTSpectrumDiscriminator spectDisc;
    spectDisc.SetMinFrequency(minAnalysisFreq);
    spectDisc.SetMaxFrequency(maxAnalysisFreq);
    spectDisc.SetSNRPowerThreshold(20.);

    KTDistanceClustering distClust;
    distClust.SetMaxFrequencyDistance(2000.);

    KTFrequencyCandidateIdentifier candIdent;

#ifdef ROOT_FOUND
    KTROOTTreeWriter treeWriter;
    treeWriter.SetFilename("candidates_manual.root");
    treeWriter.SetFileFlag("recreate");
    KTROOTTreeTypeWriterCandidates* typeWriter = treeWriter.GetTypeWriter< KTROOTTreeTypeWriterCandidates >();
#else
    KTJSONWriter jsonWriter;
    jsonWriter.SetFilename("candidates_manual.json");
    jsonWriter.SetPrettyJSONFlag(true);
    KTJSONTypeWriterCandidates* typeWriter = jsonWriter.GetTypeWriter< KTJSONTypeWriterCandidates >();
#endif

    KTThroughputProfiler prof;

    //******************************
    // Do the pre-processing work
    //******************************

    // Prepare the egg reader
    KTEgg2Reader* eggReader = new KTEgg2Reader();
    eggReader->SetSliceSize(recordSize);

    KTDAC* dac = new KTDAC();

    Nymph::KTDataPtr headerData = eggReader->BreakEgg(filename);
    if (! headerData)
    {
        KTERROR(proflog, "Egg did not break");
        delete eggReader;
        return -1;
    }
    KTEggHeader& header = headerData->Of< KTEggHeader >();

    dac->InitializeWithHeader(&header);

    // Configure the FFT with the egg header
    compFFT.InitializeWithHeader(header);

    // Start the profiler
    prof.Start();


    //**************************
    // Do the processing work
    //**************************

    unsigned iSlice = 0;
    while (true)
    {
        if (iSlice >= nSlices) break;

        KTINFO(proflog, "Slice " << iSlice);

        // Hatch the slice
        Nymph::KTDataPtr data = eggReader->HatchNextSlice();
        if (data.get() == NULL) break;

        if (iSlice == nSlices - 1) data->SetLastData(true);

        dac->ConvertData(data->Of< KTSliceHeader >(), data->Of< KTRawTimeSeriesData >());

        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(proflog, "No time-series data is present");
            continue;
        }
        KTTimeSeriesData& tsData = data->Of< KTTimeSeriesData >();

        // Mark the time of this slice
        prof.Data(data);

        // Calcualte the FFT
        if (! compFFT.TransformRealData(tsData))
        {
            KTERROR(proflog, "A problem occurred while performing the FFT");
            continue;
        }
        KTFrequencySpectrumDataFFTW& fsData = data->Of< KTFrequencySpectrumDataFFTW >();

#ifdef ROOT_FOUND
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
#endif

        // Correlate the two channels
#ifdef ROOT_FOUND
        if (! corr.Correlate(normFSData))
#else
        if (! corr.Correlate(fsData))
#endif
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
        //KTFrequencyCandidateData& freqCandData = data->Of< KTFrequencyCandidateData >();

        // Write out the candidates
        typeWriter->WriteFrequencyCandidates(data);

        iSlice++;
    }

    //*******************************
    // Do the post-processing work
    //*******************************

    // Stop the profiler
    prof.Stop();

    eggReader->CloseEgg();
    delete eggReader;
    delete dac;

    return 0;
}
