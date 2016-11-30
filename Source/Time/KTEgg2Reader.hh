/**
 @file KTEgg2Reader.hh
 @brief Contains KTEgg2Reader
 @details Reads Egg2 data files: parses the header and produces slices.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEGG2READER_HH_
#define KTEGG2READER_HH_

#include "KTEggReader.hh"

#include "M2Record.hh"
#include "M2Types.hh"

#include <map>
#include <string>
#include <vector>

#ifndef SEC_PER_NSEC
#define SEC_PER_NSEC 1.e-9
#endif

namespace monarch2
{
    class Monarch2;
    class M2Header;
}

namespace Katydid
{
    
    class KTEggHeader;

    class KTEgg2Reader : public KTEggReader
    {
        protected:
            typedef const monarch2::M2RecordBytes* (monarch2::Monarch2::*GetRecordFunction)() const;
            typedef double (KTEgg2Reader::*GetTIRFunction)() const;

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
                monarch2::AcquisitionIdType fAcquisitionID;
                unsigned fReadPtrOffset; // sample offset of the read pointer in the current record
                unsigned fReadPtrRecordOffset; // record offset of the read pointer relative to the start of the slice
                unsigned fSliceStartPtrOffset; // sample offset of the start of the slice in the relevant record
                unsigned fAbsoluteRecordOffset; // number of records read in the run
                Status fStatus;
            };

        public:
            KTEgg2Reader();
            virtual ~KTEgg2Reader();

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
            Nymph::KTDataPtr BreakEgg(const std::string& filename);
            /// Returns the next slice's time series data.
            Nymph::KTDataPtr HatchNextSlice();
            /// Closes the file.
            bool CloseEgg();

            static unsigned GetMaxChannels();

        private:
            /// Copy header information from the MonarchHeader object
            void CopyHeaderInformation(const monarch2::M2Header* monarchHeader);

            const monarch2::Monarch2* fMonarch;
            Nymph::KTDataPtr fHeaderPtr;
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
            /// Returns the time within the current acquisition
            double GetTimeInAcq() const;

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
            mutable monarch2::TimeType fT0Offset; /// Time of the first record

            double fSampleRateUnitsInHz;

            unsigned fRecordSize;
            double fBinWidth;

            uint64_t fSliceNumber;
    };

    inline unsigned KTEgg2Reader::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTEgg2Reader::SetSliceSize(unsigned size)
    {
        fSliceSize = size;
        return;
    }

    inline unsigned KTEgg2Reader::GetStride() const
    {
        return fStride;
    }

    inline void KTEgg2Reader::SetStride(unsigned stride)
    {
        fStride = stride;
        return;
    }

    inline double KTEgg2Reader::GetStartTime() const
    {
        return fStartTime;
    }

    inline void KTEgg2Reader::SetStartTime(double time)
    {
        fStartTime = time;
        return;
    }

    inline double KTEgg2Reader::GetSampleRateUnitsInHz() const
    {
        return fSampleRateUnitsInHz;
    }

    inline unsigned KTEgg2Reader::GetRecordSize() const
    {
        return fRecordSize;
    }
    inline double KTEgg2Reader::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline double KTEgg2Reader::GetTimeInRun() const
    {
        return (this->*fGetTimeInRun)();
    }

    inline double KTEgg2Reader::GetTimeInRunFromMonarch() const
    {
        return double((fMonarch->*fMonarchGetRecord[0])()->fTime - fT0Offset) * SEC_PER_NSEC + fBinWidth * double(fReadState.fReadPtrOffset);
    }

    inline double KTEgg2Reader::GetTimeInRunManually() const
    {
        return fBinWidth * double(fReadState.fAbsoluteRecordOffset * fRecordSize + fReadState.fReadPtrOffset);
    }

    inline double KTEgg2Reader::GetIntegratedTime() const
    {
        return GetTimeInRun();
    }

    inline double KTEgg2Reader::GetTimeInAcq() const
    {
        // For the Egg data taken with a free streaming digitizer, the TimeInRun happens to be equal to the TimeInAcq
        return GetTimeInRun();
    }

    inline unsigned KTEgg2Reader::GetNSlicesProcessed() const
    {
        return (unsigned)fSliceNumber;
    }

    inline unsigned KTEgg2Reader::GetNRecordsProcessed() const
    {
        return fReadState.fAbsoluteRecordOffset + 1;
    }

    inline const KTEgg2Reader::MonarchReadState& KTEgg2Reader::GetReadState() const
    {
        return fReadState;
    }




} /* namespace Katydid */

#endif /* KTEGG2READER_HH_ */
