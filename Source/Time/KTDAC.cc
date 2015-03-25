/*
 * KTDAC.cc
 *
 *  Created on: Dec 24, 2013
 *      Author: N.S. Oblath
 */

#include "KTDAC.hh"

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

    KT_REGISTER_PROCESSOR(KTDAC, "dac");

    KTDAC::KTDAC(const std::string& name) :
            KTProcessor(name),
            fNBits(8),
            fMinVoltage(-0.25),
            fVoltageRange(0.5),
            fTimeSeriesType(kRealTimeSeries),
            fBitDepthMode(kNoChange),
            fEmulatedNBits(fNBits),
            fShouldRunInitialize(true),
            fVoltages(),
            fConvertTSFunc(NULL),
            fOversamplingBins(1),
            fOversamplingScaleFactor(1.),
            fHeaderSignal("header", this),
            fTimeSeriesSignal("ts", this),
            fHeaderSlot("header", this, &KTDAC::UpdateEggHeader),
            fRawTSSlot("raw-ts", this, &KTDAC::ConvertData, &fTimeSeriesSignal)
    {
    }

    KTDAC::~KTDAC()
    {
    }

    bool KTDAC::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetNBits(node->GetValue< unsigned >("n-bits", fNBits));
        SetMinVoltage(node->GetValue< double >("min-voltage", fMinVoltage));
        SetVoltageRange(node->GetValue< double >("voltage-range", fVoltageRange));

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

    void KTDAC::Initialize()
    {
        if (! fVoltages.empty())
        {
            fVoltages.clear();
        }

        unsigned levelDivisor = 1;

        dig_calib_params params;
        if (fBitDepthMode == kReducing)
        {
            get_calib_params(fEmulatedNBits, sizeof(uint64_t), fMinVoltage, fVoltageRange, &params);
            levelDivisor = 1 << (fNBits - fEmulatedNBits);
        }
        else
        {
            get_calib_params(fNBits, sizeof(uint64_t), fMinVoltage, fVoltageRange, &params);
        }

        if (fBitDepthMode == kIncreasing)
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
        if (fTimeSeriesType == kFFTWTimeSeries)
        {
            if (fBitDepthMode != kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    fConvertTSFunc = &KTDAC::ConvertSignedToFFTW;
                    KTDEBUG(egglog_dac, "Convert function set to FFTW");
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    fConvertTSFunc = &KTDAC::ConvertUnsignedToFFTW;
                    KTDEBUG(egglog_dac, "Convert function set to FFTW");
                }
            }
            else //(fBitDepthMode == kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to FFTW oversampled");
                    fConvertTSFunc = &KTDAC::ConvertSignedToFFTWOversampled;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to FFTW oversampled");
                    fConvertTSFunc = &KTDAC::ConvertUnsignedToFFTWOversampled;
                }
            }
        }
        else //(fTimeSeriesType == kRealTimeSeries)
        {
            if (fBitDepthMode != kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to real");
                    fConvertTSFunc = &KTDAC::ConvertSignedToReal;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to real");
                    fConvertTSFunc = &KTDAC::ConvertUnsignedToReal;
                }
            }
            else //(fBitDepthMode == kIncreasing)
            {
                if (fDigitizedDataFormat == sDigitizedS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to real oversampled");
                    fConvertTSFunc = &KTDAC::ConvertSignedToRealOversampled;
                }
                else //(fDigitizedDataFormat == sDigitizedUS)
                {
                    KTDEBUG(egglog_dac, "Convert function set to real oversampled");
                    fConvertTSFunc = &KTDAC::ConvertUnsignedToRealOversampled;
                }
            }
        }


        fShouldRunInitialize = false;
        return;
    }

    void KTDAC::UpdateEggHeader(KTEggHeader* header)
    {
        if (fBitDepthMode == kIncreasing)
        {
            header->SetSliceSize(header->GetRawSliceSize() / fOversamplingBins);
        }
        if (fBitDepthMode != kNoChange)
        {
            header->SetBitDepth(fEmulatedNBits);
        }
        fHeaderSignal(header);
        return;
    }

    bool KTDAC::ConvertData(KTSliceHeader& header, KTRawTimeSeriesData& rawData)
    {
        if (fBitDepthMode == kIncreasing)
        {
            header.SetSliceSize(fOversamplingBins);
        }
        unsigned nComponents = rawData.GetNComponents();
        KTTimeSeriesData& newData = rawData.Of< KTTimeSeriesData >().SetNComponents(nComponents);
        for (unsigned component = 0; component < nComponents; ++component)
        {
            KTTimeSeries* newTS = (this->*fConvertTSFunc)(rawData.GetTimeSeries(component));
            newData.SetTimeSeries(newTS, component);
        }
        return true;
    }

    bool KTDAC::SetEmulatedNBits(unsigned nBits)
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
