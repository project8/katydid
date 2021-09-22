/**
 @file KTSingleChannelDAC.hh
 @brief Contains KTSingleChannelDAC
 @details Digital-to-Analog Conversion for a single channel
 @author: N.S. Oblath
 @date: Mar 26, 2015
 */

#ifndef KTSINGLECHANNELDAC_HH_
#define KTSINGLECHANNELDAC_HH_

#include "param.hh"

#include "KTConstants.hh"
#include "KTLogger.hh"
#include "KTMemberVariable.hh"
#include "KTRawTimeSeries.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include <vector>

namespace Katydid
{
    
    KTLOGGER(egglog_scdac, "KTSingleChannelDAC");

    class KTChannelHeader;
    class KTEggHeader;
    class KTSliceHeader;



    class KTSingleChannelDAC //: public Nymph::KTProcessor
    {
        public:
            enum TimeSeriesType
            {
                kUnknownTimeSeries, // if this type is set, DAC initialization will fail
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
            KTSingleChannelDAC(const KTSingleChannelDAC& orig);
            virtual ~KTSingleChannelDAC();

            bool Configure(const scarab::param_node& node);
            bool Configure(const KTSingleChannelDAC& master);

            /// Set input parameters with the DAC gain calculated from the number of bits and the voltage range
            void SetInputParameters(unsigned dataTypeSize, unsigned nBits, double voltageOffset, double voltageRange, unsigned bitAlignment);
            /// Set input parameters with the DAC gain specified explicitly
            void SetInputParameters(unsigned datatypeSize, unsigned nBits, double voltageOffset, double voltageRange, double dacGain, unsigned bitAlignment);

            bool SetDigitizedDataFormat(uint32_t format);

            void SetTimeSeriesType(TimeSeriesType type);

            bool SetEmulatedNBits(unsigned nBits);

            MEMBERVARIABLE_NOSET(unsigned, DataTypeSize);
            MEMBERVARIABLE_NOSET(unsigned, NBits);
            MEMBERVARIABLE_NOSET(double, VoltageOffset);
            MEMBERVARIABLE_NOSET(double, VoltageRange);
            MEMBERVARIABLE_NOSET(double, DACGain);
            MEMBERVARIABLE_NOSET(uint32_t, DigitizedDataFormat);
            MEMBERVARIABLE_NOSET(TimeSeriesType, TimeSeriesType);
            MEMBERVARIABLE_NOSET(BitDepthMode, BitDepthMode);
            MEMBERVARIABLE_NOSET(unsigned, EmulatedNBits);
            MEMBERVARIABLE_NOSET(unsigned, BitAlignment);

        public:
            bool InitializeWithHeader(KTChannelHeader* header);
            bool Initialize();
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
        KTDEBUG(egglog_scdac, "Converting raw-ts to ts-fftw");

        if (fShouldRunInitialize)
        {
            if (! Initialize())
            {
                KTERROR(egglog_scdac, "Failed to initialize single-channel DAC");
                return NULL;
            }
        }

        // ts.size() is divided by 2 because we have complex samples, and the raw time series sees each sample as 2 bins
        unsigned nBins = ts.size() / 2;
        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(nBins, ts.GetRangeMin(), ts.GetRangeMax());
        for (unsigned bin = 0; bin < nBins; ++bin)
        {
            newTS->SetRect(bin, Convert(ts(2 * bin)), Convert(ts(2 * bin + 1)));
        }
        return newTS;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTSingleChannelDAC::DoConvertToReal(const KTVarTypePhysicalArray< XInterfaceType >& ts)
    {
        KTDEBUG(egglog_scdac, "Converting raw-ts to ts-real");

        if (fShouldRunInitialize)
        {
            if (! Initialize())
            {
                KTERROR(egglog_scdac, "Failed to initialize single-channel DAC");
                return NULL;
            }
        }

        unsigned nBins = ts.size();
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(nBins, ts.GetRangeMin(), ts.GetRangeMax());
        for (unsigned bin = 0; bin < nBins; ++bin)
        {
            (*newTS)(bin) = Convert((ts)(bin));
        }
        //*** DEBUG ***//
        /*
        std::stringstream rawtsstream, tsstream;
        for (unsigned iBin = 0; iBin < 30; ++iBin)
        {
            rawtsstream << (ts)(iBin) << "  ";
            tsstream << (*newTS)(iBin) << "  ";
        }
        KTWARN(egglog_scdac, "Raw TS:  " << rawtsstream.str());
        KTWARN(egglog_scdac, "TS:  " << tsstream.str());
        */
        //*** DEBUG ***//
        return newTS;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTSingleChannelDAC::DoConvertToFFTWOversampled(const KTVarTypePhysicalArray< XInterfaceType >& ts)
    {
        KTDEBUG(egglog_scdac, "Converting raw-ts to ts-fftw with oversampling");

        if (fShouldRunInitialize)
        {
            if (! Initialize())
            {
                KTERROR(egglog_scdac, "Failed to initialize single-channel DAC");
                return NULL;
            }
        }

        // ts.size() is divided by 2 because we have complex samples, and the raw time series sees each sample as 2 bins
        unsigned nBins = ts.size() / 2 / fOversamplingBins;
        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(nBins, ts.GetRangeMin(), ts.GetRangeMax());
        double avgValueReal = 0., avgValueImag = 0.;
        unsigned bin = 0;
        for (unsigned oversampledBin = 0; oversampledBin < nBins; ++oversampledBin)
        {
            avgValueReal = 0.;
            avgValueImag = 0.;
            for (unsigned iOSBin = 0; iOSBin < fOversamplingBins; ++iOSBin)
            {
                avgValueReal += Convert(ts(2 * bin));
                avgValueImag += Convert(ts(2 * bin + 1));
                ++bin;
            }
            newTS->SetRect(oversampledBin, 
                            avgValueReal * fOversamplingScaleFactor,
                            avgValueImag * fOversamplingScaleFactor);
        }
#ifndef NDEBUG
        if (bin != ts.size() / 2)
        {
            KTWARN(egglog_scdac, "Data lost upon oversampling: " << ts.size() - bin << " samples");
        }
#endif
        return newTS;
    }

    template< typename XInterfaceType >
    KTTimeSeries* KTSingleChannelDAC::DoConvertToRealOversampled(const KTVarTypePhysicalArray< XInterfaceType >& ts)
    {
        KTDEBUG(egglog_scdac, "Converting raw-ts to ts-real with oversampling");

        if (fShouldRunInitialize)
        {
            if (! Initialize())
            {
                KTERROR(egglog_scdac, "Failed to initialize single-channel DAC");
                return NULL;
            }
        }

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
