/*
 * KTAmplitudeCounter.cc
 *
 *  Created on: Jan 24, 2014
 *      Author: laroque
 */

#include "KTAmplitudeCounter.hh"

#include "KTEggHeader.hh"
#include "KTTimeSeriesDist.hh"
#include "KTTimeSeriesDistData.hh"
#include "KTParam.hh"


using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(avlog, "KTAmplitudeCounter");

    KT_REGISTER_PROCESSOR(KTAmplitudeCounter, "amp-counter");

    KTAmplitudeCounter::KTAmplitudeCounter(const std::string& name) :
            KTProcessor(name),
            fNumberOfBins(256),
            fRangeMin(0.),
            fRangeMax(-1.),
            fTSDistSignal("ts-dist", this),
            fTSSlot("raw-ts", this, &KTAmplitudeCounter::AddData, &fTSDistSignal)
    {
    }

    KTAmplitudeCounter::~KTAmplitudeCounter()
    {
    }

    bool KTAmplitudeCounter::Configure(const KTParamNode* node)
    {
        if (node != NULL)
        {
            SetNumberOfBins(node->GetValue< unsigned >("num-bins", fNumberOfBins));
            SetRangeMin(node->GetValue< double >("range-min", fRangeMin));
            SetRangeMax(node->GetValue< double >("range-max", fRangeMax));
            return true;
        }
        return false;
    }


    bool KTAmplitudeCounter::AddData(KTRawTimeSeriesData& tsData)
    {
        unsigned nComponents = tsData.GetNComponents();
        KTTimeSeriesDistData& newData =tsData.Of< KTTimeSeriesDistData >().SetNComponents(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTRawTimeSeries* iTS = tsData.GetTimeSeries(iComponent);
            KTTimeSeriesDist* tsDist;
            if (fRangeMax <= fRangeMin)
            {
                tsDist = new KTTimeSeriesDist(fNumberOfBins, -0.5, (double)fNumberOfBins - 0.5);
            }
            else
            {
                tsDist = new KTTimeSeriesDist(fNumberOfBins, fRangeMin, fRangeMax);
            }
            this->CountTimeSeries(tsDist, iTS);
            newData.SetTimeSeriesDist(tsDist, iComponent);
        }
        return true;
    }

    bool KTAmplitudeCounter::CountTimeSeries(KTTimeSeriesDist* tsdist, const KTRawTimeSeries* ts)
    {
        unsigned nTimeBins = ts->size();
        for (unsigned iBin=0; iBin<nTimeBins; ++iBin)
        {
            unsigned val = (*ts)(iBin);
            (*tsdist)(val) += 1;
        }
        return true;
    }

} /* namespace Katydid */
