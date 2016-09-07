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

#include "KTData.hh"
#include "KTFrequencySpectrum.hh"
#include "KTMemberVariable.hh"
#include "KTPowerSpectrum.hh"
#include "KTScoredSpectrum.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"
#include "KTSlot.hh"

#include "TFile.h"
#include "TH2.h"

#include <vector>

namespace Katydid
{
    using namespace Nymph;
    class KTFrequencyDomainArrayData;
    class KTROOTSpectrogramWriter;

    class KTROOTSpectrogramTypeWriter : public KTDerivedTypeWriter< KTROOTSpectrogramWriter >
    {
        public:
            KTROOTSpectrogramTypeWriter();
            virtual ~KTROOTSpectrogramTypeWriter();
            virtual void OutputSpectrograms() = 0;

        protected:
            struct SpectrogramData {
                TH2D* fSpectrogram;
                unsigned fFirstFreqBin; // frequency-axis bin 0 is this bin in the incoming data
                unsigned fLastFreqBin; // frequency-axis last-bin is this bin in the incoming data
            };

            /// Checks to see if new spectrograms are needed, and creates them if so
            void CreateNewSpectrograms(const KTFrequencyDomainArrayData& data, unsigned nComponents, double startTime, double sliceLength, std::vector< SpectrogramData >& spectrograms, std::string histNameBase);

            template< typename XDataType >
            void AddFrequencySpectrumDataHelper(KTDataPtr data, std::vector< SpectrogramData >& spectrograms, std::string histNameBase);

            template< typename XDataType >
            void AddScoredSpectrumDataCoreHelper(KTDataPtr data, std::vector< SpectrogramData >& spectrograms, std::string histNameBase);

            template< typename XDataType >
            void AddPowerSpectrumDataCoreHelper(KTDataPtr data, std::vector< SpectrogramData >& spectrograms, std::string histNameBase);
            template< typename XDataType >
            void AddPowerSpectralDensityDataCoreHelper(KTDataPtr data, std::vector< SpectrogramData >& spectrograms, std::string histNameBase);
    };

  /*!
     @class KTROOTSpectrogramWriter
     @author N. S. Oblath

     @brief Outputs a spectrogram in the form of a 2D histogram to a ROOT file

     @details
     The first use of any of the spectrogram-contribution slots (i.e. not "write-file") will start a spectrogram.

     The "write-file" slot must be called at the end to actually write out the ROOT file.

     Configuration name: "root-spectrogram-writer"

     Available configuration values:
     - "output-file": string -- output filename
     - "file-flag": string -- TFile option: CREATE, RECREATE, or UPDATE
     - "min-time": double -- start time for the spectrograms
     - "max-time": double -- end time for the spectrograms
     - "min-freq": double -- start frequency for the spectrograms
     - "max-freq": double -- end frequency for the spectrograms
     - "buffer-freq": double -- range to extend the spectrogram around a track in frequency
     - "buffer-time": double -- range to extend the spectrogram around a track in time

     Slots:
     - "fs-fftw": void (KTDataPtr) -- Contribute a slice to a FS-FFTW spectrogram. Requires KTFrequencySpectrumDataFFTW.
     - "fs-polar": void (KTDataPtr) -- Contribute a slice to a FS-polar spectrogram.  Requires KTFrequencySpectrumDataPolar.
     - "ps": void (KTDataPtr) -- Contribute a slice to a power spectrogram.  Requires KTPowerSpectrumData.
     - "psd": void (KTDataPtr) -- Contribute a slice to a PSD spectrogram.  Requires KTPowerSpectrumData.
     - "track": void (KTDataPtr) -- Set the time and frequency bounds in accordance with a track. Requires KTProcessedTrackData.
     - "all-lines": void (KTDataPtr) -- Contribute a track to a spectrogram; Requires KTProcessedTrackData.
     - "write-file": void () -- Write out the ROOT file of any spectrograms that were built.


    */

    class KTROOTSpectrogramWriter : public KTWriterWithTypists< KTROOTSpectrogramWriter, KTROOTSpectrogramTypeWriter >//public KTWriter
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

            MEMBERVARIABLE(double, BufferFreq); // in sec
            MEMBERVARIABLE(double, BufferTime); // in Hz

            MEMBERVARIABLE_NOSET(TFile*, File);

            bool OpenAndVerifyFile();
            void WriteFile();

        private:
            KTSlotDone fWriteFileSlot;

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



    //****************************
    // KTROOTSpectrogramTypeWriter
    //****************************

     template< class XDataType >
     void KTROOTSpectrogramTypeWriter::AddFrequencySpectrumDataHelper(KTDataPtr data, std::vector< SpectrogramData >& spectrograms, std::string histNameBase)
     {
         KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
         double timeInRun = sliceHeader.GetTimeInRun();
         double sliceLength = sliceHeader.GetSliceLength();
         // Check if this is a slice we should care about.
         // The first slice of interest will contain the writer's min time;
         // The last slice of interest will contain the writer's max time.
         if (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime())
         {
             // Ok, this is a slice we should pay attention to.
             XDataType& fsData = data->Of< XDataType >();
             unsigned nComponents = fsData.GetNComponents();

             KTROOTSpectrogramTypeWriter::CreateNewSpectrograms(fsData, nComponents, timeInRun, sliceLength, spectrograms, histNameBase);

             // add this slice's data to the spectrogram
             for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
             {
                 const KTFrequencySpectrum* spectrum = fsData.GetSpectrum(iComponent);
                 unsigned iSpectFreqBin = 0;
                 int iSpectTimeBin = spectrograms[iComponent].fSpectrogram->GetXaxis()->FindBin(timeInRun + 0.5*sliceLength);
                 if (iSpectTimeBin <= 0 || iSpectTimeBin > spectrograms[iComponent].fSpectrogram->GetNbinsX()) continue;
                 std::cout << "spectrum size: " << spectrum->GetNFrequencyBins() << std::endl;
                 std::cout << "first freq bin: " << spectrograms[iComponent].fFirstFreqBin << "; last freq bin: " << spectrograms[iComponent].fLastFreqBin << std::endl;
                 for (unsigned iFreqBin = spectrograms[iComponent].fFirstFreqBin; iFreqBin <= spectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                 {
                     //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << iSpectTimeBin << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                     spectrograms[iComponent].fSpectrogram->SetBinContent(iSpectTimeBin, iSpectFreqBin, spectrum->GetAbs(iFreqBin));
                     ++iSpectFreqBin;
                 }
             }
         }

         return;
     }

     template< class XDataType >
     void KTROOTSpectrogramTypeWriter::AddPowerSpectrumDataCoreHelper(KTDataPtr data, std::vector< SpectrogramData >& spectrograms, std::string histNameBase)
     {
         KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
         double timeInRun = sliceHeader.GetTimeInRun();
         double sliceLength = sliceHeader.GetSliceLength();
         // Check if this is a slice we should care about.
         // The first slice of interest will contain the writer's min time;
         // The last slice of interest will contain the writer's max time.
         if (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime())
         {
             // Ok, this is a slice we should pay attention to.
             XDataType& fsData = data->Of< XDataType >();
             unsigned nComponents = fsData.GetNComponents();

             KTROOTSpectrogramTypeWriter::CreateNewSpectrograms(fsData, nComponents, timeInRun, sliceLength, spectrograms, histNameBase);

             // add this slice's data to the spectrogram
             for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
             {
                 KTPowerSpectrum* spectrum = fsData.GetSpectrum(iComponent);
                 spectrum->ConvertToPowerSpectrum();
                 unsigned iSpectFreqBin = 0;
                 int iSpectTimeBin = spectrograms[iComponent].fSpectrogram->GetXaxis()->FindBin(timeInRun + 0.5*sliceLength);
                 if (iSpectTimeBin <= 0 || iSpectTimeBin > spectrograms[iComponent].fSpectrogram->GetNbinsX()) continue;
                 for (unsigned iFreqBin = spectrograms[iComponent].fFirstFreqBin; iFreqBin <= spectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                 {
                     //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                     spectrograms[iComponent].fSpectrogram->SetBinContent(iSpectTimeBin, iSpectFreqBin, (*spectrum)(iFreqBin));
                     ++iSpectFreqBin;
                 }
             }
         }

         return;
     }

     template< class XDataType >
     void KTROOTSpectrogramTypeWriter::AddPowerSpectralDensityDataCoreHelper(KTDataPtr data, std::vector< SpectrogramData >& spectrograms, std::string histNameBase)
     {
         KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
         double timeInRun = sliceHeader.GetTimeInRun();
         double sliceLength = sliceHeader.GetSliceLength();
         // Check if this is a slice we should care about.
         // The first slice of interest will contain the writer's min time;
         // The last slice of interest will contain the writer's max time.
         if (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime())
         {
             // Ok, this is a slice we should pay attention to.
             XDataType& fsData = data->Of< XDataType >();
             unsigned nComponents = fsData.GetNComponents();

             KTROOTSpectrogramTypeWriter::CreateNewSpectrograms(fsData, nComponents, timeInRun, sliceLength, spectrograms, histNameBase);

             // add this slice's data to the spectrogram
             for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
             {
                 KTPowerSpectrum* spectrum = fsData.GetSpectrum(iComponent);
                 spectrum->ConvertToPowerSpectralDensity();
                 unsigned iSpectFreqBin = 0;
                 int iSpectTimeBin = spectrograms[iComponent].fSpectrogram->GetXaxis()->FindBin(timeInRun + 0.5*sliceLength);
                 if (iSpectTimeBin <= 0 || iSpectTimeBin > spectrograms[iComponent].fSpectrogram->GetNbinsX()) continue;
                 for (unsigned iFreqBin = spectrograms[iComponent].fFirstFreqBin; iFreqBin <= spectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                 {
                     //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                     spectrograms[iComponent].fSpectrogram->SetBinContent(iSpectTimeBin, iSpectFreqBin, (*spectrum)(iFreqBin));
                     ++iSpectFreqBin;
                 }
             }
         }

         return;
     }

     template< class XDataType >
     void KTROOTSpectrogramTypeWriter::AddScoredSpectrumDataCoreHelper(KTDataPtr data, std::vector< SpectrogramData >& spectrograms, std::string histNameBase)
     {
         KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
         double timeInRun = sliceHeader.GetTimeInRun();
         double sliceLength = sliceHeader.GetSliceLength();
         // Check if this is a slice we should care about.
         // The first slice of interest will contain the writer's min time;
         // The last slice of interest will contain the writer's max time.
         if (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime())
         {
             // Ok, this is a slice we should pay attention to.
             XDataType& fsData = data->Of< XDataType >();
             unsigned nComponents = fsData.GetNComponents();

             KTROOTSpectrogramTypeWriter::CreateNewSpectrograms(fsData, nComponents, timeInRun, sliceLength, spectrograms, histNameBase);

             // add this slice's data to the spectrogram
             for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
             {
                 KTScoredSpectrum* spectrum = fsData.GetSpectrum(iComponent);
                 spectrum->ConvertToScoredSpectrum();
                 unsigned iSpectFreqBin = 0;
                 int iSpectTimeBin = spectrograms[iComponent].fSpectrogram->GetXaxis()->FindBin(timeInRun + 0.5*sliceLength);
                 if (iSpectTimeBin <= 0 || iSpectTimeBin > spectrograms[iComponent].fSpectrogram->GetNbinsX()) continue;
                 for (unsigned iFreqBin = spectrograms[iComponent].fFirstFreqBin; iFreqBin <= spectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                 {
                     //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                     spectrograms[iComponent].fSpectrogram->SetBinContent(iSpectTimeBin, iSpectFreqBin, (*spectrum)(iFreqBin));
                     ++iSpectFreqBin;
                 }
             }
         }

         return;
     }


} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMWRITER_HH_ */
