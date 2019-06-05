/*
 * KTDataAccumulator.cc
 *
 *  Created on: Oct 21, 2013
 *      Author: nsoblath
 */

#include "KTDataAccumulator.hh"

using std::map;
using std::string;

namespace Katydid
{
    KTLOGGER(avlog, "KTDataAccumulator");

    KT_REGISTER_PROCESSOR(KTDataAccumulator, "data-accumulator");

    KTDataAccumulator::KTDataAccumulator(const std::string& name) :
            KTProcessor(name),
            fAccumulatorSize(10),
            fAveragingFrac(0.1),
            fSignalInterval(1),
            fDataMap(),
            fLastAccumulatorPtr(),

            fTSSignal("ts", this),
            fTSDistSignal("ts-dist", this),
            fFSPolarSignal("fs-polar", this),
            fFSFFTWSignal("fs-fftw", this),
            fPSSignal("ps", this),
            fConvFSPolarSignal("conv-fs-polar", this),
            fConvFSFFTWSignal("conv-fs-fftw", this),
            fConvPSSignal("conv-ps", this),

            fTSFinishedSignal("ts-finished", this),
            fTSDistFinishedSignal("ts-dist-finished", this),
            fFSPolarFinishedSignal("fs-polar-finished", this),
            fFSFFTWFinishedSignal("fs-fftw-finished", this),
            fPSFinishedSignal("ps-finished", this),
            fConvFSPolarFinishedSignal("conv-fs-polar-finished", this),
            fConvFSFFTWFinishedSignal("conv-fs-fftw-finished", this),
            fConvPSFinishedSignal("conv-ps-finished", this),

            fSignalMap()
    {
        RegisterSlot("ts", this, &KTDataAccumulator::SlotFunction< KTTimeSeriesData >);
        fSignalMap.insert(SignalMapValue(&typeid(KTTimeSeriesData), SignalSet(&fTSSignal, &fTSFinishedSignal)));

        RegisterSlot("ts-dist", this, &KTDataAccumulator::SlotFunction< KTTimeSeriesDistData >);
        fSignalMap.insert(SignalMapValue(&typeid(KTTimeSeriesDistData), SignalSet(&fTSDistSignal, &fTSDistFinishedSignal)));

        RegisterSlot("fs-polar", this, &KTDataAccumulator::SlotFunction< KTFrequencySpectrumDataPolar >);
        fSignalMap.insert(SignalMapValue(&typeid(KTFrequencySpectrumDataPolar), SignalSet(&fFSPolarSignal, &fFSPolarFinishedSignal)));

        RegisterSlot("fs-fftw", this, &KTDataAccumulator::SlotFunction< KTFrequencySpectrumDataFFTW >);
        fSignalMap.insert(SignalMapValue(&typeid(KTFrequencySpectrumDataFFTW), SignalSet(&fFSFFTWSignal, &fFSFFTWFinishedSignal)));

        RegisterSlot("ps", this, &KTDataAccumulator::SlotFunction< KTPowerSpectrumData >);
        fSignalMap.insert(SignalMapValue(&typeid(KTPowerSpectrumData), SignalSet(&fPSSignal, &fPSFinishedSignal)));

        RegisterSlot("conv-fs-polar", this, &KTDataAccumulator::SlotFunction< KTConvolvedFrequencySpectrumDataPolar >);
        fSignalMap.insert(SignalMapValue(&typeid(KTConvolvedPowerSpectrumData), SignalSet(&fConvPSSignal, &fConvPSFinishedSignal)));

        RegisterSlot("conv-fs-fftw", this, &KTDataAccumulator::SlotFunction< KTConvolvedFrequencySpectrumDataFFTW >);
        fSignalMap.insert(SignalMapValue(&typeid(KTConvolvedPowerSpectrumData), SignalSet(&fConvPSSignal, &fConvPSFinishedSignal)));

        RegisterSlot("conv-ps", this, &KTDataAccumulator::SlotFunction< KTConvolvedPowerSpectrumData >);
        fSignalMap.insert(SignalMapValue(&typeid(KTConvolvedPowerSpectrumData), SignalSet(&fConvPSSignal, &fConvPSFinishedSignal)));
    }

    KTDataAccumulator::~KTDataAccumulator()
    {
        while (! fDataMap.empty())
        {
            delete fDataMap.begin()->second;
            fDataMap.erase(fDataMap.begin());
        }
    }

    bool KTDataAccumulator::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetAccumulatorSize(node->get_value<unsigned>("number-to-average", fAccumulatorSize));
        SetSignalInterval(node->get_value<unsigned>("signal-interval", fSignalInterval));

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

    bool KTDataAccumulator::AddData(KTTimeSeriesDistData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTTimeSeriesDistData >();
        KTTimeSeriesDistData& accData = accDataStruct.fData->Of<KTTimeSeriesDistData>();
        return CoreAddData(data, accDataStruct, accData);
    }

    bool KTDataAccumulator::AddData(KTFrequencySpectrumDataPolar& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTFrequencySpectrumDataPolar >();
        KTFrequencySpectrumDataPolar& accData = accDataStruct.fData->Of<KTFrequencySpectrumDataPolar>();
        KTFrequencySpectrumVarianceDataPolar& devData = accDataStruct.fData->Of<KTFrequencySpectrumVarianceDataPolar>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTFrequencySpectrumDataFFTW& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTFrequencySpectrumDataFFTW >();
        KTFrequencySpectrumDataFFTW& accData = accDataStruct.fData->Of<KTFrequencySpectrumDataFFTW>();
        KTFrequencySpectrumVarianceDataFFTW& devData = accDataStruct.fData->Of<KTFrequencySpectrumVarianceDataFFTW>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTPowerSpectrumData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTPowerSpectrumData >();
        KTPowerSpectrumData& accData = accDataStruct.fData->Of<KTPowerSpectrumData>();
        KTPowerSpectrumVarianceData& devData = accDataStruct.fData->Of<KTPowerSpectrumVarianceData>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTConvolvedFrequencySpectrumDataPolar& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTConvolvedFrequencySpectrumDataPolar >();
        KTConvolvedFrequencySpectrumDataPolar& accData = accDataStruct.fData->Of<KTConvolvedFrequencySpectrumDataPolar>();
        KTConvolvedFrequencySpectrumVarianceDataPolar& devData = accDataStruct.fData->Of<KTConvolvedFrequencySpectrumVarianceDataPolar>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTConvolvedFrequencySpectrumDataFFTW& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTConvolvedFrequencySpectrumDataFFTW >();
        KTConvolvedFrequencySpectrumDataFFTW& accData = accDataStruct.fData->Of<KTConvolvedFrequencySpectrumDataFFTW>();
        KTConvolvedFrequencySpectrumVarianceDataFFTW& devData = accDataStruct.fData->Of<KTConvolvedFrequencySpectrumVarianceDataFFTW>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTConvolvedPowerSpectrumData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTConvolvedPowerSpectrumData >();
        KTConvolvedPowerSpectrumData& accData = accDataStruct.fData->Of<KTConvolvedPowerSpectrumData>();
        KTConvolvedPowerSpectrumVarianceData& devData = accDataStruct.fData->Of<KTConvolvedPowerSpectrumVarianceData>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
        {
            remainingFrac -= fAveragingFrac;
        }
        //KTDEBUG(avlog, "averaging frac: " << fAveragingFrac << "    remaining frac: " << remainingFrac);

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesReal* dataTS = static_cast< KTTimeSeriesReal* >(data.GetTimeSeries(iComponent));
                KTTimeSeriesReal* newTS = new KTTimeSeriesReal(0., dataTS->GetNTimeBins(), dataTS->GetRangeMin(), dataTS->GetRangeMax());
                accData.SetTimeSeries(newTS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

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
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
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
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
        {
            remainingFrac -= fAveragingFrac;
        }

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesFFTW* dataTS = static_cast< KTTimeSeriesFFTW* >(data.GetTimeSeries(iComponent));
                KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW({0., 0.}, dataTS->GetNTimeBins(), dataTS->GetRangeMin(), dataTS->GetRangeMax());
                accData.SetTimeSeries(newTS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

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
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avTS)(iBin)[0] = (*avTS)(iBin)[0] * remainingFrac + (*newTS)(iBin)[0] * fAveragingFrac;
                (*avTS)(iBin)[1] = (*avTS)(iBin)[1] * remainingFrac + (*newTS)(iBin)[1] * fAveragingFrac;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTTimeSeriesDistData& data, Accumulator& accDataStruct, KTTimeSeriesDistData& accData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
        {
            remainingFrac -= fAveragingFrac;
        }

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesDist* dataFS = data.GetTimeSeriesDist(iComponent);
                KTTimeSeriesDist* newFS = new KTTimeSeriesDist(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                newFS->operator*=(double(0.));
                accData.SetTimeSeriesDist(newFS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

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
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin) = (*avSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataPolarCore& accData, KTFrequencySpectrumVarianceDataCore& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
        {
            remainingFrac -= fAveragingFrac;
        }

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTFrequencySpectrumPolar* dataFS = data.GetSpectrumPolar(iComponent);

                KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                KTFrequencySpectrumVariance* newVarFS = new KTFrequencySpectrumVariance(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                
                newFS->SetNTimeBins(dataFS->GetNTimeBins());
                newFS->operator*=(double(0.));
                newVarFS->operator*=(double(0.));

                accData.SetSpectrum(newFS, iComponent);
                devData.SetSpectrum(newVarFS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrumPolar(0)->size();
        if (arraySize != accData.GetSpectrumPolar(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetSpectrum(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpect = data.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumPolar* avSpect = accData.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumVariance* varSpect = devData.GetSpectrum(iComponent);
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin) = (*avSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
                (*varSpect)(iBin) = (*varSpect)(iBin) * remainingFrac + (*newSpect)(iBin).abs() * (*newSpect)(iBin).abs() * fAveragingFrac;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataFFTWCore& accData, KTFrequencySpectrumVarianceDataCore& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
        {
            remainingFrac -= fAveragingFrac;
        }

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTFrequencySpectrumFFTW* dataFS = data.GetSpectrumFFTW(iComponent);
                
                KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                KTFrequencySpectrumVariance* newVarFS = new KTFrequencySpectrumVariance(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                
                newFS->SetNTimeBins(dataFS->GetNTimeBins());
                newFS->operator*=(double(0.));
                newVarFS->operator*=(double(0.));

                accData.SetSpectrum(newFS, iComponent);
                devData.SetSpectrum(newVarFS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrumFFTW(0)->size();
        if (arraySize != accData.GetSpectrumFFTW(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetSpectrum(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpect = data.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumFFTW* avSpect = accData.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumVariance* varSpect = devData.GetSpectrum(iComponent);
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin)[0] = (*avSpect)(iBin)[0] * remainingFrac + (*newSpect)(iBin)[0] * fAveragingFrac;
                (*avSpect)(iBin)[1] = (*avSpect)(iBin)[1] * remainingFrac + (*newSpect)(iBin)[1] * fAveragingFrac;

                (*varSpect)(iBin) = (*varSpect)(iBin) * remainingFrac + ((*newSpect)(iBin)[0] * (*newSpect)(iBin)[0] + (*newSpect)(iBin)[1] * (*newSpect)(iBin)[1]) * fAveragingFrac;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTPowerSpectrumDataCore& data, Accumulator& accDataStruct, KTPowerSpectrumDataCore& accData, KTFrequencySpectrumVarianceDataCore& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
        {
            remainingFrac -= fAveragingFrac;
        }
        KTDEBUG(avlog, "Accumulating a power spectrum; remainingFrac = " << remainingFrac << "   fAveragingFrac = " << fAveragingFrac);

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);

            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTPowerSpectrum* dataPS = data.GetSpectrum(iComponent);

                KTPowerSpectrum* newPS = new KTPowerSpectrum(dataPS->size(), dataPS->GetRangeMin(), dataPS->GetRangeMax());
                KTFrequencySpectrumVariance* newVarPS = new KTFrequencySpectrumVariance(dataPS->size(), dataPS->GetRangeMin(), dataPS->GetRangeMax());

                newPS->operator*=(double(0.));
                newVarPS->operator*=(double(0.));

                accData.SetSpectrum(newPS, iComponent);
                devData.SetSpectrum(newVarPS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrum(0)->size();
        if (arraySize != accData.GetSpectrum(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetSpectrum(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* newSpect = data.GetSpectrum(iComponent);
            KTPowerSpectrum* avSpect = accData.GetSpectrum(iComponent);
            KTFrequencySpectrumVariance* varSpect = devData.GetSpectrum(iComponent);
            avSpect->SetMode(newSpect->GetMode());
            double avCache = 0.;
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                avCache = (*avSpect)(iBin);
                (*avSpect)(iBin) = avCache * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
                (*varSpect)(iBin) = (*varSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * (*newSpect)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }


    bool KTDataAccumulator::AccumulatorType< KTTimeSeriesData >::Finalize()
    {
        double scale = fAccumulatorSize == 0 ? 1. / (double)(fSliceHeader.GetSliceNumber()) : 1.;
        KTDEBUG(avlog, "Scaling time series by " << scale);
        unsigned nComponents = fDataType.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeries* avSpect = fDataType.GetTimeSeries(iComponent);
            avSpect->Scale(scale);
        }
        return true;
    }

    bool KTDataAccumulator::AccumulatorType< KTTimeSeriesDistData >::Finalize()
    {
        double scale = fAccumulatorSize == 0 ? 1. / (double)(fSliceHeader.GetSliceNumber()) : 1.;
        KTDEBUG(avlog, "Scaling time series dist by " << scale);
        unsigned nComponents = fDataType.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesDist* avSpect = fDataType.GetTimeSeriesDist(iComponent);
            avSpect->Scale(scale);
        }
        return true;
    }

    bool KTDataAccumulator::AccumulatorType< KTFrequencySpectrumDataPolar >::Finalize()
    {
        double scale = fAccumulatorSize == 0 ? 1. / (double)(fSliceHeader.GetSliceNumber()) : 1.;
        KTDEBUG(avlog, "Scaling freq spect polar by " << scale);
        unsigned nComponents = fDataType.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* avSpect = fDataType.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumVariance* varSpect = fVarDataType.GetSpectrum(iComponent);

            avSpect->Scale(scale);
            varSpect->Scale(scale);
            unsigned nBins = varSpect->GetNFrequencyBins();
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                (*varSpect)(iBin) = (*varSpect)(iBin) - (*avSpect)(iBin).abs() * (*avSpect)(iBin).abs();
            }
        }
        return true;
    }

    bool KTDataAccumulator::AccumulatorType< KTFrequencySpectrumDataFFTW >::Finalize()
    {
        double scale = fAccumulatorSize == 0 ? 1. / (double)(fSliceHeader.GetSliceNumber()) : 1.;
        KTDEBUG(avlog, "Scaling freq spect fftw by " << scale);
        unsigned nComponents = fDataType.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* avSpect = fDataType.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumVariance* varSpect = fVarDataType.GetSpectrum(iComponent);

            avSpect->Scale(scale);
            varSpect->Scale(scale);
            unsigned nBins = varSpect->GetNFrequencyBins();
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                (*varSpect)(iBin) = (*varSpect)(iBin) - (*avSpect)(iBin)[0] * (*avSpect)(iBin)[0] - (*avSpect)(iBin)[1] * (*avSpect)(iBin)[1];
            }
        }
        return true;
    }

    bool KTDataAccumulator::AccumulatorType< KTPowerSpectrumData >::Finalize()
    {
        double scale = fAccumulatorSize == 0 ? 1. / (double)(fSliceHeader.GetSliceNumber()) : 1.;
        KTDEBUG(avlog, "Scaling power spect by " << scale);
        unsigned nComponents = fDataType.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* avSpect = fDataType.GetSpectrum(iComponent);
            KTFrequencySpectrumVariance* varSpect = fVarDataType.GetSpectrum(iComponent);

            avSpect->Scale(scale);
            varSpect->Scale(scale);
            unsigned nBins = varSpect->GetNFrequencyBins();
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                (*varSpect)(iBin) = (*varSpect)(iBin) - (*avSpect)(iBin) * (*avSpect)(iBin);
            }
        }
        return true;
    }

    bool KTDataAccumulator::AccumulatorType< KTConvolvedFrequencySpectrumDataPolar >::Finalize()
    {
        double scale = fAccumulatorSize == 0 ? 1. / (double)(fSliceHeader.GetSliceNumber()) : 1.;
        KTDEBUG(avlog, "Scaling conv freq spect polar by " << scale);
        unsigned nComponents = fDataType.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* avSpect = fDataType.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumVariance* varSpect = fVarDataType.GetSpectrum(iComponent);

            avSpect->Scale(scale);
            varSpect->Scale(scale);
            unsigned nBins = varSpect->GetNFrequencyBins();
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                (*varSpect)(iBin) = (*varSpect)(iBin) - (*avSpect)(iBin).abs() * (*avSpect)(iBin).abs();
            }
        }
        return true;
    }

    bool KTDataAccumulator::AccumulatorType< KTConvolvedFrequencySpectrumDataFFTW >::Finalize()
    {
        double scale = fAccumulatorSize == 0 ? 1. / (double)(fSliceHeader.GetSliceNumber()) : 1.;
        KTDEBUG(avlog, "Scaling conv freq spect fftw by " << scale);
        unsigned nComponents = fDataType.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* avSpect = fDataType.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumVariance* varSpect = fVarDataType.GetSpectrum(iComponent);

            avSpect->Scale(scale);
            varSpect->Scale(scale);
            unsigned nBins = varSpect->GetNFrequencyBins();
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                (*varSpect)(iBin) = (*varSpect)(iBin) - (*avSpect)(iBin)[0] * (*avSpect)(iBin)[0] - (*avSpect)(iBin)[1] * (*avSpect)(iBin)[1];
            }
        }
        return true;
    }

    bool KTDataAccumulator::AccumulatorType< KTConvolvedPowerSpectrumData >::Finalize()
    {
        double scale = fAccumulatorSize == 0 ? 1. / (double)(fSliceHeader.GetSliceNumber()) : 1.;
        KTDEBUG(avlog, "Scaling conv power spect by " << scale);
        unsigned nComponents = fDataType.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* avSpect = fDataType.GetSpectrum(iComponent);
            KTFrequencySpectrumVariance* varSpect = fVarDataType.GetSpectrum(iComponent);

            avSpect->Scale(scale);
            varSpect->Scale(scale);
            unsigned nBins = varSpect->GetNFrequencyBins();
            for (unsigned iBin = 0; iBin < nBins; ++iBin)
            {
                (*varSpect)(iBin) = (*varSpect)(iBin) - (*avSpect)(iBin) * (*avSpect)(iBin);
            }
        }
        return true;
    }

} /* namespace Katydid */
