/*
 * KTTSGenerator.hh
 *
 *  Created on: May 3, 2013
 *      Author: nsoblath
 */

#ifndef KTTSGENERATOR_HH_
#define KTTSGENERATOR_HH_

#include "KTPrimaryProcessor.hh"

#include "KTSlot.hh"
#include "KTData.hh"

namespace Katydid
{
    class KTEggHeader;
    class KTProcSummary;
    class KTTimeSeriesData;

    /*!
     @class KTTSGenerator
     @author N. S. Oblath

     @brief Base class for generating time series data

     @details
     Can create a new time series and drive processing, or can add to an existing time series.

     This class handles the creation of the TS data and the blank slices.
     Derived classes are responsible for adding actual values to the time series.

     Available configuration options:
       - "n-slices": UInt_t -- Number of slices to create (used only if creating new slices)
       - "n-channels": UInt_t -- Number of channels per slice to create (used only if creating new slices)
       - "time-series-size": UInt_t -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": double -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": UInt_t -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)

     Slots:
     - "slice": void (KTDataPtr) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals:
     - "header": void (const KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (KTDataPtr) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
     - "summary": void (const KTProcSummary*) -- emitted when the job is complete, after "done"
    */
    class KTTSGenerator : public KTPrimaryProcessor
    {
        public:
            enum TimeSeriesType
            {
                kRealTimeSeries,
                kFFTWTimeSeries
            };

        public:
            KTTSGenerator(const std::string& name = "default-ts-generator");
            virtual ~KTTSGenerator();

            virtual Bool_t Configure(const KTPStoreNode* node);
            virtual Bool_t ConfigureDerivedGenerator(const KTPStoreNode* node) = 0;

            UInt_t GetNSlices() const;
            void SetNSlices(UInt_t slices);

            UInt_t GetNChannels() const;
            void SetNChannels(UInt_t channels);

            UInt_t GetSliceSize() const;
            void SetSliceSize(UInt_t size);

            double GetBinWidth() const;
            void SetBinWidth(double bw);

            UInt_t GetRecordSize() const;
            void SetRecordSize(UInt_t rec);

            TimeSeriesType GetTimeSeriesType() const;
            void SetTimeSeriesType(TimeSeriesType type);

        private:
            UInt_t fNSlices;

            UInt_t fNChannels;
            UInt_t fSliceSize;
            double fBinWidth;
            UInt_t fRecordSize;

            TimeSeriesType fTimeSeriesType;

        public:
            Bool_t Run();

            KTEggHeader* CreateEggHeader() const;

            KTDataPtr CreateNewData() const;

            Bool_t AddSliceHeader(KTData& data) const;

            Bool_t AddEmptySlice(KTData& data) const;

            virtual Bool_t GenerateTS(KTTimeSeriesData& data) = 0;

            UInt_t GetSliceCounter() const;
            void SetSliceCounter(UInt_t slices);

        private:
            UInt_t fSliceCounter;


            //***************
            // Slots
            //***************
        private:
            KTSlotDataOneType< KTTimeSeriesData > fDataSlot;

            //***************
            // Signals
            //***************
        private:
            KTSignalOneArg< const KTEggHeader* > fHeaderSignal;
            KTSignalData fDataSignal;
            KTSignalOneArg< void > fDoneSignal;
            KTSignalOneArg< const KTProcSummary* > fSummarySignal;
    };

    inline UInt_t KTTSGenerator::GetNSlices() const
    {
        return fNSlices;
    }

    inline void KTTSGenerator::SetNSlices(UInt_t slices)
    {
        fNSlices = slices;
        return;
    }

    inline UInt_t KTTSGenerator::GetNChannels() const
    {
        return fNChannels;
    }

    inline void KTTSGenerator::SetNChannels(UInt_t channels)
    {
        fNChannels = channels;
        return;
    }

    inline UInt_t KTTSGenerator::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTTSGenerator::SetSliceSize(UInt_t size)
    {
        fSliceSize = size;
        return;
    }

    inline double KTTSGenerator::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTTSGenerator::SetBinWidth(double bw)
    {
        fBinWidth = bw;
        return;
    }

    inline KTTSGenerator::TimeSeriesType KTTSGenerator::GetTimeSeriesType() const
    {
        return fTimeSeriesType;
    }

    inline void KTTSGenerator::SetTimeSeriesType(TimeSeriesType type)
    {
        fTimeSeriesType = type;
        return;
    }

    inline UInt_t KTTSGenerator::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTTSGenerator::SetRecordSize(UInt_t rec)
    {
        fRecordSize = rec;
        return;
    }

    inline UInt_t KTTSGenerator::GetSliceCounter() const
    {
        return fSliceCounter;
    }

    inline void KTTSGenerator::SetSliceCounter(UInt_t slices)
    {
        fSliceCounter = slices;
        return;
    }


} /* namespace Katydid */
#endif /* KTTSGENERATOR_HH_ */
