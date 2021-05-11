/*
 * KTInnerProduct.cc
 *
 *  Created on: Apr 28, 2021
 *      Author: F. Thomas
 */

#include "KTInnerProduct.hh"
#include "KTAggregatedTemplateMatrixData.hh"
#include "KTAggregatedTSMatrixData.hh"
#include "KTInnerProductData.hh"

#include "KTLogger.hh"

#include "param.hh"


namespace Katydid
{
    KTLOGGER(iprodlog, "KTInnerProduct");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTInnerProduct, "inner-product");

    KTInnerProduct::KTInnerProduct(const std::string& name) :
            KTProcessor(name),
            fSnrSignal("snr-matrix", this),
    		fDataSlot("data-matrix", this, &KTInnerProduct::Multiply, &fSnrSignal),
			fTemplateSlot("template-matrix", this, &KTInnerProduct::SetTemplates) //does not emit a signal on purpose
    {
    }


    bool KTInnerProduct::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        return true;
    }

    bool KTInnerProduct::SetTemplates(KTAggregatedTemplateMatrixData& fTemplate)
    {
    	KTDEBUG(iprodlog, "Setting Template matrix");
    	fTemplateMatrix = std::move(fTemplate.GetData());

    	return true;
    }

    bool KTInnerProduct::Multiply(KTAggregatedTSMatrixData& fData)
    {
    	KTInnerProductData& newData = fData.Of< KTInnerProductData >();

    	KTDEBUG(iprodlog, "Run inner product");
    	newData.GetData() = fTemplateMatrix.matrix() * fData.GetData().matrix();

    	return true;
    }

} /* namespace Katydid */
