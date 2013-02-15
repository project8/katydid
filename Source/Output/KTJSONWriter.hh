/*
 * KTJSONWriter.hh
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONWRITER_HH_
#define KTJSONWRITER_HH_

#include "KTWriter.hh"

#include "KTJSONMaker.hh"

#include <cstdio>


namespace Katydid
{
    class KTJSONWriter;
    typedef KTDerivedTypeWriter< KTJSONWriter > KTJSONTypeWriter;


    class KTJSONWriter : public KTWriterWithTypists< KTJSONWriter >
    {
        public:
            typedef KTJSONMaker< rapidjson::FileStream > JSONMaker;

        public:
            KTJSONWriter();
            virtual ~KTJSONWriter();

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

    inline Bool_t KTJSONWriter::GetPrettyJSONFlag() const
    {
        return fPrettyJSONFlag;
    }
    inline void KTJSONWriter::SetPrettyJSONFlag(Bool_t flag)
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
