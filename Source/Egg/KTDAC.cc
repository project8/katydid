/*
 * KTDAC.cc
 *
 *  Created on: Dec 24, 2013
 *      Author: N.S. Oblath
 */

#include "KTDAC.hh"

#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTPStoreNode.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesREal.hh"

#include "thorax.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(egglog, "katydid.egg");

    static KTNORegistrar< KTProcessor, KTDAC > sDACRegistrar("dac");

    KTDAC::KTDAC(const std::string& name) :
            KTProcessor(name),
            fNBits(8),
            fMinVoltage(-0.25),
            fVoltageRange(0.5),
            fTimeSeriesType(kRealTimeSeries),
            fVoltages(),
            fConvertTSFunc(&KTDAC::ConvertToReal),
            fTimeSeriesSignal("ts", this),
            fRawTSSlot("raw-ts", this, &KTDAC::ConvertData, &fTimeSeriesSignal)
    {
        CalculateVoltages();
    }

    KTDAC::~KTDAC()
    {
    }

    bool KTDAC::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        SetNBits(node->GetData< unsigned >("n-bits", fNBits));
        SetMinVoltage(node->GetData< double >("min-voltage", fMinVoltage));
        SetVoltageRange(node->GetData< double >("voltage-range", fVoltageRange));

        string timeSeriesTypeString = node->GetData< string >("time-series-type", "real");
        if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
        else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
        else
        {
            KTERROR(egglog, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
            return false;
        }

        CalculateVoltages();

        return true;
    }

    void KTDAC::CalculateVoltages()
    {
        if (! fVoltages.empty())
        {
            fVoltages.clear();
        }

        dig_calib_params params;
        params.levels = 1 << fNBits;
        params.v_range = fVoltageRange;
        params.v_min = fMinVoltage;

        fVoltages.resize(params.levels);
        for (unsigned level = 0; level < params.levels; ++level)
        {
            fVoltages[level] = dd2a((data_type)level, &params);
        }
    }

    bool KTDAC::ConvertData(KTRawTimeSeriesData& rawData)
    {
        unsigned nComponents = rawData.GetNComponents();
        KTTimeSeriesData& newData = rawData.Of< KTTimeSeriesData >().SetNComponents(nComponents);
        for (unsigned component = 0; component < nComponents; ++component)
        {
            KTTimeSeries* newTS = (this->*fConvertTSFunc)(rawData.GetTimeSeries(component));
            newData.SetTimeSeries(newTS);
        }
        return true;
    }

    KTTimeSeries* KTDAC::ConvertToFFTW(KTRawTimeSeries* ts)
    {
        unsigned nBins = ts->size();
        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned bin = 0; bin < nBins; ++bin)
        {
            (*newTS)(bin)[0] = Convert((*ts)(bin));
        }
        return newTS;
    }

    KTTimeSeries* KTDAC::ConvertToReal(KTRawTimeSeries* ts)
    {
        unsigned nBins = ts->size();
        KTTimeSeriesReal* newTS = new KTTimeSeriesReal(nBins, ts->GetRangeMin(), ts->GetRangeMax());
        for (unsigned bin = 0; bin < nBins; ++bin)
        {
            (*newTS)(bin) = Convert((*ts)(bin));
        }
        return newTS;
    }



} /* namespace Katydid */
