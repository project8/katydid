/**
 @file KTDAC.hh
 @brief Contains KTDAC
 @details Digital-to-Analog Conversion
 @author: N.S. Oblath
 @date: Dec 24, 2013
 */

#ifndef KTDAC_HH_
#define KTDAC_HH_

#include "KTProcessor.hh"

#include "KTSingleChannelDAC.hh"
#include "KTSlot.hh"

#include <vector>
/*
namespace Nymph
{
    class KTParamNode;
}
;*/

namespace Katydid
{
    using namespace Nymph;
    class KTEggHeader;
    class KTRawTimeSeries;
    class KTRawTimeSeriesData;
    class KTSliceHeader;
    class KTTimeSeries;
    class KTTimeSeriesData;

    /*!
     @class KTDAC
     @author N.S. Oblath

     @brief Digital-to-Analog Conversion

     @details
     Pre-calculates the conversion from digital values to analog values.
     At runtime, uses table-lookup to make DAC more efficient.

     Configuration name: "dac"

     Available configuration values:
     - "n-bits": unsigned -- Set the number of bits in the digitized data
     - "min-voltage": double -- Set the minimum voltage for the digitizer
     - "voltage-range": double -- Set the full-scale voltage range for the digitizer
     - "n-bits-emulated": unsigned -- Set the number of bits to emulate

     Slots:
     - "header": void (KTEggHeader*) -- Sets up the DACs with the header information and then updates the contents if the bit depths are being changed; Emits signal "header"
     - "header-no-init": void (KTEggHeader*) -- Updates the contents of the egg header if the bit depths are being changed; Emits signal "header"
     - "raw-ts": void (KTDataPtr) -- Performs the DAC process on a single slice; Requires KTRawTimeSeriesData; Adds KTTimeSeriesData; Emits signal "ts"

     Signals:
     - "header": void (KTEggHeader*) -- Emitted upon update of an egg header.
     - "ts": void (KTDataPtr) -- Emitted upon DAC completion for a single slice; Guarantees KTTimeSeriesData.
    */
    class KTDAC : public KTProcessor
    {
        public:
            KTDAC(const std::string& name = "dac");
            virtual ~KTDAC();

            bool Configure(const scarab::param_node* node);

            unsigned GetNChannels() const;
            void SetNChannels(unsigned num);

            const KTSingleChannelDAC& GetChannelDAC(unsigned channel = 0) const;
            KTSingleChannelDAC& GetChannelDAC(unsigned channel = 0);


        private:
            std::vector< KTSingleChannelDAC > fChannelDACs;

        public:
            void Initialize();
            void InitializeWithHeader(KTEggHeader* header);

            void UpdateEggHeader(KTEggHeader* header);

            bool ConvertData(KTSliceHeader& header, KTRawTimeSeriesData& rawData);


            //***************
            // Signals
            //***************

        private:
            KTSignalOneArg< KTEggHeader* > fHeaderSignal;
            KTSignalData fTimeSeriesSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotOneArg< void (KTEggHeader*) > fHeaderSlot;
            KTSlotOneArg< void (KTEggHeader*) > fNoInitHeaderSlot;
            KTSlotDataTwoTypes< KTSliceHeader, KTRawTimeSeriesData > fRawTSSlot;

    };


    inline unsigned KTDAC::GetNChannels() const
    {
        return fChannelDACs.size();
    }

    inline void KTDAC::SetNChannels(unsigned num)
    {
        fChannelDACs.resize(num);
        return;
    }

    inline const KTSingleChannelDAC& KTDAC::GetChannelDAC(unsigned channel) const
    {
        return fChannelDACs[channel];
    }

    inline KTSingleChannelDAC& KTDAC::GetChannelDAC(unsigned channel)
    {
        return fChannelDACs[channel];
    }

}
 /* namespace Katydid */
#endif /* KTDAC_HH_ */
