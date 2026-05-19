/**
 @file KTUnetSpectrumDiscriminator.cc
 @brief Implements KTUnetSpectrumDiscriminator
 @details Loads a pre-trained ONNX U-Net model and performs stride or downsampling inference on multi-power-spectrum data.
 @author: LEE Seung Mok (@physmlee)
 @date: May 18, 2026
 */

#include "KTUnetSpectrumDiscriminator.hh"

#include <set>
#include <algorithm>
#include <limits>

namespace Katydid
{
    KTLOGGER(sdlog, "KTUnetSpectrumDiscriminator");

    KT_REGISTER_PROCESSOR(KTUnetSpectrumDiscriminator, "unet-spectrum-discriminator");

    namespace
    {
        const std::set<std::string> kInferenceOptions = {"stride", "downsampling"};
    }

    KTUnetSpectrumDiscriminator::KTUnetSpectrumDiscriminator(const std::string &name) : KTProcessor(name),
                                                                                        fModelFilePath("trained_unet.onnx"),
                                                                                        fInferenceOption("stride"),
                                                                                        fSamplingMethod("average"),
                                                                                        fInputWidth(512),
                                                                                        fInputHeight(512),
                                                                                        fOutputWidth(512),
                                                                                        fOutputHeight(512),
                                                                                        fMultiPSSlot("multi-ps", this, &KTUnetSpectrumDiscriminator::Discriminate, &fDiscrimMultiPSDataSignal),
                                                                                        fDiscrimMultiPSDataSignal("discrim-multi-ps", this)
    {
    }

    KTUnetSpectrumDiscriminator::~KTUnetSpectrumDiscriminator()
    {
        delete session;
        delete session_options;
        delete env;
    }

    bool KTUnetSpectrumDiscriminator::Configure(const scarab::param_node *node)
    {
        if (node == nullptr)
        {
            KTERROR(sdlog, "Null configuration node provided. Configuration failed.");
            return false;
        }

        if (node->has("model-file-path"))
        {
            fModelFilePath = node->get_value<std::string>("model-file-path");
            KTINFO(sdlog, "Model file path set to: " << fModelFilePath);
        }
        else
        {
            KTERROR(sdlog, "No model file path provided in configuration! Did you forget to set it?");
            return false;
        }

        if (node->has("inference-option"))
        {
            fInferenceOption = node->get_value<std::string>("inference-option");
        }

        if (kInferenceOptions.find(fInferenceOption) == kInferenceOptions.end())
        {
            KTERROR(sdlog, "Unsupported inference option <" << fInferenceOption << "> provided in configuration.");
            return false;
        }

        KTINFO(sdlog, "Inference option set to: " << fInferenceOption);

        if (node->has("sampling-method"))
        {
            fSamplingMethod = node->get_value<std::string>("sampling-method");
            if (fSamplingMethod != "average" && fSamplingMethod != "max")
            {
                KTERROR(sdlog, "Unsupported sampling-method <" << fSamplingMethod << ">. Allowed: average, max.");
                return false;
            }
            KTINFO(sdlog, "Sampling method set to: " << fSamplingMethod);
        }

        if (node->has("input-width"))
        {
            fInputWidth = node->get_value<unsigned>("input-width");
        }
        else
        {
            KTERROR(sdlog, "No input width provided in configuration!");
            return false;
        }

        if (node->has("input-height"))
        {
            fInputHeight = node->get_value<unsigned>("input-height");
        }
        else
        {
            KTERROR(sdlog, "No input height provided in configuration!");
            return false;
        }

        if (node->has("output-width"))
        {
            fOutputWidth = node->get_value<unsigned>("output-width");
        }
        else
        {
            KTERROR(sdlog, "No output width provided in configuration!");
            return false;
        }

        if (node->has("output-height"))
        {
            fOutputHeight = node->get_value<unsigned>("output-height");
        }
        else
        {
            KTERROR(sdlog, "No output height provided in configuration!");
            return false;
        }

        if (fInputWidth == 0 || fInputHeight == 0 || fOutputWidth == 0 || fOutputHeight == 0)
        {
            KTERROR(sdlog, "UNet dimensions must be positive.");
            return false;
        }

        if (fInputWidth != fOutputWidth || fInputHeight != fOutputHeight)
        {
            KTERROR(sdlog, "UNet input and output dimensions must match for stride inference.");
            return false;
        }

        KTINFO(sdlog, "UNet input shape set to: " << fInputHeight << "x" << fInputWidth);
        KTINFO(sdlog, "UNet output shape set to: " << fOutputHeight << "x" << fOutputWidth);

        if (!ReadModel())
        {
            KTERROR(sdlog, "Failed to read model.");
            return false;
        }

        return true;
    }

    bool KTUnetSpectrumDiscriminator::ReadModel()
    {
        try
        {
            std::string env_name = std::string(this->GetConfigName().c_str()) + "-env";
            env = new Ort::Env(ORT_LOGGING_LEVEL_WARNING, env_name.c_str());
            session_options = new Ort::SessionOptions();

            session = new Ort::Session(*env, fModelFilePath.c_str(), *session_options);
        }
        catch (const Ort::Exception& e)
        {
            KTERROR(sdlog, "ONNX Runtime exception: " << e.what());
            return false;
        }
        catch (const std::exception& e)
        {
            KTERROR(sdlog, "Standard exception: " << e.what());
            return false;
        }
        catch (...)
        {
            KTERROR(sdlog, "Unknown exception occurred while loading the model.");
            return false;
        }

        KTINFO(sdlog, "Model loaded successfully from path: " << fModelFilePath);

        return true;
    }

    bool KTUnetSpectrumDiscriminator::InferStride(const std::vector<std::vector<float>>& spectraVectors,
                                                  std::vector<std::vector<float>>& outputVectors,
                                                  size_t nSpectra,
                                                  size_t nFrequencyBins)
    {
        KTWARN(sdlog, "Running stride inference in patches due to input size constraints.");

        Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        const char *inputNames[] = {"input_1"};
        const char *outputNames[] = {"conv2d_26"};

        std::int64_t numberOfSamples = 1;
        std::int64_t inputW = static_cast<std::int64_t>(fInputWidth);
        std::int64_t inputH = static_cast<std::int64_t>(fInputHeight);
        std::int64_t outputW = static_cast<std::int64_t>(fOutputWidth);
        std::int64_t outputH = static_cast<std::int64_t>(fOutputHeight);

        std::vector<int64_t> input_shape = {numberOfSamples, inputH, inputW, 1};
        std::vector<int64_t> output_shape = {numberOfSamples, outputH, outputW, 1};

        std::vector<float> output_data(numberOfSamples * outputH * outputW * 1);
        Ort::Value output_tensors = Ort::Value::CreateTensor<float>(memInfo, output_data.data(), output_data.size(),
                                                                    output_shape.data(), output_shape.size());

        size_t inputPatchesW = nSpectra / static_cast<size_t>(inputW) + (nSpectra % static_cast<size_t>(inputW) != 0 ? 1 : 0);
        size_t inputPatchesH = nFrequencyBins / static_cast<size_t>(inputH) + (nFrequencyBins % static_cast<size_t>(inputH) != 0 ? 1 : 0);
        for (size_t iPatchW = 0; iPatchW < inputPatchesW; ++iPatchW)
        {
            for (size_t iPatchH = 0; iPatchH < inputPatchesH; ++iPatchH)
            {
                std::int32_t w0 = iPatchW * inputW;
                std::int32_t h0 = iPatchH * inputH;

                std::vector<float> input_data(numberOfSamples * inputH * inputW * 1);
                for (size_t iSpectrum = w0; iSpectrum < std::min((size_t)(w0 + inputW), (size_t)nSpectra); ++iSpectrum)
                {
                    const std::vector<float>& spectrumVector = spectraVectors[iSpectrum];
                    for (size_t iBin = h0; iBin < std::min((size_t)(h0 + inputH), (size_t)spectrumVector.size()); ++iBin)
                    {
                        input_data[numberOfSamples * inputH * (iSpectrum - w0) + (iBin - h0)] = spectrumVector[iBin];
                    }
                }

                Ort::Value input_tensors = Ort::Value::CreateTensor<float>(memInfo, input_data.data(), input_data.size(),
                                                                            input_shape.data(), input_shape.size());
                Ort::RunOptions run_options;
                session->Run(run_options, inputNames, &input_tensors, 1, outputNames, &output_tensors, 1);

                for (size_t iSpectrum = w0; iSpectrum < std::min((size_t)(w0 + inputW), (size_t)nSpectra); ++iSpectrum)
                {
                    for (size_t iBin = h0; iBin < std::min((size_t)(h0 + outputH), (size_t)nFrequencyBins); ++iBin)
                    {
                        outputVectors[iSpectrum][iBin] = output_data[numberOfSamples * outputH * (iSpectrum - w0) + (iBin - h0)];
                    }
                }
            }
        }

        return true;
    }

    bool KTUnetSpectrumDiscriminator::InferDownsampling(const std::vector<std::vector<float>>& spectraVectors,
                                                        std::vector<std::vector<float>>& outputVectors,
                                                        size_t nSpectra,
                                                        size_t nFrequencyBins)
    {
        KTINFO(sdlog, "Running downsampling inference.");

        Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        const char *inputNames[] = {"input_1"};
        const char *outputNames[] = {"conv2d_26"};

        std::int64_t numberOfSamples = 1;
        std::int64_t inputW = static_cast<std::int64_t>(fInputWidth);
        std::int64_t inputH = static_cast<std::int64_t>(fInputHeight);
        std::int64_t outputW = static_cast<std::int64_t>(fOutputWidth);
        std::int64_t outputH = static_cast<std::int64_t>(fOutputHeight);

        std::vector<int64_t> input_shape = {numberOfSamples, inputH, inputW, 1};
        std::vector<int64_t> output_shape = {numberOfSamples, outputH, outputW, 1};

        std::vector<float> output_data(numberOfSamples * outputH * outputW * 1);
        Ort::Value output_tensors = Ort::Value::CreateTensor<float>(memInfo, output_data.data(), output_data.size(),
                                                                    output_shape.data(), output_shape.size());

        size_t origW = nSpectra;
        size_t origH = nFrequencyBins;

        size_t s_w = (origW + fInputWidth - 1) / fInputWidth;
        size_t s_h = (origH + fInputHeight - 1) / fInputHeight;

        if (origW < fInputWidth && origH < fInputHeight)
        {
            std::vector<float> input_data(numberOfSamples * inputH * inputW * 1, 0.0f);
            for (size_t iSpectrum = 0; iSpectrum < origW; ++iSpectrum)
            {
                const std::vector<float>& spectrumVector = spectraVectors[iSpectrum];
                for (size_t iBin = 0; iBin < origH; ++iBin)
                {
                    input_data[numberOfSamples * inputH * iSpectrum + iBin] = spectrumVector[iBin];
                }
            }

            Ort::Value input_tensors = Ort::Value::CreateTensor<float>(memInfo, input_data.data(), input_data.size(),
                                                                        input_shape.data(), input_shape.size());
            Ort::RunOptions run_options;
            session->Run(run_options, inputNames, &input_tensors, 1, outputNames, &output_tensors, 1);

            for (size_t iSpectrum = 0; iSpectrum < origW; ++iSpectrum)
            {
                for (size_t iBin = 0; iBin < origH; ++iBin)
                {
                    if (iSpectrum < (size_t)outputW && iBin < (size_t)outputH)
                    {
                        outputVectors[iSpectrum][iBin] = output_data[numberOfSamples * outputH * iSpectrum + iBin];
                    }
                    else
                    {
                        outputVectors[iSpectrum][iBin] = 0.0f;
                    }
                }
            }
        }
        else
        {
            size_t downW = (origW + s_w - 1) / s_w;
            size_t downH = (origH + s_h - 1) / s_h;

            std::vector<float> downsampled(downW * downH, 0.0f);

            for (size_t bx = 0; bx < downW; ++bx)
            {
                size_t x0 = bx * s_w;
                size_t x1 = std::min(origW, x0 + s_w);
                for (size_t by = 0; by < downH; ++by)
                {
                    size_t y0 = by * s_h;
                    size_t y1 = std::min(origH, y0 + s_h);

                    float agg = (fSamplingMethod == "max") ? -std::numeric_limits<float>::infinity() : 0.0f;
                    size_t count = 0;
                    for (size_t ix = x0; ix < x1; ++ix)
                    {
                        const std::vector<float>& spec = spectraVectors[ix];
                        for (size_t iy = y0; iy < y1; ++iy)
                        {
                            float v = spec[iy];
                            if (fSamplingMethod == "max")
                            {
                                if (v > agg) agg = v;
                            }
                            else
                            {
                                agg += v;
                            }
                            ++count;
                        }
                    }
                    if (fSamplingMethod != "max")
                    {
                        agg = (count > 0) ? (agg / static_cast<float>(count)) : 0.0f;
                    }
                    downsampled[by * downW + bx] = agg;
                }
            }

            std::vector<float> input_data(numberOfSamples * inputH * inputW * 1, 0.0f);
            for (size_t bx = 0; bx < downW; ++bx)
            {
                for (size_t by = 0; by < downH; ++by)
                {
                    input_data[numberOfSamples * inputH * bx + by] = downsampled[by * downW + bx];
                }
            }

            Ort::Value input_tensors = Ort::Value::CreateTensor<float>(memInfo, input_data.data(), input_data.size(),
                                                                        input_shape.data(), input_shape.size());
            Ort::RunOptions run_options;
            session->Run(run_options, inputNames, &input_tensors, 1, outputNames, &output_tensors, 1);

            for (size_t x = 0; x < origW; ++x)
            {
                size_t dx = std::min(x / s_w, (size_t)outputW - 1);
                for (size_t y = 0; y < origH; ++y)
                {
                    size_t dy = std::min(y / s_h, (size_t)outputH - 1);
                    outputVectors[x][y] = output_data[numberOfSamples * outputH * dx + dy];
                }
            }
        }

        return true;
    }

    bool KTUnetSpectrumDiscriminator::CoreDiscriminate(KTMultiPSData &data, KTMultiPSData &newData)
    {
        // Check the contents of the input data
        unsigned nComponents = data.GetNComponents();
        KTDEBUG(sdlog, "Received MultiPSData with " << nComponents << " components.");

        KTINFO(sdlog, "TEST: Assuming the first component.");
        KTMultiPS &spectra = *data.GetSpectra(0);
        size_t nSpectra = spectra.size();
        KTPowerSpectrum* firstSpectrum = spectra(0);
        size_t nFrequencyBins = firstSpectrum->GetNFrequencyBins();

        std::vector<std::vector<float>> spectraVectors = data.GetAsVector<float>(0);
        std::vector<std::vector<float>> outputVectors(nSpectra, std::vector<float>(nFrequencyBins, 0.0f));

        if (fInferenceOption == "stride")
        {
            if (!InferStride(spectraVectors, outputVectors, nSpectra, nFrequencyBins))
            {
                return false;
            }
        }
        else if (fInferenceOption == "downsampling")
        {
            if (!InferDownsampling(spectraVectors, outputVectors, nSpectra, nFrequencyBins))
            {
                return false;
            }
        }
        else
        {
            KTERROR(sdlog, "Inference option <" << fInferenceOption << "> is recognized but not implemented.");
            return false;
        }

        // Convert to KTMultiPSData format and fill newData
        for (size_t iSpectrum = 0; iSpectrum < outputVectors.size(); ++iSpectrum)
        {
            KTPowerSpectrum* spectrum = new KTPowerSpectrum(nFrequencyBins);
            for (size_t j = 0; j < nFrequencyBins; ++j)
            {
                (*spectrum)(j) = outputVectors[iSpectrum][j];
            }
            newData.SetSpectrum(spectrum, iSpectrum, 0);
        }
        KTDEBUG(sdlog, "New MultiPSData created and filled with discriminated spectra. Emitting signal.");

        return true;
    }

    bool KTUnetSpectrumDiscriminator::Discriminate(KTMultiPSData &data)
    {
        KTMultiPSData &newData = data.Of<KTMultiPSData>().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, newData);
    }

} /* namespace Katydid */
