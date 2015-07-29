/**
 @file KTBasicROOTFileWriter.hh
 @brief Contains KTBasicROOTFileWriter
 @details Basic Rootfile writer.
 @author: N. S. Oblath
 @date: Aug 24, 2012
 */

#ifndef KTBASICROOTFILEWRITER_HH_
#define KTBASICROOTFILEWRITER_HH_

#include "KTWriter.hh"

#include "TFile.h"

namespace Katydid
{
    class KTBasicROOTFileWriter;

    typedef KTDerivedTypeWriter< KTBasicROOTFileWriter > KTBasicROOTTypeWriter;

  /*!
     @class KTBasicROOTFileWriter
     @author N. S. Oblath

     @brief Outputs the histograms directly to a root file.

     @details 

     Configuration name: "basic-root-writer"

     Available configuration values:
     - "output-file": string -- output filename
     - "file-flag": string -- TFile option: CREATE, RECREATE, or UPDATE

     Slots:
     - "aa":
     - "aa-dist":
     - "corr":
     - "corr-dist":
     - "corr-ts":
     - "corr-ts-dist":
     - "hough":
     - "gain-var":
     - "fs-fftw":
     - "fs-polar":
     - "fs-fftw-phase":
     - "fs-polar-phase":
     - "fs-fftw-power":
     - "fs-polar-power":
     - "fs-fftw-power-dist":
     - "fs-polar-power-dist":
     - "multi-fs-fftw":
     - "multi-fs-polar":
     - "norm-fs-fftw":
     - "norm-fs-polar":
     - "ts":
     - "ts-dist":
     - "wv":
     - "wv-2d":
     
    */

    class KTBasicROOTFileWriter : public KTWriterWithTypists< KTBasicROOTFileWriter, KTBasicROOTTypeWriter >//public KTWriter
    {
        public:
            KTBasicROOTFileWriter(const std::string& name = "basic-root-writer");
            virtual ~KTBasicROOTFileWriter();

            bool Configure(const KTParamNode* node);

        public:
            TFile* OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            const std::string& GetFileFlag() const;
            void SetFileFlag(const std::string& flag);

            TFile* GetFile();

            bool OpenAndVerifyFile();

        protected:
            std::string fFilename;
            std::string fFileFlag;

            TFile* fFile;

    };

    inline TFile* KTBasicROOTFileWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = new TFile(filename.c_str(), flag.c_str());
        return fFile;
    }
    inline void KTBasicROOTFileWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    inline const std::string& KTBasicROOTFileWriter::GetFilename() const
    {
        return fFilename;
    }
    inline void KTBasicROOTFileWriter::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTBasicROOTFileWriter::GetFileFlag() const
    {
        return fFileFlag;
    }
    inline void KTBasicROOTFileWriter::SetFileFlag(const std::string& flag)
    {
        fFileFlag = flag;
        return;
    }

    inline TFile* KTBasicROOTFileWriter::GetFile()
    {
        return fFile;
    }


} /* namespace Katydid */
#endif /* KTBASICROOTFILEWRITER_HH_ */
