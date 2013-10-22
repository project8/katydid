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

#include <boost/shared_ptr.hpp>

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
     - "number-to-average": UInt_t -- Number of slices to average
     - "signal-interval": UInt_t -- Number of slices between signaling

     Slots:
     - "ts": void (boost::shared_ptr<KTData>) -- add to the ts sum; Requires KTTimeSeriesData; Emits signal "ts"
     - "fs-polar": void (boost::shared_ptr<KTData>) -- add to the fs-polar sum; Requires KTFrequencySpectrumPolar; Emits signal "fs-polar"
     - "fs-fftw": void (boost::shared_ptr<KTData>) -- add to the fs-fftw sum; Requires KTFrequencySpectrumFFTW; Emits signal "fs-fftw"

     Signals:
     - "ts": void (boost::shared_ptr<KTData>) -- emitted when the ts sum is updated; guarantees KTTimeSeriesData
     - "fs-polar": void (boost::shared_ptr<KTData>) -- emitted when the fs-polar sum is updated; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw": void (boost::shared_ptr<KTData>) -- emitted when the fs-fftw sum is updated; guarantees KTFrequencySpectrumDataFFTW
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

        private:
            struct Accumulator
            {
                UInt_t fCount;
                UInt_t fSignalCount;
                boost::shared_ptr< KTData > fData;
            };

            typedef std::map< const std::type_info*, Accumulator > AccumulatorMap;
            typedef AccumulatorMap::iterator AccumulatorMapIt;

        public:
            KTDataAccumulator(const std::string& name = "data-averager");
            virtual ~KTDataAccumulator();

            Bool_t Configure(const KTPStoreNode* node);

            UInt_t GetAccumulatorSize() const;
            Double_t GetAveragingFrac() const;
            void SetAccumulatorSize(UInt_t size);

            UInt_t GetSignalInterval() const;
            void SetSignalInterval(UInt_t interval);

        private:
            UInt_t fAccumulatorSize;
            Double_t fAveragingFrac;
            UInt_t fSignalInterval;

        public:
            Bool_t AddData(KTTimeSeriesData& data);
            Bool_t AddData(KTFrequencySpectrumDataPolar& data);
            Bool_t AddData(KTFrequencySpectrumDataFFTW& data);

        private:
            Bool_t CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& avDataStruct, KTTimeSeriesData& avData);
            Bool_t CoreAddTSDataFFTW(KTTimeSeriesData& data, Accumulator& avDataStruct, KTTimeSeriesData& avData);

            Bool_t CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& avDataStruct, KTFrequencySpectrumDataPolarCore& avData);
            Bool_t CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& avDataStruct, KTFrequencySpectrumDataFFTWCore& avData);

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

    inline UInt_t KTDataAccumulator::GetAccumulatorSize() const
    {
        return fAccumulatorSize;
    }

    inline Double_t KTDataAccumulator::GetAveragingFrac() const
    {
        return fAveragingFrac;
    }

    inline void KTDataAccumulator::SetAccumulatorSize(UInt_t size)
    {
        fAccumulatorSize = size;
        fAveragingFrac = 1. / (Double_t)fAccumulatorSize;
        return;
    }

    inline UInt_t KTDataAccumulator::GetSignalInterval() const
    {
        return fSignalInterval;
    }

    inline void KTDataAccumulator::SetSignalInterval(UInt_t interval)
    {
        fSignalInterval = interval;
        return;
    }

} /* namespace Katydid */
#endif /* KTDATAACCUMULATOR_HH_ */
