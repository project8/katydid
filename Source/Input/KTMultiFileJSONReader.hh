/**
 @file KTMultiFileJSONReader.hh
 @brief Contains KTMultiFileJSONReader
 @details 
 @author: N. S. Oblath
 @date: May 31, 2013
*/

#ifndef KTMULTIFILEJSONREADER_HH_
#define KTMULTIFILEJSONREADER_HH_

#include "KTReader.hh"

#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

#include "document.h"

#include <cstdio>
#include <deque>
#include <string>

namespace Katydid
{
    class KTData;

    /*!
     @class KTMultiFileJSONReader
     @author N. S. Oblath

     @brief JSON file reader for reading multiple files, and emitting a signal for each

     @details
     Multiple data-types can be read from each file by specifying multiple run-data-types. This only works if the processor is being used as a primary processor.

     Configuration name: "multifile-json-reader"

     Available configuration values:
     - "input-file": string -- input filename (may be repeated)
     - "file-mode": string -- cstdio FILE mode: r (default), a, r+, a+
     - "run-data-type": string -- the type of file being read (may be repeated). This option is only necessary if the processor is being used as a primary processor.  See options below.

     The run-data-type option determines the function used to read the file.
     The available options are:
     - "cc-results" -- Emits signal "cc-results" after a file is read

     Signals:
     - "cc-results": void (shared_ptr<KTData>) -- Emitted after reading an mc-truth-events file; Guarantees KTMCTruthEvents.
    */


    class KTMultiFileJSONReader : public KTReader
    {
        private:
            typedef Bool_t (KTMultiFileJSONReader::*AppendFcn)(rapidjson::Document&, KTData&);
            struct DataType
            {
                    std::string fName;
                    AppendFcn fAppendFcn;
                    KTSignalData* fSignal;
                    DataType(const std::string& name, AppendFcn fcn, KTSignalData* signal)
                    {
                        fName = name;
                        fAppendFcn = fcn;
                        fSignal = signal;
                    }
            };

        public:
            KTMultiFileJSONReader(const std::string& name = "multifile-json-reader");
            virtual ~KTMultiFileJSONReader();

            Bool_t Configure(const KTPStoreNode* node);

            const std::deque< std::string >& GetFilenames() const;
            void AddFilename(const std::string& filename);

            const std::string& GetFileMode() const;
            void SetFileMode(const std::string& mode);

            const std::deque< DataType >& GetDataTypes() const;
            Bool_t AddDataType(const std::string& type);

        private:
            std::deque< std::string > fFilenames;
            std::string fFileMode;


        public:
            virtual Bool_t Run();

        private:
            std::deque< DataType > fDataTypes;

            Bool_t OpenAndParseFile(const std::string& filename, rapidjson::Document& document) const;

        private:
            Bool_t AppendCCResults(rapidjson::Document& document, KTData& data);


            //**************
            // Signals
            //**************
        private:
            KTSignalData fCCResultsSignal;

            //**************
            // Slots
            //**************
        //private:
            //KTSlotDataOneType< KTData > fAppendCCResultsSlot;
    };

    inline const std::deque< std::string >& KTMultiFileJSONReader::GetFilenames() const
    {
        return fFilenames;
    }

    inline void KTMultiFileJSONReader::AddFilename(const std::string& filename)
    {
        fFilenames.push_back(filename);
        return;
    }

    inline const std::string& KTMultiFileJSONReader::GetFileMode() const
    {
        return fFileMode;
    }

    inline void KTMultiFileJSONReader::SetFileMode(const std::string& mode)
    {
        if (mode == "w" || mode == "a" || mode == "r+" || mode == "w+" || mode == "a+")
        {
            fFileMode = mode;
        }
        return;
    }

    inline const std::deque< KTMultiFileJSONReader::DataType >& KTMultiFileJSONReader::GetDataTypes() const
    {
        return fDataTypes;
    }


} /* namespace Katydid */
#endif /* KTMULTIFILEJSONREADER_HH_ */
