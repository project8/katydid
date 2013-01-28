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

    //typedef KTDerivedTypeWriter< KTMultiEventROOTWriter > KTMultiEventROOTTypeWriter;
    class KTMultiEventROOTTypeWriter : public KTDerivedTypeWriter< KTMultiEventROOTWriter >
    {
        public:
            KTMultiEventROOTTypeWriter() {}
            virtual ~KTMultiEventROOTTypeWriter() {}

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

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            const std::string& GetFileFlag() const;
            void SetFileFlag(const std::string& flag);

            TFile* GetFile();

            Bool_t OpenAndVerifyFile();

        private:
            std::string fFilename;
            std::string fFileFlag;

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

    inline const std::string& KTMultiEventROOTWriter::GetFilename() const
    {
        return fFilename;
    }
    inline void KTMultiEventROOTWriter::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTMultiEventROOTWriter::GetFileFlag() const
    {
        return fFileFlag;
    }
    inline void KTMultiEventROOTWriter::SetFileFlag(const std::string& flag)
    {
        fFileFlag = flag;
        return;
    }

    inline TFile* KTMultiEventROOTWriter::GetFile()
    {
        return fFile;
    }


} /* namespace Katydid */
#endif /* KTMULTIEVENTROOTWRITER_HH_ */
