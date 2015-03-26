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
#include "KTRawTimeSeriesData.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include "thorax.hh"

using std::string;

namespace Katydid
{

    KTSingleChannelDAC::KTSingleChannelDAC(/*const std::string& name*/) :
            //KTProcessor(name),
            fNBits(8),
            fMinVoltage(-0.25),
            fVoltageRange(0.5),
            fDACGain(-1.),
            fDigitizedDataFormat(sInvalidFormat),
            fTimeSeriesType(KTDAC::kRealTimeSeries),
            fBitDepthMode(KTDAC::kNoChange),
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

        if (node->Has("dac-gain") && node->GetValue("dac-gain") >= 0.)
        {
            SetInputParameters(
                    node->GetValue< unsigned >("n-bits", fNBits),
                    node->GetValue< double >("min-voltage", fMinVoltage),
                    node->GetValue< double >("voltage-range", fVoltageRange),
                    node->GetValue< double >("dac-gain", fDACGain));
        }
        else
        {
            SetInputParameters(
                    node->GetValue< unsigned >("n-bits", fNBits),
                    node->GetValue< double >("min-voltage", fMinVoltage),
                    node->GetValue< double >("voltage-range", fVoltageRange));
        }

        string timeSeriesTypeString = node->GetValue("time-series-type", "real");
        if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
        else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
        else
        {
            KTERROR(egglog_dac, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
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
        SetInputParameters(master.GetNBits(), master.GetMinVoltage(), master.GetVoltageRange(), master.GetDACGain());

        SetTimeSeriesType(master.GetTimeSeriesType());
        SetEmulatedNBits(master.GetEmulatedNBits());

        return true;
    }

    void KTSingleChannelDAC::SetInputParameters(unsigned nBits, double minVoltage, double voltageRange)
    {
        fNBits = nBits;
        fMinVoltage = minVoltage;
        fVoltageRange = voltageRange;
        fDACGain = -1.;
        fShouldRunInitialize = true;
        return;
    }

    void KTSingleChannelDAC::SetInputParameters(unsigned nBits, double minVoltage, double voltageRange, double dacGain)
    {
        fNBits = nBits;
        fMinVoltage = minVoltage;
        fVoltageRange = voltageRange;
        fDACGain = dacGain;
        fShouldRunInitialize = true;
        return;
    }

    void KTSingleChannelDAC::InitializeWithHeader(KTChannelHeader* header)
    {
        //nbits, min voltage, voltage range, dac gain

        Initialize();
        return;
    }

    void KTSingleChannelDAC::Initialize()
    {
        if (! fShouldRunInitialize)
        {
            return;
        }

        if (! fVoltages.empty())
        {
            fVoltages.clear();
        }

        unsigned levelDivisor = 1;

        dig_calib_params params;
        if (fBitDepthMode == KTDAC::kReducing)
        {
            get_calib_params(fEmulatedNBits, sizeof(uint64_t), fMinVoltage, fVoltageRange, &params);
            levelDivisor = 1 << (fNBits - fEmulatedNBits);
            //TODO: for reducing the bit depth, we currently don't account for a separately specified DAC gain
        }
        else
        {
            if (fDACGain < 0)
            {
                get_calib_params(fNBits, sizeof(uint64_t), fMinVoltage, fVoltageRange, &params);
                fDACGain = params.dac_gain;
            }
            else
            {
                get_calib_params2(fNBits, sizeof(uint64_t), fMinVoltage, fVoltageRange, fDACGain, &params);
            }
        }

        if (fBitDepthMode == KTDAC::kIncreasing)
        {
            if (fNBits >= fEmulatedNBits)
            {
                KTERROR(egglog_dac, "Increasing-bit-depth mode was indicated, but emulated bits (" << fEmulatedNBits << ") <= actual bits (" << fNBits << ")");
                return;
            }
            unsigned additionalBits = fEmulatedNBits - fNBits;
            fOversamplingBins = pow(2, 2 * additionalBits);
            fOversamplingScaleFactor = 1. / pow(2, additionalBits);
        }

        KTDEBUG(egglog_dac, "Assigning voltages with:\n" <<
                "\tDigitizer bits: " << fNBits << '\n' <<
                "\tVoltage levels: " << (1 << fNBits) << '\n' <<
                "\tEmulated bits: " << fEmulatedNBits << '\n' <<
                "\tLevel divisor: " << levelDivisor << '\n' <<
                "\tReduced levels: " << params.levels << '\n' <<
                "\tVoltage range: " << params.v_range << '\n' <<
                "\tMinimum voltage: " << params.v_min << " V\n" <<
                "\tOversampling bins: " << fOversamplingBins << '\n' <<
                "\tOversampling scale factor: " << fOversamplingScaleFactor << '\n');


        // calculating the voltage conversion
        fVoltages.resize(params.levels);

        if (fDigitizedDataFormat == sDigitizedS)
        {
            fIntLevelOffset = params.levels / 2;
            for (int64_t level = -fIntLevelOffset; level < fIntLevelOffset; ++level)
            {
                fVoltages[level + fIntLevelOffset] = dd2a(level / levelDivisor, &params);
                //KTWARN(egglog_dac, "level " << level << ", voltage " << fVoltages[level + fIntLevelOffset]);
            }
        }
        else //(fDigitizedDataFormat == sDigitizedUS)
        {
            fIntLevelOffset = 0;
            for (uint64_t level = 0; level < params.levels; ++level)
            {
                fVoltages[level] = dd2a(level / levelDivisor, &params);
                //KTWARN(egglog_dac, "level " << level << ", voltage " << fVoltages[level]);
            }
        }

        // setting the convert function
        if (fTimeSeriesType == KTDAC::kFFTWTimeSeries)
        {
            if (fBitDepthMode != KTDAC::kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertSignedToFFTW;
                    KTDEBUG(egglog_dac, "Convert function set to FFTW");
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertUnsignedToFFTW;
                    KTDEBUG(egglog_dac, "Convert function set to FFTW");
                }
            }
            else //(fBitDepthMode == KTDAC::kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to FFTW oversampled");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertSignedToFFTWOversampled;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to FFTW oversampled");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertUnsignedToFFTWOversampled;
                }
            }
        }
        else //(fTimeSeriesType == kRealTimeSeries)
        {
            if (fBitDepthMode != KTDAC::kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to real");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertSignedToReal;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to real");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertUnsignedToReal;
                }
            }
            else //(fBitDepthMode == KTDAC::kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to real oversampled");
                    fConvertTSFunc = &KTSingleChannelDAC::ConvertSignedToRealOversampled;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to real oversampled");
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
            fBitDepthMode = KTDAC::kNoChange;
            fEmulatedNBits = fNBits;
        }
        else if (nBits > fNBits)
        {
            fBitDepthMode = KTDAC::kIncreasing;
            fEmulatedNBits = nBits;
        }
        else
        {
            // otherwise nBits < fNBits
            fBitDepthMode = KTDAC::kReducing;
            fEmulatedNBits = nBits;
        }

        fShouldRunInitialize = true;
        return true;
    }

} /* namespace Katydid */
