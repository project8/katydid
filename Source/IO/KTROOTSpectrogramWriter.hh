/**
 @file KTROOTSpectrogramWriter.hh
 @brief Contains KTROOTSpectrogramWriter
 @details Writes 2-D histograms of spectrograms
 @author: N. S. Oblath
 @date: June 18, 2015
 */

#ifndef KTROOTSPECTROGRAMWRITER_HH_
#define KTROOTSPECTROGRAMWRITER_HH_

#include "KTWriter.hh"

#include "KTMemberVariable.hh"

#include "TFile.h"

namespace Katydid
{
    class KTROOTSpectrogramWriter;

    typedef KTDerivedTypeWriter< KTROOTSpectrogramWriter > KTROOTSpectrogramTypeWriter;

  /*!
     @class KTROOTSpectrogramWriter
     @author N. S. Oblath

     @brief Outputs a spectrogram in the form of a 2D histogram to a ROOT file

     @details 

     Configuration name: "root-spectrogram-writer"

     Available configuration values:
     - "output-file": string -- output filename
     - "file-flag": string -- TFile option: CREATE, RECREATE, or UPDATE
     - "spectrogram-size": unsigned -- number of slices per spectrogram

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

    class KTROOTSpectrogramWriter : public KTWriterWithTypists< KTROOTSpectrogramWriter >//public KTWriter
    {
        public:
            KTROOTSpectrogramWriter(const std::string& name = "basic-root-writer");
            virtual ~KTROOTSpectrogramWriter();

            bool Configure(const KTParamNode* node);

        public:
            TFile* OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            MEMBERVARIABLEREF(std::string, Filename);
            MEMBERVARIABLEREF(std::string, FileFlag);

            MEMBERVARIABLE(unsigned, SpectrogramSize);

            TFile* GetFile();

            bool OpenAndVerifyFile();

        private:
            TFile* fFile;

    };

    inline TFile* KTROOTSpectrogramWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = new TFile(filename.c_str(), flag.c_str());
        return fFile;
    }
    inline void KTROOTSpectrogramWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    inline const std::string& KTROOTSpectrogramWriter::GetFilename() const
    {
        return fFilename;
    }
    inline void KTROOTSpectrogramWriter::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTROOTSpectrogramWriter::GetFileFlag() const
    {
        return fFileFlag;
    }
    inline void KTROOTSpectrogramWriter::SetFileFlag(const std::string& flag)
    {
        fFileFlag = flag;
        return;
    }

    inline TFile* KTROOTSpectrogramWriter::GetFile()
    {
        return fFile;
    }


} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMWRITER_HH_ */
