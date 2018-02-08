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
#include "KTLogger.hh"
#include "KTSlot.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesDistData.hh"

#include <map>
#include <typeinfo>

namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTDataAccumulator.hh");

    class KTConvolvedPowerSpectrumData;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTPowerSpectrumData;
    class KTPowerSpectrumDataCore;
    class KTTimeSeriesData;

    /*!
     @class KTDataAccumulator
     @author N. S. Oblath

     @brief Averages data objects

     @details
     The data accumulator running average of the data from a series of slices.
     The size of the accumulator is the number of slices that are averaged together (configuration parameter "number-to-average").
     If the accumulator size is 0, the data from all of the slices received will be summed together, and averaged at the end.

     For slice i, the data received by the accumulator is D_i, and the average A_i is:
       A_i = A_(i-1) * (S-1)/S + D_i / S
     where S is the size of the accumulator.
     
     Note that this method of calculating A_i gives an effective average of S slices:  
     At slice i, the amount removed from the previous sum is (1 / S) * A_(i-1) instead of (1 / S) * D_(i-N).
     The effective average will be closest to the true average for large N.
     
     The signal interval is how often the output signal will be emitted.  
     If the signal interval is 0, there will be no slice signals.

     Configuration name: "data-accumulator"

     Available configuration options:
     - "number-to-average": unsigned -- Number of slices to average; 0 will sum all slices together, and average at the end
     - "signal-interval": unsigned -- Number of slices between signaling; set to 0 to stop slice signals

     Slots:
     - "ts": void (Nymph::KTDataPtr) -- add to the ts sum; Requires KTTimeSeriesData; Emits signal "ts"
     - "ts-dist": void (Nymph::KTDataPtr) -- add to the ts-dist sum; Requires KTTimeSeriesDistData; Emits signal "ts-dist"
     - "fs-polar": void (Nymph::KTDataPtr) -- add to the fs-polar sum; Requires KTFrequencySpectrumPolar; Emits signal "fs-polar"
     - "fs-fftw": void (Nymph::KTDataPtr) -- add to the fs-fftw sum; Requires KTFrequencySpectrumFFTW; Emits signal "fs-fftw"
     - "conv-ps": void (Nymph::KTDataPtr) -- add to the ps sum (PS or PSD); Requires KTConvolvedPowerSpectrumData; Emits signal "conv-ps"
     - "ps": void (Nymph::KTDataPtr) -- add to the ps sum (PS or PSD); Requires KTPowerSpectrumData; Emits signal "ps"

     Signals:
     - "ts": void (Nymph::KTDataPtr) -- emitted when the ts sum is updated; guarantees KTTimeSeriesData
     - "ts-dist": void (Nymph::KTDataPtr) -- emitted when the ts-dist sum is updated; guarantees KTTimeSeriesDistData
     - "fs-polar": void (Nymph::KTDataPtr) -- emitted when the fs-polar sum is updated; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw": void (Nymph::KTDataPtr) -- emitted when the fs-fftw sum is updated; guarantees KTFrequencySpectrumDataFFTW
     - "conv-ps": void (Nymph::KTDataPtr) -- emitted when the conv-ps sum is updated; guarantees KTConvolvedPowerSpectrumData
     - "ps": void (Nymph::KTDataPtr) -- emitted when the ps sum is updated; guarantees KTPowerSpectrumData
     - "ts-variance": void (Nymph::KTDataPtr) -- emitted when the ts sum is updated; guarantees KTTimeSeriesData
     - "ts-dist-variance": void (Nymph::KTDataPtr) -- emitted when the ts-dist sum is updated; guarantees KTTimeSeriesDistData
     - "fs-polar-variance": void (Nymph::KTDataPtr) -- emitted when the fs-polar sum is updated; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw-variance": void (Nymph::KTDataPtr) -- emitted when the fs-fftw sum is updated; guarantees KTFrequencySpectrumDataFFTW
     - "conv-ps-variance": void (Nymph::KTDataPtr) -- emitted when the conv-ps sum is updated; guarantees KTConvolvedPowerSpectrumData
     - "ps-variance": void (Nymph::KTDataPtr) -- emitted when the ps sum is updated; guarantees KTPowerSpectrumData
     - "ts-finished": void (Nymph::KTDataPtr) -- emitted when the <finish> slot is called; guarantees KTTimeSeriesData
     - "ts-dist-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTTimeSeriesDistData
     - "fs-polar-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTFrequencySpectrumDataFFTW
     - "core-ps-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTConvolvedPowerSpectrumData
     - "ps-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTPowerSpectrumData
    */

    class KTDataAccumulator : public Nymph::KTProcessor
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
                Nymph::KTDataPtr fData;
                Nymph::KTDataPtr fVarianceData;
                KTSliceHeader& fSliceHeader;

                void IncrementSlice();
                Accumulator() : fData(new Nymph::KTData()), fSliceHeader(fData->Of<KTSliceHeader>())
                {
                }
                unsigned GetSliceNumber() const
                {
                    return fSliceHeader.GetSliceNumber();
                }
                void BumpSliceNumber()
                {
                    fSliceHeader.SetSliceNumber(fSliceHeader.GetSliceNumber() + 1);
                    return;
                }
            };

            typedef std::map< const std::type_info*, Accumulator > AccumulatorMap;
            typedef AccumulatorMap::iterator AccumulatorMapIt;

            struct SignalSet
            {
                    unsigned fSignalCount;
                    Nymph::KTSignalData* fAccumulatingSignal;
                    Nymph::KTSignalData* fVarianceSignal;
                    Nymph::KTSignalData* fFinishedSignal;
                    SignalSet(Nymph::KTSignalData* accSig, Nymph::KTSignalData* varSig, Nymph::KTSignalData* finishedSig) :
                        fSignalCount(0),
                        fAccumulatingSignal(accSig),
                        fVarianceSignal(varSig),
                        fFinishedSignal(finishedSig)
                    {}
            };
            typedef std::map< const std::type_info*, SignalSet > SignalMap;
            typedef SignalMap::iterator SignalMapIt;
            typedef SignalMap::value_type SignalMapValue;

        public:
            KTDataAccumulator(const std::string& name = "data-accumulator");
            virtual ~KTDataAccumulator();

            bool Configure(const scarab::param_node* node);

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
            bool AddData(KTTimeSeriesDistData& data);
            bool AddData(KTFrequencySpectrumDataPolar& data);
            bool AddData(KTFrequencySpectrumDataFFTW& data);
            bool AddData(KTConvolvedPowerSpectrumData& data);
            bool AddData(KTPowerSpectrumData& data);

            const AccumulatorMap& GetAccumulators() const;
            template< class XDataType >
            const Accumulator& GetAccumulator() const;

        private:
            template< class XDataType >
            Accumulator& GetOrCreateAccumulator();

            bool CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData, KTTimeSeriesData& devData);
            bool CoreAddTSDataFFTW(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData, KTTimeSeriesData& devData);

            bool CoreAddData(KTTimeSeriesDistData& data, Accumulator& accDataStruct, KTTimeSeriesDistData& accData, KTTimeSeriesDistData& devData);

            bool CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataPolarCore& accData, KTFrequencySpectrumDataPolarCore& devData);
            bool CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataFFTWCore& accData, KTFrequencySpectrumDataFFTWCore& devData);

            bool CoreAddData(KTPowerSpectrumDataCore& data, Accumulator& accDataStruct, KTPowerSpectrumDataCore& accData, KTPowerSpectrumDataCore& devData);

            bool Scale(KTTimeSeriesData& data, KTSliceHeader& header);
            bool Scale(KTTimeSeriesDistData& data, KTSliceHeader& header);
            bool Scale(KTFrequencySpectrumDataPolar& data, KTSliceHeader& header);
            bool Scale(KTFrequencySpectrumDataFFTW& data, KTSliceHeader& header);
            bool Scale(KTConvolvedPowerSpectrumData& data, KTSliceHeader& header);
            bool Scale(KTPowerSpectrumData& data, KTSliceHeader& header);

            AccumulatorMap fDataMap;
            mutable Accumulator* fLastAccumulatorPtr;


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSSignal;
            Nymph::KTSignalData fTSDistSignal;
            Nymph::KTSignalData fFSPolarSignal;
            Nymph::KTSignalData fFSFFTWSignal;
            Nymph::KTSignalData fConvPSSignal;
            Nymph::KTSignalData fPSSignal;

            Nymph::KTSignalData fTSVarianceSignal;
            Nymph::KTSignalData fTSDistVarianceSignal;
            Nymph::KTSignalData fFSPolarVarianceSignal;
            Nymph::KTSignalData fFSFFTWVarianceSignal;
            Nymph::KTSignalData fConvPSVarianceSignal;
            Nymph::KTSignalData fPSVarianceSignal;

            Nymph::KTSignalData fTSFinishedSignal;
            Nymph::KTSignalData fTSDistFinishedSignal;
            Nymph::KTSignalData fFSPolarFinishedSignal;
            Nymph::KTSignalData fFSFFTWFinishedSignal;
            Nymph::KTSignalData fConvPSFinishedSignal;
            Nymph::KTSignalData fPSFinishedSignal;

            SignalMap fSignalMap;

            //***************
            // Slots
            //***************

        private:
            template< class XDataType >
            void SlotFunction(Nymph::KTDataPtr data);

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
        if (fAccumulatorSize == 0)
        {
            fAveragingFrac = 1.;
        }
        else
        {
            fAveragingFrac = 1. / (double)fAccumulatorSize;
        }
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
    void KTDataAccumulator::SlotFunction(Nymph::KTDataPtr data)
    {
        // Standard data slot pattern:
        // Check to ensure that the required data type is present
        if (! data->Has< XDataType >())
        {
            KTERROR(avlog_hh, "Data not found with type <" << typeid(XDataType).name() << ">");
            return;
        }
        // Call the function
        if (! AddData(data->Of< XDataType >()))
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing data with type <" << typeid(XDataType).name() << ">");
            return;
        }
        // If there's a signal pointer, emit the signal
        SignalMapIt sigIt = fSignalMap.find(&typeid(XDataType));
        if (sigIt != fSignalMap.end())
        {
            sigIt->second.fSignalCount++;
            unsigned sigCount = sigIt->second.fSignalCount;
            if (sigCount == fSignalInterval)
            {
                (*sigIt->second.fAccumulatingSignal)(fLastAccumulatorPtr->fData);
                (*sigIt->second.fVarianceSignal)(fLastAccumulatorPtr->fVarianceData);
                sigIt->second.fSignalCount = 0;
            }
            if (data->GetLastData())
            {
                KTDEBUG(avlog_hh, "Emitting last-data signal");
                if (! Scale(fLastAccumulatorPtr->fData->Of< XDataType >(), fLastAccumulatorPtr->fSliceHeader))
                {
                    KTERROR(avlog_hh, "Something went wrong while scaling data with type <" << typeid(XDataType).name() << ">");
                }
                if (! Scale(fLastAccumulatorPtr->fVarianceData->Of< XDataType >(), fLastAccumulatorPtr->fSliceHeader))
                {
                    KTERROR(avlog_hh, "Something went wrong while scaling data with type <" << typeid(XDataType).name() << ">");
                }
                (*sigIt->second.fFinishedSignal)(fLastAccumulatorPtr->fData);
                (*sigIt->second.fVarianceSignal)(fLastAccumulatorPtr->fVarianceData);
            }
        }
        return;
    }



} /* namespace Katydid */
#endif /* KTDATAACCUMULATOR_HH_ */
