/*
 * KTCavityNoiseGenerator.cc
 *
 *  Created on: May 28, 2025
 *      Author: ehtkarim
 */

#include "KTCavityNoiseGenerator.hh"

#include "param.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTReverseFFTW.hh"

#include <cmath>
#include <memory>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "KTCavityNoiseGenerator");

    KT_REGISTER_PROCESSOR(KTCavityNoiseGenerator, "cavity-noise-generator");

    KTCavityNoiseGenerator::KTCavityNoiseGenerator(const string& name) :
            KTGaussianNoiseGenerator(name),
            fF0(25.904e9),
            fQL(625.0),
            fQ0(1.e4),
            fA(0.90),
            fTLineStart(80.0),
            fTLineEnd(5.2),
            fTCav(80.0),
            fTIsol(5.2),
            fEpsilon(0.5),
            fFLo(25.9702e9),
            fTransformFlag("ESTIMATE"),
            fNoiseScaling(1.0)
    {
    }

    KTCavityNoiseGenerator::~KTCavityNoiseGenerator()
    {
    }

    bool KTCavityNoiseGenerator::ConfigureDerivedGenerator(const scarab::param_node* node)
    {
        if (node == NULL) return false;
        if (! KTGaussianNoiseGenerator::ConfigureDerivedGenerator(node)) return false;

        fRNG.param(KTRNGGaussian<>::param_type(0.0, 1.0));  // Cavity noise should have fRNG() with default (mean, sigma), not inherited from KTGaussianNoiseGenerator

        if (node->has("cavity"))
        {
            const scarab::param_node& m = (*node)["cavity"].as_node();
            fF0         = m.get_value("f0", fF0);
            fQL = m.get_value("Q_L", fQL);
            fQ0 = m.get_value("Q0",  fQ0);
            fA          = m.get_value("A", fA);
            fTLineStart = m.get_value("T_line_start", fTLineStart);
            fTLineEnd   = m.get_value("T_line_end", fTLineEnd);
            fTCav       = m.get_value("T_cav", fTCav);
            fTIsol      = m.get_value("T_isol", fTIsol);
            fEpsilon    = m.get_value("epsilon", fEpsilon);
            fFLo        = m.get_value("f_lo", fFLo);
        }

        fNoiseScaling = node->get_value<double>("noise-scaling", fNoiseScaling);
        if (fNoiseScaling <= 0.0)
        {
            KTWARN(genlog, "\"noise-scaling\" must be > 0; using 1.0");
            fNoiseScaling = 1.0;
        }

        fTransformFlag = node->get_value("transform-flag", fTransformFlag);

        return true;
    }

    bool KTCavityNoiseGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const double binWidth   = data.GetTimeSeries(0)->GetTimeBinWidth();
        const unsigned sliceSize = data.GetTimeSeries(0)->GetNTimeBins();
        const unsigned nComponents = data.GetNComponents();

        const double fs = 1.0 / binWidth;
        const double df = fs / sliceSize;
        const unsigned N2 = sliceSize / 2;

        bool isComplex = dynamic_cast< KTTimeSeriesFFTW* >(data.GetTimeSeries(0)) != NULL;

        KTFrequencySpectrumFFTW spec(sliceSize, -fs*0.5, fs*0.5, false);
        spec.SetNTimeBins(sliceSize);

        if (isComplex)
        {
            for (unsigned k = 0; k < sliceSize; ++k)
            {
                double fIf = (k <= N2) ? k * df : (static_cast<int>(k) - static_cast<int>(sliceSize)) * df;
                double fRf = fIf + fFLo;     // Down-converted
                double pBin = NoisePSD(fRf) * df;    // PSD -> power in one FFT bin
                double amp  = fNoiseScaling*fGain*std::sqrt(fResistance)*std::pow(sliceSize, 1.5)*std::sqrt(pBin / 2.0);  // N^{3/2}*sqrt(P_bin/2) - N^{3/2} since ReverseFFTW does a sqrt(N) normalization

                spec.SetRect(k, amp * fRNG(), amp * fRNG());
            }
        }
        else
        {
            for (unsigned k = 0; k <= N2; ++k)
            {
                double fIf = k * df;
                double fRf = fIf + fFLo;     // Down-converted
                double pBin = NoisePSD(fRf) * df;    // PSD -> power in one FFT bin
                double amp  = fNoiseScaling*fGain*std::sqrt(fResistance)*std::pow(sliceSize, 1.5)*std::sqrt(pBin);  // N^{3/2}*sqrt(P_bin) - for real signal bins P_bin/2 -> P_bin

                double re = amp * fRNG();
                double im = (k==0 || (sliceSize%2==0 && k==N2)) ? 0.0 : amp * fRNG(); // Set imag component 0 for the DC bin (k = 0) and for the Nyquist bin (k = N/2) (even); otherwise amp * fRNG()

                spec.SetRect(k, re, im);
                if (k>0 && k<N2)
                    spec.SetRect(sliceSize - k,  re, -im);
            }
        }

        KTReverseFFTW rfft;
        rfft.SetTransformFlag(fTransformFlag);
        rfft.InitializeForComplexTDD(sliceSize);

        std::unique_ptr< KTTimeSeriesFFTW > noiseTS( rfft.TransformToComplex(&spec) );
        if (! noiseTS)
        {
            KTERROR(genlog, "Inverse FFT failed while producing cavity noise");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeries* ts = data.GetTimeSeries(iComponent);

            if (auto* tsFFTW = dynamic_cast< KTTimeSeriesFFTW* >(ts))
            {
                for (unsigned i = 0; i < sliceSize; ++i)
                    tsFFTW->SetRect(i, tsFFTW->GetReal(i) + noiseTS->GetReal(i), tsFFTW->GetImag(i) + noiseTS->GetImag(i));
            }
            else
            {
                for (unsigned i = 0; i < sliceSize; ++i)
                    ts->SetValue(i, ts->GetValue(i) + noiseTS->GetReal(i));
            }
        }

        return true;
    }

    double KTCavityNoiseGenerator::Eta(double x) const
    {
        return x/(std::exp(x)-1.0) + 0.5;
    }

    double KTCavityNoiseGenerator::NoisePSD(double f) const
    {
        const double lor = 1.0 / ( 1.0 + ( 2.0 * fQL * (f - fF0) / fF0)*( 2.0 * fQL * (f - fF0) / fF0) );    // Lorentzian

        constexpr double kB  = KTMath::BoltzmannConstant();
        constexpr double hbar= KTMath::ReducedPlanckConstant();
        const double omega = KTMath::TwoPi() * f;

        const double Tcav  = fTCav  * Eta(hbar*omega/(kB*fTCav));
        const double Tisol = fTIsol * Eta(hbar*omega/(kB*fTIsol));

        const double P_cav  = kB*Tcav  * (4.*fG/((1.+fG)*(1.+fG))) * lor;
        const double P_loss = kB*( fA*fTLineStart + (1.-fA)*fTLineEnd );
        const double P_isol = kB*Tisol * (1. - (4.*fG/((1.+fG)*(1.+fG)))*lor);
        const double P_amp  = hbar*omega / fEpsilon;

        return P_cav + P_loss + P_isol + P_amp;
    }

} /* namespace Katydid */
