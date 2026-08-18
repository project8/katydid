/**
 @file KTUnetSpectrumDiscriminator.hh
 @brief Contains KTUnetSpectrumDiscriminator
 @details Discriminates spectrum data using a pre-trained U-Net model.
 @author: LEE Seung Mok (@physmlee)
 @date: May 18, 2026
 */

#ifndef KTUNETSPECTRUMDISCRIMINATOR_HH_
#define KTUNETSPECTRUMDISCRIMINATOR_HH_

#include "KTProcessor.hh"
#include "KTSlot.hh"
#include "KTMultiPSData.hh"
#include "KTPowerSpectrum.hh"

#include "onnxruntime_cxx_api.h"

#include <vector>

namespace Katydid
{
    /*! 
     @class KTUnetSpectrumDiscriminator
     @author S. M. Lee

     @brief Applies a pre-trained U-Net to multi-power-spectrum data.

     @details
     The processor supports stride and downsampling inference over the input
     spectrogram. The model is loaded from ONNX Runtime and configured through
     YAML parameters.

     Available configuration options:
     - "model-file-path": std::string -- path to the ONNX model file
     - "inference-option": std::string -- inference mode; "stride" or "downsampling"
     - "sampling-method": std::string -- downsampling method; "average" or "max"
     - "input-width": unsigned -- width of the inference tile in bins
     - "input-height": unsigned -- height of the inference tile in bins
     - "output-width": unsigned -- width of the output tile in bins
     - "output-height": unsigned -- height of the output tile in bins

     Slots:
     - "multi-ps": void (Nymph::KTDataPtr) -- accepts KTMultiPSData input and runs inference

     Signals:
     - "discrim-multi-ps": void (Nymph::KTDataPtr) -- emits the discriminated KTMultiPSData
     */

    class KTMultiPSData;

    class KTUnetSpectrumDiscriminator : public Nymph::KTProcessor
    {
        public:
            KTUnetSpectrumDiscriminator(const std::string& name = "unet-spectrum-discriminator");
            virtual ~KTUnetSpectrumDiscriminator();

            bool Configure(const scarab::param_node* node);

        private:
            MEMBERVARIABLE_NOSET(std::string, ModelFilePath);
            MEMBERVARIABLE_NOSET(std::string, InferenceOption);
            MEMBERVARIABLE_NOSET(unsigned, InputWidth);
            MEMBERVARIABLE_NOSET(unsigned, InputHeight);
            MEMBERVARIABLE_NOSET(unsigned, OutputWidth);
            MEMBERVARIABLE_NOSET(unsigned, OutputHeight);
            MEMBERVARIABLE_NOSET(std::string, SamplingMethod);

        private:
            bool ReadModel();
            bool Discriminate(KTMultiPSData& data);
            bool InferStride(const std::vector<std::vector<float>>& spectraVectors,
                             std::vector<std::vector<float>>& outputVectors,
                             size_t nSpectra,
                             size_t nFrequencyBins);
            bool InferDownsampling(const std::vector<std::vector<float>>& spectraVectors,
                                   std::vector<std::vector<float>>& outputVectors,
                                   size_t nSpectra,
                                   size_t nFrequencyBins);

            Ort::Env* env;
            Ort::Session* session;
            Ort::SessionOptions* session_options;

        public:
            bool CoreDiscriminate(KTMultiPSData& data, KTMultiPSData& newData);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fDiscrimMultiPSDataSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTMultiPSData > fMultiPSSlot;
    };
} /* namespace Katydid */
#endif /* KTUNETSPECTRUMDISCRIMINATOR_HH_ */