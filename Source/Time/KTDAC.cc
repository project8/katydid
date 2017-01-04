/*
 * KTDAC.cc
 *
 *  Created on: Dec 24, 2013
 *      Author: N.S. Oblath
 */

#include "KTDAC.hh"

#include "KTEggHeader.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"

#include "KTLogger.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(egglog, "KTDAC");

    KT_REGISTER_PROCESSOR(KTDAC, "dac");

    KTDAC::KTDAC(const std::string& name) :
            KTProcessor(name),
            fChannelDACs(1),
            fHeaderSignal("header", this),
            fTimeSeriesSignal("ts", this),
            fHeaderSlot("header", this, &KTDAC::InitializeWithHeader, &fHeaderSignal),
            fNoInitHeaderSlot("header-no-init", this, &KTDAC::UpdateEggHeader, &fHeaderSignal),
            fRawTSSlot("raw-ts", this, &KTDAC::ConvertData, &fTimeSeriesSignal)
    {
    }

    KTDAC::~KTDAC()
    {
    }

    bool KTDAC::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("channels"))
        {
            const scarab::param_array* channelsArray = node->array_at("channels");
            SetNChannels(channelsArray->size());
            for (unsigned iChannel = 0; iChannel < fChannelDACs.size(); ++iChannel)
            {
                fChannelDACs[iChannel].Configure(&(*channelsArray)[iChannel].as_node());
            }
        }
        else
        {
            SetNChannels(node->get_value("n-channels", 1U));
            KTSingleChannelDAC masterCopy;
            masterCopy.Configure(node);
            for (unsigned iChannel = 0; iChannel < fChannelDACs.size(); ++iChannel)
            {
                fChannelDACs[iChannel].Configure(masterCopy);
            }
        }

        return true;
    }

    bool KTDAC::Initialize()
    {
        for (std::vector< KTSingleChannelDAC >::iterator scDACIt = fChannelDACs.begin(); scDACIt != fChannelDACs.end(); ++scDACIt)
        {
            if (! scDACIt->Initialize())
            {
                KTERROR(egglog, "Failed to initialize single-channel DAC for channel " << scDACIt - fChannelDACs.begin());
                return false;
            }
        }

        return true;
    }

    bool KTDAC::InitializeWithHeader(KTEggHeader& header)
    {
        // setup each channel DAC with header info
        unsigned nComponents = header.GetNChannels();
        for (unsigned component = 0; component < nComponents; ++component)
        {
            if (! fChannelDACs[component].InitializeWithHeader(header.GetChannelHeader(component)))
            {
                KTERROR(egglog, "Failed to initialize single-channel DAC for channel " << component);
                return false;
            }
        }

        return UpdateEggHeader(header);
    }

    bool KTDAC::UpdateEggHeader(KTEggHeader& header)
    {
        unsigned nComponents = header.GetNChannels();
        for (unsigned component = 0; component < nComponents; ++component)
        {
            if (fChannelDACs[component].GetBitDepthMode() == KTSingleChannelDAC::kIncreasing)
            {
                header.GetChannelHeader(component)->SetSliceSize(
                        header.GetChannelHeader(component)->GetRawSliceSize() / fChannelDACs[component].GetOversamplingBins());
            }
            if (fChannelDACs[component].GetBitDepthMode() != KTSingleChannelDAC::kNoChange)
            {
                header.GetChannelHeader(component)->SetBitDepth(fChannelDACs[component].GetEmulatedNBits());
            }
        }
        return true;
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
            KTDEBUG(egglog, "Doing DAC for component " << component);
            KTTimeSeries* newTS = fChannelDACs[component].ConvertTimeSeries(rawData.GetTimeSeries(component));
            newData.SetTimeSeries(newTS, component);
        }
        return true;
    }

} /* namespace Katydid */
