/**
 @file KTOneShotJSONReader.hh
 @brief Contains KTOneShotJSONReader
 @details 
 @author: N. S. Oblath
 @date: Apr 11, 2013
*/

#ifndef KTONESHOTJSONREADER_HH_
#define KTONESHOTJSONREADER_HH_

#include "KTReader.hh"

#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

#include "document.h"

#include <cstdio>
#include <string>

namespace Katydid
{
    class KTData;

    /*!
     @class KTOneShotJSONReader
     @author N. S. Oblath

     @brief JSON file reader for reading an entire file at once

     @details

     Available configuration values:
     - "input-file": string -- input filename
     - "file-mode": string -- cstdio FILE mode: r (default), a, r+, a+
     - "file-type": string -- the type of file being read. See options below.

     The file-type option determines the function used to read the file.
     The available options are:
     - "mc-truth-events" -- Emits signal "mc-truth-events" after file read
     - "analysis-candidates" -- Emits signal "analysis-candidates" after file read

     Signals:
     - "mc-truth-events": void (shared_ptr<KTData>) -- Emitted after reading an mc-truth-events file; Guarantees KTMCTruthEvents.
     - "analysis-candidates": void (shared-ptr<KTData>) -- Emitted after reading an analysis candidates file; Guarantees KTAnalysisCandidates.
    */


    class KTOneShotJSONReader : public KTReader
    {
        public:
            KTOneShotJSONReader(const std::string& name = "oneshot-json-reader");
            virtual ~KTOneShotJSONReader();

            Bool_t Configure(const KTPStoreNode* node);

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            const std::string& GetFileMode() const;
            void SetFileMode(const std::string& mode);

            const std::string& GetFileType() const;
            Bool_t SetFileType(const std::string& type);

        private:
            std::string fFilename;
            std::string fFileMode;
            std::string fFileType;


        public:
            virtual Bool_t Run();

        private:
            Bool_t (KTOneShotJSONReader::*fRunFcn)();

            Bool_t OpenAndParseFile(rapidjson::Document& document);

        public:
            boost::shared_ptr<KTData> ReadMCTruthEventsFile(boost::shared_ptr<KTData> appendToData = boost::shared_ptr<KTData>());
            boost::shared_ptr<KTData> ReadAnalysisCandidatesFile(boost::shared_ptr<KTData> appendToData = boost::shared_ptr<KTData>());

            Bool_t AppendMCTruthEventsFile(KTData& data);
            Bool_t AppendAnalysisCandidatesFile(KTData& data);

            Bool_t RunMCTruthEventsFile();
            Bool_t RunAnalysisCandidatesFile();


            //**************
            // Signals
            //**************
        private:
            KTSignalData fMCTruthEventsSignal;
            KTSignalData fAnalysisCandidatesSignal;

            //**************
            // Slots
            //**************
        private:
            KTSlotDataOneType< KTData > fAppendMCTruthEventsSlot;
            KTSlotDataOneType< KTData > fAppendAnalysisCandidatesSlot;
    };

    inline const std::string& KTOneShotJSONReader::GetFilename() const
    {
        return fFilename;
    }
    inline void KTOneShotJSONReader::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTOneShotJSONReader::GetFileMode() const
    {
        return fFileMode;
    }
    inline void KTOneShotJSONReader::SetFileMode(const std::string& mode)
    {
        if (mode == "w" || mode == "a" || mode == "r+" || mode == "w+" || mode == "a+")
        {
            fFileMode = mode;
        }
        return;
    }

    inline const std::string& KTOneShotJSONReader::GetFileType() const
    {
        return fFileType;
    }

    inline Bool_t KTOneShotJSONReader::Run()
    {
        return (this->*fRunFcn)();
    }

} /* namespace Katydid */
#endif /* KTONESHOTJSONREADER_HH_ */
