/**
 @file KTOfficialCandidatesWriter.hh
 @brief Contains KTOfficialCandidatesWriter
 @details 
 @author: N. S. Oblath
 @date: Apr 23, 2013
*/

#ifndef KTOFFICIALCANDIDATESWRITER_HH_
#define KTOFFICIALCANDIDATESWRITER_HH_

#include "KTWriter.hh"

#include "KTJSONMaker.hh"
#include "KTSlot.hh"

#include <cstdio>


namespace Katydid
{
    class KTEggHeader;
    class KTWaterfallCandidateData;

    /*!
     @class KTOfficialCandidatesWriter
     @author N. S. Oblath

     @brief JSON file writer

     @details

     Configuration name: "official-candidate-writer"

     Available configuration values:
     - "output-file": string -- output filename
     - "pretty-json": bool -- if true, prints a human-readable file
     - "file-mode": string -- cstdio FILE mode: w, a, r+, w+ (default) or a+

     Slots:
     - "header": void (const KTEggHeader*) -- writes the header information to the candidates file; not valid if candidate writing has started
     - "waterfall-candidate": void (boost::shared_ptr<KTData>) -- writes candidate information; starts candidate writing mode if it hasn't started yet
     - "stop": void () -- stops writing candidates and closes the file
    */

    class KTOfficialCandidatesWriter : public KTWriter
    {
        public:
            typedef KTJSONMaker< rapidjson::FileStream > JSONMaker;

            enum Status
            {
                kNotOpenedYet,
                kPriorToCandidates,
                kWritingCandidates,
                kStopped
            };

        public:
            KTOfficialCandidatesWriter(const std::string& name = "official-candidate-writer");
            virtual ~KTOfficialCandidatesWriter();

            Bool_t Configure(const KTPStoreNode* node);

        public:
            Bool_t OpenFile();
            void CloseFile();

            Bool_t OpenAndVerifyFile();

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            const std::string& GetFileMode() const;
            void SetFileMode(const std::string& mode);

            Bool_t GetPrettyJSONFlag() const;
            void SetPrettyJSONFlag(Bool_t flag);

            KTJSONMaker< rapidjson::FileStream >* GetJSONMaker() const;

            Status GetStatus() const;

        protected:
            std::string fFilename;
            std::string fFileMode;

            Bool_t fPrettyJSONFlag;

            FILE* fFile;
            rapidjson::FileStream* fFileStream;
            KTJSONMaker< rapidjson::FileStream >* fJSONMaker;

            Status fStatus;

        public:
            void WriteHeaderInformation(const KTEggHeader* header);

            Bool_t WriteWaterfallCandidate(KTWaterfallCandidateData& wcData);

            //**************
            // Slots
            //**************
        private:
            KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            KTSlotDataOneType< KTWaterfallCandidateData > fWaterfallCandidateSlot;
            KTSlotNoArg< void () > fStopWritingSlot;
    };

    inline const std::string& KTOfficialCandidatesWriter::GetFilename() const
    {
        return fFilename;
    }
    inline void KTOfficialCandidatesWriter::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTOfficialCandidatesWriter::GetFileMode() const
    {
        return fFileMode;
    }
    inline void KTOfficialCandidatesWriter::SetFileMode(const std::string& mode)
    {
        if (mode == "w" || mode == "a" || mode == "r+" || mode == "w+" || mode == "a+")
        {
            fFileMode = mode;
        }
        return;
    }

    inline Bool_t KTOfficialCandidatesWriter::GetPrettyJSONFlag() const
    {
        return fPrettyJSONFlag;
    }
    inline void KTOfficialCandidatesWriter::SetPrettyJSONFlag(Bool_t flag)
    {
        fPrettyJSONFlag = flag;
        return;
    }

    inline KTJSONMaker< rapidjson::FileStream >* KTOfficialCandidatesWriter::GetJSONMaker() const
    {
        return fJSONMaker;
    }

    inline KTOfficialCandidatesWriter::Status KTOfficialCandidatesWriter::GetStatus() const
    {
        return fStatus;
    }

} /* namespace Katydid */
#endif /* KTOFFICIALCANDIDATESWRITER_HH_ */
