/*
 * KTJSONWriter.hh
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONWRITER_HH_
#define KTJSONWRITER_HH_

#include "KTWriter.hh"


namespace Katydid
{
    class KTJSONWriter;

    typedef KTDerivedTypeWriter< KTJSONWriter > KTBasicROOTTypeWriter;


    class KTJSONWriter : public KTWriterWithTypists< KTJSONWriter >
    {
        public:
            KTJSONWriter();
            virtual ~KTJSONWriter();

            Bool_t Configure(const KTPStoreNode* node);

        public:
            void OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            const std::string& GetFileMode() const;
            void SetFileMode(const std::string& mode);

            Bool_t GetPrettyJSONFlag() const;
            void SetPrettyJSONFlag(Bool_t flag);

        protected:
            std::string fFilename;
            std::string fFileMode;

            Bool_t fPrettyJSONFlag;

            //************************
            // Basic Publish and Write
            //************************
        public:

            void Publish(const KTWriteableData* data);

            void Write(const KTWriteableData* data);

    };

    inline void KTJSONWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        return;
    }
    inline void KTJSONWriter::CloseFile()
    {
        return;
    }

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

} /* namespace Katydid */
#endif /* KTJSONWRITER_HH_ */
