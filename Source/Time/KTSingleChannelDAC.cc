/*
 * KTSingleChannelDAC.cc
 *
 *  Created on: Mar 26, 2015
 *      Author: N.S. Oblath
 */

#include "KTSingleChannelDAC.hh"

#include "KTEggHeader.hh"
#include "KTParam.hh"
#include "KTRawTimeSeries.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "digital.hh"

using std::string;

namespace Katydid
{

    KTSingleChannelDAC::KTSingleChannelDAC(/*const std::string& name*/) :
            //KTProcessor(name),
            fDataTypeSize(1),
            fNBits(8),
            fVoltageOffset(0.),
            fVoltageRange(0.),
            fDACGain(-1.),
            fBitAlignment(sBitsAlignedLeft),
            fDigitizedDataFormat(sInvalidFormat),
            fTimeSeriesType(kRealTimeSeries),
            fBitDepthMode(kNoChange),
            fEmulatedNBits(fNBits),
            fShouldRunInitialize(true),
            fVoltages(),
            fIntLevelOffset(0),
            fConvertTSFunc(NULL),
            fOversamplingBins(1),
            fOversamplingScaleFactor(1.)
    {
    }

    KTSingleChannelDAC::~KTSingleChannelDAC()
    {
    }

    bool KTSingleChannelDAC::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        if (node->Has("dac-gain") && node->GetValue< double >("dac-gain") >= 0.)
        {
            SetInputParameters(
                    node->GetValue< unsigned >("data-type-size", fDataTypeSize),
                    node->GetValue< unsigned >("n-bits", fNBits),
                    node->GetValue< double >("voltage-offset", fVoltageOffset),
                    node->GetValue< double >("voltage-range", fVoltageRange),
                    node->GetValue< double >("dac-gain", fDACGain),
                    node->GetValue< unsigned >("bit-alignment", fBitAlignment));
        }
        else
        {
            SetInputParameters(
                    node->GetValue< unsigned >("data-type-size", fDataTypeSize),
                    node->GetValue< unsigned >("n-bits", fNBits),
                    node->GetValue< double >("voltage-offset", fVoltageOffset),
                    node->GetValue< double >("voltage-range", fVoltageRange),
                    node->GetValue< unsigned >("bit-alignment", fBitAlignment));
        }

        string timeSeriesTypeString = node->GetValue("time-series-type", "real");
        if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
        else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
        else
        {
            KTERROR(egglog_scdac, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
            return false;
        }

        if (node->Has("n-bits-emulated"))
        {
            SetEmulatedNBits(node->GetValue< unsigned >("n-bits-emulated", fEmulatedNBits));
        }

        return true;
    }

    bool KTSingleChannelDAC::Configure(const KTSingleChannelDAC& master)
    {
        //nbits, min voltage, voltage range, dac gain
        SetInputParameters(master.GetDataTypeSize(), master.GetNBits(), master.GetVoltageOffset(), master.GetVoltageRange(), master.GetDACGain(), master.GetBitAlignment());

        SetTimeSeriesType(master.GetTimeSeriesType());
        SetEmulatedNBits(master.GetEmulatedNBits());

        return true;
    }

    void KTSingleChannelDAC::SetInputParameters(unsigned dataTypeSize, unsigned nBits, double voltageOffset, double voltageRange, unsigned bitAlignment)
    {
        fDataTypeSize = dataTypeSize;
        fNBits = nBits;
        fVoltageOffset = voltageOffset;
        fVoltageRange = voltageRange;
        fDACGain = -1.;
        fBitAlignment = bitAlignment;
        fShouldRunInitialize = true;
        return;
    }

    void KTSingleChannelDAC::SetInputParameters(unsigned dataTypeSize, unsigned nBits, double voltageOffset, double voltageRange, double dacGain, unsigned bitAlignment)
    {
        fDataTypeSize = dataTypeSize;
        fNBits = nBits;
        fVoltageOffset = voltageOffset;
        fVoltageRange = voltageRange;
        fDACGain = dacGain;
        fBitAlignment = bitAlignment;
        fShouldRunInitialize = true;
        return;
    }

    void KTSingleChannelDAC::InitializeWithHeader(KTChannelHeader* header)
    {
        SetInputParameters(header->GetDataTypeSize(), header->GetBitDepth(), header->GetVoltageOffset(), header->GetVoltageRange(), header->GetDACGain(), header->GetBitAlignment());
        fDigitizedDataFormat = header->GetDataFormat();
        Initialize();
        return;
    }

    void KTSingleChannelDAC::Initialize()
    {
        if (! fShouldRunInitialize)
        {
            return;
        }

        KTDEBUG(egglog_scdac, "Initializing single-channel DAC");

        if (! fVoltages.empty())
        {
            fVoltages.clear();
        }

        unsigned levelDivisor = 1;

        scarab::dig_calib_params params;
        // The bit adjustment is the number of bits that will be ignored, starting at the LSB.
        // This will be used to account for both left-alignment of the data, if that's the case and if the bit depth is smaller than the data type size,
        // and any simulated reduction in the bit depth that's requested by the user.
        // Levels are skipped according to the bit adjustment: levelRepeat = 2^bitAdjustment
        // The level skipping is done by repeating the same voltage for levelRepeat levels.
        unsigned bitAdjustment = 0;
        unsigned dataTypeBitSize = fDataTypeSize * 8;
        if (fBitAlignment == sBitsAlignedLeft)
        {
            bitAdjustment += dataTypeBitSize - fNBits;
        }
        if (fBitDepthMode == kReducing)
        {
            bitAdjustment += fNBits - fEmulatedNBits;
            //get_calib_params(fEmulatedNBits, sizeof(uint64_t), fVoltageOffset, fVoltageRange, fBitAlignment == sBitsAlignedRight, &params);
            //levelDivisor = 1 << (fNBits - fEmulatedNBits);
            //TODO: for reducing the bit depth, we currently don't account for a separately specified DAC gain
        }
        unsigned levelRepeat = pow( 2, bitAdjustment );

        unsigned bitsForVoltageArray = fBitAlignment == sBitsAlignedLeft ? dataTypeBitSize : fNBits;
        if (fDACGain < 0)
        {
            get_calib_params(bitsForVoltageArray, sizeof(uint64_t), fVoltageOffset, fVoltageRange, fBitAlignment == sBitsAlignedRight, &params);
            fDACGain = params.dac_gain;
        }
        else
        {
            get_calib_params2(bitsForVoltageArray, sizeof(uint64_t), fVoltageOffset, fVoltageRange, fDACGain, fBitAlignment == sBitsAlignedRight, &params);
        }

        if (fBitDepthMode == kIncreasing)
        {
            if (fNBits >= fEmulatedNBits)
            {
                KTERROR(egglog_scdac, "Increasing-bit-depth mode was indicated, but emulated bits (" << fEmulatedNBits << ") <= actual bits (" << fNBits << ")");
                return;
            }
            unsigned additionalBits = fEmulatedNBits - fNBits;
            fOversamplingBins = pow(2, 2 * additionalBits);
            fOversamplingScaleFactor = 1. / pow(2, additionalBits);
        }

        KTDEBUG(egglog_scdac, "Assigning voltages with:\n" <<
                "\tData type size: " << fDataTypeSize << " bytes\n" <<
                "\tDigitizer bits: " << fNBits << '\n' <<
                "\tBit depth mode: " << fBitDepthMode << '\n' <<
                "\tBit alignment: " << fBitAlignment << '\n' <<
                "\tVoltage levels: " << (1 << fNBits) << '\n' <<
                "\tEmulated bits: " << fEmulatedNBits << '\n' <<
                "\tLevel divisor: " << levelDivisor << '\n' <<
                "\tReduced levels: " << params.levels << '\n' <<
                "\tVoltage range: " << params.v_range << '\n' <<
                "\tVoltage offset: " << params.v_offset << " V\n" <<
                "\tDAC gain: " << params.dac_gain << " V\n" <<
                "\tOversampling bins: " << fOversamplingBins << '\n' <<
                "\tOversampling scale factor: " << fOversamplingScaleFactor << '\n' <<
                "\tBit adjustment: " << bitAdjustment << '\n' <<
                "\tLevel repeat period: " << levelRepeat << '\n');

        // calculating the voltage conversion
        fVoltages.resize(params.levels);

        if (fDigitizedDataFormat == sDigitizedS)
        {
            fIntLevelOffset = params.levels / 2;
            double valueHoldPos = scarab::d2a< int64_t, double >(0, &params);
            double valueHoldNeg = valueHoldPos;
            KTDEBUG( egglog_scdac, "Calculating voltage conversion for signed integers; integer level offset: " << fIntLevelOffset );
            fVoltages[fIntLevelOffset] = valueHoldPos;
            unsigned levelSkipCounter = 1;
            for (int64_t level = 1; level < fIntLevelOffset; ++level)
            {
                if (levelSkipCounter == 0)
                {
                    valueHoldPos = scarab::d2a< int64_t, double >(level, &params);
                    valueHoldNeg = scarab::d2a< int64_t, double >(-level, &params);
                }
                fVoltages[fIntLevelOffset + level] = valueHoldPos;
                fVoltages[fIntLevelOffset - level] = valueHoldNeg;
                KTWARN( egglog_scdac, "level <" << fIntLevelOffset + level << "> = " << fVoltages[fIntLevelOffset + level] );
                KTWARN( egglog_scdac, "level <" << fIntLevelOffset - level << "> = " << fVoltages[fIntLevelOffset - level] );
                ++levelSkipCounter;
                if (levelSkipCounter == levelRepeat) levelSkipCounter = 0;
            }
            if (levelSkipCounter == 0)
            {
                valueHoldNeg = scarab::d2a< int64_t, double >(-fIntLevelOffset, &params);
            }
            fVoltages[0] = valueHoldNeg;
        }
        else //(fDigitizedDataFormat == sDigitizedUS)
        {
            fIntLevelOffset = 0;
            unsigned levelSkipCounter = 0;
            double valueHold = 0.;
            KTDEBUG( egglog_scdac, "Calculating voltage conversion for unsigned integers up to " << params.levels << "; integer level offset: " << fIntLevelOffset );
            for (uint64_t level = 0; level < params.levels; ++level)
            {
                if (levelSkipCounter == 0) valueHold = scarab::d2a< uint64_t, double >(level, &params);
                fVoltages[level] = valueHold;
                KTWARN(egglog_scdac, "level <" << level << "> = voltage " << fVoltages[level]);
                ++levelSkipCounter;
                if (levelSkipCounter == levelRepeat) levelSkipCounter = 0;
            }
        }

        // setting the convert function
        if (fTimeSeriesType == kFFTWTimeSeries)
        {
            if (fBitDepthMode != kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertSignedToFFTW;
                    KTDEBUG(egglog_scdac, "Convert function set to signed int --> FFTW");
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertUnsignedToFFTW;
                    KTDEBUG(egglog_scdac, "Convert function set to unsigned int --> FFTW");
                }
            }
            else //(fBitDepthMode == kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_scdac, "Convert function set to signed int --> FFTW oversampled");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertSignedToFFTWOversampled;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_scdac, "Convert function set to unsigned int --> FFTW oversampled");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertUnsignedToFFTWOversampled;
                }
            }
        }
        else //(fTimeSeriesType == kRealTimeSeries)
        {
            if (fBitDepthMode != kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_scdac, "Convert function set to signed int --> real");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertSignedToReal;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_scdac, "Convert function set to unsigned int --> real");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertUnsignedToReal;
                }
            }
            else //(fBitDepthMode == kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_scdac, "Convert function set to signed int --> real oversampled");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertSignedToRealOversampled;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_scdac, "Convert function set to unsigned int --> real oversampled");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertUnsignedToRealOversampled;
                }
            }
        }


        fShouldRunInitialize = false;
        return;
    }

    bool KTSingleChannelDAC::SetEmulatedNBits(unsigned nBits)
    {
        if (nBits == fNBits)
        {
            fBitDepthMode = kNoChange;
            fEmulatedNBits = fNBits;
        }
        else if (nBits > fNBits)
        {
            fBitDepthMode = kIncreasing;
            fEmulatedNBits = nBits;
        }
        else
        {
            // otherwise nBits < fNBits
            fBitDepthMode = kReducing;
            fEmulatedNBits = nBits;
        }

        fShouldRunInitialize = true;
        return true;
    }

} /* namespace Katydid */
