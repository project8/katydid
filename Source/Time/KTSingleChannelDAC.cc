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

#include "thorax.hh"

using std::string;

namespace Katydid
{

    KTSingleChannelDAC::KTSingleChannelDAC(/*const std::string& name*/) :
            //KTProcessor(name),
            fNBits(8),
            fVoltageOffset(0.),
            fVoltageRange(0.),
            fDACGain(-1.),
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
                    node->GetValue< unsigned >("n-bits", fNBits),
                    node->GetValue< double >("voltage-offset", fVoltageOffset),
                    node->GetValue< double >("voltage-range", fVoltageRange),
                    node->GetValue< double >("dac-gain", fDACGain));
        }
        else
        {
            SetInputParameters(
                    node->GetValue< unsigned >("n-bits", fNBits),
                    node->GetValue< double >("voltage-offset", fVoltageOffset),
                    node->GetValue< double >("voltage-range", fVoltageRange));
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
        SetInputParameters(master.GetNBits(), master.GetVoltageOffset(), master.GetVoltageRange(), master.GetDACGain());

        SetTimeSeriesType(master.GetTimeSeriesType());
        SetEmulatedNBits(master.GetEmulatedNBits());

        return true;
    }

    void KTSingleChannelDAC::SetInputParameters(unsigned nBits, double voltageOffset, double voltageRange)
    {
        fNBits = nBits;
        fVoltageOffset = voltageOffset;
        fVoltageRange = voltageRange;
        fDACGain = -1.;
        fShouldRunInitialize = true;
        return;
    }

    void KTSingleChannelDAC::SetInputParameters(unsigned nBits, double voltageOffset, double voltageRange, double dacGain)
    {
        fNBits = nBits;
        fVoltageOffset = voltageOffset;
        fVoltageRange = voltageRange;
        fDACGain = dacGain;
        fShouldRunInitialize = true;
        return;
    }

    void KTSingleChannelDAC::InitializeWithHeader(KTChannelHeader* header)
    {
        SetInputParameters(header->GetBitDepth(), header->GetVoltageOffset(), header->GetVoltageRange(), header->GetDACGain());
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

        dig_calib_params params;
        if (fBitDepthMode == kReducing)
        {
            get_calib_params(fEmulatedNBits, sizeof(uint64_t), fVoltageOffset, fVoltageRange, &params);
            levelDivisor = 1 << (fNBits - fEmulatedNBits);
            //TODO: for reducing the bit depth, we currently don't account for a separately specified DAC gain
        }
        else
        {
            if (fDACGain < 0)
            {
                get_calib_params(fNBits, sizeof(uint64_t), fVoltageOffset, fVoltageRange, &params);
                fDACGain = params.dac_gain;
            }
            else
            {
                get_calib_params2(fNBits, sizeof(uint64_t), fVoltageOffset, fVoltageRange, fDACGain, &params);
            }
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
                "\tDigitizer bits: " << fNBits << '\n' <<
                "\tVoltage levels: " << (1 << fNBits) << '\n' <<
                "\tEmulated bits: " << fEmulatedNBits << '\n' <<
                "\tLevel divisor: " << levelDivisor << '\n' <<
                "\tReduced levels: " << params.levels << '\n' <<
                "\tVoltage range: " << params.v_range << '\n' <<
                "\tVoltage offset: " << params.v_offset << " V\n" <<
                "\tDAC gain: " << params.dac_gain << " V\n" <<
                "\tOversampling bins: " << fOversamplingBins << '\n' <<
                "\tOversampling scale factor: " << fOversamplingScaleFactor << '\n');


        // calculating the voltage conversion
        fVoltages.resize(params.levels);

        if (fDigitizedDataFormat == sDigitizedS)
        {
            fIntLevelOffset = params.levels / 2;
            KTDEBUG( egglog_scdac, "Calculating voltage conversion for signed integers; integer level offset: " << fIntLevelOffset );
            for (int64_t level = -fIntLevelOffset; level < fIntLevelOffset; ++level)
            {
                fVoltages[level + fIntLevelOffset] = d2a_id(level / levelDivisor, &params);
            }
        }
        else //(fDigitizedDataFormat == sDigitizedUS)
        {
            fIntLevelOffset = 0;
            KTDEBUG( egglog_scdac, "Calculating voltage conversion for unsigned integers; integer level offset: " << fIntLevelOffset );
            for (uint64_t level = 0; level < params.levels; ++level)
            {
                fVoltages[level] = d2a_ud(level / levelDivisor, &params);
                //KTWARN(egglog_scdac, "level " << level << ", voltage " << fVoltages[level]);
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
