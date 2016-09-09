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
     - "mc-truth-events": void (Nymph::KTDataPtr) -- Add MC truth events data; Requires KTData; Adds KTMCTruthEvents; Emits signal "mc-truth-events" upon successful file read.
     - "analysis-candidates": void (Nymph::KTDataPtr) -- Add analysis candidates data; Requires KTData; Adds KTAnalysisCandidates; Emits signal "analysis-candidates" upon successful file read.
     - "cc-results": void (Nymph::KTDataPtr) -- Add CC (candidate comparison) Results data; Requires KTData; Adds KTCCResults; Emits signal "cc-results" upon successful file read.

     Signals:
     - "mc-truth-events": void (Nymph::KTDataPtr) -- Emitted after reading an mc-truth-events file; Guarantees KTMCTruthEvents.
     - "analysis-candidates": void (shared-ptr<KTData>) -- Emitted after reading an analysis candidates file; Guarantees KTAnalysisCandidates.
     - "cc-results": void (Nymph::KTDataPtr) -- Emitted after reading an cc-results file; Guarantees KTCCResults.
    */


    class KTMultiFileJSONReader : public Nymph::KTReader
    {
        private:
            typedef bool (KTMultiFileJSONReader::*AppendFcn)(rapidjson::Document&, Nymph::KTData&);
            struct DataType
            {
                    std::string fName;
                    AppendFcn fAppendFcn;
                    Nymph::KTSignalData* fSignal;
                    DataType(const std::string& name, AppendFcn fcn, Nymph::KTSignalData* signal)
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

            bool Append(Nymph::KTData& data);

        private:
            std::deque< DataType > fDataTypes;

            bool OpenAndParseFile(const std::string& filename, rapidjson::Document& document) const;

        private:
            bool AppendMCTruthEvents(rapidjson::Document& document, Nymph::KTData& data);
            bool AppendAnalysisCandidates(rapidjson::Document& document, Nymph::KTData& data);
            bool AppendCCResults(rapidjson::Document& document, Nymph::KTData& data);


            //**************
            // Signals
            //**************
        private:
            Nymph::KTSignalData fMCTruthEventsSignal;
            Nymph::KTSignalData fAnalysisCandidatesSignal;
            Nymph::KTSignalData fCCResultsSignal;
            Nymph::KTSignalOneArg< void > fDoneSignal;

            //**************
            // Slots
            //**************
        private:
            Nymph::KTSlotDataOneType< Nymph::KTData > fAppendMCTruthEventsSlot;
            Nymph::KTSlotDataOneType< Nymph::KTData > fAppendAnalysisCandidatesSlot;
            Nymph::KTSlotDataOneType< Nymph::KTData > fAppendCCResultsSlot;
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
