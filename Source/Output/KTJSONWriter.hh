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

            const std::string& GetFileFlag() const;
            void SetFileFlag(const std::string& flag);

            //TFile* GetFile();

        protected:
            std::string fFilename;
            std::string fFileFlag;

            //TFile* fFile;

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

    inline const std::string& KTJSONWriter::GetFileFlag() const
    {
        return fFileFlag;
    }
    inline void KTJSONWriter::SetFileFlag(const std::string& flag)
    {
        fFileFlag = flag;
        return;
    }
/*
    inline TFile* KTJSONWriter::GetFile()
    {
        return fFile;
    }
*/

} /* namespace Katydid */
#endif /* KTJSONWRITER_HH_ */
