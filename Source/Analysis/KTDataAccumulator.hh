/*
 * KTDataAccumulator.hh
 *
 *  Created on: Oct 21, 2013
 *      Author: nsoblath
 */

#ifndef KTDATAACCUMULATOR_HH_
#define KTDATAACCUMULATOR_HH_

#include "KTProcessor.hh"

#include "KTData.hh"
#include "KTSlot.hh"

#include <map>
#include <typeinfo>

namespace Katydid
{
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTPStoreNode;
    class KTTimeSeriesData;

    /*!
     @class KTDataAccumulator
     @author N. S. Oblath

     @brief Averages data objects

     @details


     Configuration name: "data-averager"

     Available configuration options:
     - "number-to-average": unsigned -- Number of slices to average
     - "signal-interval": unsigned -- (not currently in use) Number of slices between signaling

     Slots:
     - "ts": void (KTDataPtr) -- add to the ts sum; Requires KTTimeSeriesData; Emits signal "ts"
     - "fs-polar": void (KTDataPtr) -- add to the fs-polar sum; Requires KTFrequencySpectrumPolar; Emits signal "fs-polar"
     - "fs-fftw": void (KTDataPtr) -- add to the fs-fftw sum; Requires KTFrequencySpectrumFFTW; Emits signal "fs-fftw"

     Signals:
     - "ts": void (KTDataPtr) -- emitted when the ts sum is updated; guarantees KTTimeSeriesData
     - "fs-polar": void (KTDataPtr) -- emitted when the fs-polar sum is updated; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw": void (KTDataPtr) -- emitted when the fs-fftw sum is updated; guarantees KTFrequencySpectrumDataFFTW
    */

    class KTDataAccumulator : public KTProcessor
    {
        public:
            struct CompareTypeInfo
            {
                bool operator() (const std::type_info* lhs, const std::type_info* rhs)
                {
                    return lhs->before(*rhs);
                }
            };

            struct Accumulator
            {
                unsigned fCount;
                unsigned fSignalCount;
                KTDataPtr fData;
                Accumulator() : fCount(0), fSignalCount(0), fData(new KTData())
                {}
            };

            typedef std::map< const std::type_info*, Accumulator > AccumulatorMap;
            typedef AccumulatorMap::iterator AccumulatorMapIt;

        public:
            KTDataAccumulator(const std::string& name = "data-averager");
            virtual ~KTDataAccumulator();

            bool Configure(const KTPStoreNode* node);

            unsigned GetAccumulatorSize() const;
            double GetAveragingFrac() const;
            void SetAccumulatorSize(unsigned size);

            unsigned GetSignalInterval() const;
            void SetSignalInterval(unsigned interval);

        private:
            unsigned fAccumulatorSize;
            double fAveragingFrac;
            unsigned fSignalInterval;

        public:
            bool AddData(KTTimeSeriesData& data);
            bool AddData(KTFrequencySpectrumDataPolar& data);
            bool AddData(KTFrequencySpectrumDataFFTW& data);

            const AccumulatorMap& GetAccumulators() const;
            template< class XDataType >
            const Accumulator& GetAccumulator() const;

        private:
            template< class XDataType >
            Accumulator& GetOrCreateAccumulator();

            bool CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& avDataStruct, KTTimeSeriesData& avData);
            bool CoreAddTSDataFFTW(KTTimeSeriesData& data, Accumulator& avDataStruct, KTTimeSeriesData& avData);

            bool CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& avDataStruct, KTFrequencySpectrumDataPolarCore& avData);
            bool CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& avDataStruct, KTFrequencySpectrumDataFFTWCore& avData);

            AccumulatorMap fDataMap;


            //***************
            // Signals
            //***************

        private:
            KTSignalData fTSSignal;
            KTSignalData fFSPolarSignal;
            KTSignalData fFSFFTWSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTTimeSeriesData > fTSSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataPolar > fFSPolarSlot;
            KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFSFFTWSlot;
    };

    inline unsigned KTDataAccumulator::GetAccumulatorSize() const
    {
        return fAccumulatorSize;
    }

    inline double KTDataAccumulator::GetAveragingFrac() const
    {
        return fAveragingFrac;
    }

    inline void KTDataAccumulator::SetAccumulatorSize(unsigned size)
    {
        fAccumulatorSize = size;
        fAveragingFrac = 1. / (double)fAccumulatorSize;
        return;
    }

    inline unsigned KTDataAccumulator::GetSignalInterval() const
    {
        return fSignalInterval;
    }

    inline void KTDataAccumulator::SetSignalInterval(unsigned interval)
    {
        fSignalInterval = interval;
        return;
    }

    inline const KTDataAccumulator::AccumulatorMap& KTDataAccumulator::GetAccumulators() const
    {
        return fDataMap;
    }

    template< class XDataType >
    const KTDataAccumulator::Accumulator& KTDataAccumulator::GetAccumulator() const
    {
        return fDataMap.at(&typeid(XDataType));
    }

    template< class XDataType >
    KTDataAccumulator::Accumulator& KTDataAccumulator::GetOrCreateAccumulator()
    {
        return fDataMap[&typeid(XDataType)];
    }


} /* namespace Katydid */
#endif /* KTDATAACCUMULATOR_HH_ */
