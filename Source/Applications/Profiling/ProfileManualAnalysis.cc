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

#include "KTEvent.hh"
#include "KTTimeSeriesData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTGainVariationData.hh"
#include "KTCorrelationData.hh"
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
    UInt_t nEvents = 50;
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
    KTEggReaderMonarch eggReader;
    eggReader.SetTimeSeriesSizeRequest(recordSize);
    eggReader.SetTimeSeriesType(tsType);

    // Prepare and break the egg
    KTEgg egg;
    egg.SetReader(&eggReader);

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

    UInt_t iEvent = 0;
    while (kTRUE)
    {
        if (iEvent >= nEvents) break;

        KTINFO(proflog, "Event " << iEvent);

        // Hatch the event
        boost::shared_ptr<KTEvent> event = egg.HatchNextEvent();
        if (event.get() == NULL) break;

        if (iEvent == nEvents - 1) event->SetIsLastEvent(true);

        KTTimeSeriesData* tsData = event->GetData< KTTimeSeriesData >("time-series");
        if (tsData != NULL)
        {
            KTDEBUG(proflog, "Time series data is present.");
            //fDataSignal(newData);
        }
        else
        {
            KTWARN(proflog, "No time-series data present in event");
        }

        // Pass the event to any subscribers
        //fEventSignal(event);

        prof.ProcessEvent(event);

        KTFrequencySpectrumDataFFTW* fsData = compFFT.TransformData(tsData);

        KTGainVariationData* gainVarData = gainVar.CalculateGainVariation(fsData);

        KTFrequencySpectrumDataFFTW* normFSData = gainNorm.Normalize(fsData, gainVarData);

        KTCorrelationData* corrData = corr.Correlate(normFSData);

        KTDiscriminatedPoints1DData* discPointsData = spectDisc.Discriminate(corrData);

        KTCluster1DData* clusterData = distClust.FindClusters(discPointsData);

        KTFrequencyCandidateData* freqCandData = candIdent.IdentifyCandidates(clusterData, corrData);

        typeWriter->WriteFrequencyCandidates(freqCandData);

        delete fsData;
        delete gainVarData;
        delete normFSData;
        delete corrData;
        delete discPointsData;
        delete clusterData;
        delete freqCandData;

        iEvent++;
    }

    //*******************************
    // Do the post-processing work
    //*******************************

    // Stop the profiler
    prof.Stop();

    return 0;
}
