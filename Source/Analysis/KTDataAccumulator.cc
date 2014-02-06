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
#include "KTTimeSeriesDistData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"



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
            fLastAccumulatorPtr(),
            fTSSignal("ts", this),
            //fTSDistSignal("ts-dist", this),
            fFSPolarSignal("fs-polar", this),
            fFSFFTWSignal("fs-fftw", this),
            fTSFinishedSignal("ts-finished", this),
            fFSPolarFinishedSignal("fs-polar-finished", this),
            fFSFFTWFinishedSignal("fs-fftw-finished", this),
            fSignalMap(),
            fFinishAccumulationSlot("finish", this, &KTDataAccumulator::CallAllSlots)
    {
        RegisterSlot("ts", this, &KTDataAccumulator::SlotFunction< KTTimeSeriesData >);
        RegisterSlot("fs-polar", this, &KTDataAccumulator::SlotFunction< KTFrequencySpectrumDataPolar >);
        RegisterSlot("fs-fftw", this, &KTDataAccumulator::SlotFunction< KTFrequencySpectrumDataFFTW >);

        fSignalMap.insert(SignalMapValue(&typeid(KTTimeSeriesData), SignalSet(&fTSSignal, &fTSFinishedSignal)));
        fSignalMap.insert(SignalMapValue(&typeid(KTFrequencySpectrumDataPolar), SignalSet(&fFSPolarSignal, &fFSPolarFinishedSignal)));
        fSignalMap.insert(SignalMapValue(&typeid(KTFrequencySpectrumDataFFTW), SignalSet(&fFSFFTWSignal, &fFSFFTWFinishedSignal)));
    }

    KTDataAccumulator::~KTDataAccumulator()
    {
    }

    bool KTDataAccumulator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        SetAccumulatorSize(node->GetData<unsigned>("number-to-average", fAccumulatorSize));
        SetSignalInterval(node->GetData<unsigned>("signal-interval", fSignalInterval));

        return true;
    }

    bool KTDataAccumulator::AddData(KTTimeSeriesData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTTimeSeriesData >();
        KTTimeSeriesData& accData = accDataStruct.fData->Of<KTTimeSeriesData>();
        if (dynamic_cast< KTTimeSeriesReal* >(data.GetTimeSeries(0)) != NULL)
        {
            return CoreAddTSDataReal(data, accDataStruct, accData);
        }
        else
        {
            return CoreAddTSDataFFTW(data, accDataStruct, accData);
        }
    }

/*
    bool KTDataAccumulator::AddData(KTTimeSeriesDistData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTTimeSeriesDistData >();
        KTTimeSeriesDistData& accData = accDataStruct.fData->Of<KTTimeSeriesDistData>();
        return CoreAddData(data, accDataStruct, accData);
    }
    */

    bool KTDataAccumulator::AddData(KTFrequencySpectrumDataPolar& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTFrequencySpectrumDataPolar >();
        KTFrequencySpectrumDataPolar& accData = accDataStruct.fData->Of<KTFrequencySpectrumDataPolar>();
        return CoreAddData(data, accDataStruct, accData);
    }

    bool KTDataAccumulator::AddData(KTFrequencySpectrumDataFFTW& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTFrequencySpectrumDataFFTW >();
        KTFrequencySpectrumDataFFTW& accData = accDataStruct.fData->Of<KTFrequencySpectrumDataFFTW>();
        return CoreAddData(data, accDataStruct, accData);
    }

    bool KTDataAccumulator::CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData)
    {
        double remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;
        //KTDEBUG(avlog, "averaging frac: " << fAveragingFrac << "    remaining frac: " << remainingFrac);

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.fCount == 0)
        {
            accData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesReal* dataTS = static_cast< KTTimeSeriesReal* >(data.GetTimeSeries(iComponent));
                KTTimeSeriesReal* newTS = new KTTimeSeriesReal(dataTS->GetNTimeBins(), dataTS->GetRangeMin(), dataTS->GetRangeMax());
                (*newTS) *= 0.;
                accData.SetTimeSeries(newTS, iComponent);
            }
        }

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetTimeSeries(0)->GetNTimeBins();
        if (arraySize != accData.GetTimeSeries(0)->GetNTimeBins())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesReal* newTS = static_cast< KTTimeSeriesReal* >(data.GetTimeSeries(iComponent));
            KTTimeSeriesReal* avTS = static_cast< KTTimeSeriesReal* >(accData.GetTimeSeries(iComponent));
            for (unsigned iBin = 0; iBin < arraySize; iBin++)
            {
                //KTDEBUG(avlog, (*avTS)(iBin) << "  " << (*newTS)(iBin) << "  " << remainingFrac << "  " << fAveragingFrac);
                (*avTS)(iBin) = (*avTS)(iBin) * remainingFrac + (*newTS)(iBin) * fAveragingFrac;
                //KTDEBUG(avlog, (*avTS)(iBin));
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddTSDataFFTW(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData)
    {
        double remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.fCount == 0)
        {
            accData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesFFTW* dataTS = static_cast< KTTimeSeriesFFTW* >(data.GetTimeSeries(iComponent));
                KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(dataTS->GetNTimeBins(), dataTS->GetRangeMin(), dataTS->GetRangeMax());
                (*newTS) *= 0.;
                accData.SetTimeSeries(newTS, iComponent);
            }
        }

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetTimeSeries(0)->GetNTimeBins();
        if (arraySize != accData.GetTimeSeries(0)->GetNTimeBins())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesFFTW* newTS = static_cast< KTTimeSeriesFFTW* >(data.GetTimeSeries(iComponent));
            KTTimeSeriesFFTW* avTS = static_cast< KTTimeSeriesFFTW* >(accData.GetTimeSeries(iComponent));
            for (unsigned iBin = 0; iBin < arraySize; iBin++)
            {
                (*avTS)(iBin)[0] = (*avTS)(iBin)[0] * remainingFrac + (*newTS)(iBin)[0] * fAveragingFrac;
                (*avTS)(iBin)[1] = (*avTS)(iBin)[1] * remainingFrac + (*newTS)(iBin)[1] * fAveragingFrac;
            }
        }

        return true;
    }

/*////// **************************************************************************
    bool KTDataAccumulator::CoreAddData(KTTimeSeriesDistData& data, Accumulator& accDataStruct, KTTimeSeriesDistData& accData)
    {
        double remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.fCount == 0)
        {
            accData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesDist* dataFS = data.GetTimeSeriesDist(iComponent);
                unsigned dataSize = dataFS->size();
                KTTimeSeriesDist* newFS = new KTTimeSeriesDist(dataSize);
                newFS->operator*=(double(0.));
                accData.SetTimeSeriesDist(newFS, iComponent);
            }
        }

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetTimeSeriesDist(0)->size();
        if (arraySize != accData.GetTimeSeriesDist(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesDist* newSpect = data.GetTimeSeriesDist(iComponent);
            KTTimeSeriesDist* avSpect = accData.GetTimeSeriesDist(iComponent);
            for (unsigned iBin = 0; iBin < arraySize; iBin++)
            {
                (*avSpect)(iBin) = (*avSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }

*/

    bool KTDataAccumulator::CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataPolarCore& accData)
    {
        double remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.fCount == 0)
        {
            accData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTFrequencySpectrumPolar* dataFS = data.GetSpectrumPolar(iComponent);
                KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                newFS->operator*=(double(0.));
                accData.SetSpectrum(newFS, iComponent);
            }
        }

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrumPolar(0)->size();
        if (arraySize != accData.GetSpectrumPolar(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpect = data.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumPolar* avSpect = accData.GetSpectrumPolar(iComponent);
            for (unsigned iBin = 0; iBin < arraySize; iBin++)
            {
                (*avSpect)(iBin) = (*avSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataFFTWCore& accData)
    {
        double remainingFrac = 1.;
        if (accDataStruct.fCount >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.fCount == 0)
        {
            accData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTFrequencySpectrumFFTW* dataFS = data.GetSpectrumFFTW(iComponent);
                KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                newFS->operator*=(double(0.));
                accData.SetSpectrum(newFS, iComponent);
            }
        }

        ++accDataStruct.fCount;
        ++accDataStruct.fSignalCount;


        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrumFFTW(0)->size();
        if (arraySize != accData.GetSpectrumFFTW(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpect = data.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumFFTW* avSpect = accData.GetSpectrumFFTW(iComponent);
            for (unsigned iBin = 0; iBin < arraySize; iBin++)
            {
                (*avSpect)(iBin)[0] = (*avSpect)(iBin)[0] * remainingFrac + (*newSpect)(iBin)[0] * fAveragingFrac;
                (*avSpect)(iBin)[1] = (*avSpect)(iBin)[1] * remainingFrac + (*newSpect)(iBin)[1] * fAveragingFrac;
            }
        }

        return true;
    }

    void KTDataAccumulator::CallAllSlots()
    {
        for (AccumulatorMapIt accIt = fDataMap.begin(); accIt != fDataMap.end(); ++accIt)
        {
            SignalMapIt sigIt = fSignalMap.find(accIt->first);
            if (sigIt != fSignalMap.end())
            {
                (*sigIt->second.fFinishedSignal)(accIt->second.fData);
            }
        }
        return;
    }


} /* namespace Katydid */
