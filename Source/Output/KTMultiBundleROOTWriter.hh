/*
 * KTMultiBundleROOTWriter.hh
 *
 *  Created on: Jan 28, 2013
 *      Author: nsoblath
 */

#ifndef KTMULTIBUNDLEROOTWRITER_HH_
#define KTMULTIBUNDLEROOTWRITER_HH_

#include "KTWriter.hh"

#include "TFile.h"

namespace Katydid
{
    class KTMultiBundleROOTWriter;

    typedef KTDerivedTypeWriter< KTMultiBundleROOTWriter > KTMultiBundleROOTTypeWriter;
    class KTMEROOTTypeWriterBase : public KTMultiBundleROOTTypeWriter
    {
        public:
            KTMEROOTTypeWriterBase() {}
            virtual ~KTMEROOTTypeWriterBase() {}

            virtual void StartNewHistograms() = 0;

            virtual void FinishHistograms() = 0;
    };


    class KTMultiBundleROOTWriter : public KTWriterWithTypists< KTMultiBundleROOTWriter >//public KTWriter
    {
        public:
            KTMultiBundleROOTWriter();
            virtual ~KTMultiBundleROOTWriter();

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

    };

    inline TFile* KTMultiBundleROOTWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = new TFile(filename.c_str(), flag.c_str());
        return fFile;
    }
    inline void KTMultiBundleROOTWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    inline Bool_t KTMultiBundleROOTWriter::GetUseTFile() const
    {
        return fUseTFile;
    }
    inline void KTMultiBundleROOTWriter::SetUseTFile(Bool_t flag)
    {
        fUseTFile = flag;
        return;
    }

    inline const std::string& KTMultiBundleROOTWriter::GetTFilename() const
    {
        return fTFilename;
    }
    inline void KTMultiBundleROOTWriter::SetTFilename(const std::string& filename)
    {
        fTFilename = filename;
        return;
    }

    inline const std::string& KTMultiBundleROOTWriter::GetTFileFlag() const
    {
        return fTFileFlag;
    }
    inline void KTMultiBundleROOTWriter::SetTFileFlag(const std::string& flag)
    {
        fTFileFlag = flag;
        return;
    }

    inline Bool_t KTMultiBundleROOTWriter::GetUseGraphics() const
    {
        return fUseGraphics;
    }
    inline void KTMultiBundleROOTWriter::SetUseGraphics(Bool_t flag)
    {
        fUseGraphics = flag;
        return;
    }

    inline const std::string& KTMultiBundleROOTWriter::GetGraphicsFilePath() const
    {
        return fGraphicsFilePath;
    }

    inline void KTMultiBundleROOTWriter::SetGraphicsFilePath(const std::string& path)
    {
        fGraphicsFilePath = path;
        return;
    }

    inline const std::string& KTMultiBundleROOTWriter::GetGraphicsFilenameBase() const
    {
        return fGraphicsFilenameBase;
    }

    inline void KTMultiBundleROOTWriter::SetGraphicsFilenameBase(const std::string& fname)
    {
        fGraphicsFilenameBase = fname;
        return;
    }

    inline const std::string& KTMultiBundleROOTWriter::GetGraphicsFileType() const
    {
        return fGraphicsFileType;
    }

    inline void KTMultiBundleROOTWriter::SetGraphicsFileType(const std::string& type)
    {
        fGraphicsFileType = type;
        return;
    }

    inline TFile* KTMultiBundleROOTWriter::GetFile()
    {
        return fFile;
    }


} /* namespace Katydid */
#endif /* KTMULTIBUNDLEROOTWRITER_HH_ */
