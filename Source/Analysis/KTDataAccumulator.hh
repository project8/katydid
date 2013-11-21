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
     - "signal-interval": UInt_t -- (not currently in use) Number of slices between signaling

     Slots:
     - "ts": void (boost::shared_ptr<KTData>) -- add to the ts sum; Requires KTTimeSeriesData; Emits signal "ts"
     - "fs-polar": void (boost::shared_ptr<KTData>) -- add to the fs-polar sum; Requires KTFrequencySpectrumPolar; Emits signal "fs-polar"
     - "fs-fftw": void (boost::shared_ptr<KTData>) -- add to the fs-fftw sum; Requires KTFrequencySpectrumFFTW; Emits signal "fs-fftw"
     - "finish": void () -- call all slots for which data has accumulated

     Signals:
     - "ts": void (boost::shared_ptr<KTData>) -- emitted when the ts sum is updated; guarantees KTTimeSeriesData
     - "fs-polar": void (boost::shared_ptr<KTData>) -- emitted when the fs-polar sum is updated; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw": void (boost::shared_ptr<KTData>) -- emitted when the fs-fftw sum is updated; guarantees KTFrequencySpectrumDataFFTW
     - "ts-finished": void (boost::shared_ptr<KTData>) -- emitted when the <finish> slot is called; guarantees KTTimeSeriesData
     - "fs-polar-finished": void (boost::shared_ptr<KTData>) -- emitted when the <finish> slot is called; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw-finished": void (boost::shared_ptr<KTData>) -- emitted when the <finish> slot is called; guarantees KTFrequencySpectrumDataFFTW
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
                UInt_t fCount;
                UInt_t fSignalCount;
                boost::shared_ptr< KTData > fData;
                Accumulator() : fCount(0), fSignalCount(0), fData(new KTData())
                {}
            };

            typedef std::map< const std::type_info*, Accumulator > AccumulatorMap;
            typedef AccumulatorMap::iterator AccumulatorMapIt;

            struct SignalSet
            {
                    KTSignalData* fAccumulatingSignal;
                    KTSignalData* fFinishedSignal;
                    SignalSet(KTSignalData* accSig, KTSignalData* finishedSig) :
                        fAccumulatingSignal(accSig),
                        fFinishedSignal(finishedSig)
                    {}
            };
            typedef std::map< const std::type_info*, SignalSet > SignalMap;
            typedef SignalMap::iterator SignalMapIt;
            typedef SignalMap::value_type SignalMapValue;

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

            const AccumulatorMap& GetAccumulators() const;
            template< class XDataType >
            const Accumulator& GetAccumulator() const;

        private:
            template< class XDataType >
            Accumulator& GetOrCreateAccumulator();

            Bool_t CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& avDataStruct, KTTimeSeriesData& avData);
            Bool_t CoreAddTSDataFFTW(KTTimeSeriesData& data, Accumulator& avDataStruct, KTTimeSeriesData& avData);

            Bool_t CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& avDataStruct, KTFrequencySpectrumDataPolarCore& avData);
            Bool_t CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& avDataStruct, KTFrequencySpectrumDataFFTWCore& avData);

            AccumulatorMap fDataMap;
            mutable Accumulator* fLastAccumulatorPtr;


            //***************
            // Signals
            //***************

        private:
            KTSignalData fTSSignal;
            KTSignalData fFSPolarSignal;
            KTSignalData fFSFFTWSignal;

            KTSignalData fTSFinishedSignal;
            KTSignalData fFSPolarFinishedSignal;
            KTSignalData fFSFFTWFinishedSignal;

            SignalMap fSignalMap;

            //***************
            // Slots
            //***************

        private:
            KTSlotNoArg< void () > fFinishAccumulationSlot;

            template< class XDataType >
            void SlotFunction(boost::shared_ptr< KTData > data);

            void CallAllSlots();

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

    inline const KTDataAccumulator::AccumulatorMap& KTDataAccumulator::GetAccumulators() const
    {
        return fDataMap;
    }

    template< class XDataType >
    const KTDataAccumulator::Accumulator& KTDataAccumulator::GetAccumulator() const
    {
        fLastAccumulatorPtr = const_cast< Accumulator* >(&fDataMap.at(&typeid(XDataType)));
        return *fLastAccumulatorPtr;
    }

    template< class XDataType >
    KTDataAccumulator::Accumulator& KTDataAccumulator::GetOrCreateAccumulator()
    {
        fLastAccumulatorPtr = &fDataMap[&typeid(XDataType)];
        return *fLastAccumulatorPtr;
    }

    template< class XDataType >
    void KTDataAccumulator::SlotFunction(boost::shared_ptr< KTData > data)
    {
        // Standard data slot pattern:
        // Check to ensure that the required data type is present
        if (! data->Has< XDataType >())
        {
            KTERROR(slotlog, "Data not found with type <" << typeid(XDataType).name() << ">");
            return;
        }
        // Call the function
        if (! AddData(data->Of< XDataType >()))
        {
            KTERROR(slotlog, "Something went wrong while analyzing data with type <" << typeid(XDataType).name() << ">");
            return;
        }
        // If there's a signal pointer, emit the signal
        SignalMapIt sigIt = fSignalMap.find(&typeid(XDataType));
        if (sigIt != fSignalMap.end())
        {
            (*sigIt->second.fAccumulatingSignal)(fLastAccumulatorPtr->fData);
        }
        return;
    }



} /* namespace Katydid */
#endif /* KTDATAACCUMULATOR_HH_ */
