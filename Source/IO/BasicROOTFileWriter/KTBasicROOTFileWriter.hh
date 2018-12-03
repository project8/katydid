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
    class KTROOTWriterFileManager;

    typedef Nymph::KTDerivedTypeWriter< KTBasicROOTFileWriter > KTBasicROOTTypeWriter;

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
     - "fs-var-fftw":
     - "fs-var-polar":
     - "multi-fs-fftw":
     - "multi-fs-polar":
     - "multi-ps-polar":
     - "norm-fs-fftw":
     - "norm-fs-polar":
     - "ps":
     - "psd":
     - "ps-dist":
     - "psd-dist":
     - "ps-var":
     - "tf-polar":
     - "tf-polar-phase":
     - "tf-polar-power":
     - "ts": Works for ts-real and ts-fftw; for ts-fftw, outputs sqrt(real*real + imag*imag)
     - "ts-dist":
     - "ts-fftw": Separate histograms for real and imaginary components
     - "wv":
     - "wv-2d":
     
    */

    class KTBasicROOTFileWriter : public Nymph::KTWriterWithTypists< KTBasicROOTFileWriter, KTBasicROOTTypeWriter >//public KTWriter
    {
        public:
            KTBasicROOTFileWriter(const std::string& name = "basic-root-writer");
            virtual ~KTBasicROOTFileWriter();

            bool Configure(const scarab::param_node* node);

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

            KTROOTWriterFileManager* fFileManager;

    };

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
