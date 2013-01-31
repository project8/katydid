/*
 * KTMultiEventROOTWriter.hh
 *
 *  Created on: Jan 28, 2013
 *      Author: nsoblath
 */

#ifndef KTMULTIEVENTROOTWRITER_HH_
#define KTMULTIEVENTROOTWRITER_HH_

#include "KTWriter.hh"

#include "TFile.h"

namespace Katydid
{
    class KTMultiEventROOTWriter;

    typedef KTDerivedTypeWriter< KTMultiEventROOTWriter > KTMultiEventROOTTypeWriter;
    class KTMEROOTTypeWriterBase : public KTMultiEventROOTTypeWriter
    {
        public:
            KTMEROOTTypeWriterBase() {}
            virtual ~KTMEROOTTypeWriterBase() {}

            virtual void StartNewHistograms() = 0;

            virtual void FinishHistograms() = 0;
    };


    class KTMultiEventROOTWriter : public KTWriterWithTypists< KTMultiEventROOTWriter >//public KTWriter
    {
        public:
            KTMultiEventROOTWriter();
            virtual ~KTMultiEventROOTWriter();

            Bool_t Configure(const KTPStoreNode* node);

        public:
            TFile* OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            Bool_t GetUseTFile() const;
            void SetUseTFile(Bool_t flag);

            const std::string& GetTFilename() const;
            void SetTFilename(const std::string& filename);

            const std::string& GetTFileFlag() const;
            void SetTFileFlag(const std::string& flag);

            Bool_t GetUseGraphics() const;
            void SetUseGraphics(Bool_t flag);

            const std::string& GetGraphicsFilePath() const;
            void SetGraphicsFilePath(const std::string& path);

            const std::string& GetGraphicsFilenameBase() const;
            void SetGraphicsFilenameBase(const std::string& fname);

            const std::string& GetGraphicsFileType() const;
            void SetGraphicsFileType(const std::string& type);

            TFile* GetFile();

            Bool_t OpenAndVerifyFile();

        private:
            Bool_t fUseTFile;
            std::string fTFilename;
            std::string fTFileFlag;

            Bool_t fUseGraphics;
            std::string fGraphicsFilePath;
            std::string fGraphicsFilenameBase;
            std::string fGraphicsFileType;

            TFile* fFile;

            //***********
            // Slots
            //***********

        public:
            void Start();

            void Finish();

            //************************
            // Basic Publish and Write
            //************************
        public:

            void Publish(const KTWriteableData* data);

            void Write(const KTWriteableData* data);

    };

    inline TFile* KTMultiEventROOTWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = new TFile(filename.c_str(), flag.c_str());
        return fFile;
    }
    inline void KTMultiEventROOTWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    inline Bool_t KTMultiEventROOTWriter::GetUseTFile() const
    {
        return fUseTFile;
    }
    inline void KTMultiEventROOTWriter::SetUseTFile(Bool_t flag)
    {
        fUseTFile = flag;
        return;
    }

    inline const std::string& KTMultiEventROOTWriter::GetTFilename() const
    {
        return fTFilename;
    }
    inline void KTMultiEventROOTWriter::SetTFilename(const std::string& filename)
    {
        fTFilename = filename;
        return;
    }

    inline const std::string& KTMultiEventROOTWriter::GetTFileFlag() const
    {
        return fTFileFlag;
    }
    inline void KTMultiEventROOTWriter::SetTFileFlag(const std::string& flag)
    {
        fTFileFlag = flag;
        return;
    }

    inline Bool_t KTMultiEventROOTWriter::GetUseGraphics() const
    {
        return fUseGraphics;
    }
    inline void KTMultiEventROOTWriter::SetUseGraphics(Bool_t flag)
    {
        fUseGraphics = flag;
        return;
    }

    inline const std::string& KTMultiEventROOTWriter::GetGraphicsFilePath() const
    {
        return fGraphicsFilePath;
    }

    inline void KTMultiEventROOTWriter::SetGraphicsFilePath(const std::string& path)
    {
        fGraphicsFilePath = path;
        return;
    }

    inline const std::string& KTMultiEventROOTWriter::GetGraphicsFilenameBase() const
    {
        return fGraphicsFilenameBase;
    }

    inline void KTMultiEventROOTWriter::SetGraphicsFilenameBase(const std::string& fname)
    {
        fGraphicsFilenameBase = fname;
        return;
    }

    inline const std::string& KTMultiEventROOTWriter::GetGraphicsFileType() const
    {
        return fGraphicsFileType;
    }

    inline void KTMultiEventROOTWriter::SetGraphicsFileType(const std::string& type)
    {
        fGraphicsFileType = type;
        return;
    }

    inline TFile* KTMultiEventROOTWriter::GetFile()
    {
        return fFile;
    }


} /* namespace Katydid */
#endif /* KTMULTIEVENTROOTWRITER_HH_ */
