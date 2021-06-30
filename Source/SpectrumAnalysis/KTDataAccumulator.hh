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
#include "KTDemangle.hh"
#include "KTLogger.hh"
#include "KTSlot.hh"
#include "KTSliceHeader.hh"

#include "KTConvolvedSpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesDistData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#include <map>
#include <typeinfo>

namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTDataAccumulator.hh");

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

     When the size of the accumulator is not specified, and during accumulation,
     the accumulating data object will contain the sum of the accumulated data
     and, where applicable, the variance-accumulating data object will contain the sum of the squares
     of the accumulated data.  This means that when the mid-accumulation signals are emitted, if you
     want to get the mean of the data, divide by the number of slices accumulated, and if you want the
     variance, subtract the square of the mean and divide by the number of slices accumulated.

     Prior to emitting the "finished" signals, the means and variances are properly calculated.

     Configuration name: "data-accumulator"

     Available configuration options:
     - "number-to-average": unsigned -- Number of slices to average; 0 will sum all slices together, and average at the end
     - "signal-interval": unsigned -- Number of slices between signaling; set to 0 to stop slice signals

     Slots:
     - "ts": void (Nymph::KTDataPtr) -- add to the ts sum; Requires KTTimeSeriesData; Emits signal "ts"
     - "ts-dist": void (Nymph::KTDataPtr) -- add to the ts-dist sum; Requires KTTimeSeriesDistData; Emits signal "ts-dist"
     - "fs-polar": void (Nymph::KTDataPtr) -- add to the fs-polar sum; Requires KTFrequencySpectrumPolar; Emits signal "fs-polar"
     - "fs-fftw": void (Nymph::KTDataPtr) -- add to the fs-fftw sum; Requires KTFrequencySpectrumFFTW; Emits signal "fs-fftw"
     - "ps": void (Nymph::KTDataPtr) -- add to the ps sum (PS or PSD); Requires KTPowerSpectrumData; Emits signal "ps"
     - "conv-fs-polar": void (Nymph::KTDataPtr) -- add to the fs-polar sum; Requires KTConvolvedFrequencySpectrumDataPolar; Emits signal "conv-fs-polar"
     - "conv-fs-fftw": void (Nymph::KTDataPtr) -- add to the fs-fftw sum; Requires KTConvolvedFrequencySpectrumDataFFTW; Emits signal "conv-fs-fftw"
     - "conv-ps": void (Nymph::KTDataPtr) -- add to the ps sum (PS or PSD); Requires KTConvolvedPowerSpectrumData; Emits signal "conv-ps"

     Signals:

     - "ts": void (Nymph::KTDataPtr) -- emitted when the ts sum is updated; guarantees KTTimeSeriesData
     - "ts-dist": void (Nymph::KTDataPtr) -- emitted when the ts-dist sum is updated; guarantees KTTimeSeriesDistData
     - "fs-polar": void (Nymph::KTDataPtr) -- emitted when the fs-polar sum is updated; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw": void (Nymph::KTDataPtr) -- emitted when the fs-fftw sum is updated; guarantees KTFrequencySpectrumDataFFTW
     - "conv-ps": void (Nymph::KTDataPtr) -- emitted when the conv-ps sum is updated; guarantees KTConvolvedPowerSpectrumData
     - "ps": void (Nymph::KTDataPtr) -- emitted when the ps sum is updated; guarantees KTPowerSpectrumData

     - "ts-finished": void (Nymph::KTDataPtr) -- emitted when the <finish> slot is called; guarantees KTTimeSeriesData
     - "ts-dist-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTTimeSeriesDistData
     - "fs-polar-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTFrequencySpectrumDataPolar
     - "fs-fftw-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTFrequencySpectrumDataFFTW
     - "ps-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTPowerSpectrumData
     - "conv-fs-polar-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTConvolvedFrequencySpectrumDataPolar
     - "conv-fs-fftw-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTConvolvedFrequencySpectrumDataFFTW
     - "conv-ps-finished": void (Nymph::KTDataPtr) -- emitted when the last data is received; guarantees KTConvolvedPowerSpectrumData
     

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
                KTSliceHeader& fSliceHeader;
                unsigned fAccumulatorSize;

                void IncrementSlice();
                Accumulator() : fData(new Nymph::KTData()), fSliceHeader(fData->Of<KTSliceHeader>()), fAccumulatorSize(0) {}
                virtual ~Accumulator() {}

                unsigned GetSliceNumber() const
                {
                    return fSliceHeader.GetSliceNumber();
                }
                void BumpSliceNumber()
                {
                    fSliceHeader.SetSliceNumber(fSliceHeader.GetSliceNumber() + 1);
                    return;
                }

                virtual bool Finalize() = 0;
            };

            template< class XDataType >
            struct AccumulatorType : Accumulator
            {
                XDataType& fDataType;

                AccumulatorType() : Accumulator(), fDataType(fData->Of<XDataType>()) {}
                virtual ~AccumulatorType() {}

                virtual bool Finalize();
            };

            typedef std::map< const std::type_info*, Accumulator* > AccumulatorMap;
            typedef AccumulatorMap::iterator AccumulatorMapIt;

            struct SignalSet
            {
                    unsigned fSignalCount;
                    Nymph::KTSignalData* fAccumulatingSignal;
                    Nymph::KTSignalData* fFinishedSignal;
                    SignalSet(Nymph::KTSignalData* accSig, Nymph::KTSignalData* finishedSig) :
                        fSignalCount(0),
                        fAccumulatingSignal(accSig),
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
            bool AddData(KTPowerSpectrumData& data);
            bool AddData(KTConvolvedFrequencySpectrumDataPolar& data);
            bool AddData(KTConvolvedFrequencySpectrumDataFFTW& data);
            bool AddData(KTConvolvedPowerSpectrumData& data);

            const AccumulatorMap& GetAccumulators() const;
            template< class XDataType >
            const Accumulator& GetAccumulator() const;
            template< class XDataType >
            Accumulator& GetAccumulatorNonConst() const;

        private:
            template< class XDataType >
            Accumulator& GetOrCreateAccumulator();

            bool CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData);
            bool CoreAddTSDataFFTW(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData);

            bool CoreAddData(KTTimeSeriesDistData& data, Accumulator& accDataStruct, KTTimeSeriesDistData& accData);

            bool CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataPolarCore& accData, KTFrequencySpectrumVarianceDataCore& devData);
            bool CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataFFTWCore& accData, KTFrequencySpectrumVarianceDataCore& devData);

            bool CoreAddData(KTPowerSpectrumDataCore& data, Accumulator& accDataStruct, KTPowerSpectrumDataCore& accData, KTFrequencySpectrumVarianceDataCore& devData);

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
            Nymph::KTSignalData fPSSignal;
            Nymph::KTSignalData fConvFSPolarSignal;
            Nymph::KTSignalData fConvFSFFTWSignal;
            Nymph::KTSignalData fConvPSSignal;

            Nymph::KTSignalData fTSFinishedSignal;
            Nymph::KTSignalData fTSDistFinishedSignal;
            Nymph::KTSignalData fFSPolarFinishedSignal;
            Nymph::KTSignalData fFSFFTWFinishedSignal;
            Nymph::KTSignalData fPSFinishedSignal;
            Nymph::KTSignalData fConvFSPolarFinishedSignal;
            Nymph::KTSignalData fConvFSFFTWFinishedSignal;
            Nymph::KTSignalData fConvPSFinishedSignal;

            SignalMap fSignalMap;

            //***************
            // Slots
            //***************

        private:
            template< class XDataType >
            void SlotFunction(Nymph::KTDataPtr data);

    };

    template<>
    struct KTDataAccumulator::AccumulatorType< KTTimeSeriesData > : Accumulator
    {
        KTTimeSeriesData& fDataType;

        AccumulatorType() : Accumulator(), fDataType(fData->Of<KTTimeSeriesData>()) {}
        virtual ~AccumulatorType() {}

        virtual bool Finalize();
    };

    template<>
    struct KTDataAccumulator::AccumulatorType< KTTimeSeriesDistData > : Accumulator
    {
        KTTimeSeriesDistData& fDataType;

        AccumulatorType() : Accumulator(), fDataType(fData->Of<KTTimeSeriesDistData>()) {}
        virtual ~AccumulatorType() {}

        virtual bool Finalize();
    };

    template<>
    struct KTDataAccumulator::AccumulatorType< KTFrequencySpectrumDataPolar > : Accumulator
    {
            KTFrequencySpectrumDataPolar& fDataType;
            KTFrequencySpectrumVarianceDataPolar& fVarDataType;

            AccumulatorType() : Accumulator(), fDataType(fData->Of<KTFrequencySpectrumDataPolar>()), fVarDataType(fData->Of<KTFrequencySpectrumVarianceDataPolar>()) {}
            virtual ~AccumulatorType() {}

            virtual bool Finalize();
    };

    template<>
    struct KTDataAccumulator::AccumulatorType< KTFrequencySpectrumDataFFTW > : Accumulator
    {
            KTFrequencySpectrumDataFFTW& fDataType;
            KTFrequencySpectrumVarianceDataFFTW& fVarDataType;

            AccumulatorType() : Accumulator(), fDataType(fData->Of<KTFrequencySpectrumDataFFTW>()), fVarDataType(fData->Of<KTFrequencySpectrumVarianceDataFFTW>()) {}
            virtual ~AccumulatorType() {}

            virtual bool Finalize();
    };

    template<>
    struct KTDataAccumulator::AccumulatorType< KTPowerSpectrumData > : Accumulator
    {
            KTPowerSpectrumData& fDataType;
            KTPowerSpectrumVarianceData& fVarDataType;

            AccumulatorType() : Accumulator(), fDataType(fData->Of<KTPowerSpectrumData>()), fVarDataType(fData->Of<KTPowerSpectrumVarianceData>()) {}
            virtual ~AccumulatorType() {}

            virtual bool Finalize();
    };

    template<>
    struct KTDataAccumulator::AccumulatorType< KTConvolvedFrequencySpectrumDataPolar > : Accumulator
    {
            KTConvolvedFrequencySpectrumDataPolar& fDataType;
            KTConvolvedFrequencySpectrumVarianceDataPolar& fVarDataType;

            AccumulatorType() : Accumulator(), fDataType(fData->Of<KTConvolvedFrequencySpectrumDataPolar>()), fVarDataType(fData->Of<KTConvolvedFrequencySpectrumVarianceDataPolar>()) {}
            virtual ~AccumulatorType() {}

            virtual bool Finalize();
    };

    template<>
    struct KTDataAccumulator::AccumulatorType< KTConvolvedFrequencySpectrumDataFFTW > : Accumulator
    {
            KTConvolvedFrequencySpectrumDataFFTW& fDataType;
            KTConvolvedFrequencySpectrumVarianceDataFFTW& fVarDataType;

            AccumulatorType() : Accumulator(), fDataType(fData->Of<KTConvolvedFrequencySpectrumDataFFTW>()), fVarDataType(fData->Of<KTConvolvedFrequencySpectrumVarianceDataFFTW>()) {}
            virtual ~AccumulatorType() {}

            virtual bool Finalize();
    };

    template<>
    struct KTDataAccumulator::AccumulatorType< KTConvolvedPowerSpectrumData > : Accumulator
    {
            KTConvolvedPowerSpectrumData& fDataType;
            KTConvolvedPowerSpectrumVarianceData& fVarDataType;

            AccumulatorType() : Accumulator(), fDataType(fData->Of<KTConvolvedPowerSpectrumData>()), fVarDataType(fData->Of<KTConvolvedPowerSpectrumVarianceData>()) {}
            virtual ~AccumulatorType() {}

            virtual bool Finalize();
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
        fLastAccumulatorPtr = const_cast< Accumulator* >(fDataMap.at(&typeid(XDataType)));
        return *fLastAccumulatorPtr;
    }

    template< class XDataType >
    KTDataAccumulator::Accumulator& KTDataAccumulator::GetAccumulatorNonConst() const
    {
        fLastAccumulatorPtr = fDataMap.at(&typeid(XDataType));
        return *fLastAccumulatorPtr;
    }

    template< class XDataType >
    KTDataAccumulator::Accumulator& KTDataAccumulator::GetOrCreateAccumulator()
    {
        const std::type_info& dataType = typeid(XDataType);
        fLastAccumulatorPtr = fDataMap[&dataType];
        if (fLastAccumulatorPtr == nullptr)
        {
            KTDEBUG(avlog_hh, "Creating new Accumulator for " << DemangledName(dataType));
            fLastAccumulatorPtr = new KTDataAccumulator::AccumulatorType< XDataType >();
            fLastAccumulatorPtr->fAccumulatorSize = fAccumulatorSize;
            fDataMap[&dataType] = fLastAccumulatorPtr;
        }
        return *fLastAccumulatorPtr;
    }

    template< class XDataType >
    void KTDataAccumulator::SlotFunction(Nymph::KTDataPtr data)
    {
        // Standard data slot pattern:
        // Check to ensure that the required data type is present
        if (! data->Has< XDataType >())
        {
            KTERROR(avlog_hh, "Data not found with type <" << DemangledName(typeid(XDataType)) << ">");
            return;
        }
        // Call the function
        if (! AddData(data->Of< XDataType >()))
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing data with type <" << DemangledName(typeid(XDataType)) << ">");
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
                sigIt->second.fSignalCount = 0;
            }
            if (data->GetLastData())
            {
                KTDEBUG(avlog_hh, "Emitting last-data signal");
                if (! fLastAccumulatorPtr->Finalize())
                {
                    KTERROR(avlog_hh, "Something went wrong while scaling data with type <" << typeid(XDataType).name() << ">");
                }
                (*sigIt->second.fFinishedSignal)(fLastAccumulatorPtr->fData);
            }
        }
        return;
    }



} /* namespace Katydid */
#endif /* KTDATAACCUMULATOR_HH_ */
