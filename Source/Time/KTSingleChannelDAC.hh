/**
 @file KTSingleChannelDAC.hh
 @brief Contains KTSingleChannelDAC
 @details Digital-to-Analog Conversion for a single channel
 @author: N.S. Oblath
 @date: Mar 26, 2015
 */

#ifndef KTSINGLECHANNELDAC_HH_
#define KTSINGLECHANNELDAC_HH_

#include "KTConstants.hh"
#include "KTLogger.hh"
#include "KTMemberVariable.hh"
#include "KTRawTimeSeries.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include <vector>

namespace Nymph
{
    class KTParamNode;
}
;

namespace Katydid
{
    using namespace Nymph;
    KTLOGGER(egglog_scdac, "KTSingleChannelDAC");

    class KTChannelHeader;
    class KTEggHeader;
    class KTSliceHeader;



    class KTSingleChannelDAC //: public KTProcessor
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
            KTSingleChannelDAC(/*const std::string& name = "dac"*/);
            virtual ~KTSingleChannelDAC();

            bool Configure(const KTParamNode* node);
            bool Configure(const KTSingleChannelDAC& master);

            /// Set input parameters with the DAC gain calculated from the number of bits and the voltage range
            void SetInputParameters(unsigned nBits, double voltageOffset, double voltageRange);
            /// Set input parameters with the DAC gain specified explicitly
            void SetInputParameters(unsigned nBits, double voltageOffset, double voltageRange, double dacGain);

            bool SetDigitizedDataFormat(uint32_t format);

            void SetTimeSeriesType(TimeSeriesType type);

            bool SetEmulatedNBits(unsigned nBits);

            MEMBERVARIABLE_NOSET(unsigned, NBits);
            MEMBERVARIABLE_NOSET(double, VoltageOffset);
            MEMBERVARIABLE_NOSET(double, VoltageRange);
            MEMBERVARIABLE_NOSET(double, DACGain);
            MEMBERVARIABLE_NOSET(uint32_t, DigitizedDataFormat);
            MEMBERVARIABLE_NOSET(TimeSeriesType, TimeSeriesType);
            MEMBERVARIABLE_NOSET(BitDepthMode, BitDepthMode);
            MEMBERVARIABLE_NOSET(unsigned, EmulatedNBits);

        public:
            void InitializeWithHeader(KTChannelHeader* header);
            void Initialize();
            bool GetShouldRunInitialize();

            KTTimeSeries* ConvertTimeSeries(KTRawTimeSeries* ts);

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

            KTTimeSeries* (KTSingleChannelDAC::*fConvertTSFunc)(KTRawTimeSeries*);

            MEMBERVARIABLE_NOSET(unsigned, OversamplingBins);
            MEMBERVARIABLE_NOSET(double, OversamplingScaleFactor);

    };

    inline bool KTSingleChannelDAC::SetDigitizedDataFormat(uint32_t format)
    {
        if (fDigitizedDataFormat != sDigitizedS && fDigitizedDataFormat != sDigitizedUS)
        {
            KTERROR(egglog_scdac, "Invalid digitized data format: " << fDigitizedDataFormat);
            return false;
        }
        fDigitizedDataFormat = format;
        fShouldRunInitialize = true;
        return true;
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertTimeSeries(KTRawTimeSeries* ts)
    {
        return (this->*fConvertTSFunc)(ts);
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertUnsignedToFFTW(KTRawTimeSeries* ts)
    {
        return DoConvertToFFTW(*ts);
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertUnsignedToReal(KTRawTimeSeries* ts)
    {
        return DoConvertToReal(*ts);
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertSignedToFFTW(KTRawTimeSeries* ts)
    {
        return DoConvertToFFTW(KTVarTypePhysicalArray< int64_t >(*ts, false));
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertSignedToReal(KTRawTimeSeries* ts)
    {
        return DoConvertToReal(KTVarTypePhysicalArray< int64_t >(*ts, false));
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertUnsignedToFFTWOversampled(KTRawTimeSeries* ts)
    {
        return DoConvertToFFTWOversampled(*ts);
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertUnsignedToRealOversampled(KTRawTimeSeries* ts)
    {
        return DoConvertToRealOversampled(*ts);
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertSignedToFFTWOversampled(KTRawTimeSeries* ts)
    {
        return DoConvertToFFTWOversampled(KTVarTypePhysicalArray< int64_t >(*ts, false));
    }

    inline KTTimeSeries* KTSingleChannelDAC::ConvertSignedToRealOversampled(KTRawTimeSeries* ts)
    {
        return DoConvertToRealOversampled(KTVarTypePhysicalArray< int64_t >(*ts, false));
    }

    inline void KTSingleChannelDAC::SetTimeSeriesType(TimeSeriesType type)
    {
        fTimeSeriesType = type;
        fShouldRunInitialize = true;
        return;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTSingleChannelDAC::DoConvertToFFTW(const KTVarTypePhysicalArray< XInterfaceType >& ts)
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
    KTTimeSeries* KTSingleChannelDAC::DoConvertToReal(const KTVarTypePhysicalArray< XInterfaceType >& ts)
    {
        if (fShouldRunInitialize) Initialize();

        unsigned nBins = ts.size();
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(nBins, ts.GetRangeMin(), ts.GetRangeMax());
        for (unsigned bin = 0; bin < nBins; ++bin)
        {
            (*newTS)(bin) = Convert((ts)(bin));
        }
        //*** DEBUG ***//
        /**/
        std::stringstream rawtsstream, tsstream;
        for (unsigned iBin = 0; iBin < 10; ++iBin)
        {
            rawtsstream << (ts)(iBin) << "  ";
            tsstream << (*newTS)(iBin) << "  ";
        }
        KTWARN(egglog_scdac, "Raw TS:  " << rawtsstream.str());
        KTWARN(egglog_scdac, "TS:  " << tsstream.str());
        /**/
        //*** DEBUG ***//
        return newTS;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTSingleChannelDAC::DoConvertToFFTWOversampled(const KTVarTypePhysicalArray< XInterfaceType >& ts)
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
            KTWARN(egglog_scdac, "Data lost upon oversampling: " << ts.size() - bin << " samples");
        }
#endif
        return newTS;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTSingleChannelDAC::DoConvertToRealOversampled(const KTVarTypePhysicalArray< XInterfaceType >& ts)
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
            KTWARN(egglog_scdac, "Data lost upon oversampling: " << ts.size() - bin << " samples");
        }
#endif
        return newTS;
    }



    inline double KTSingleChannelDAC::Convert(uint64_t level)
    {
        return fVoltages[level];
    }

    inline double KTSingleChannelDAC::Convert(int64_t level)
    {
        return fVoltages[level + fIntLevelOffset];
    }

    inline bool KTSingleChannelDAC::GetShouldRunInitialize()
    {
        return fShouldRunInitialize;
    }


}
 /* namespace Katydid */
#endif /* KTSINGLECHANNELDAC_HH_ */
