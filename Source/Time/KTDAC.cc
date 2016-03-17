/*
 * KTDAC.cc
 *
 *  Created on: Dec 24, 2013
 *      Author: N.S. Oblath
 */

#include "KTDAC.hh"

#include "KTEggHeader.hh"
#include "KTParam.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"

using std::string;

namespace Katydid
{

    KT_REGISTER_PROCESSOR(KTDAC, "dac");

    KTDAC::KTDAC(const std::string& name) :
            KTProcessor(name),
            fChannelDACs(1),
            fHeaderSignal("header", this),
            fTimeSeriesSignal("ts", this),
            fHeaderSlot("header", this, &KTDAC::InitializeWithHeader),
            fNoInitHeaderSlot("header-no-init", this, &KTDAC::UpdateEggHeader),
            fRawTSSlot("raw-ts", this, &KTDAC::ConvertData, &fTimeSeriesSignal)
    {
    }

    KTDAC::~KTDAC()
    {
    }

    bool KTDAC::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        if (node->Has("channels"))
        {
            const KTParamArray* channelsArray = node->ArrayAt("channels");
            SetNChannels(channelsArray->Size());
            for (unsigned iChannel = 0; iChannel < fChannelDACs.size(); ++iChannel)
            {
                fChannelDACs[iChannel].Configure(&(*channelsArray)[iChannel].AsNode());
            }
        }
        else
        {
            SetNChannels(node->GetValue("n-channels", 1U));
            KTSingleChannelDAC masterCopy;
            masterCopy.Configure(node);
            for (unsigned iChannel = 0; iChannel < fChannelDACs.size(); ++iChannel)
            {
                fChannelDACs[iChannel].Configure(masterCopy);
            }
        }

        return true;
    }

    void KTDAC::Initialize()
    {
        for (std::vector< KTSingleChannelDAC >::iterator scDACIt = fChannelDACs.begin(); scDACIt != fChannelDACs.end(); ++scDACIt)
        {
            scDACIt->Initialize();
        }

        return;
    }

    void KTDAC::InitializeWithHeader(KTEggHeader* header)
    {
        // setup each channel DAC with header info
        unsigned nComponents = header->GetNChannels();
        for (unsigned component = 0; component < nComponents; ++component)
        {
            fChannelDACs[component].InitializeWithHeader(header->GetChannelHeader(component));
        }

        UpdateEggHeader(header);

        return;
    }

    void KTDAC::UpdateEggHeader(KTEggHeader* header)
    {
        unsigned nComponents = header->GetNChannels();
        for (unsigned component = 0; component < nComponents; ++component)
        {
            if (fChannelDACs[component].GetBitDepthMode() == KTSingleChannelDAC::kIncreasing)
            {
                header->GetChannelHeader(component)->SetSliceSize(
                        header->GetChannelHeader(component)->GetRawSliceSize() / fChannelDACs[component].GetOversamplingBins());
            }
            if (fChannelDACs[component].GetBitDepthMode() != KTSingleChannelDAC::kNoChange)
            {
                header->GetChannelHeader(component)->SetBitDepth(fChannelDACs[component].GetEmulatedNBits());
            }
        }
        fHeaderSignal(header);
        return;
    }

    bool KTDAC::ConvertData(KTSliceHeader& header, KTRawTimeSeriesData& rawData)
    {
        unsigned nComponents = rawData.GetNComponents();
        KTTimeSeriesData& newData = rawData.Of< KTTimeSeriesData >().SetNComponents(nComponents);
        for (unsigned component = 0; component < nComponents; ++component)
        {
            if (fChannelDACs[component].GetBitDepthMode() == KTSingleChannelDAC::kIncreasing)
            {
                header.SetSliceSize(fChannelDACs[component].GetOversamplingBins());
            }
            KTTimeSeries* newTS = fChannelDACs[component].ConvertTimeSeries(rawData.GetTimeSeries(component));
            newData.SetTimeSeries(newTS, component);
        }
        return true;
    }

} /* namespace Katydid */
