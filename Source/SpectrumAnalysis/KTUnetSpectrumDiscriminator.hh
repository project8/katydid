/**
 @file KTUnetSpectrumDiscriminator.hh
 @brief Contains KTUnetSpectrumDiscriminator
 @details Discriminates Spectrum using a pre-trained UNet
 @author: LEE Seung Mok (@physmlee)
 @date: Mar 15, 2026
 */

#ifndef KTUNETSPECTRUMDISCRIMINATOR_HH_
#define KTUNETSPECTRUMDISCRIMINATOR_HH_

#include "KTProcessor.hh"
#include "KTSlot.hh"
#include "KTMultiPSData.hh"
#include "KTPowerSpectrum.hh"

#include "onnxruntime_cxx_api.h"

namespace Katydid
{
    /*!
     TODO: write documentation
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

        private:
            bool ReadModel();
            bool Discriminate(KTMultiPSData& data);

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