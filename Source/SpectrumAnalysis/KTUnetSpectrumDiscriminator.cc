/*
 * KTUnetSpectrumDiscriminator.cc
 *
 *  Created on: Mar 15, 2026
 *      Author: LEE Seung Mok (@physmlee)
 */

#include "KTUnetSpectrumDiscriminator.hh"

// TODO: include UNet header

namespace Katydid
{
    KTLOGGER(sdlog, "KTUnetSpectrumDiscriminator");

    KT_REGISTER_PROCESSOR(KTUnetSpectrumDiscriminator, "unet-spectrum-discriminator");

    KTUnetSpectrumDiscriminator::KTUnetSpectrumDiscriminator(const std::string &name) : KTProcessor(name),
                                                                                        fModelFilePath("trained_unet.onnx"),
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

        if (!ReadModel())
        {
            KTERROR(sdlog, "Failed to read model.");
            return false;
        }

        return true;
    }

    bool KTUnetSpectrumDiscriminator::ReadModel()
    {
        // TODO: Implement model loading using ONNX Runtime
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

    bool KTUnetSpectrumDiscriminator::CoreDiscriminate(KTMultiPSData &data, KTMultiPSData &newData)
    {
        // TEST: Check the contents of the input data
        unsigned nComponents = data.GetNComponents();
        KTDEBUG(sdlog, "TEST: Received MultiPSData with " << nComponents << " components.");

        KTINFO(sdlog, "TEST: Assuming the first component.");
        KTMultiPS &spectra = *data.GetSpectra(0);
        size_t nSpectra = spectra.size();
        KTPowerSpectrum* firstSpectrum = spectra(0);

        std::vector<std::vector<float>> spectraVectors = data.GetAsVector<float>(0);
        std::vector<std::vector<float>> outputVectors(nSpectra, std::vector<float>(firstSpectrum->GetNFrequencyBins(), 0.0f));
        
        Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        const char *inputNames[] = {"input_1"};
        const char *outputNames[] = {"conv2d_26"};

        KTDEBUG(sdlog, "Preparing input and output tensors.");
        std::int64_t numberOfSamples = 1;
        std::int64_t inputW = 512;
        std::int64_t inputH = 512;
        std::vector<int64_t> input_shape = {numberOfSamples, inputH, inputW, 1};

        std::vector<int64_t> output_shape = {numberOfSamples, inputH, inputW, 1};
        std::vector<float> output_data(numberOfSamples * inputH * inputW * 1);
        Ort::Value output_tensors = Ort::Value::CreateTensor<float>(memInfo, output_data.data(), output_data.size(),
                                                                    output_shape.data(), output_shape.size());

        KTWARN(sdlog, "TEST: Running inference in patches due to input size constraints.");
        size_t inputPatchesW = nSpectra / inputW + (nSpectra % inputW != 0 ? 1 : 0);
        size_t inputPatchesH = firstSpectrum->GetNFrequencyBins() / inputH + (firstSpectrum->GetNFrequencyBins() % inputH != 0 ? 1 : 0);
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

                Ort::Value input_tensors = Ort::Value::CreateTensor<float>(memInfo, input_data.data(), input_data.size(), input_shape.data(), input_shape.size());
                Ort::RunOptions run_options;
                session->Run(run_options, inputNames, &input_tensors, 1, outputNames, &output_tensors, 1);

                for (size_t iSpectrum = w0; iSpectrum < std::min((size_t)(w0 + inputW), (size_t)nSpectra); ++iSpectrum)
                {
                    for (size_t iBin = h0; iBin < std::min((size_t)(h0 + inputH), (size_t)firstSpectrum->GetNFrequencyBins()); ++iBin)
                    {
                        outputVectors[iSpectrum][iBin] = output_data[numberOfSamples * inputH * (iSpectrum - w0) + (iBin - h0)];
                    }
                }
            }
        }

        // Convert to KTMultiPSData format and fill newData
        for (size_t iSpectrum = 0; iSpectrum < outputVectors.size(); ++iSpectrum)
        {
            KTPowerSpectrum* spectrum = new KTPowerSpectrum(firstSpectrum->GetNFrequencyBins());
            for (int j = 0; j < firstSpectrum->GetNFrequencyBins(); ++j)
            {
                (*spectrum)(j) = outputVectors[iSpectrum][j];
            }
            newData.SetSpectrum(spectrum, iSpectrum, 0);
        }
        KTDEBUG(sdlog, "TEST: New MultiPSData created and filled with discriminated spectra. Emitting signal.");

        return true;
    }

    bool KTUnetSpectrumDiscriminator::Discriminate(KTMultiPSData &data)
    {
        KTMultiPSData &newData = data.Of<KTMultiPSData>().SetNComponents(data.GetNComponents());
        return CoreDiscriminate(data, newData);
    }

} /* namespace Katydid */
