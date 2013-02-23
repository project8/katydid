/*
 * KTMultiSliceROOTWriter.hh
 *
 *  Created on: Jan 28, 2013
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTWRITER_HH_
#define KTMULTISLICEROOTWRITER_HH_

#include "KTWriter.hh"

#include "TFile.h"

namespace Katydid
{
    class KTMultiSliceROOTWriter;

    typedef KTDerivedTypeWriter< KTMultiSliceROOTWriter > KTMultiSliceROOTTypeWriter;

    class KTMEROOTTypeWriterBase : public KTMultiSliceROOTTypeWriter
    {
        public:
            KTMEROOTTypeWriterBase() {}
            virtual ~KTMEROOTTypeWriterBase() {}

            virtual void StartNewHistograms() = 0;

            virtual void FinishHistograms() = 0;
    };


    class KTMultiSliceROOTWriter : public KTWriterWithTypists< KTMultiSliceROOTWriter >//public KTWriter
    {
        public:
            KTMultiSliceROOTWriter();
            virtual ~KTMultiSliceROOTWriter();

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

    inline TFile* KTMultiSliceROOTWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = new TFile(filename.c_str(), flag.c_str());
        return fFile;
    }
    inline void KTMultiSliceROOTWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    inline Bool_t KTMultiSliceROOTWriter::GetUseTFile() const
    {
        return fUseTFile;
    }
    inline void KTMultiSliceROOTWriter::SetUseTFile(Bool_t flag)
    {
        fUseTFile = flag;
        return;
    }

    inline const std::string& KTMultiSliceROOTWriter::GetTFilename() const
    {
        return fTFilename;
    }
    inline void KTMultiSliceROOTWriter::SetTFilename(const std::string& filename)
    {
        fTFilename = filename;
        return;
    }

    inline const std::string& KTMultiSliceROOTWriter::GetTFileFlag() const
    {
        return fTFileFlag;
    }
    inline void KTMultiSliceROOTWriter::SetTFileFlag(const std::string& flag)
    {
        fTFileFlag = flag;
        return;
    }

    inline Bool_t KTMultiSliceROOTWriter::GetUseGraphics() const
    {
        return fUseGraphics;
    }
    inline void KTMultiSliceROOTWriter::SetUseGraphics(Bool_t flag)
    {
        fUseGraphics = flag;
        return;
    }

    inline const std::string& KTMultiSliceROOTWriter::GetGraphicsFilePath() const
    {
        return fGraphicsFilePath;
    }

    inline void KTMultiSliceROOTWriter::SetGraphicsFilePath(const std::string& path)
    {
        fGraphicsFilePath = path;
        return;
    }

    inline const std::string& KTMultiSliceROOTWriter::GetGraphicsFilenameBase() const
    {
        return fGraphicsFilenameBase;
    }

    inline void KTMultiSliceROOTWriter::SetGraphicsFilenameBase(const std::string& fname)
    {
        fGraphicsFilenameBase = fname;
        return;
    }

    inline const std::string& KTMultiSliceROOTWriter::GetGraphicsFileType() const
    {
        return fGraphicsFileType;
    }

    inline void KTMultiSliceROOTWriter::SetGraphicsFileType(const std::string& type)
    {
        fGraphicsFileType = type;
        return;
    }

    inline TFile* KTMultiSliceROOTWriter::GetFile()
    {
        return fFile;
    }


} /* namespace Katydid */
#endif /* KTMULTISICEROOTWRITER_HH_ */
