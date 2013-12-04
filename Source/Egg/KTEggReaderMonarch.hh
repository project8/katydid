/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces slices.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEGGREADERMONARCH_HH_
#define KTEGGREADERMONARCH_HH_

#include "KTEggReader.hh"

#include "KTEggHeader.hh"

#include "MonarchRecord.hpp"
#include "MonarchTypes.hpp"

#include <map>
#include <string>
#include <vector>

#ifndef SEC_PER_NSEC
#define SEC_PER_NSEC 1.e-9
#endif

class Monarch;
class MonarchHeader;

namespace Katydid
{

    class KTEggReaderMonarch : public KTEggReader
    {
        protected:
            typedef const MonarchRecord* (Monarch::*GetRecordFunction)() const;
            typedef Double_t (KTEggReaderMonarch::*GetTIRFunction)() const;

            typedef std::map< UInt_t, Int_t > AcquisitionModeMap;
            typedef AcquisitionModeMap::value_type AcqModeMapValue;

            struct MonarchReadState
            {
                enum Status
                {
                    kInvalid,
                    kAtStartOfRun,
                    kContinueReading,
                    kReachedNextRecord
                };
                AcquisitionIdType fAcquisitionID;
                UInt_t fReadPtrOffset; // sample offset of the read pointer in the current record
                UInt_t fReadPtrRecordOffset; // record offset of the read pointer relative to the start of the slice
                UInt_t fSliceStartPtrOffset; // sample offset of the start of the slice in the relevant record
                UInt_t fAbsoluteRecordOffset; // number of records read in the run
                Status fStatus;
            };

        public:
            enum TimeSeriesType
            {
                kRealTimeSeries,
                kFFTWTimeSeries
            };

        public:
            KTEggReaderMonarch();
            virtual ~KTEggReaderMonarch();

        public:
            TimeSeriesType GetTimeSeriesType() const;
            void SetTimeSeriesType(TimeSeriesType type);

            UInt_t GetSliceSize() const;
            void SetSliceSize(UInt_t size);

            UInt_t GetStride() const;
            void SetStride(UInt_t stride);

        protected:
            TimeSeriesType fTimeSeriesType;
            UInt_t fSliceSize;
            UInt_t fStride;

        public:
            /// Opens the egg file and returns a new copy of the header information.
            KTEggHeader* BreakEgg(const std::string& filename);
            /// Returns the next slice's time series data.
            KTDataPtr HatchNextSlice();
            /// Closes the file.
            Bool_t CloseEgg();

            static UInt_t GetMaxChannels();

        private:
            /// Copy header information from the MonarchHeader object
            void CopyHeaderInformation(const MonarchHeader* monarchHeader);

            const Monarch* fMonarch;
            KTEggHeader fHeader;
            MonarchReadState fReadState;

            static const UInt_t fMaxChannels = 2;
            GetRecordFunction fMonarchGetRecord[fMaxChannels];

            AcquisitionModeMap fNumberOfChannels;

        public:
            Double_t GetSampleRateUnitsInHz() const;

            Double_t GetFullVoltageScale() const;
            UInt_t GetNADCLevels() const;

            UInt_t GetRecordSize() const;
            Double_t GetBinWidth() const;

            /// Returns the time since the run started in seconds
            Double_t GetTimeInRun() const;
            /// Same as GetTimeInRun
            virtual Double_t GetIntegratedTime() const;

            /// Returns the number of slices processed
            virtual UInt_t GetNSlicesProcessed() const;

            /// Returns the number of records processed (including partial usage)
            virtual UInt_t GetNRecordsProcessed() const;

            const MonarchReadState& GetReadState() const;

        private:
            mutable GetTIRFunction fGetTimeInRun;
            Double_t GetTimeInRunFirstCall() const;
            Double_t GetTimeInRunFromMonarch() const;
            Double_t GetTimeInRunManually() const;

            Double_t fSampleRateUnitsInHz;

            UInt_t fRecordSize;
            Double_t fBinWidth;

            ULong64_t fSliceNumber;

    };

    inline KTEggReaderMonarch::TimeSeriesType KTEggReaderMonarch::GetTimeSeriesType() const
    {
        return fTimeSeriesType;
    }

    inline void KTEggReaderMonarch::SetTimeSeriesType(TimeSeriesType type)
    {
        fTimeSeriesType = type;
        return;
    }

    inline UInt_t KTEggReaderMonarch::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTEggReaderMonarch::SetSliceSize(UInt_t size)
    {
        fSliceSize = size;
        return;
    }

    inline UInt_t KTEggReaderMonarch::GetStride() const
    {
        return fStride;
    }

    inline void KTEggReaderMonarch::SetStride(UInt_t stride)
    {
        fStride = stride;
        return;
    }

    inline Double_t KTEggReaderMonarch::GetSampleRateUnitsInHz() const
    {
        return fSampleRateUnitsInHz;
    }

    inline UInt_t KTEggReaderMonarch::GetRecordSize() const
    {
        return fRecordSize;
    }
    inline Double_t KTEggReaderMonarch::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline Double_t KTEggReaderMonarch::GetTimeInRun() const
    {
        return (this->*fGetTimeInRun)();
    }

    inline Double_t KTEggReaderMonarch::GetTimeInRunFromMonarch() const
    {
        return Double_t((fMonarch->*fMonarchGetRecord[0])()->fTime) * SEC_PER_NSEC + fBinWidth * Double_t(fReadState.fReadPtrOffset);
    }

    inline Double_t KTEggReaderMonarch::GetTimeInRunManually() const
    {
        return fBinWidth * Double_t(fReadState.fAbsoluteRecordOffset * fRecordSize + fReadState.fReadPtrOffset);
    }

    inline Double_t KTEggReaderMonarch::GetIntegratedTime() const
    {
        return GetTimeInRun();
    }

    inline UInt_t KTEggReaderMonarch::GetNSlicesProcessed() const
    {
        return (UInt_t)fSliceNumber;
    }

    inline UInt_t KTEggReaderMonarch::GetNRecordsProcessed() const
    {
        return fReadState.fAbsoluteRecordOffset + 1;
    }

    inline const KTEggReaderMonarch::MonarchReadState& KTEggReaderMonarch::GetReadState() const
    {
        return fReadState;
    }




} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
