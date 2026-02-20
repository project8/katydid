/*
 * KTGaussianNoiseGenerator.cc
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 *   Edited on: Jul 25, 2025
 *      Author: ehtkarim
 */

#include "KTGaussianNoiseGenerator.hh"

#include "param.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesFFTW.hh"

#include <cmath>
#include <random>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "KTGaussianNoiseGenerator");

    KT_REGISTER_PROCESSOR(KTGaussianNoiseGenerator, "gaussian-noise-generator");

    KTGaussianNoiseGenerator::KTGaussianNoiseGenerator(const string& name) :
            KTTSGenerator(name),
            fRNG(),
            fSigmaPSD(0.0),
            fGain(1.0),
            fResistance(50.0) // (Ω)
    {
    }

    KTGaussianNoiseGenerator::~KTGaussianNoiseGenerator()
    {
    }

    bool KTGaussianNoiseGenerator::ConfigureDerivedGenerator(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        typedef KTRNGGaussian<>::input_type input_type;

        if (node->has("noise-floor-psd") && node->has("noise-temperature"))
        {
            KTERROR(genlog, "Both noise-floor-psd and noise-temperature are defined. Only one can be used!");
            return false;
        }

        input_type sigma = 0.0;

        if (node->has("noise-floor-psd"))
        {
            sigma = std::sqrt(node->get_value<input_type>("noise-floor-psd"));
        }
        else if (node->has("noise-temperature"))
        {
            sigma = std::sqrt(KTMath::BoltzmannConstant() * node->get_value<input_type>("noise-temperature"));
        }
        else   // falling back to the old "sigma" parameter
        {
            sigma = node->get_value<input_type>("sigma", fRNG.sigma());
        }

        input_type mean = node->get_value<input_type>("mean", fRNG.mean());

        fSigmaPSD = sigma; // to avoid sigma growing in each time-slice
        fRNG.param(KTRNGGaussian<>::param_type(mean, sigma));

        unsigned seed;
        if (node-> has("seed"))
        {
            seed = node->get_value< unsigned >("seed");
        }
        else
        {
            std::random_device rd;
            seed = rd();
        }
        fRNG.SetSeed(seed);

        fGain = node->get_value<double>("gain", fGain);
        fResistance = node->get_value<double>("resistance", fResistance);

        return true;
    }

    bool KTGaussianNoiseGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const double binWidth        = data.GetTimeSeries(0)->GetTimeBinWidth();
        const double acquisitionRate = 1.0 / binWidth; // (Hz)
        static constexpr double kInvSqrt2 = M_SQRT1_2;   // sqrt(0.5) - sigma scaling for complex signal

        // Converting the stored PSD‑sigma (V/sqrt(Hz)) into the per‑sample sigma (V)
        double sampledSigma = fSigmaPSD * std::sqrt(acquisitionRate);

        sampledSigma *= fGain * std::sqrt(fResistance); // V = sigma x Gain x sqrt(R)
        fRNG.param(KTRNGGaussian<>::param_type(fRNG.mean(), sampledSigma)); // Updating the RNG

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
                    tsFFTW->SetRect(iBin, tsFFTW->GetReal(iBin) + fRNG() * kInvSqrt2, tsFFTW->GetImag(iBin) + fRNG() * kInvSqrt2);  // Complex white-Gaussian noise
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
