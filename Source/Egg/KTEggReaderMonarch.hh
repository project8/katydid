/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces events.
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
                    kAcquisitionIDHasChanged,
                    kAtEndOfFile
                };
                AcqIdType fAcquisitionID;
                UInt_t fDataPtrOffset;
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

            UInt_t GetTimeSeriesSizeRequest() const;
            void SetTimeSeriesSizeRequest(UInt_t size);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        protected:
            TimeSeriesType fTimeSeriesType;
            UInt_t fTimeSeriesSizeRequest;
            std::string fOutputDataName;

        public:
            /// Opens the egg file and returns a new copy of the header information.
            KTEggHeader* BreakEgg(const std::string& filename);
            /// Returns the next event's time series data.
            KTTimeSeriesData* HatchNextEvent();
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

            UInt_t GetMonarchRecordsRead() const;
            UInt_t GetMonarchRecordSize() const;
            Double_t GetBinWidth() const;

            /// Returns the time since the run started in seconds
            Double_t GetTimeInRun() const;

        protected:
            Double_t fSampleRateUnitsInHz;

            Double_t fFullVoltageScale;
            UInt_t fNADCLevels;

            UInt_t fMonarchRecordsRead;
            UInt_t fMonarchRecordSize;
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

    inline UInt_t KTEggReaderMonarch::GetTimeSeriesSizeRequest() const
    {
        return fTimeSeriesSizeRequest;
    }

    inline void KTEggReaderMonarch::SetTimeSeriesSizeRequest(UInt_t size)
    {
        fTimeSeriesSizeRequest = size;
        return;
    }

    inline const std::string& KTEggReaderMonarch::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTEggReaderMonarch::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
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


    inline UInt_t KTEggReaderMonarch::GetMonarchRecordsRead() const
    {
        return fMonarchRecordsRead;
    }
    inline UInt_t KTEggReaderMonarch::GetMonarchRecordSize() const
    {
        return fMonarchRecordSize;
    }
    inline Double_t KTEggReaderMonarch::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline Double_t KTEggReaderMonarch::GetTimeInRun() const
    {
        return fBinWidth * Double_t(fMonarchRecordsRead * fMonarchRecordSize + fReadState.fDataPtrOffset);
    }



} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
