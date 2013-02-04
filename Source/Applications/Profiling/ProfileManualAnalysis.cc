/*
 * ProfileManualAnalysis.cc
 *
 *  Created on: Feb 4, 2013
 *      Author: nsoblath
 */

#include "KTEggProcessor.hh"
#include "KTComplexFFTW.hh"
#include "KTGainVariationProcessor.hh"
#include "KTGainNormalization.hh"
#include "KTCorrelator.hh"
#include "KTSpectrumDiscriminator.hh"
#include "KTDistanceClustering.hh"
#include "KTFrequencyCandidateIdentifier.hh"
#include "KTROOTTreeWriter.hh"
#include "KTThroughputProfiler.hh"


using namespace Katydid;
using namespace std;

int main()
{
    Double_t minAnalysisFreq = 6.e6;
    Double_t maxAnalysisFreq = 95.e6;

    KTEggProcessor eggProc;
    eggProc.SetFilename("../data/mc_file_20s_p1e-15_1hz.egg");
    eggProc.SetNEvents(50);
    eggProc.SetRecordSizeRequest(32768);
    eggProc.SetTimeSeriesType(KTEggProcessor::kFFTWTimeSeries);

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

    KTDistanceClustering distClust;
    distClust.SetMaxFrequencyDistance(2000.);

    KTFrequencyCandidateIdentifier candIdent;

    KTROOTTreeWriter treeWriter;
    treeWriter.SetFilename("candidates_manual.root");
    treeWriter.SetFileFlag("recreate");

    KTThroughputProfiler prof;










    return 0;
}
