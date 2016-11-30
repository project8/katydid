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

namespace monarch
{
    class Monarch;
    struct MonarchRecord;
}

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
     - "header": void (KTEggHeader*) -- writes the header information to the candidates file; opens the file first if it hasn't been opened
     - "ts": void (Nymph::KTDataPtr) -- writes a record from a time series;
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

            bool Configure(const scarab::param_node* node);

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            monarch::FormatModeType GetFormatMode() const;
            void SetFormatMode(monarch::FormatModeType mode);

            double GetDigitizerFullscale() const;
            void SetDigitizerFullscale(double scale);

        private:
            std::string fFilename;

            monarch::FormatModeType fFormatMode;

            double fDigitizerFullscale;

        public:
            bool OpenFile();
            void CloseFile();

            void WriteHeader(KTEggHeader* header);

            bool WriteTSData(KTSliceHeader& slHeader, KTTimeSeriesData& tsData);

            FileStatus GetFileStatus() const;

        private:
            bool CopyATimeSeries(unsigned component, const KTSliceHeader& slHeader, const KTTimeSeriesData& tsData, MonarchRecord* record);

            FileStatus fFileStatus;
            unsigned fExpectedNChannels;
            unsigned fExpectedRecordSize;

            monarch::Monarch* fMonarch;

            //**************
            // Slots
            //**************
        private:
            Nymph::KTSlotOneArg< void (KTEggHeader*) > fHeaderSlot;
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTTimeSeriesData > fTimeSeriesSlot;
            Nymph::KTSlotNoArg< void () > fDoneSlot;
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

    inline monarch::FormatModeType KTEggWriter::GetFormatMode() const
    {
        return fFormatMode;
    }

    inline void KTEggWriter::SetFormatMode(monarch::FormatModeType mode)
    {
        fFormatMode = mode;
        return;
    }

    inline KTEggWriter::FileStatus KTEggWriter::GetFileStatus() const
    {
        return fFileStatus;
    }

    inline double KTEggWriter::GetDigitizerFullscale() const
    {
        return fDigitizerFullscale;
    }

    inline void KTEggWriter::SetDigitizerFullscale(double scale)
    {
        fDigitizerFullscale = scale;
        return;
    }



} /* namespace Katydid */
#endif /* KTEGGWRITER_HH_ */
