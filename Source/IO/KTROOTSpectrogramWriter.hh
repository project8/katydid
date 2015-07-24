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
     - "min-time": double -- start time for the spectrograms
     - "max-time": double -- end time for the spectrograms
     - "min-freq": double -- start frequency for the spectrograms
     - "max-freq": double -- end frequency for the spectrograms

     Slots:
     - "fs-fftw": void (KTDataPtr) -- Contribute a spectrum to a FS-FFTW spectrogram.
     - "fs-polar": void (KTDataPtr) -- Contribute a spectrum to a FS-polar spectrogram.

     
    */

    class KTROOTSpectrogramWriter : public KTWriterWithTypists< KTROOTSpectrogramWriter >//public KTWriter
    {
        public:
            KTROOTSpectrogramWriter(const std::string& name = "root-spectrogram-writer");
            virtual ~KTROOTSpectrogramWriter();

            bool Configure(const KTParamNode* node);

        public:
            TFile* OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            MEMBERVARIABLEREF(std::string, Filename);
            MEMBERVARIABLEREF(std::string, FileFlag);

            MEMBERVARIABLE(double, MinTime); // in sec
            MEMBERVARIABLE(double, MaxTime); // in sec

            MEMBERVARIABLE(double, MinFreq); // in Hz
            MEMBERVARIABLE(double, MaxFreq); // in Hz

            MEMBERVARIABLE_NOSET(TFile*, File);

            bool OpenAndVerifyFile();

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

} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMWRITER_HH_ */
