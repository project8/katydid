/*
 * KTMeasuredNoiseGenerator.cc
 *
 *  Created on: Sep 17, 2025
 *      Author: ehtkarim
 */

#include "KTMeasuredNoiseGenerator.hh"

#include "param.hh"
#include "param_yaml.hh"
#include "KTMath.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTReverseFFTW.hh"

#include <algorithm>
#include <cmath>
#include <memory>

using std::string;

namespace Katydid
{
    KTLOGGER(genlog, "KTMeasuredNoiseGenerator");

    KT_REGISTER_PROCESSOR(KTMeasuredNoiseGenerator, "measured-noise-generator");

    // ---------- KCubicSpline ----------

    KTMeasuredNoiseGenerator::KCubicSpline::KCubicSpline() :
            fX(), fA(), fB(), fC(), fD()
    {}

    KTMeasuredNoiseGenerator::KCubicSpline::~KCubicSpline() = default;

    void KTMeasuredNoiseGenerator::KCubicSpline::Clear()
    {
        fX.clear(); fA.clear(); fB.clear(); fC.clear(); fD.clear();
    }

    bool KTMeasuredNoiseGenerator::KCubicSpline::IsBuilt() const
    {
        return ! fX.empty();
    }

    bool KTMeasuredNoiseGenerator::KCubicSpline::Build(const std::vector< double >& x, const std::vector< double >& y)
    {
        Clear();

        if (x.size() < 2 || x.size() != y.size()) return false;

        for (size_t i = 1; i < x.size(); ++i)
        {
            if (x[i] <= x[i-1]) return false;
        }

        size_t n = x.size();
        fX = x;
        fA = y;
        fB.assign(n - 1, 0.0);
        fC.assign(n,     0.0);
        fD.assign(n - 1, 0.0);

        std::vector< double > h(n - 1, 0.0);
        for (size_t i = 0; i < n - 1; ++i) h[i] = fX[i+1] - fX[i];

        std::vector< double > alpha(n, 0.0);
        for (size_t i = 1; i < n - 1; ++i)
        {
            alpha[i] = 3.0/h[i]*(fA[i+1]-fA[i]) - 3.0/h[i-1]*(fA[i]-fA[i-1]);
        }

        std::vector< double > l(n, 0.0), mu(n, 0.0), z(n, 0.0);
        l[0] = 1.0;
        mu[0] = 0.0;
        z[0] = 0.0;

        for (size_t i = 1; i < n - 1; ++i)
        {
            l[i] = 2.0*(fX[i+1]-fX[i-1]) - h[i-1]*mu[i-1];
            if (l[i] == 0.0) return false;
            mu[i] = h[i]/l[i];
            z[i] = (alpha[i] - h[i-1]*z[i-1]) / l[i];
        }

        l[n-1] = 1.0;
        z[n-1] = 0.0;
        fC[n-1] = 0.0;

        for (int j = static_cast<int>(n) - 2; j >= 0; --j)
        {
            fC[j] = z[j] - mu[j]*fC[j+1];
            fB[j] = (fA[j+1]-fA[j])/h[j] - h[j]*(fC[j+1] + 2.0*fC[j])/3.0;
            fD[j] = (fC[j+1]-fC[j]) / (3.0*h[j]);
        }

        return true;
    }

    double KTMeasuredNoiseGenerator::KCubicSpline::Evaluate(double xval) const
    {
        if (fX.empty()) return 0.0;

        if (xval <= fX.front()) return fA.front();
        if (xval >= fX.back())  return fA.back();

        std::vector<double>::const_iterator it = std::upper_bound(fX.begin(), fX.end(), xval);
        size_t j = static_cast<size_t>(std::distance(fX.begin(), it) - 1);

        double dx = xval - fX[j];
        return fA[j] + fB[j]*dx + fC[j]*dx*dx + fD[j]*dx*dx*dx;
    }

    // ---------- KTMeasuredNoiseGenerator ----------

    KTMeasuredNoiseGenerator::KTMeasuredNoiseGenerator(const string& name) :
            KTGaussianNoiseGenerator(name),
            fTransformFlag("ESTIMATE"),
            fNoiseScaling(1.0),
            fUseVariance(true),
            fFixedRadius(true),
            fFreqScale(1.0e6),
            fFreqMinHz(0.0),
            fFreqMaxHz(400.0e6),
            fMeanXHz(),
            fMeanY(),
            fVarY(),
            fMeanSpline(),
            fVarSpline(),
            fHaveMean(false),
            fHaveVar(false)
    {}

    KTMeasuredNoiseGenerator::~KTMeasuredNoiseGenerator() = default;

    bool KTMeasuredNoiseGenerator::LoadPointsFromArray(const scarab::param_array& arr, std::vector< double >& xs, std::vector< double >& ys, double freqScale) const
    {
        xs.clear();
        ys.clear();

        for (unsigned i = 0; i < arr.size(); ++i)
        {
            double f_in = 0.0;
            double v    = 0.0;

            if (arr[i].is_array())
            {
                const scarab::param_array& pair = arr[i].as_array();
                if (pair.size() < 2 || ! pair[0].is_value() || ! pair[1].is_value())
                {
                    KTERROR(genlog, "Each point must be a two-element array [frequency, value]");
                    return false;
                }
                f_in = pair[0].as_value().as_double();
                v    = pair[1].as_value().as_double();
            }
            else if (arr[i].is_node())
            {
                const scarab::param_node& pn = arr[i].as_node();
                f_in = pn.get_value("f", 0.0);
                if (pn.has("value"))      v = pn["value"].as_value().as_double();
                else if (pn.has("psd"))   v = pn["psd"].as_value().as_double();
                else if (pn.has("var"))   v = pn["var"].as_value().as_double();
                else
                {
                    KTERROR(genlog, "Point node must contain \"value\", \"psd\", or \"var\"");
                    return false;
                }
            }
            else
            {
                KTERROR(genlog, "Unexpected YAML element type in measured spectrum array");
                return false;
            }

            const double f_hz = f_in * freqScale;

            if (f_hz < fFreqMinHz || f_hz > fFreqMaxHz)
            {
                KTERROR(genlog, "Frequency value " << f_in << " (scaled to " << f_hz << " Hz) is outside the allowed range [0, 400 MHz]");
                return false;
            }

            xs.push_back(f_hz);
            ys.push_back(v);
        }

        if (xs.size() < 2)
        {
            KTERROR(genlog, "At least two points are required to build a spline");
            return false;
        }

        std::vector< size_t > order(xs.size());
        for (size_t i = 0; i < order.size(); ++i) order[i] = i;
        std::sort(order.begin(), order.end(), [&](size_t a, size_t b){ return xs[a] < xs[b]; });

        std::vector< double > xs_sorted(xs.size()), ys_sorted(xs.size());
        for (size_t i = 0; i < order.size(); ++i)
        {
            xs_sorted[i] = xs[order[i]];
            ys_sorted[i] = ys[order[i]];
        }
        for (size_t i = 1; i < xs_sorted.size(); ++i)
        {
            if (xs_sorted[i] <= xs_sorted[i-1])
            {
                KTERROR(genlog, "Input frequencies must be strictly increasing");
                return false;
            }
        }

        xs.swap(xs_sorted);
        ys.swap(ys_sorted);
        return true;
    }

    bool KTMeasuredNoiseGenerator::BuildSplines()
    {
        if (! fHaveMean)
        {
            KTERROR(genlog, "Mean PSD points were not provided");
            return false;
        }
        if (! fMeanSpline.Build(fMeanXHz, fMeanY))
        {
            KTERROR(genlog, "Failed to build mean PSD spline");
            return false;
        }

        if (fHaveVar)
        {
            if (! fVarSpline.Build(fMeanXHz, fVarY))
            {
                KTERROR(genlog, "Failed to build variance PSD spline");
                return false;
            }
        }

        return true;
    }

    bool KTMeasuredNoiseGenerator::ConfigureDerivedGenerator(const scarab::param_node* node)
    {
        if (node == NULL) return false;
        if (! KTGaussianNoiseGenerator::ConfigureDerivedGenerator(node)) return false;

        fRNG.param(KTRNGGaussian<>::param_type(0.0, 1.0));

        fNoiseScaling  = node->get_value<double>("noise-scaling", fNoiseScaling);
        if (fNoiseScaling <= 0.0)
        {
            KTWARN(genlog, "\"noise-scaling\" must be > 0; using 1.0");
            fNoiseScaling = 1.0;
        }

        fTransformFlag = node->get_value("transform-flag", fTransformFlag);
        fUseVariance   = node->get_value("use-variance", fUseVariance);
        fFixedRadius   = node->get_value("fixed-radius", fFixedRadius);

        // Default units for values provided in *this* config
        string units = node->get_value("frequency-units", string("MHz"));
        if      (units == "Hz")  fFreqScale = 1.0;
        else if (units == "kHz") fFreqScale = 1.0e3;
        else if (units == "MHz") fFreqScale = 1.0e6;
        else if (units == "GHz") fFreqScale = 1.0e9;
        else
        {
            KTWARN(genlog, "Unrecognized frequency-units \"" << units << "\"; assuming MHz");
            fFreqScale = 1.0e6;
        }

        if (! node->has("measured-noise"))
        {
            KTERROR(genlog, "Missing required \"measured-noise\" configuration node");
            return false;
        }

        const scarab::param_node& mn = (*node)["measured-noise"].as_node();

        // ---- loading arrays from an external YAML file if provided ----
        string yamlFile;
        if (mn.has("psd-file"))           yamlFile = mn.get_value("psd-file", string());
        else if (mn.has("psd-values-file")) yamlFile = mn.get_value("psd-values-file", string());
        else if (mn.has("psd-file-dir"))
        {
            string dir  = mn.get_value("psd-file-dir", string());
            string name = mn.get_value("psd-file-name", string());
            if (name.empty())
            {
                KTERROR(genlog, "\"psd-file-dir\" provided but \"psd-file-name\" is missing");
                return false;
            }
            yamlFile = dir;
            if (! yamlFile.empty() && yamlFile.back() != '/' ) yamlFile += "/";
            yamlFile += name;
        }

        if (! yamlFile.empty())
        {
            scarab::param_input_yaml reader;
            auto doc = reader.read_file(yamlFile);
            if (!doc || !doc->is_node())
            {
                KTERROR(genlog, "Failed to read YAML file \"" << yamlFile << "\"");
                return false;
            }
            const scarab::param_node& root = doc->as_node();
            const scarab::param_node* src = &root;
            if (root.has("measured-noise") && root["measured-noise"].is_node())
                src = &root["measured-noise"].as_node();

            // Allowing the file to override frequency units (for values in the file)
            double fileFreqScale = fFreqScale;
            if (src->has("frequency-units"))
            {
                string fu = src->get_value("frequency-units", string("MHz"));
                if      (fu == "Hz")  fileFreqScale = 1.0;
                else if (fu == "kHz") fileFreqScale = 1.0e3;
                else if (fu == "MHz") fileFreqScale = 1.0e6;
                else if (fu == "GHz") fileFreqScale = 1.0e9;
                else
                {
                    KTERROR(genlog, "Unrecognized frequency-units in PSD file: \"" << fu << "\"");
                    return false;
                }
            }

            // PSD units must be W/Hz (default)
            string psdUnits = src->get_value("psd-units", string("W/Hz"));
            if (psdUnits != "W/Hz")
            {
                KTERROR(genlog, "Unsupported psd-units \"" << psdUnits
                        << "\"; only W/Hz is accepted");
                return false;
            }

            if (! src->has("psd-mean") || ! (*src)["psd-mean"].is_array())
            {
                KTERROR(genlog, "PSD file is missing required array \"psd-mean\"");
                return false;
            }

            const scarab::param_array& meanArr = (*src)["psd-mean"].as_array();
            fHaveMean = LoadPointsFromArray(meanArr, fMeanXHz, fMeanY, fileFreqScale);
            if (! fHaveMean) return false;

            fHaveVar = false;
            if (src->has("psd-variance"))
            {
                if (! (*src)["psd-variance"].is_array())
                {
                    KTERROR(genlog, "\"psd-variance\" must be an array of [frequency, value] pairs");
                    return false;
                }
                const scarab::param_array& varArr = (*src)["psd-variance"].as_array();
                std::vector< double > varXHz, varVals;
                if (! LoadPointsFromArray(varArr, varXHz, varVals, fileFreqScale)) return false;

                if (varXHz.size() != fMeanXHz.size())
                {
                    KTERROR(genlog, "Variance and mean arrays must use the same frequency knots");
                    return false;
                }
                for (size_t i = 0; i < varXHz.size(); ++i)
                {
                    if (std::abs(varXHz[i] - fMeanXHz[i]) > 0.5)
                    {
                        KTERROR(genlog, "Variance and mean arrays must share identical frequency knots");
                        return false;
                    }
                }

                for (double v : varVals)
                {
                    if (v < 0.0)
                    {
                        KTERROR(genlog, "Variance values must be non-negative");
                        return false;
                    }
                }

                fVarY   = varVals;
                fHaveVar= true;
            }
            else
            {
                KTWARN(genlog, "PSD file has no \"psd-variance\"; will inject using mean only");
            }
        }
        else
        {
            // ---- Backward-compatible path: arrays inline in main config ----
            if (! mn.has("psd-mean"))
            {
                KTERROR(genlog, "Missing required array \"measured-noise.psd-mean\"");
                return false;
            }
            if (! mn["psd-mean"].is_array())
            {
                KTERROR(genlog, "\"measured-noise.psd-mean\" must be an array of [frequency, value] pairs");
                return false;
            }
            const scarab::param_array& meanArr = mn["psd-mean"].as_array();
            fHaveMean = LoadPointsFromArray(meanArr, fMeanXHz, fMeanY, fFreqScale);
            if (! fHaveMean) return false;

            fHaveVar = false;
            if (mn.has("psd-variance"))
            {
                if (! mn["psd-variance"].is_array())
                {
                    KTERROR(genlog, "\"measured-noise.psd-variance\" must be an array of [frequency, value] pairs");
                    return false;
                }
                const scarab::param_array& varArr = mn["psd-variance"].as_array();
                std::vector< double > varXHz, varVals;
                if (! LoadPointsFromArray(varArr, varXHz, varVals, fFreqScale)) return false;

                if (varXHz.size() != fMeanXHz.size())
                {
                    KTERROR(genlog, "Variance and mean arrays must use the same frequency knots");
                    return false;
                }
                for (size_t i = 0; i < varXHz.size(); ++i)
                {
                    if (std::abs(varXHz[i] - fMeanXHz[i]) > 0.5)
                    {
                        KTERROR(genlog, "Variance and mean arrays must share identical frequency knots");
                        return false;
                    }
                }

                for (double v : varVals)
                {
                    if (v < 0.0)
                    {
                        KTERROR(genlog, "Variance values must be non-negative");
                        return false;
                    }
                }

                fVarY    = varVals;
                fHaveVar = true;
            }
            else
            {
                KTWARN(genlog, "No \"psd-variance\" provided; will inject using mean only");
            }
        }

        if (! BuildSplines()) return false;

        return true;
    }

    double KTMeasuredNoiseGenerator::DrawPSD(double f_abs_hz)
    {
        const double mean_psd = std::max(0.0, fMeanSpline.Evaluate(f_abs_hz));

        if (! fUseVariance || ! fVarSpline.IsBuilt()) return mean_psd;

        const double var_psd   = std::max(0.0, fVarSpline.Evaluate(f_abs_hz));
        const double stdev_psd = std::sqrt(var_psd);

        const double draw = mean_psd + stdev_psd * fRNG();
        return (draw > 0.0) ? draw : 0.0;
    }

    void KTMeasuredNoiseGenerator::RandomUnitComplex(double& c, double& s)
    {
        const double x = fRNG();
        const double y = fRNG();
        const double r = std::sqrt(x*x + y*y);
        if (r > 0.0) { c = x / r; s = y / r; }
        else { c = 1.0; s = 0.0; }
    }

    bool KTMeasuredNoiseGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const double binWidth     = data.GetTimeSeries(0)->GetTimeBinWidth();
        const unsigned sliceSize  = data.GetTimeSeries(0)->GetNTimeBins();
        const unsigned nComponents = data.GetNComponents();

        const double fs = 1.0 / binWidth;
        const double df = fs / sliceSize;
        const unsigned N2 = sliceSize / 2;

        bool isComplex = dynamic_cast< KTTimeSeriesFFTW* >(data.GetTimeSeries(0)) != NULL;

        KTFrequencySpectrumFFTW spec(sliceSize, -fs*0.5, fs*0.5, false);
        spec.SetNTimeBins(sliceSize);

        const double scale = fNoiseScaling * fGain * std::sqrt(fResistance) * sliceSize;

        if (isComplex)
        {
            for (unsigned k = 0; k < sliceSize; ++k)
            {
                const double f_if  = (k <= N2) ? k * df : (static_cast<int>(k) - static_cast<int>(sliceSize)) * df;
                const double f_abs = std::fabs(f_if);

                const double psd  = DrawPSD(f_abs);
                const double pBin = psd * df;

                if (fFixedRadius)
                {
                    const double R = scale * std::sqrt(pBin);
                    double c = 1.0, s = 0.0; RandomUnitComplex(c, s);
                    spec.SetRect(k, R * c, R * s);
                }
                else
                {
                    const double amp = scale * std::sqrt(pBin / 2.0);
                    spec.SetRect(k, amp * fRNG(), amp * fRNG());
                }
            }
        }
        else
        {
            for (unsigned k = 0; k <= N2; ++k)
            {
                const double f_if = k * df;
                const double psd  = DrawPSD(f_if);
                const double pBin = psd * df;

                if (fFixedRadius)
                {
                    if (k==0 || (sliceSize%2==0 && k==N2))
                    {
                        const double R0 = scale * std::sqrt(pBin);
                        const double sign = (fRNG() >= 0.0) ? 1.0 : -1.0;
                        spec.SetRect(k, sign * R0, 0.0);
                    }
                    else
                    {
                        const double R = scale * std::sqrt(pBin);
                        double c = 1.0, s = 0.0; RandomUnitComplex(c, s);
                        const double re = R * c;
                        const double im = R * s;

                        spec.SetRect(k, re, im);
                        spec.SetRect(sliceSize - k,  re, -im);
                    }
                }
                else
                {
                    const double amp  = scale * std::sqrt(pBin);
                    const double re   = amp * fRNG();
                    const double im   = (k==0 || (sliceSize%2==0 && k==N2)) ? 0.0 : amp * fRNG();

                    spec.SetRect(k, re, im);
                    if (k>0 && k<N2)
                        spec.SetRect(sliceSize - k,  re, -im);
                }
            }
        }

        KTReverseFFTW rfft;
        rfft.SetTransformFlag(fTransformFlag);
        rfft.InitializeForComplexTDD(sliceSize);

        std::unique_ptr< KTTimeSeriesFFTW > noiseTS( rfft.TransformToComplex(&spec) );
        if (! noiseTS)
        {
            KTERROR(genlog, "Inverse FFT failed while producing measured noise");
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

} /* namespace Katydid */
