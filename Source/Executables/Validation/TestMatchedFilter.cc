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
#include "KTInnerProduct.hh"
#include "KTInnerProductOptimizer.hh"

#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include "KTAggregatedTSMatrixData.hh"
#include "KTAggregatedTemplateMatrixData.hh"
#include "KTInnerProductData.hh"
#include "KTInnerProductOptimizerData.hh"

#include "KTLogger.hh"

using namespace std;
using namespace Katydid;

KTLOGGER(testlog, "TestMatrixAggregator");

int main()
{
    // Create and setup processors
    KTMatrixAggregator tAgg {};
    KTConvertToTemplate tConvert {};
    KTInnerProduct tInnerProd {};
    KTInnerProductOptimizer tOpt {};
    
    unsigned maxCols = 5;
    unsigned nCols = 19;
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

    tInnerProd.SetTemplates(templateMatrix);
    tInnerProd.Multiply(dataMatrix);

    KTInnerProductData& product = data->Of< KTInnerProductData >();
    KTDEBUG(testlog, "Product matrix: " << product);

    Eigen::ArrayXd norm = dataMatrix.GetData().matrix().colwise().norm();

    tOpt.FindOptimum(product);

    KTInnerProductOptimizerData& res = data->Of< KTInnerProductOptimizerData >();

    KTDEBUG(testlog, "Arg: " << res.fMaxInds);
    KTDEBUG(testlog, "Vals: " << res.fMaxVals);
    KTDEBUG(testlog, "Norm: " << norm);

    //checking results
    Eigen::ArrayXi indices = Eigen::ArrayXi::LinSpaced(dataMatrix.cols(), 0, dataMatrix.cols()-1);

    if(indices.matrix() == res.fMaxInds.matrix().cast<int>() &&
    		norm.isApprox(res.fMaxVals, 1e-7))
    {
    	KTPROG(testlog, "Result matches!");
    } else {
    	KTPROG(testlog, "Result does not match!");
    	return 1;
    }

    KTPROG(testlog, "Test passed!");

    return 0;
}
