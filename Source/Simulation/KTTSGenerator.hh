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
       - "number-of-slices": unsigned -- Number of slices to create (used only if creating new slices)
       - "n-channels": unsigned -- Number of channels per slice to create (used only if creating new slices)
       - "slice-size": unsigned -- Specify the size of the time series (used only if creating new slices)
       - "bin-width": double -- Specify the bin width
       - "time-series-type": string -- Type of time series to produce (options: real [default], fftw)
       - "record-size": unsigned -- Size of the imaginary record that this slice came from (only used to fill in the egg header; does not affect the simulation at all)

     Slots:
     - "slice": void (Nymph::KTDataPtr) -- Add a signal to an existing time series; Requires KTTimeSeriesData; Emits signal "slice" when done.

     Signals:
     - "header": void (KTEggHeader*) -- emitted when the egg header is created.
     - "slice": void (Nymph::KTDataPtr) -- emitted when the new time series is produced or processed.
     - "done": void () --  emitted when the job is complete.
     - "summary": void (const KTProcSummary*) -- emitted when the job is complete, after "done"
    */
    class KTTSGenerator : public Nymph::KTPrimaryProcessor
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

            virtual bool Configure(const scarab::param_node* node);
            virtual bool ConfigureDerivedGenerator(const scarab::param_node* node) = 0;

            unsigned GetNSlices() const;
            void SetNSlices(unsigned slices);

            unsigned GetNChannels() const;
            void SetNChannels(unsigned channels);

            unsigned GetSliceSize() const;
            void SetSliceSize(unsigned size);

            double GetBinWidth() const;
            void SetBinWidth(double bw);

            unsigned GetRecordSize() const;
            void SetRecordSize(unsigned rec);

            TimeSeriesType GetTimeSeriesType() const;
            void SetTimeSeriesType(TimeSeriesType type);

        private:
            unsigned fNSlices;

            unsigned fNChannels;
            unsigned fSliceSize;
            double fBinWidth;
            unsigned fRecordSize;

            TimeSeriesType fTimeSeriesType;

        public:
            bool Run();

            KTEggHeader* CreateEggHeader() const;

            Nymph::KTDataPtr CreateNewData() const;

            bool AddSliceHeader(Nymph::KTData& data) const;

            bool AddEmptySlice(Nymph::KTData& data) const;

            virtual bool GenerateTS(KTTimeSeriesData& data) = 0;

            unsigned GetSliceCounter() const;
            void SetSliceCounter(unsigned slices);

        private:
            unsigned fSliceCounter;


            //***************
            // Slots
            //***************
        private:
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fDataSlot;

            //***************
            // Signals
            //***************
        private:
            Nymph::KTSignalOneArg< KTEggHeader* > fHeaderSignal;
            Nymph::KTSignalData fDataSignal;
            Nymph::KTSignalDone fDoneSignal;
            Nymph::KTSignalOneArg< const KTProcSummary* > fSummarySignal;
    };

    inline unsigned KTTSGenerator::GetNSlices() const
    {
        return fNSlices;
    }

    inline void KTTSGenerator::SetNSlices(unsigned slices)
    {
        fNSlices = slices;
        return;
    }

    inline unsigned KTTSGenerator::GetNChannels() const
    {
        return fNChannels;
    }

    inline void KTTSGenerator::SetNChannels(unsigned channels)
    {
        fNChannels = channels;
        return;
    }

    inline unsigned KTTSGenerator::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTTSGenerator::SetSliceSize(unsigned size)
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

    inline unsigned KTTSGenerator::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTTSGenerator::SetRecordSize(unsigned rec)
    {
        fRecordSize = rec;
        return;
    }

    inline unsigned KTTSGenerator::GetSliceCounter() const
    {
        return fSliceCounter;
    }

    inline void KTTSGenerator::SetSliceCounter(unsigned slices)
    {
        fSliceCounter = slices;
        return;
    }


} /* namespace Katydid */
#endif /* KTTSGENERATOR_HH_ */
