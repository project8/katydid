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

#include "document.h"

#include <cstdio>
#include <deque>
#include <string>

namespace Katydid
{
    using namespace Nymph;
    /*!
     @class KTMultiFileJSONReader
     @author N. S. Oblath

     @brief JSON file reader for reading multiple files, and emitting a signal for each

     @details
     Multiple data-types can be read from each file by specifying multiple run-data-types.

     Configuration name: "multifile-json-reader"

     Available configuration values:
     - "input-file": string -- input filename (may be repeated)
     - "file-mode": string -- cstdio FILE mode: r (default), a, r+, a+
     - "data-type": string -- the type of file being read (may be repeated). This option is only necessary if the processor is being used as a primary processor.  See options below.

     The run-data-type option determines the function used to read the file.
     The available options are:
     - "mc-truth-events" -- Emits signal "mc-truth-events" after file read
     - "analysis-candidates" -- Emits signal "analysis-candidates" after file read
     - "cc-results" -- Emits signal "cc-results" after a file is read

     Slots:
     - "mc-truth-events": void (KTDataPtr) -- Add MC truth events data; Requires KTData; Adds KTMCTruthEvents; Emits signal "mc-truth-events" upon successful file read.
     - "analysis-candidates": void (KTDataPtr) -- Add analysis candidates data; Requires KTData; Adds KTAnalysisCandidates; Emits signal "analysis-candidates" upon successful file read.
     - "cc-results": void (KTDataPtr) -- Add CC (candidate comparison) Results data; Requires KTData; Adds KTCCResults; Emits signal "cc-results" upon successful file read.

     Signals:
     - "mc-truth-events": void (KTDataPtr) -- Emitted after reading an mc-truth-events file; Guarantees KTMCTruthEvents.
     - "analysis-candidates": void (shared-ptr<KTData>) -- Emitted after reading an analysis candidates file; Guarantees KTAnalysisCandidates.
     - "cc-results": void (KTDataPtr) -- Emitted after reading an cc-results file; Guarantees KTCCResults.
    */


    class KTMultiFileJSONReader : public KTReader
    {
        private:
            typedef bool (KTMultiFileJSONReader::*AppendFcn)(rapidjson::Document&, KTData&);
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

            bool Configure(const scarab::param_node* node);

            const std::deque< std::string >& GetFilenames() const;
            void AddFilename(const std::string& filename);

            const std::string& GetFileMode() const;
            void SetFileMode(const std::string& mode);

            const std::deque< DataType >& GetDataTypes() const;
            bool AddDataType(const std::string& type);

        private:
            std::deque< std::string > fFilenames;
            std::deque< std::string >::const_iterator fFileIter;

            std::string fFileMode;

        public:
            virtual bool Run();

            bool Append(KTData& data);

        private:
            std::deque< DataType > fDataTypes;

            bool OpenAndParseFile(const std::string& filename, rapidjson::Document& document) const;

        private:
            bool AppendMCTruthEvents(rapidjson::Document& document, KTData& data);
            bool AppendAnalysisCandidates(rapidjson::Document& document, KTData& data);
            bool AppendCCResults(rapidjson::Document& document, KTData& data);


            //**************
            // Signals
            //**************
        private:
            KTSignalData fMCTruthEventsSignal;
            KTSignalData fAnalysisCandidatesSignal;
            KTSignalData fCCResultsSignal;
            KTSignalOneArg< void > fDoneSignal;

            //**************
            // Slots
            //**************
        private:
            KTSlotDataOneType< KTData > fAppendMCTruthEventsSlot;
            KTSlotDataOneType< KTData > fAppendAnalysisCandidatesSlot;
            KTSlotDataOneType< KTData > fAppendCCResultsSlot;
    };

    inline const std::deque< std::string >& KTMultiFileJSONReader::GetFilenames() const
    {
        return fFilenames;
    }

    inline void KTMultiFileJSONReader::AddFilename(const std::string& filename)
    {
        fFilenames.push_back(filename);
        fFileIter = fFilenames.begin();
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
