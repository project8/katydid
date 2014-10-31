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

#include "KTSlot.hh"

#include <vector>

namespace Katydid
{
    class KTEggHeader;
    class KTParamNode;
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
     - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
     - "n-bits-emulated": unsigned -- Set the number of bits to emulate

     Slots:
     - "header": void (KTEggHeader*) -- Update the contents of the egg header if the bit depths is being changed; Emits signal "header"
     - "raw-ts": void (KTDataPtr) -- Performs the DAC process on a single slice; Requires KTRawTimeSeriesData; Adds KTTimeSeriesData; Emits signal "ts"

     Signals:
     - "header": void (KTEggHeader*) -- Emitted upon update of an egg header.
     - "ts": void (KTDataPtr) -- Emitted upon DAC completion for a single slice; Guarantees KTTimeSeriesData.
    */

    class KTDAC : public KTProcessor
    {
        public:
            enum TimeSeriesType
            {
                kRealTimeSeries,
                kFFTWTimeSeries
            };

            enum BitDepthMode
            {
                kNoChange,
                kReducing,
                kIncreasing
            };

        public:
            KTDAC(const std::string& name = "dac");
            virtual ~KTDAC();

            bool Configure(const KTParamNode* node);

            unsigned GetNBits() const;
            void SetNBits(unsigned nBits);

            double GetMinVoltage() const;
            void SetMinVoltage(double volts);

            double GetVoltageRange() const;
            void SetVoltageRange(double volts);

            TimeSeriesType GetTimeSeriesType() const;
            void SetTimeSeriesType(TimeSeriesType type);

            BitDepthMode GetBitDepthMode() const;

            unsigned GetEmulatedNBits() const;
            bool SetEmulatedNBits(unsigned nBits);

        private:
            unsigned fNBits;
            double fMinVoltage;
            double fVoltageRange;

            TimeSeriesType fTimeSeriesType;

            BitDepthMode fBitDepthMode;
            unsigned fEmulatedNBits;

        public:
            void Initialize();
            bool GetShouldRunInitialize();

            void UpdateEggHeader(KTEggHeader* header);

            bool ConvertData(KTSliceHeader& header, KTRawTimeSeriesData& rawData);

            KTTimeSeries* ConvertToFFTW(KTRawTimeSeries* ts);
            KTTimeSeries* ConvertToReal(KTRawTimeSeries* ts);

            KTTimeSeries* ConvertToFFTWOversampled(KTRawTimeSeries* ts);
            KTTimeSeries* ConvertToRealOversampled(KTRawTimeSeries* ts);

            double Convert(uint64_t level);

        private:
            bool fShouldRunInitialize;

            std::vector< double > fVoltages;

            KTTimeSeries* (KTDAC::*fConvertTSFunc)(KTRawTimeSeries*);

            unsigned fOversamplingBins;
            double fOversamplingScaleFactor;

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
            KTSlotDataTwoTypes< KTSliceHeader, KTRawTimeSeriesData > fRawTSSlot;

    };

    inline unsigned KTDAC::GetNBits() const
    {
        return fNBits;
    }
    inline void KTDAC::SetNBits(unsigned nBits)
    {
        fNBits = nBits;
        fShouldRunInitialize = true;
        return;
    }

    inline double KTDAC::GetMinVoltage() const
    {
        return fMinVoltage;
    }
    inline void KTDAC::SetMinVoltage(double volts)
    {
        fMinVoltage = volts;
        fShouldRunInitialize = true;
        return;
    }

    inline double KTDAC::GetVoltageRange() const
    {
        return fVoltageRange;
    }
    inline void KTDAC::SetVoltageRange(double volts)
    {
        fVoltageRange = volts;
        fShouldRunInitialize = true;
        return;
    }

    inline KTDAC::TimeSeriesType KTDAC::GetTimeSeriesType() const
    {
        return fTimeSeriesType;
    }

    inline KTDAC::BitDepthMode KTDAC::GetBitDepthMode() const
    {
        return fBitDepthMode;
    }

    inline unsigned KTDAC::GetEmulatedNBits() const
    {
        return fEmulatedNBits;
    }

    inline double KTDAC::Convert(uint64_t level)
    {
        return fVoltages[level];
    }

    inline bool KTDAC::GetShouldRunInitialize()
    {
        return fShouldRunInitialize;
    }


}
 /* namespace Katydid */
#endif /* KTDAC_HH_ */
