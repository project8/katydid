/*
 * KTSingleChannelDAC.cc
 *
 *  Created on: Mar 26, 2015
 *      Author: N.S. Oblath
 */

#include "KTSingleChannelDAC.hh"

#include "KTEggHeader.hh"
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
            fTimeSeriesType(kUnknownTimeSeries),
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

    KTSingleChannelDAC::KTSingleChannelDAC(const KTSingleChannelDAC& orig) :
            fDataTypeSize(orig.fDataTypeSize),
            fNBits(orig.fNBits),
            fVoltageOffset(orig.fVoltageOffset),
            fVoltageRange(orig.fVoltageRange),
            fDACGain(orig.fDACGain),
            fBitAlignment(orig.fBitAlignment),
            fDigitizedDataFormat(orig.fDigitizedDataFormat),
            fTimeSeriesType(orig.fTimeSeriesType),
            fBitDepthMode(orig.fBitDepthMode),
            fEmulatedNBits(orig.fEmulatedNBits),
            fShouldRunInitialize(orig.fShouldRunInitialize),
            fVoltages(orig.fVoltages),
            fIntLevelOffset(orig.fIntLevelOffset),
            fConvertTSFunc(orig.fConvertTSFunc),
            fOversamplingBins(orig.fOversamplingBins),
            fOversamplingScaleFactor(orig.fOversamplingScaleFactor)
    {
    }

    KTSingleChannelDAC::~KTSingleChannelDAC()
    {
    }

    bool KTSingleChannelDAC::Configure(const scarab::param_node& node)
    {
        if (node.has("dac-gain") && node.get_value< double >("dac-gain") >= 0.)
        {
            SetInputParameters(
                    node->get_value< unsigned >("data-type-size", fDataTypeSize),
                    node->get_value< unsigned >("n-bits", fNBits),
                    node->get_value< double >("voltage-offset", fVoltageOffset),
                    node->get_value< double >("voltage-range", fVoltageRange),
                    node->get_value< double >("dac-gain", fDACGain),
                    node->get_value< unsigned >("bit-alignment", fBitAlignment));
        }
        else
        {
            SetInputParameters(
                    node->get_value< unsigned >("data-type-size", fDataTypeSize),
                    node->get_value< unsigned >("n-bits", fNBits),
                    node->get_value< double >("voltage-offset", fVoltageOffset),
                    node->get_value< double >("voltage-range", fVoltageRange),
                    node->get_value< unsigned >("bit-alignment", fBitAlignment));
        }

        string timeSeriesTypeString = node.get_value("time-series-type", "real");
        if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
        else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
        else
        {
            KTERROR(egglog_scdac, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
            return false;
        }

        if (node.has("n-bits-emulated"))
        {
            SetEmulatedNBits(node.get_value("n-bits-emulated", fEmulatedNBits));
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
        fDACGain = -1.; // will be updated in Initialize()
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

    bool KTSingleChannelDAC::InitializeWithHeader(KTChannelHeader* header)
    {
        SetInputParameters(header->GetDataTypeSize(), header->GetBitDepth(), header->GetVoltageOffset(), header->GetVoltageRange(), header->GetDACGain(), header->GetBitAlignment());
        fDigitizedDataFormat = header->GetDataFormat();
        if (fTimeSeriesType == kUnknownTimeSeries)
        {
            if (header->GetTSDataType() == KTChannelHeader::kReal)
            {
                KTDEBUG(egglog_scdac, "Initializing for real TS data via the egg header");
                fTimeSeriesType = kRealTimeSeries;
            } else if (header->GetTSDataType() == KTChannelHeader::kIQ || header->GetTSDataType() == KTChannelHeader::kComplex)
            {
                KTDEBUG(egglog_scdac, "Initializing for FFTW TS data via the egg header");
                fTimeSeriesType = kFFTWTimeSeries;
            }
        }
        return Initialize();;
    }

    bool KTSingleChannelDAC::Initialize()
    {
        if (! fShouldRunInitialize)
        {
            return true;
        }

        KTDEBUG(egglog_scdac, "Initializing single-channel DAC");

        if (fTimeSeriesType == kUnknownTimeSeries)
        {
            KTERROR(egglog_scdac, "Time series type is not set; cannot initialize DAC");
            return false;
        }

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
                return false;
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
                "\tVoltage range: " << params.v_range << "V\n" <<
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
            unsigned levelSkipCounter = 1; // starting this at 1 is effectively ++levelSkipCounter after filling in the bin at fIntLevelOffset
            if (levelSkipCounter == levelRepeat) levelSkipCounter = 0;
            for (int64_t level = 1; level < fIntLevelOffset; ++level)
            {
                if (levelSkipCounter == 0)
                {
                    valueHoldPos = scarab::d2a< int64_t, double >(level, &params);
                    valueHoldNeg = scarab::d2a< int64_t, double >(-level, &params);
                }
                fVoltages[fIntLevelOffset + level] = valueHoldPos;
                fVoltages[fIntLevelOffset - level] = valueHoldNeg;
                KTTRACE( egglog_scdac, "level <" << fIntLevelOffset + level << "> = " << fVoltages[fIntLevelOffset + level] );
                KTTRACE( egglog_scdac, "level <" << fIntLevelOffset - level << "> = " << fVoltages[fIntLevelOffset - level] );
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
                KTTRACE(egglog_scdac, "level <" << level << "> = voltage " << fVoltages[level]);
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
        return true;
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
