/*
 * KTGaussianNoiseGenerator.cc
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#include "KTGaussianNoiseGenerator.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "katydid.simulation");

    static KTDerivedNORegistrar< KTProcessor, KTGaussianNoiseGenerator > sGaussNoiseGenRegistrar("gaussian-noise-generator");

    KTGaussianNoiseGenerator::KTGaussianNoiseGenerator(const string& name) :
            KTTSGenerator(name),
            fRNG()
    {
    }

    KTGaussianNoiseGenerator::~KTGaussianNoiseGenerator()
    {
    }

    Bool_t KTGaussianNoiseGenerator::ConfigureDerivedGenerator(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        typedef KTRNGGaussian<>::input_type input_type;
        input_type mean = node->GetData< input_type >("mean", fRNG.mean());
        input_type sigma = node->GetData< input_type >("sigma", fRNG.sigma());
        fRNG.param(KTRNGGaussian<>::param_type(mean, sigma));

        return true;
    }

    Bool_t KTGaussianNoiseGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        //const Double_t binWidth = data.GetTimeSeries(0)->GetTimeBinWidth();
        const UInt_t sliceSize = data.GetTimeSeries(0)->GetNTimeBins();

        UInt_t nComponents = data.GetNComponents();

        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            KTTimeSeries* timeSeries = data.GetTimeSeries(iComponent);

            if (timeSeries == NULL)
            {
                KTERROR(genlog, "Time series " << iComponent << " was not present");
                continue;
            }

            //Double_t binCenter = 0.5 * binWidth;
            for (UInt_t iBin = 0; iBin < sliceSize; iBin++)
            {
                timeSeries->SetValue(iBin, fRNG() + timeSeries->GetValue(iBin));
                //binCenter += binWidth;
                //KTDEBUG(genlog, iBin << "  " << timeSeries->GetValue(iBin));
            }
        }

        return true;
    }


} /* namespace Katydid */
