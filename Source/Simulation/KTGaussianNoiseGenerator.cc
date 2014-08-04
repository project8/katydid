/*
 * KTGaussianNoiseGenerator.cc
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#include "KTGaussianNoiseGenerator.hh"

#include "KTParam.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "KTGaussianNoiseGenerator");

    KT_REGISTER_PROCESSOR(KTGaussianNoiseGenerator, "gaussian-noise-generator");

    KTGaussianNoiseGenerator::KTGaussianNoiseGenerator(const string& name) :
            KTTSGenerator(name),
            fRNG()
    {
    }

    KTGaussianNoiseGenerator::~KTGaussianNoiseGenerator()
    {
    }

    bool KTGaussianNoiseGenerator::ConfigureDerivedGenerator(const KTParamNode* node)
    {
        if (node == NULL) return false;

        typedef KTRNGGaussian<>::input_type input_type;
        input_type mean = node->GetValue< input_type >("mean", fRNG.mean());
        input_type sigma = node->GetValue< input_type >("sigma", fRNG.sigma());
        fRNG.param(KTRNGGaussian<>::param_type(mean, sigma));

        return true;
    }

    bool KTGaussianNoiseGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        //const double binWidth = data.GetTimeSeries(0)->GetTimeBinWidth();
        const unsigned sliceSize = data.GetTimeSeries(0)->GetNTimeBins();

        unsigned nComponents = data.GetNComponents();

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeries* timeSeries = data.GetTimeSeries(iComponent);

            if (timeSeries == NULL)
            {
                KTERROR(genlog, "Time series " << iComponent << " was not present");
                continue;
            }

            //double binCenter = 0.5 * binWidth;
            for (unsigned iBin = 0; iBin < sliceSize; iBin++)
            {
                timeSeries->SetValue(iBin, fRNG() + timeSeries->GetValue(iBin));
                //binCenter += binWidth;
                //KTDEBUG(genlog, iBin << "  " << timeSeries->GetValue(iBin));
            }
        }

        return true;
    }


} /* namespace Katydid */
