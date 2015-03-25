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

#include "KTLogger.hh"
#include "KTSlot.hh"

#include <vector>

namespace Katydid
{
    KTLOGGER(egglog_dac, "KTDAC");

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

            uint32_t GetDigitizedDataFormat() const;
            bool SetDigitizedDataFormat(uint32_t format);

            TimeSeriesType GetTimeSeriesType() const;
            void SetTimeSeriesType(TimeSeriesType type);

            BitDepthMode GetBitDepthMode() const;

            unsigned GetEmulatedNBits() const;
            bool SetEmulatedNBits(unsigned nBits);

        private:
            unsigned fNBits;
            double fMinVoltage;
            double fVoltageRange;

            uint32_t fDigitizedDataFormat;

            TimeSeriesType fTimeSeriesType;

            BitDepthMode fBitDepthMode;
            unsigned fEmulatedNBits;

        public:
            void Initialize();
            bool GetShouldRunInitialize();

            void UpdateEggHeader(KTEggHeader* header);

            bool ConvertData(KTSliceHeader& header, KTRawTimeSeriesData& rawData);

            KTTimeSeries* ConvertUnsignedToFFTW(KTRawTimeSeries* ts);
            KTTimeSeries* ConvertUnsignedToReal(KTRawTimeSeries* ts);

            KTTimeSeries* ConvertSignedToFFTW(KTRawTimeSeries* ts);
            KTTimeSeries* ConvertSignedToReal(KTRawTimeSeries* ts);

            KTTimeSeries* ConvertUnsignedToFFTWOversampled(KTRawTimeSeries* ts);
            KTTimeSeries* ConvertUnsignedToRealOversampled(KTRawTimeSeries* ts);

            KTTimeSeries* ConvertSignedToFFTWOversampled(KTRawTimeSeries* ts);
            KTTimeSeries* ConvertSignedToRealOversampled(KTRawTimeSeries* ts);

            double Convert(uint64_t level);
            double Convert(int64_t level);

        private:
            template< typename XInterfaceType >
            KTTimeSeries* DoConvertToFFTW(const KTVarTypePhysicalArray< XInterfaceType >& ts);
            template< typename XInterfaceType >
            KTTimeSeries* DoConvertToReal(const KTVarTypePhysicalArray< XInterfaceType >& ts);

            template< typename XInterfaceType >
            KTTimeSeries* DoConvertToFFTWOversampled(const KTVarTypePhysicalArray< XInterfaceType >& ts);
            template< typename XInterfaceType >
            KTTimeSeries* DoConvertToRealOversampled(const KTVarTypePhysicalArray< XInterfaceType >& ts);

            bool fShouldRunInitialize;

            std::vector< double > fVoltages;
            int64_t fIntLevelOffset;

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

    inline uint32_t KTDAC::GetDigitizedDataFormat() const
    {
        return fDigitizedDataFormat;
    }

    inline bool KTDAC::SetDigitizedDataFormat(uint32_t format)
    {
        if (fDigitizedDataFormat != sDigitizedS && fDigitizedDataFormat != sDigitizedUS)
        {
            KTERROR(egglog_dac, "Invalid digitized data format: " << fDigitizedDataFormat);
            return false;
        }
        fDigitizedDataFormat = format;
        fShouldRunInitialize = true;
        return true;
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

    inline KTTimeSeries* KTDAC::ConvertUnsignedToFFTW(KTRawTimeSeries* ts)
    {
        return DoConvertToFFTW(*ts);
    }

    inline KTTimeSeries* KTDAC::ConvertUnsignedToReal(KTRawTimeSeries* ts)
    {
        return DoConvertToReal(*ts);
    }

    inline KTTimeSeries* KTDAC::ConvertSignedToFFTW(KTRawTimeSeries* ts)
    {
        return DoConvertToFFTW(KTVarTypePhysicalArray< int64_t >(*ts, false));
    }

    inline KTTimeSeries* KTDAC::ConvertSignedToReal(KTRawTimeSeries* ts)
    {
        return DoConvertToReal(KTVarTypePhysicalArray< int64_t >(*ts, false));
    }

    inline KTTimeSeries* KTDAC::ConvertUnsignedToFFTWOversampled(KTRawTimeSeries* ts)
    {
        return DoConvertToFFTWOversampled(*ts);
    }

    inline KTTimeSeries* KTDAC::ConvertUnsignedToRealOversampled(KTRawTimeSeries* ts)
    {
        return DoConvertToRealOversampled(*ts);
    }

    inline KTTimeSeries* KTDAC::ConvertSignedToFFTWOversampled(KTRawTimeSeries* ts)
    {
        return DoConvertToFFTWOversampled(KTVarTypePhysicalArray< int64_t >(*ts, false));
    }

    inline KTTimeSeries* KTDAC::ConvertSignedToRealOversampled(KTRawTimeSeries* ts)
    {
        return DoConvertToRealOversampled(KTVarTypePhysicalArray< int64_t >(*ts, false));
    }

    void KTDAC::SetTimeSeriesType(KTDAC::TimeSeriesType type)
    {
        fTimeSeriesType = type;
        fShouldRunInitialize = true;
        return;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTDAC::DoConvertToFFTW(const KTVarTypePhysicalArray< XInterfaceType >& ts)
    {
        if (fShouldRunInitialize) Initialize();

        unsigned nBins = ts.size();
        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(nBins, ts.GetRangeMin(), ts.GetRangeMax());
        for (unsigned bin = 0; bin < nBins; ++bin)
        {
            (*newTS)(bin)[0] = Convert((ts)(bin));
        }
        return newTS;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTDAC::DoConvertToReal(const KTVarTypePhysicalArray< XInterfaceType >& ts)
    {
        if (fShouldRunInitialize) Initialize();

        unsigned nBins = ts.size();
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(nBins, ts.GetRangeMin(), ts.GetRangeMax());
        for (unsigned bin = 0; bin < nBins; ++bin)
        {
            (*newTS)(bin) = Convert((ts)(bin));
        }
        return newTS;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTDAC::DoConvertToFFTWOversampled(const KTVarTypePhysicalArray< XInterfaceType >& ts)
    {
        if (fShouldRunInitialize) Initialize();

        unsigned nBins = ts.size() / fOversamplingBins;
        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(nBins, ts.GetRangeMin(), ts.GetRangeMax());
        double avgValue;
        unsigned bin = 0;
        for (unsigned oversampledBin = 0; oversampledBin < nBins; ++oversampledBin)
        {
            avgValue = 0.;
            for (unsigned iOSBin = 0; iOSBin < fOversamplingBins; ++iOSBin)
            {
                avgValue += Convert((ts)(bin));
                ++bin;
            }
            (*newTS)(oversampledBin)[0] = avgValue * fOversamplingScaleFactor;
        }
#ifndef NDEBUG
        if (bin != ts.size())
        {
            KTWARN(egglog_dac, "Data lost upon oversampling: " << ts.size() - bin << " samples");
        }
#endif
        return newTS;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTDAC::DoConvertToRealOversampled(const KTVarTypePhysicalArray< XInterfaceType >& ts)
    {
        if (fShouldRunInitialize) Initialize();

        unsigned nBins = ts.size() / fOversamplingBins;
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(nBins, ts.GetRangeMin(), ts.GetRangeMax());
        double avgValue;
        unsigned bin = 0;
        for (unsigned oversampledBin = 0; oversampledBin < nBins; ++oversampledBin)
        {
            avgValue = 0.;
            for (unsigned iOSBin = 0; iOSBin < fOversamplingBins; ++iOSBin)
            {
                avgValue += Convert((ts)(bin));
                ++bin;
            }
            (*newTS)(oversampledBin) = avgValue * fOversamplingScaleFactor;
        }
#ifndef NDEBUG
        if (bin != ts.size())
        {
            KTWARN(egglog_dac, "Data lost upon oversampling: " << ts.size() - bin << " samples");
        }
#endif
        return newTS;
    }



    inline double KTDAC::Convert(uint64_t level)
    {
        return fVoltages[level];
    }

    inline double KTDAC::Convert(int64_t level)
    {
        return fVoltages[level + fIntLevelOffset];
    }

    inline bool KTDAC::GetShouldRunInitialize()
    {
        return fShouldRunInitialize;
    }


}
 /* namespace Katydid */
#endif /* KTDAC_HH_ */
