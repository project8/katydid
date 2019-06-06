/**
 @file KTEgg3Reader.hh
 @brief Contains KTEgg3Reader
 @details Reads Egg3 data files: parses the header and produces slices.
 @author: N. S. Oblath
 @date: Jan 7, 2015
 */

#ifndef KTEGG3READER_HH_
#define KTEGG3READER_HH_

#include "KTEggReader.hh"
#include "KTSliceHeader.hh"

#include "M3Stream.hh"
#include "M3Types.hh"

#include <map>
#include <string>
#include <vector>

#ifndef SEC_PER_NSEC
#define SEC_PER_NSEC 1.e-9
#endif

namespace monarch3
{
    class Monarch3;
}

namespace Katydid
{
    
    class KTEggHeader;

    // NOTE: the first version of this KTEgg3Reader operates in much the same way as KTEgg2Reader, and does not take advantage of
    // the flexibility of the full egg3 file format.  In particular, it only uses the channels in stream0 (though it uses as
    // many channels as exist in that stream).
    class KTEgg3Reader : public KTEggReader
    {
        protected:
            typedef double (KTEgg3Reader::*GetTIRFunction)() const;

            struct MonarchReadState
            {
                enum Status
                {
                    kInvalid,
                    kAtStartOfRun,
                    kContinueReading,
                    //kReachedNextRecord
                };
                //unsigned fAcquisitionID;
                //unsigned fReadPtrOffset; // sample offset of the read pointer in the current record
                //unsigned fReadPtrRecordOffset; // record offset of the read pointer relative to the start of the slice
                //unsigned fSliceStartPtrOffset; // sample offset of the start of the slice in the relevant record
                //unsigned fAbsoluteRecordOffset; // number of records read in the run

                unsigned fStartOfLastSliceRecord;
                unsigned fStartOfLastSliceReadPtr;
                unsigned fStartOfSliceAcquisitionId;
                unsigned fCurrentRecord;
                Status fStatus;
            };

        public:
            KTEgg3Reader();
            virtual ~KTEgg3Reader();

        public:
            unsigned GetSliceSize() const;
            void SetSliceSize(unsigned size);

            unsigned GetStride() const;
            void SetStride(unsigned stride);

            double GetStartTime() const;
            void SetStartTime(double time);

            unsigned GetStartRecord() const;
            void SetStartRecord(unsigned rec);

        protected:
            unsigned fSliceSize;
            unsigned fStride;
            double fStartTime;
            unsigned fStartRecord;

        public:
            bool Configure(const KTEggProcessor& eggProc);

            /// Opens the egg file and returns a new copy of the header information.
            Nymph::KTDataPtr BreakEgg(const path_vec& filenames);
            /// Returns the next slice's time series data.
            Nymph::KTDataPtr HatchNextSlice();
            /// Closes the file.
            bool CloseEgg();

            static unsigned GetMaxChannels();

        private:
            /// Copy header information from the M3Header object
            void CopyHeader(const monarch3::M3Header* monarchHeader);

            bool LoadNextFile();

            //Nymph::KTDataPtr (KTEgg3Reader::*fHatchNextSlicePtr)();
            //Nymph::KTDataPtr HatchNextSliceRealUnsigned();
            //Nymph::KTDataPtr HatchNextSliceRealSigned();
            //Nymph::KTDataPtr HatchNextSliceComplex();

            KTEggReader::path_vec fFilenames;
            KTEggReader::path_vec::const_iterator fCurrentFileIt;

            const monarch3::Monarch3* fMonarch;
            const monarch3::M3Stream* fM3Stream;
            const monarch3::M3StreamHeader* fM3StreamHeader;

            Nymph::KTDataPtr fHeaderPtr;
            KTEggHeader& fHeader;
            KTSliceHeader fMasterSliceHeader;

            MonarchReadState fReadState;

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

            /// Returns the time since the run started in seconds of the current acquisition
            double GetAcqTimeInRun() const;

        private:
            mutable GetTIRFunction fGetTimeInRun;
            double GetTimeInRunFromMonarch() const;
            double GetTimeInRunManually() const;
            mutable monarch3::TimeType fT0Offset; /// Time of the first record
            mutable double fAcqTimeInRun; /// Time-in-run of the current acquisition

            double fSampleRateUnitsInHz;

            unsigned fRecordSize;
            double fBinWidth;

            uint64_t fSliceNumber;

            uint64_t fRecordsProcessed;
    };


    uint32_t ConvertMonarch3DataFormat( uint32_t m3DataFormat );



    inline unsigned KTEgg3Reader::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTEgg3Reader::SetSliceSize(unsigned size)
    {
        fSliceSize = size;
        return;
    }

    inline unsigned KTEgg3Reader::GetStride() const
    {
        return fStride;
    }

    inline void KTEgg3Reader::SetStride(unsigned stride)
    {
        fStride = stride;
        return;
    }

    inline double KTEgg3Reader::GetStartTime() const
    {
        return fStartTime;
    }

    inline void KTEgg3Reader::SetStartTime(double time)
    {
        fStartTime = time;
        return;
    }

    inline unsigned KTEgg3Reader::GetStartRecord() const
    {
        return fStartRecord;
    }

    inline void KTEgg3Reader::SetStartRecord(unsigned rec)
    {
        fStartRecord = rec;
        return;
    }

    inline double KTEgg3Reader::GetSampleRateUnitsInHz() const
    {
        return fSampleRateUnitsInHz;
    }

    inline unsigned KTEgg3Reader::GetRecordSize() const
    {
        return fRecordSize;
    }
    inline double KTEgg3Reader::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline double KTEgg3Reader::GetTimeInRun() const
    {
        return (this->*fGetTimeInRun)();
    }

    inline double KTEgg3Reader::GetTimeInRunFromMonarch() const
    {
        return double(fM3Stream->GetChannelRecord(0)->GetTime()) * SEC_PER_NSEC + fBinWidth * double(fReadState.fStartOfLastSliceReadPtr);
    }

    inline double KTEgg3Reader::GetTimeInRunManually() const
    {
        return fBinWidth * double(fReadState.fStartOfLastSliceRecord * fRecordSize + fReadState.fStartOfLastSliceReadPtr);
    }

    inline double KTEgg3Reader::GetIntegratedTime() const
    {
        return (double)fRecordsProcessed * (double)fRecordSize * fBinWidth;
    }

    inline unsigned KTEgg3Reader::GetNSlicesProcessed() const
    {
        return (unsigned)fSliceNumber + 1;
    }

    inline unsigned KTEgg3Reader::GetNRecordsProcessed() const
    {
        return fRecordsProcessed;
    }

    inline const KTEgg3Reader::MonarchReadState& KTEgg3Reader::GetReadState() const
    {
        return fReadState;
    }

    inline double KTEgg3Reader::GetAcqTimeInRun() const
    {
        return fAcqTimeInRun;
    }



} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
