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

#include <cstdio>


namespace Katydid
{
    /*!
     @class KTOfficialCandidatesWriter
     @author N. S. Oblath

     @brief JSON file writer

     @details

     Available configuration values:
     \li \c "output-file": string -- output filename
     \li \c "pretty-json": bool -- if true, prints a human-readable file
     \li \c "file-mode": string -- cstdio FILE mode: w, a, r+, w+ or a+

     Slots:
     \li \c "frequency-candidates": void WriteFrequencyCandidates(const KTFrequencyCandidateData*)
     \li \c "header": void WriteEggHeader(const KTEggHeader*)
    */


    class KTOfficialCandidatesWriter : public KTWriter
    {
        public:
            typedef KTJSONMaker< rapidjson::FileStream > JSONMaker;

        public:
            KTOfficialCandidatesWriter(const std::string& name = "json-writer");
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

        protected:
            std::string fFilename;
            std::string fFileMode;

            Bool_t fPrettyJSONFlag;

            FILE* fFile;
            rapidjson::FileStream* fFileStream;
            KTJSONMaker< rapidjson::FileStream >* fJSONMaker;

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

} /* namespace Katydid */
#endif /* KTOFFICIALCANDIDATESWRITER_HH_ */
