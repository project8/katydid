/*
 * KTConvertToTemplate.cc
 *
 *  Created on: May 4, 2021
 *      Author: F. Thomas
 */

#include "KTConvertToTemplate.hh"

#include "KTLogger.hh"
#include "KTAggregatedTemplateMatrixData.hh"
#include "KTAggregatedTSMatrixData.hh"
#include "KTMath.hh"

#include "param.hh"


namespace Katydid
{
    KTLOGGER(ctemplatelog, "KTConvertToTemplate");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTConvertToTemplate, "template-converter");

    KTConvertToTemplate::KTConvertToTemplate(const std::string& name) :
            KTProcessor(name),
            fNoiseTemperature(0.),
            fTSSignal("template-matrix", this),
            fTSSlot("ts-matrix", this, &KTConvertToTemplate::Convert, &fTSSignal)
    {
    }

    void KTConvertToTemplate::CalcNoiseStd()
    {
    	double kb = 1.380649e-23;
    	double R = 50.0;
    	// Johnson-Nyquist noise
    	fNoiseStd = sqrt(4*kb*R*fNoiseTemperature*fBandwidth);
    	KTDEBUG(ctemplatelog, "Noise standard deviation is: " << fNoiseStd);
    }

    bool KTConvertToTemplate::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetNoiseTemperature(node->get_value("T", GetNoiseTemperature()));
        SetBandwidth(node->get_value("bandwidth", GetBandwidth()));

        CalcNoiseStd();

        return true;
    }

    bool KTConvertToTemplate::Convert(KTAggregatedTSMatrixData& fData)
    {

        KTAggregatedTemplateMatrixData& newData = fData.Of< KTAggregatedTemplateMatrixData >();

        KTDEBUG(ctemplatelog, "Calculating energy");
        auto energy = fData.GetData().abs2().colwise().sum();

        auto normalization = sqrt(2)/(sqrt(energy)*fNoiseStd);

        KTDEBUG(ctemplatelog, "Calculating normalized matrix");
        auto normalized = fData.GetData().rowwise()*normalization;

        KTDEBUG(ctemplatelog, "Store transposed matrix in newData");
        // Eigen does not calculate anything before this assignment
        newData.GetData() = normalized.transpose();

        KTDEBUG(ctemplatelog, "Template matrix has shape ("
        						<< newData.GetData().rows()
        						<< "," << newData.GetData().cols() << ")" );

        return true;
    }

} /* namespace Katydid */
