/**
 @file KTJSONReader.hh
 @brief Contains KTJSONReader
 @details 
 @author: N. S. Oblath
 @date: Apr 11, 2013
*/

#ifndef KTJSONREADER_HH_
#define KTJSONREADER_HH_

#include "KTReader.hh"

#include "KTJSONMaker.hh"
#include "KTSignal.hh"

#include <boost/shared_ptr.hpp>

#include <cstdio>
#include <string>

namespace Katydid
{
    class KTData;

    /*!
     @class KTJSONReader
     @author N. S. Oblath

     @brief JSON file reader

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


    class KTJSONReader : public KTReader
    {
        public:
            typedef KTJSONMaker< rapidjson::FileStream > JSONMaker;

        public:
            KTJSONReader(const std::string& name = "json-writer");
            virtual ~KTJSONReader();

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

            //Bool_t ReadFile();


            //Bool_t OpenFile();
            //void CloseFile();

            //Bool_t OpenAndVerifyFile();

            //KTJSONMaker< rapidjson::FileStream >* GetJSONMaker() const;

        private:
            Bool_t (KTJSONReader::*fRunFcn)();

            //FILE* fFile;
            //rapidjson::FileStream* fFileStream;
            //KTJSONMaker< rapidjson::FileStream >* fJSONMaker;

        public:
            boost::shared_ptr<KTData> ReadMCTruthEventsFile();
            boost::shared_ptr<KTData> ReadAnalysisCandidatesFile();

            Bool_t RunMCTruthEventsFile();
            Bool_t RunAnalysisCandidatesFile();


            //**************
            // Signals
            //**************
        private:
            KTSignalData fMCTruthEventsSignal;
            KTSignalData fAnalysisCandidatesSignal;

    };

    inline const std::string& KTJSONReader::GetFilename() const
    {
        return fFilename;
    }
    inline void KTJSONReader::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTJSONReader::GetFileMode() const
    {
        return fFileMode;
    }
    inline void KTJSONReader::SetFileMode(const std::string& mode)
    {
        if (mode == "w" || mode == "a" || mode == "r+" || mode == "w+" || mode == "a+")
        {
            fFileMode = mode;
        }
        return;
    }

    inline const std::string& KTJSONReader::GetFileType() const
    {
        return fFileType;
    }

    inline Bool_t KTJSONReader::Run()
    {
        return (this->*fRunFcn)();
    }


    //inline KTJSONMaker< rapidjson::FileStream >* KTJSONReader::GetJSONMaker() const
    //{
    //    return fJSONMaker;
    //}

} /* namespace Katydid */
#endif /* KTJSONREADER_HH_ */
