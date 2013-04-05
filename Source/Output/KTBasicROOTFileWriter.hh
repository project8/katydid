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

     Available configuration values:
     \li \c "output-file": string -- output filename
     \li \c "file-flag": string -- TFile option: CREATE, RECREATE, or UPDATE

     Slots:
     \li \c "aa":
     \li \c "corr":
     \li \c "hough":
     \li \c "gain-var":
     \li \c "fs-fftw":
     \li \c "fs-polar":
     \li \c "fs-fftw-phase":
     \li \c "fs-polar-phase":
     \li \c "fs-fftw-power":
     \li \c "fs-polar-power":
     \li \c "norm-fs-fftw":
     \li \c "norm-fs-polar":
     \li \c "ts":
     \li \c "wv":
     
    */

    class KTBasicROOTFileWriter : public KTWriterWithTypists< KTBasicROOTFileWriter >//public KTWriter
    {
        public:
            KTBasicROOTFileWriter(const std::string& name = "basic-root-writer");
            virtual ~KTBasicROOTFileWriter();

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
