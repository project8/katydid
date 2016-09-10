/**
 @file KTJSONWriter.hh
 @brief Contains KTJSONWriter
 @details 
 @author: N. S. Oblath
 @date: Jan 3, 2013
*/

#ifndef KTJSONWRITER_HH_
#define KTJSONWRITER_HH_

#include "KTWriter.hh"

#include "KTJSONMaker.hh"

#include <cstdio>


namespace Katydid
{
    
    class KTJSONWriter;
    typedef Nymph::KTDerivedTypeWriter< KTJSONWriter > KTJSONTypeWriter;

    /*!
     @class KTJSONWriter
     @author N. S. Oblath

     @brief JSON file writer

     @details

     Configuration name: "json-writer"

     Available configuration values:
     - "output-file": string -- output filename
     - "pretty-json": bool -- if true, prints a human-readable file
     - "file-mode": string -- cstdio FILE mode: w, a, r+, w+ or a+

     Slots:
     - "frequency-candidates": void WriteFrequencyCandidates(const KTFrequencyCandidateData*)
     - "header": void WriteEggHeader(const KTEggHeader*)
    */


    class KTJSONWriter : public Nymph::KTWriterWithTypists< KTJSONWriter, KTJSONTypeWriter >
    {
        public:
            typedef KTJSONMaker< rapidjson::FileStream > JSONMaker;

        public:
            KTJSONWriter(const std::string& name = "json-writer");
            virtual ~KTJSONWriter();

            bool Configure(const scarab::param_node* node);

        public:
            bool OpenFile();
            void CloseFile();

            bool OpenAndVerifyFile();

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            const std::string& GetFileMode() const;
            void SetFileMode(const std::string& mode);

            bool GetPrettyJSONFlag() const;
            void SetPrettyJSONFlag(bool flag);

            KTJSONMaker< rapidjson::FileStream >* GetJSONMaker() const;

        protected:
            std::string fFilename;
            std::string fFileMode;

            bool fPrettyJSONFlag;

            FILE* fFile;
            rapidjson::FileStream* fFileStream;
            KTJSONMaker< rapidjson::FileStream >* fJSONMaker;

    };

    inline const std::string& KTJSONWriter::GetFilename() const
    {
        return fFilename;
    }
    inline void KTJSONWriter::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTJSONWriter::GetFileMode() const
    {
        return fFileMode;
    }
    inline void KTJSONWriter::SetFileMode(const std::string& mode)
    {
        if (mode == "w" || mode == "a" || mode == "r+" || mode == "w+" || mode == "a+")
        {
            fFileMode = mode;
        }
        return;
    }

    inline bool KTJSONWriter::GetPrettyJSONFlag() const
    {
        return fPrettyJSONFlag;
    }
    inline void KTJSONWriter::SetPrettyJSONFlag(bool flag)
    {
        fPrettyJSONFlag = flag;
        return;
    }

    inline KTJSONMaker< rapidjson::FileStream >* KTJSONWriter::GetJSONMaker() const
    {
        return fJSONMaker;
    }

} /* namespace Katydid */
#endif /* KTJSONWRITER_HH_ */
