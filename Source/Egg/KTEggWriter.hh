/*
 * KTEggWriter.hh
 *
 *  Created on: May 29, 2013
 *      Author: nsoblath
 */

#ifndef KTEGGWRITER_HH_
#define KTEGGWRITER_HH_

#include "KTWriter.hh"

#include "MonarchTypes.hpp"

class Monarch;

namespace Katydid
{
    class KTData;
    class KTEggHeader;
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

     Slots:
     - "header": void (const KTEggHeader*) -- writes the header information to the candidates file
     - "ts": void (boost::shared_ptr<KTData>) -- writes a record from a time series;
     - "stop": void () -- stops writing candidates and closes the file
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
            Bool_t CloseFile();

            void WriteHeader(const KTEggHeader* header);

            Bool_t WriteTSData(KTTimeSeriesData& tsData);

            FileStatus GetFileStatus() const;

        private:
            Bool_t WriteTSDataInterleaved(KTTimeSeriesData& tsData);
            Bool_t WriteTSDataSeparate(KTTimeSeriesData& tsData);

            FileStatus fFileStatus;
            UInt_t fExpectedNChannels;
            UInt_t fExpectedRecordSize;

            Monarch* fEggFile;

            //**************
            // Slots
            //**************
        private:
            KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesSlot;

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
