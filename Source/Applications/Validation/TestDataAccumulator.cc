/*
 * TestDataAccumulator.cc
 *
 *  Created on: Oct 22, 2013
 *      Author: nsoblath
 */

#include "KTDataAccumulator.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesReal.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"
#endif

using namespace Katydid;
using namespace std;

KTLOGGER(vallog, "katydid.applications.validation");

int main()
{
    const UInt_t nToAccumulate = 10;
    const UInt_t multiplier = 2;
    UInt_t nBins = (nToAccumulate + 1) * multiplier;

    KTDataAccumulator accumulator;
    accumulator.SetAccumulatorSize(nToAccumulate);

    for (UInt_t iAcc = 0; iAcc < nToAccumulate; ++iAcc)
    {
        KTINFO(vallog, "Iteration: " << iAcc);

        KTTimeSeriesData newData;
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(nBins, 0., nBins + 1);
        (*newTS)(iAcc * multiplier) = 1.;
        newData.SetTimeSeries(newTS, 0);

        accumulator.AddData(newData);
    }

    const KTDataAccumulator::Accumulator tsAcc = accumulator.GetAccumulator< KTTimeSeriesData >();
    KTINFO(vallog, "The TS accumulator has added " << tsAcc.fCount << " time series");

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
