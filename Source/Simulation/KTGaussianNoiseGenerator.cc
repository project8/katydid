/*
 * KTGaussianNoiseGenerator.cc
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#include "KTGaussianNoiseGenerator.hh"

#include "param.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesFFTW.hh"

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

    bool KTGaussianNoiseGenerator::ConfigureDerivedGenerator(const scarab::param_node* node) //I changed this to scarab::param somewhere else
    {
        if (node == NULL) return false;

        typedef KTRNGGaussian<>::input_type input_type;
        input_type mean = node->get_value< input_type >("mean", fRNG.mean());
        input_type sigma = node->get_value< input_type >("sigma", fRNG.sigma());
        fRNG.param(KTRNGGaussian<>::param_type(mean, sigma));
        // fRNG.SetSeed(1) we still need to add a set seed function here but this one does not work

        return true;
    }

    bool KTGaussianNoiseGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const double binWidth = data.GetTimeSeries(0)->GetTimeBinWidth();
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

            double binCenter = 0.5 * binWidth;
            if (auto* tsFFTW = dynamic_cast<KTTimeSeriesFFTW*>(timeSeries))   // Handling complex FFTW time series correctly
            {
                for (unsigned iBin = 0; iBin < sliceSize; ++iBin)
                {
                    tsFFTW->SetRect(iBin, tsFFTW->GetReal(iBin) + fRNG(), tsFFTW->GetImag(iBin) + fRNG());  // Complex white-Gaussian noise
                }
            }
            else
            {
                for (unsigned iBin = 0; iBin < sliceSize; ++iBin)
                {
                    timeSeries->SetValue(iBin, timeSeries->GetValue(iBin) + fRNG());
                }
            }
            }

        return true;
    }


} /* namespace Katydid */
