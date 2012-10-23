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

            UInt_t GetTimeSeriesSize() const;
            void SetTimeSeriesSize(UInt_t size);

        protected:
            TimeSeriesType fTimeSeriesType;
            UInt_t fTimeSeriesSize;

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
            Double_t GetSampleRateUnitsInHz();

            Double_t GetFullVoltageScale();
            UInt_t GetNADCLevels();

        protected:
            Double_t fSampleRateUnitsInHz;

            Double_t fFullVoltageScale;
            UInt_t fNADCLevels;

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

    inline UInt_t KTEggReaderMonarch::GetTimeSeriesSize() const
    {
        return fTimeSeriesSize;
    }

    inline void KTEggReaderMonarch::SetTimeSeriesSize(UInt_t size)
    {
        fTimeSeriesSize = size;
        return;
    }


} /* namespace Katydid */

#endif /* KTEGGREADERMONARCH_HH_ */
