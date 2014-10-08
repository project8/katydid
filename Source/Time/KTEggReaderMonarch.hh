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

#include "MonarchRecord.hpp"
#include "MonarchTypes.hpp"

#include <map>
#include <string>
#include <vector>

#ifndef SEC_PER_NSEC
#define SEC_PER_NSEC 1.e-9
#endif

namespace monarch
{
    class Monarch;
    class MonarchHeader;
}

namespace Katydid
{
    class KTEggHeader;

    class KTEggReaderMonarch : public KTEggReader
    {
        protected:
            typedef const monarch::MonarchRecordBytes* (monarch::Monarch::*GetRecordFunction)() const;
            typedef double (KTEggReaderMonarch::*GetTIRFunction)() const;

            typedef std::map< unsigned, int > AcquisitionModeMap;
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
                monarch::AcquisitionIdType fAcquisitionID;
                unsigned fReadPtrOffset; // sample offset of the read pointer in the current record
                unsigned fReadPtrRecordOffset; // record offset of the read pointer relative to the start of the slice
                unsigned fSliceStartPtrOffset; // sample offset of the start of the slice in the relevant record
                unsigned fAbsoluteRecordOffset; // number of records read in the run
                Status fStatus;
            };

        public:
            KTEggReaderMonarch();
            virtual ~KTEggReaderMonarch();

        public:
            unsigned GetSliceSize() const;
            void SetSliceSize(unsigned size);

            unsigned GetStride() const;
            void SetStride(unsigned stride);

            double GetStartTime() const;
            void SetStartTime(double time);

        protected:
            unsigned fSliceSize;
            unsigned fStride;
            double fStartTime;

        public:
            bool Configure(const KTEggProcessor& eggProc);

            /// Opens the egg file and returns a new copy of the header information.
            KTDataPtr BreakEgg(const std::string& filename);
            /// Returns the next slice's time series data.
            KTDataPtr HatchNextSlice();
            /// Closes the file.
            bool CloseEgg();

            static unsigned GetMaxChannels();

        private:
            /// Copy header information from the MonarchHeader object
            void CopyHeaderInformation(const monarch::MonarchHeader* monarchHeader);

            const monarch::Monarch* fMonarch;
            KTDataPtr fHeaderPtr;
            KTEggHeader& fHeader;
            MonarchReadState fReadState;

            static const unsigned fMaxChannels = 2;
            GetRecordFunction fMonarchGetRecord[fMaxChannels];

            AcquisitionModeMap fNumberOfChannels;

        public:
            double GetSampleRateUnitsInHz() const;

            double GetFullVoltageScale() const;
            unsigned GetNADCLevels() const;

            unsigned GetRecordSize() const;
            double GetBinWidth() const;

            /// Returns the time since the run started in seconds
            double GetTimeInRun() const;
            /// Same as GetTimeInRun
            virtual double GetIntegratedTime() const;

            /// Returns the number of slices processed
            virtual unsigned GetNSlicesProcessed() const;

            /// Returns the number of records processed (including partial usage)
            virtual unsigned GetNRecordsProcessed() const;

            const MonarchReadState& GetReadState() const;

        private:
            mutable GetTIRFunction fGetTimeInRun;
            double GetTimeInRunFirstCall() const;
            double GetTimeInRunFromMonarch() const;
            double GetTimeInRunManually() const;
            mutable monarch::TimeType fT0Offset; /// Time of the first record

            double fSampleRateUnitsInHz;

            unsigned fRecordSize;
            double fBinWidth;

            uint64_t fSliceNumber;
    };

    inline unsigned KTEggReaderMonarch::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTEggReaderMonarch::SetSliceSize(unsigned size)
    {
        fSliceSize = size;
        return;
    }

    inline unsigned KTEggReaderMonarch::GetStride() const
    {
        return fStride;
    }

    inline void KTEggReaderMonarch::SetStride(unsigned stride)
    {
        fStride = stride;
        return;
    }

    inline double KTEggReaderMonarch::GetStartTime() const
    {
        return fStartTime;
    }

    inline void KTEggReaderMonarch::SetStartTime(double time)
    {
        fStartTime = time;
        return;
    }

    inline double KTEggReaderMonarch::GetSampleRateUnitsInHz() const
    {
        return fSampleRateUnitsInHz;
    }

    inline unsigned KTEggReaderMonarch::GetRecordSize() const
    {
        return fRecordSize;
    }
    inline double KTEggReaderMonarch::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline double KTEggReaderMonarch::GetTimeInRun() const
    {
        return (this->*fGetTimeInRun)();
    }

    inline double KTEggReaderMonarch::GetTimeInRunFromMonarch() const
    {
        return double((fMonarch->*fMonarchGetRecord[0])()->fTime - fT0Offset) * SEC_PER_NSEC + fBinWidth * double(fReadState.fReadPtrOffset);
    }

    inline double KTEggReaderMonarch::GetTimeInRunManually() const
    {
        return fBinWidth * double(fReadState.fAbsoluteRecordOffset * fRecordSize + fReadState.fReadPtrOffset);
    }

    inline double KTEggReaderMonarch::GetIntegratedTime() const
    {
        return GetTimeInRun();
    }

    inline unsigned KTEggReaderMonarch::GetNSlicesProcessed() const
    {
        return (unsigned)fSliceNumber;
    }

    inline unsigned KTEggReaderMonarch::GetNRecordsProcessed() const
    {
        return fReadState.fAbsoluteRecordOffset + 1;
    }

    inline const KTEggReaderMonarch::MonarchReadState& KTEggReaderMonarch::GetReadState() const
    {
        return fReadState;
    }




} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
