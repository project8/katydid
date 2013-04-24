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

#include "MonarchTypes.hpp"

#include "Rtypes.h"

#include <map>
#include <string>
#include <vector>

class MonarchPP;
class MonarchHeader;

namespace Katydid
{

    class KTEggReaderMonarch : public KTEggReader
    {
        protected:
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
                AcqIdType fAcquisitionID;
                UInt_t fReadPtrOffset;
                UInt_t fReadPtrRecordOffset;
                UInt_t fSliceStartPtrOffset;
                UInt_t fAbsoluteRecordOffset;
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
            boost::shared_ptr< KTData > HatchNextSlice();
            /// Closes the file.
            Bool_t CloseEgg();

        protected:
            /// Copy header information from the MonarchHeader object
            void CopyHeaderInformation(const MonarchHeader* monarchHeader);

            const MonarchPP* fMonarch;
            KTEggHeader fHeader;
            MonarchReadState fReadState;

            AcquisitionModeMap fNumberOfChannels;

        public:
            Double_t GetSampleRateUnitsInHz() const;

            Double_t GetFullVoltageScale() const;
            UInt_t GetNADCLevels() const;

            UInt_t GetRecordSize() const;
            Double_t GetBinWidth() const;

            /// Returns the time since the run started in seconds
            Double_t GetTimeInRun() const;

        protected:
            Double_t fSampleRateUnitsInHz;

            Double_t fFullVoltageScale;
            UInt_t fNADCLevels;

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

    inline Double_t KTEggReaderMonarch::GetFullVoltageScale() const
    {
        return fFullVoltageScale;
    }
    inline UInt_t KTEggReaderMonarch::GetNADCLevels() const
    {
        return fNADCLevels;
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
        return fBinWidth * Double_t(fReadState.fAbsoluteRecordOffset * fRecordSize + fReadState.fReadPtrOffset);
    }



} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
