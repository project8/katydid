/*
 * KTDataAccumulator.cc
 *
 *  Created on: Oct 21, 2013
 *      Author: nsoblath
 */

#include "KTDataAccumulator.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

using boost::shared_ptr;

using std::map;
using std::string;

namespace Katydid
{
    KTLOGGER(avlog, "katydid.analysis");

    KTDataAccumulator::KTDataAccumulator(const std::string& name) :
            KTProcessor(name),
            fAccumulatorSize(10),
            fAveragingFrac(0.1),
            fSignalInterval(1),
            fDataMap(),
            fTSSignal("ts", this),
            fFSPolarSignal("fs-polar", this),
            fFSFFTWSignal("fs-fftw", this),
            fTSSlot("ts", this, &KTDataAccumulator::AddData, &fTSSignal),
            fFSPolarSlot("fs-polar", this, &KTDataAccumulator::AddData, &fFSPolarSignal),
            fFSFFTWSlot("fs-fftw", this, &KTDataAccumulator::AddData, &fFSFFTWSignal)
    {
    }

    KTDataAccumulator::~KTDataAccumulator()
    {
    }

    Bool_t KTDataAccumulator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        SetAccumulatorSize(node->GetData<UInt_t>("number-to-average", fAccumulatorSize));
        SetSignalInterval(node->GetData<UInt_t>("signal-interval", fSignalInterval));

        return true;
    }

    Bool_t KTDataAccumulator::AddData(KTTimeSeriesData& data)
    {
        Accumulator& accDataStruct = fDataMap[&typeid(KTTimeSeriesData)];
        KTTimeSeriesData& accData = accDataStruct.fData->Of<KTTimeSeriesData>();
        if (dynamic_cast< KTTimeSeriesReal* >(accData.GetTimeSeries()) != NULL)
        {
            return CoreAddTSDataReal(data, accDataStruct, accData);
        }
        else
        {
            return CoreAddTSDataFFTW(data, accDataStruct, accData);
        }
    }

    Bool_t KTDataAccumulator::AddData(KTFrequencySpectrumDataPolar& data)
    {
        Accumulator& accDataStruct = fDataMap[&typeid(KTFrequencySpectrumDataPolar)];
        KTFrequencySpectrumDataPolar& accData = accDataStruct.fData->Of<KTFrequencySpectrumDataPolar>();
        return CoreAddData(data, accDataStruct, accData);
    }

    Bool_t KTDataAccumulator::AddData(KTFrequencySpectrumDataFFTW& data)
    {
        Accumulator& accDataStruct = fDataMap[&typeid(KTFrequencySpectrumDataFFTW)];
        KTFrequencySpectrumDataFFTW& accData = accDataStruct.fData->Of<KTFrequencySpectrumDataFFTW>();
        return CoreAddData(data, accDataStruct, accData);
    }

    Bool_t KTDataAccumulator::CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData)
    {
        Double_t remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;

        UInt_t nComponents = data.GetNComponents();
        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        UInt_t arraySize = data.GetTimeSeries(0)->GetNTimeBins();
        if (arraySize != accData.GetTimeSeries(0)->GetNTimeBins())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesReal* newTS = static_cast< KTTimeSeriesReal* >(data.GetTimeSeries(iComponent));
            KTTimeSeriesReal* avTS = static_cast< KTTimeSeriesReal* >(accData.GetTimeSeries(iComponent));
            for (UInt_t iBin = 0; iBin < arraySize; iBin++)
            {
                (*avTS)(iBin) = (*avTS)(iBin) * remainingFrac + (*newTS)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }

    Bool_t KTDataAccumulator::CoreAddTSDataFFTW(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData)
    {
        Double_t remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;

        UInt_t nComponents = data.GetNComponents();
        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        UInt_t arraySize = data.GetTimeSeries(0)->GetNTimeBins();
        if (arraySize != accData.GetTimeSeries(0)->GetNTimeBins())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesFFTW* newTS = static_cast< KTTimeSeriesFFTW* >(data.GetTimeSeries(iComponent));
            KTTimeSeriesFFTW* avTS = static_cast< KTTimeSeriesFFTW* >(accData.GetTimeSeries(iComponent));
            for (UInt_t iBin = 0; iBin < arraySize; iBin++)
            {
                (*avTS)(iBin)[0] = (*avTS)(iBin)[0] * remainingFrac + (*newTS)(iBin)[0] * fAveragingFrac;
                (*avTS)(iBin)[1] = (*avTS)(iBin)[1] * remainingFrac + (*newTS)(iBin)[1] * fAveragingFrac;
            }
        }

        return true;
    }


    Bool_t KTDataAccumulator::CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataPolarCore& accData)
    {
        Double_t remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;

        UInt_t nComponents = data.GetNComponents();
        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        UInt_t arraySize = data.GetSpectrumPolar(0)->size();
        if (arraySize != accData.GetSpectrumPolar(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpect = data.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumPolar* avSpect = accData.GetSpectrumPolar(iComponent);
            for (UInt_t iBin = 0; iBin < arraySize; iBin++)
            {
                (*avSpect)(iBin) = (*avSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }

    Bool_t KTDataAccumulator::CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataFFTWCore& accData)
    {
        Double_t remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;

        UInt_t nComponents = data.GetNComponents();
        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        UInt_t arraySize = data.GetSpectrumFFTW(0)->size();
        if (arraySize != accData.GetSpectrumFFTW(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpect = data.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumFFTW* avSpect = accData.GetSpectrumFFTW(iComponent);
            for (UInt_t iBin = 0; iBin < arraySize; iBin++)
            {
                (*avSpect)(iBin)[0] = (*avSpect)(iBin)[0] * remainingFrac + (*newSpect)(iBin)[0] * fAveragingFrac;
                (*avSpect)(iBin)[1] = (*avSpect)(iBin)[1] * remainingFrac + (*newSpect)(iBin)[1] * fAveragingFrac;
            }
        }

        return true;
    }


} /* namespace Katydid */
