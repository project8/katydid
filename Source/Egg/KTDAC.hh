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
    class KTPStoreNode;
    class KTRawTimeSeries;
    class KTRawTimeSeriesData;
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

     Slots:
     - "[slot-name]": void (shared_ptr<KTData>) -- [what it does]; Requires [input data type]; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "[signal-name]": void (shared_ptr<KTData>) -- Emitted upon [whatever was done]; Guarantees [output data type].
    */

    class KTDAC : public KTProcessor
    {
        public:
            enum TimeSeriesType
            {
                kRealTimeSeries,
                kFFTWTimeSeries
            };

        public:
            KTDAC(const std::string& name = "dac");
            virtual ~KTDAC();

            bool Configure(const KTPStoreNode* node);

            unsigned GetNBits() const;
            void SetNBits(unsigned nBits);

            double GetMinVoltage() const;
            void SetMinVoltage(double volts);

            double GetVoltageRange() const;
            void SetVoltageRange(double volts);

            TimeSeriesType GetTimeSeriesType() const;
            void SetTimeSeriesType(TimeSeriesType type);

        private:
            unsigned fNBits;
            double fMinVoltage;
            double fVoltageRange;

            TimeSeriesType fTimeSeriesType;

        public:
            void CalculateVoltages();

            bool ConvertData(KTRawTimeSeriesData& rawData);

            KTTimeSeries* ConvertToFFTW(KTRawTimeSeries* ts);
            KTTimeSeries* ConvertToReal(KTRawTimeSeries* ts);

            double Convert(uint16_t level);

        private:
            std::vector< double > fVoltages;

            KTTimeSeries* (KTDAC::*fConvertTSFunc)(KTRawTimeSeries*);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fTimeSeriesSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTRawTimeSeriesData > fRawTSSlot;

    };

    inline unsigned KTDAC::GetNBits() const
    {
        return fNBits;
    }
    inline void KTDAC::SetNBits(unsigned nBits)
    {
        fNBits = nBits;
        return;
    }

    inline double KTDAC::GetMinVoltage() const
    {
        return fMinVoltage;
    }
    inline void KTDAC::SetMinVoltage(double volts)
    {
        fMinVoltage = volts;
        return;
    }

    inline double KTDAC::GetVoltageRange() const
    {
        return fVoltageRange;
    }
    inline void KTDAC::SetVoltageRange(double volts)
    {
        fVoltageRange = volts;
        return;
    }

    inline KTDAC::TimeSeriesType KTDAC::GetTimeSeriesType() const
    {
        return fTimeSeriesType;
    }
    inline void KTDAC::SetTimeSeriesType(KTDAC::TimeSeriesType type)
    {
        fTimeSeriesType = type;
        if (type == kFFTWTimeSeries)
        {
            fConvertTSFunc = &KTDAC::ConvertToFFTW;
        }
        else
        {
            fConvertTSFunc = &KTDAC::ConvertToReal;
        }
        return;
    }

    inline double KTDAC::Convert(uint16_t level)
    {
        return fVoltages[level];
    }

}
 /* namespace Katydid */
#endif /* KTDAC_HH_ */
