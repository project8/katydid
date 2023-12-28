/*
 * TestDataAccumulator.cc
 *
 *  Created on: Oct 22, 2013
 *      Author: nsoblath
 */

#include "KTDataAccumulator.hh"
#include "logger.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesReal.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"
#endif

using namespace Katydid;
using namespace std;

LOGGER(vallog, "TestDataAccumulator");

int main()
{
    const unsigned nToAccumulate = 10;
    const unsigned multiplier = 2;
    unsigned nBins = (nToAccumulate + 1) * multiplier;

    KTDataAccumulator accumulator;
    accumulator.SetAccumulatorSize(nToAccumulate);

    for (unsigned iAcc = 0; iAcc < nToAccumulate; ++iAcc)
    {
        LINFO(vallog, "Iteration: " << iAcc);

        KTTimeSeriesData newData;
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(0., nBins, 0., nBins + 1);
        (*newTS)(iAcc * multiplier) = 1.;
        newData.SetTimeSeries(newTS, 0);

        accumulator.AddData(newData);
    }

    for (unsigned iAcc = 0; iAcc < nToAccumulate; ++iAcc)
    {
        LINFO(vallog, "Iteration: " << nToAccumulate + iAcc);

        KTTimeSeriesData newData;
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(0., nBins, 0., nBins + 1);
        (*newTS)(iAcc * multiplier + 1) = 1.;
        newData.SetTimeSeries(newTS, 0);

        accumulator.AddData(newData);
    }

    const KTDataAccumulator::Accumulator& tsAcc = accumulator.GetAccumulator< KTTimeSeriesData >();
    LINFO(vallog, "The TS accumulator has added " << tsAcc.GetSliceNumber() << " time series");

#ifdef ROOT_FOUND
    TFile* output = new TFile("test_data_accumulator.root", "recreate");
    TH1D* tsHist = tsAcc.fData->Of< KTTimeSeriesData >().GetTimeSeries(0)->CreateHistogram("dataAcc_TS");
    tsHist->SetDirectory(output);
    tsHist->Write();
    output->Close();
    delete output;
#endif

    return 0;
}
