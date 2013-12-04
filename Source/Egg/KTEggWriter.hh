/*
 * KTEggWriter.hh
 *
 *  Created on: May 29, 2013
 *      Author: nsoblath
 */

#ifndef KTEGGWRITER_HH_
#define KTEGGWRITER_HH_

#include "KTWriter.hh"

#include "KTSlot.hh"

#include "MonarchTypes.hpp"

class Monarch;
struct MonarchRecord;

namespace Katydid
{
    class KTEggHeader;
    class KTSliceHeader;
    class KTTimeSeriesData;

    /*!
     @class KTEggWriter
     @author N. S. Oblath

     @brief Egg file writer

     @details
     Most of the header information should be provided via the "header" slot or the WriteHeader function.
     If the header information is not separately provided, default values will be used.

     The following header information MUST be provided via configuration of this Writer:
     - Filename
     - Format Mode (separate or interleaved)

     Configuration name: "egg-writer"

     Available configuration values:
     - "output-file": string -- output filename
     - "format-mode": string -- how multiple channels are written: "separate" or "interleaved" (ignored for single-channel)
     - "digitizer-fullscale": double -- full-scale range of the digitizer

     Slots:
     - "header": void (const KTEggHeader*) -- writes the header information to the candidates file; opens the file first if it hasn't been opened
     - "ts": void (KTDataPtr) -- writes a record from a time series;
     - "done": void () -- closes the file
    */
    class KTEggWriter : public KTWriter
    {
        public:
            enum FileStatus
            {
                kClosed,
                kOpened,
                kHeaderWritten,
                kWritingRecords
            };

        public:
            KTEggWriter(const std::string& name = "egg-writer");
            virtual ~KTEggWriter();

            Bool_t Configure(const KTPStoreNode* node);

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            FormatModeType GetFormatMode() const;
            void SetFormatMode(FormatModeType mode);

            Double_t GetDigitizerFullscale() const;
            void SetDigitizerFullscale(Double_t scale);

        private:
            std::string fFilename;

            FormatModeType fFormatMode;

            Double_t fDigitizerFullscale;

        public:
            Bool_t OpenFile();
            void CloseFile();

            void WriteHeader(const KTEggHeader* header);

            Bool_t WriteTSData(KTSliceHeader& slHeader, KTTimeSeriesData& tsData);

            FileStatus GetFileStatus() const;

        private:
            Bool_t CopyATimeSeries(UInt_t component, const KTSliceHeader& slHeader, const KTTimeSeriesData& tsData, MonarchRecord* record);

            FileStatus fFileStatus;
            UInt_t fExpectedNChannels;
            UInt_t fExpectedRecordSize;

            Monarch* fMonarch;

            //**************
            // Slots
            //**************
        private:
            KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            KTSlotDataTwoTypes< KTSliceHeader, KTTimeSeriesData > fTimeSeriesSlot;
            KTSlotNoArg< void () > fDoneSlot;
    };

    inline const std::string& KTEggWriter::GetFilename() const
    {
        return fFilename;
    }
    inline void KTEggWriter::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline FormatModeType KTEggWriter::GetFormatMode() const
    {
        return fFormatMode;
    }

    inline void KTEggWriter::SetFormatMode(FormatModeType mode)
    {
        fFormatMode = mode;
        return;
    }

    inline KTEggWriter::FileStatus KTEggWriter::GetFileStatus() const
    {
        return fFileStatus;
    }

    inline Double_t KTEggWriter::GetDigitizerFullscale() const
    {
        return fDigitizerFullscale;
    }

    inline void KTEggWriter::SetDigitizerFullscale(Double_t scale)
    {
        fDigitizerFullscale = scale;
        return;
    }



} /* namespace Katydid */
#endif /* KTEGGWRITER_HH_ */
