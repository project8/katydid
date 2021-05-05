/*
 * TestMatchedFilter.cc
 *
 *  Created on: May 4, 2021
 *      Author: F. Thomas
 *
 *  Tests KTMatrixAggregator, KTConvertToTemplate
 *
 *  Usage: > TestMatchedFilter
 */
#include <memory>
#include <boost/make_shared.hpp>

#include "KTMatrixAggregator.hh"
#include "KTConvertToTemplate.hh"

#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include "KTAggregatedTSMatrixData.hh"
#include "KTAggregatedTemplateMatrixData.hh"

#include "KTLogger.hh"

using namespace std;
using namespace Katydid;

KTLOGGER(testlog, "TestMatrixAggregator");

int main()
{
    // Create and setup processors
    KTMatrixAggregator tAgg {};
    KTConvertToTemplate tConvert {};
    
    unsigned maxCols = 3;
    unsigned nCols = 5;
    unsigned nChannels = 3;
    unsigned nTimeBins = 4;

    double noiseStd = sqrt(2.0);

    tAgg.SetMaxCols(maxCols);

    tConvert.SetNoiseStd(noiseStd);

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
    		KTDEBUG(testlog, "Add a Channel");
    		//who will manage this memory after tsData has it?
    		//modern way would just move it
        	KTTimeSeriesFFTW *ts = new KTTimeSeriesFFTW(nTimeBins);
    		for (int k=0; k<nTimeBins; ++k)
    		{
    			ts->SetRect(k, i+j+2*k, i-j+k);
    		}
    		KTDEBUG(testlog, "TS: " << *ts);
    		tsData.SetTimeSeries(ts, j);
    	}
    	KTDEBUG(testlog, "Call the slot function");
        tAgg.SlotFunction(data);
        tAgg.PrintBuffer();
    }

    // Check Results
    KTAggregatedTSMatrixData& dataMatrix = data->Of< KTAggregatedTSMatrixData >();
    
    KTDEBUG(testlog, "Aggregated: " << dataMatrix);

    // Test conversion to template
    tConvert.Convert(dataMatrix);

    KTAggregatedTemplateMatrixData& templateMatrix = data->Of< KTAggregatedTemplateMatrixData >();

    KTDEBUG(testlog, "Template Matrix: " << templateMatrix);

    KTDEBUG(testlog, "Norm" << sqrt((dataMatrix.GetData()*conj(dataMatrix.GetData())).colwise().sum()));

    KTDEBUG(testlog, " Product: " << (templateMatrix.GetData().transpose()*conj(dataMatrix.GetData())).colwise().sum());

    return 0;
}
