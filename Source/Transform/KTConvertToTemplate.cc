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

        auto energy = sqrt((fData.GetData()*conj(fData.GetData())).colwise().sum());

        auto normalized = fData.GetData()*sqrt(2)/(energy*fNoiseStd);

        newData.GetData() = normalized.transpose();

        return true;
    }

} /* namespace Katydid */
