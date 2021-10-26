/*
 * KTInnerProductOptimizer.cc
 *
 *  Created on: May 5, 2021
 *      Author: F. Thomas
 */

#include "KTInnerProductOptimizer.hh"
#include "KTInnerProductOptimizerData.hh"
#include "KTInnerProductData.hh"

#include "KTLogger.hh"

#include <eigen3/Eigen/Dense>

#include "param.hh"


namespace Katydid
{
    KTLOGGER(ipolog, "KTInnerProductOptimizer");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTInnerProductOptimizer, "inner-product-optimizer");

    KTInnerProductOptimizer::KTInnerProductOptimizer(const std::string& name) :
            KTProcessor(name),
            fOptSignal("opt", this),
			fIPSlot("snr-matrix", this, &KTInnerProductOptimizer::FindOptimum, &fOptSignal)
    {
    }


    bool KTInnerProductOptimizer::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;


        return true;
    }

    bool KTInnerProductOptimizer::FindOptimum(KTInnerProductData& fData)
    {

    	KTInnerProductOptimizerData& fOpt = fData.Of<KTInnerProductOptimizerData>();

    	auto snr = fData.GetData().abs().eval();

        fOpt.fMaxInds.resize(snr.cols());
        fOpt.fMaxVals.resize(snr.cols());

        KTPROG(ipolog, "Finding best matches");
        for(int i=0;i<snr.cols();++i)
            fOpt.fMaxVals(i) = snr.col(i).maxCoeff( &fOpt.fMaxInds(i) );

        return true;
    }

} /* namespace Katydid */
