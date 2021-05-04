/*
 * TestMatrixAggregator.cc
 *
 *  Created on: May 4, 2021
 *      Author: F. Thomas
 *
 *  Tests performance of KTMatrixAggregator
 *
 *  Usage: > TestMatrixAggregator
 */
#include <memory>
#include <boost/make_shared.hpp>

#include "KTMatrixAggregator.hh"

#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include "KTAggregatedTSMatrixData.hh"

#include "KTLogger.hh"

using namespace std;
using namespace Katydid;

KTLOGGER(testlog, "TestMatrixAggregator");

int main()
{
    // Create and setup processor
    KTMatrixAggregator tAgg {};
    
    unsigned maxCols = 3;
    unsigned nCols = 5;
    unsigned nChannels = 3;
    unsigned nTimeBins = 4;

    tAgg.SetMaxCols(maxCols);

    // Test processing of KTTimeSeriesData

    Nymph::KTDataPtr data = boost::make_shared<Nymph::KTData>();
    for (int i=0; i<nCols; ++i)
    {
    	if(i==nCols-1)
    	{
    		data->SetLastData(true);
    	}
        KTTimeSeriesData& tsData = data->Of< KTTimeSeriesData >();
        tsData.SetNComponents(nChannels);
    	for (int j=0; j<nChannels; ++j)
    	{
    		KTDEBUG("Add a Channel");
    		//who will manage this memory after tsData has it?
    		//modern way would just move it
        	KTTimeSeriesFFTW *ts = new KTTimeSeriesFFTW(nTimeBins);
    		for (int k=0; k<nTimeBins; ++k)
    		{
    			ts->SetRect(k, i+j+2*k, i-j+k);
    		}
    		KTDEBUG("TS: " << *ts);
    		tsData.SetTimeSeries(ts, j);
    	}
    	KTDEBUG("Call the slot function");
        tAgg.SlotFunction(data);
        tAgg.PrintBuffer();
    }

    // Check Results
    KTAggregatedTSMatrixData& dataMatrix = data->Of< KTAggregatedTSMatrixData >();
    
    KTDEBUG("Aggregated: " << dataMatrix);

    return 0;
}
