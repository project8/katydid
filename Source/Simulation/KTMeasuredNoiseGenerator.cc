/*
 * KTMeasuredNoiseGenerator.cc
 *
 *  Created on: Sep 17, 2025
 *      Author: ehtkarim
 */

#include "KTMeasuredNoiseGenerator.hh"

#include "param.hh"
#include "param_yaml.hh"
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

    namespace
    {
        bool GetFrequencyScale(const string& units, double& freqScale)
        {
            if      (units == "Hz")  freqScale = 1.0;
            else if (units == "kHz") freqScale = 1.0e3;
            else if (units == "MHz") freqScale = 1.0e6;
            else if (units == "GHz") freqScale = 1.0e9;
            else return false;

            return true;
        }
    }

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

        for (unsigned iPoint = 0; iPoint < arr.size(); ++iPoint)
        {
            double fIn = 0.0;
            double value = 0.0;

            if (arr[iPoint].is_array())
            {
                const scarab::param_array& pair = arr[iPoint].as_array();
                if (pair.size() < 2 || ! pair[0].is_value() || ! pair[1].is_value())
                {
                    KTERROR(genlog, "Each point must be a two-element array [frequency, value]");
                    return false;
                }
                fIn = pair[0].as_value().as_double();
                value = pair[1].as_value().as_double();
            }
            else if (arr[iPoint].is_node())
            {
                const scarab::param_node& pointNode = arr[iPoint].as_node();

                if      (pointNode.has("f"))         fIn = pointNode["f"].as_value().as_double();
                else if (pointNode.has("frequency")) fIn = pointNode["frequency"].as_value().as_double();
                else if (pointNode.has("freq"))      fIn = pointNode["freq"].as_value().as_double();
                else
                {
                    KTERROR(genlog, "Point node must contain \"f\", \"frequency\", or \"freq\"");
                    return false;
                }

                if      (pointNode.has("value")) value = pointNode["value"].as_value().as_double();
                else if (pointNode.has("psd"))   value = pointNode["psd"].as_value().as_double();
                else if (pointNode.has("var"))   value = pointNode["var"].as_value().as_double();
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

            const double fHz = fIn * freqScale;

            if (! std::isfinite(fHz) || ! std::isfinite(value))
            {
                KTERROR(genlog, "Input frequencies and values must be finite");
                return false;
            }

            if (fHz < fFreqMinHz || fHz > fFreqMaxHz)
            {
                KTERROR(genlog, "Frequency value " << fIn << " (scaled to " << fHz
                        << " Hz) is outside the allowed range [" << fFreqMinHz
                        << ", " << fFreqMaxHz << "] Hz");
                return false;
            }

            xs.push_back(fHz);
            ys.push_back(value);
        }

        if (xs.size() < 2)
        {
            KTERROR(genlog, "At least two points are required to build a spline");
            return false;
        }

        std::vector< size_t > order(xs.size());
        for (size_t iPoint = 0; iPoint < order.size(); ++iPoint) order[iPoint] = iPoint;
        std::sort(order.begin(), order.end(), [&](size_t a, size_t b){ return xs[a] < xs[b]; });

        std::vector< double > xsSorted(xs.size()), ysSorted(xs.size());
        for (size_t iPoint = 0; iPoint < order.size(); ++iPoint)
        {
            xsSorted[iPoint] = xs[order[iPoint]];
            ysSorted[iPoint] = ys[order[iPoint]];
        }
        for (size_t iPoint = 1; iPoint < xsSorted.size(); ++iPoint)
        {
            if (xsSorted[iPoint] <= xsSorted[iPoint-1])
            {
                KTERROR(genlog, "Input frequencies must be strictly increasing");
                return false;
            }
        }

        xs.swap(xsSorted);
        ys.swap(ysSorted);
        return true;
    }

    bool KTMeasuredNoiseGenerator::LoadMeasuredNoiseNode(const scarab::param_node& node, double freqScale)
    {
        fMeanXHz.clear();
        fMeanY.clear();
        fVarY.clear();
        fHaveMean = false;
        fHaveVar = false;

        if (! node.has("psd-mean") || ! node["psd-mean"].is_array())
        {
            KTERROR(genlog, "Missing required array \"psd-mean\"");
            return false;
        }

        fHaveMean = LoadPointsFromArray(node["psd-mean"].as_array(), fMeanXHz, fMeanY, freqScale);
        if (! fHaveMean) return false;

        for (double meanValue : fMeanY)
        {
            if (meanValue < 0.0)
            {
                KTERROR(genlog, "Mean PSD values must be non-negative");
                return false;
            }
        }

        if (! node.has("psd-variance"))
        {
            if (fUseVariance)
            {
                KTWARN(genlog, "No \"psd-variance\" provided; will inject using mean only");
            }
            return true;
        }

        if (! node["psd-variance"].is_array())
        {
            KTERROR(genlog, "\"psd-variance\" must be an array of [frequency, value] pairs");
            return false;
        }

        std::vector< double > varXHz, varVals;
        if (! LoadPointsFromArray(node["psd-variance"].as_array(), varXHz, varVals, freqScale)) return false;

        if (varXHz.size() != fMeanXHz.size())
        {
            KTERROR(genlog, "Variance and mean arrays must use the same frequency knots");
            return false;
        }
        for (size_t iPoint = 0; iPoint < varXHz.size(); ++iPoint)
        {
            if (std::fabs(varXHz[iPoint] - fMeanXHz[iPoint]) > 0.5)
            {
                KTERROR(genlog, "Variance and mean arrays must share identical frequency knots");
                return false;
            }
        }

        for (double varValue : varVals)
        {
            if (varValue < 0.0)
            {
                KTERROR(genlog, "Variance values must be non-negative");
                return false;
            }
        }

        fVarY.swap(varVals);
        fHaveVar = true;
        return true;
    }

    bool KTMeasuredNoiseGenerator::BuildSplines()
    {
        if (! fHaveMean)
        {
            KTERROR(genlog, "Mean PSD points were not provided");
            return false;
        }

        fMeanSpline = KTSpline(fMeanXHz.data(), fMeanY.data(), (unsigned)fMeanXHz.size());

        if (fHaveVar)
        {
            fVarSpline = KTSpline(fMeanXHz.data(), fVarY.data(), (unsigned)fMeanXHz.size());
        }
        else
        {
            fVarSpline = KTSpline();
        }

        return true;
    }

    bool KTMeasuredNoiseGenerator::ConfigureDerivedGenerator(const scarab::param_node* node)
    {
        if (node == NULL) return false;
        if (! KTGaussianNoiseGenerator::ConfigureDerivedGenerator(node)) return false;

        fRNG.param(KTRNGGaussian<>::param_type(0.0, 1.0));

        fNoiseScaling = node->get_value<double>("noise-scaling", fNoiseScaling);
        if (fNoiseScaling <= 0.0)
        {
            KTWARN(genlog, "\"noise-scaling\" must be > 0; using 1.0");
            fNoiseScaling = 1.0;
        }

        fTransformFlag = node->get_value("transform-flag", fTransformFlag);
        fUseVariance = node->get_value("use-variance", fUseVariance);
        fFixedRadius = node->get_value("fixed-radius", fFixedRadius);

        string units = node->get_value("frequency-units", string("MHz"));
        if (! GetFrequencyScale(units, fFreqScale))
        {
            KTWARN(genlog, "Unrecognized frequency-units \"" << units << "\"; assuming MHz");
            fFreqScale = 1.0e6;
        }

        if (! node->has("measured-noise"))
        {
            KTERROR(genlog, "Missing required \"measured-noise\" configuration node");
            return false;
        }

        const scarab::param_node& measuredNoiseNode = (*node)["measured-noise"].as_node();

        string yamlFile;
        if (measuredNoiseNode.has("psd-file")) yamlFile = measuredNoiseNode.get_value("psd-file", string());
        else if (measuredNoiseNode.has("psd-values-file")) yamlFile = measuredNoiseNode.get_value("psd-values-file", string());
        else if (measuredNoiseNode.has("psd-file-dir"))
        {
            string dir = measuredNoiseNode.get_value("psd-file-dir", string());
            string name = measuredNoiseNode.get_value("psd-file-name", string());
            if (name.empty())
            {
                KTERROR(genlog, "\"psd-file-dir\" provided but \"psd-file-name\" is missing");
                return false;
            }
            yamlFile = dir;
            if (! yamlFile.empty() && yamlFile.back() != '/') yamlFile += "/";
            yamlFile += name;
        }

        if (! yamlFile.empty())
        {
            scarab::param_input_yaml reader;
            auto doc = reader.read_file(yamlFile);
            if (! doc || ! doc->is_node())
            {
                KTERROR(genlog, "Failed to read YAML file \"" << yamlFile << "\"");
                return false;
            }

            const scarab::param_node& root = doc->as_node();
            const scarab::param_node* sourceNode = &root;
            if (root.has("measured-noise") && root["measured-noise"].is_node())
            {
                sourceNode = &root["measured-noise"].as_node();
            }

            double fileFreqScale = fFreqScale;
            if (sourceNode->has("frequency-units"))
            {
                string fileUnits = sourceNode->get_value("frequency-units", string("MHz"));
                if (! GetFrequencyScale(fileUnits, fileFreqScale))
                {
                    KTERROR(genlog, "Unrecognized frequency-units in PSD file: \"" << fileUnits << "\"");
                    return false;
                }
            }

            string psdUnits = sourceNode->get_value("psd-units", string("W/Hz"));
            if (psdUnits != "W/Hz")
            {
                KTERROR(genlog, "Unsupported psd-units \"" << psdUnits << "\"; only W/Hz is accepted");
                return false;
            }

            if (! LoadMeasuredNoiseNode(*sourceNode, fileFreqScale)) return false;
        }
        else
        {
            if (! LoadMeasuredNoiseNode(measuredNoiseNode, fFreqScale)) return false;
        }

        if (! BuildSplines()) return false;

        return true;
    }

    double KTMeasuredNoiseGenerator::DrawPSD(double meanPSD, double varPSD)
    {
        const double clippedMeanPSD = std::max(0.0, meanPSD);

        if (! fUseVariance || ! fHaveVar) return clippedMeanPSD;

        const double clippedVarPSD = std::max(0.0, varPSD);
        const double stdevPSD = std::sqrt(clippedVarPSD);
        const double draw = clippedMeanPSD + stdevPSD * fRNG();

        return (draw > 0.0) ? draw : 0.0;
    }

    void KTMeasuredNoiseGenerator::RandomUnitComplex(double& c, double& s)
    {
        const double x = fRNG();
        const double y = fRNG();
        const double radius = std::sqrt(x*x + y*y);

        if (radius > 0.0)
        {
            c = x / radius;
            s = y / radius;
        }
        else
        {
            c = 1.0;
            s = 0.0;
        }
    }

    bool KTMeasuredNoiseGenerator::GenerateTS(KTTimeSeriesData& data)
    {
        const unsigned nComponents = data.GetNComponents();
        if (nComponents == 0)
        {
            KTERROR(genlog, "Cannot add measured noise to data with no components");
            return false;
        }

        if (data.GetTimeSeries(0) == NULL)
        {
            KTERROR(genlog, "Cannot add measured noise to null time series");
            return false;
        }

        const double binWidth = data.GetTimeSeries(0)->GetTimeBinWidth();
        const unsigned sliceSize = data.GetTimeSeries(0)->GetNTimeBins();

        if (binWidth <= 0.0 || sliceSize == 0)
        {
            KTERROR(genlog, "Invalid time-series bin width or slice size");
            return false;
        }

        const double fs = 1.0 / binWidth;
        const double df = fs / (double)sliceSize;
        const unsigned n2 = sliceSize / 2;
        const unsigned nPositiveBins = n2 + 1;

        std::shared_ptr< KTSpline::Implementation > meanPSD = fMeanSpline.Implement(nPositiveBins, -0.5 * df, ((double)nPositiveBins - 0.5) * df);
        std::shared_ptr< KTSpline::Implementation > varPSD;
        if (fUseVariance && fHaveVar)
        {
            varPSD = fVarSpline.Implement(nPositiveBins, -0.5 * df, ((double)nPositiveBins - 0.5) * df);
        }

        const double scale = fNoiseScaling * fGain * std::sqrt(fResistance) * sliceSize;

        KTReverseFFTW rfft;
        rfft.SetTransformFlag(fTransformFlag);
        rfft.InitializeForComplexTDD(sliceSize);

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeries* timeSeries = data.GetTimeSeries(iComponent);
            if (timeSeries == NULL)
            {
                KTWARN(genlog, "Time series " << iComponent << " is null; skipping measured-noise injection for this component");
                continue;
            }

            const bool isComplex = dynamic_cast< KTTimeSeriesFFTW* >(timeSeries) != NULL;

            KTFrequencySpectrumFFTW spec(sliceSize, -fs*0.5, fs*0.5, false);
            spec.SetNTimeBins(sliceSize);

            if (isComplex)
            {
                for (unsigned k = 0; k < sliceSize; ++k)
                {
                    const unsigned iFreq = (k <= n2) ? k : sliceSize - k;
                    const double psd = DrawPSD((*meanPSD)(iFreq), varPSD ? (*varPSD)(iFreq) : 0.0);
                    const double pBin = psd * df;

                    if (fFixedRadius)
                    {
                        const double radius = scale * std::sqrt(pBin);
                        double c = 1.0;
                        double s = 0.0;
                        RandomUnitComplex(c, s);
                        spec.SetRect(k, radius * c, radius * s);
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
                for (unsigned k = 0; k <= n2; ++k)
                {
                    const bool isDC = (k == 0);
                    const bool isNyquist = (sliceSize % 2 == 0 && k == n2);

                    const double psd = DrawPSD((*meanPSD)(k), varPSD ? (*varPSD)(k) : 0.0);
                    const double pBin = psd * df;

                    if (fFixedRadius)
                    {
                        if (isDC || isNyquist)
                        {
                            const double radius = scale * std::sqrt(pBin);
                            const double sign = (fRNG() >= 0.0) ? 1.0 : -1.0;
                            spec.SetRect(k, sign * radius, 0.0);
                        }
                        else
                        {
                            const double radius = scale * std::sqrt(pBin);
                            double c = 1.0;
                            double s = 0.0;
                            RandomUnitComplex(c, s);
                            const double re = radius * c;
                            const double im = radius * s;

                            spec.SetRect(k, re, im);
                            spec.SetRect(sliceSize - k, re, -im);
                        }
                    }
                    else
                    {
                        const double amp = scale * std::sqrt(pBin);
                        const double re = amp * fRNG();
                        const double im = (isDC || isNyquist) ? 0.0 : amp * fRNG();

                        spec.SetRect(k, re, im);
                        if (! isDC && ! isNyquist)
                        {
                            spec.SetRect(sliceSize - k, re, -im);
                        }
                    }
                }
            }

            std::unique_ptr< KTTimeSeriesFFTW > noiseTS( rfft.TransformToComplex(&spec) );
            if (! noiseTS)
            {
                KTERROR(genlog, "Inverse FFT failed while producing measured noise");
                return false;
            }

            if (auto* tsFFTW = dynamic_cast< KTTimeSeriesFFTW* >(timeSeries))
            {
                for (unsigned iBin = 0; iBin < sliceSize; ++iBin)
                {
                    tsFFTW->SetRect(iBin, tsFFTW->GetReal(iBin) + noiseTS->GetReal(iBin), tsFFTW->GetImag(iBin) + noiseTS->GetImag(iBin));
                }
            }
            else
            {
                for (unsigned iBin = 0; iBin < sliceSize; ++iBin)
                {
                    timeSeries->SetValue(iBin, timeSeries->GetValue(iBin) + noiseTS->GetReal(iBin));
                }
            }
        }

        return true;
    }

} /* namespace Katydid */
