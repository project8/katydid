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

#include "KTFrequencySpectrum.hh"
#include "KTPowerSpectrum.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"

#include "KTData.hh"
#include "logger.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include "TH2.h"
#include "TFile.h"

#include <vector>

namespace Katydid
{
    LOGGER(publog_rsw, "KTROOTSpectrogramWriter");

    class KTFrequencyDomainArrayData;
    class KTROOTSpectrogramWriter;

    class KTROOTSpectrogramTypeWriter : public Nymph::KTDerivedTypeWriter< KTROOTSpectrogramWriter >
    {
        public:
            KTROOTSpectrogramTypeWriter();
            virtual ~KTROOTSpectrogramTypeWriter();
            virtual void OutputSpectrograms() = 0;

        protected:
            struct SpectrogramPack
            {
                TH2D* fSpectrogram;
                unsigned fFirstFreqBin; // frequency-axis bin 0 is this bin in the incoming data
                unsigned fLastFreqBin; // frequency-axis last-bin is this bin in the incoming data
            };

            struct DataTypeBundle
            {
                std::vector< SpectrogramPack > fSpectrograms; // vector over components
                std::string fHistNameBase;
                unsigned fHistCount;
                unsigned fNTimeBins;
                double fTimeAxisMin;
                double fTimeAxisMax;
                int fCurrentTimeBin;
                DataTypeBundle(const std::string& histNameBase);
            };

            /// Checks to see if new spectrograms are needed, and creates them if so
            int UpdateSpectrograms(const KTFrequencyDomainArrayData& data, unsigned nComponents, double timeInRun, double sliceLength, bool isNewAcq, DataTypeBundle& dataBundle);

            template< typename XDataType >
            void AddFrequencySpectrumDataHelper(Nymph::KTDataPtr data, DataTypeBundle& dataBundle);

            template< typename XDataType >
            void AddPowerSpectrumDataCoreHelper(Nymph::KTDataPtr data, DataTypeBundle& dataBundle);
            template< typename XDataType >
            void AddPowerSpectralDensityDataCoreHelper(Nymph::KTDataPtr data, DataTypeBundle& dataBundle);

            void OutputASpectrogramSet(DataTypeBundle& dataBundle, bool cloneSpectrograms);
            void ClearASpectrogramSet(DataTypeBundle& dataBundle);
    };


    class KTROOTWriterFileManager;

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
     - "mode": string -- switches the mode of operation of the writer; see below
     - "min-time": double -- start time for the spectrograms. Only used in "single" mode.  Overwritten by the 'track' slot
     - "max-time": double -- end time for the spectrograms. Only used in "single" mode.  Overwritten by the 'track' slot
     - "n-time-bins": unsigned int -- number of bins on the spectrogram's time (x) axis.  Only used in "sequential" mode.
     - "min-freq": double -- start frequency for the spectrograms. Overwritten by the 'track' slot
     - "max-freq": double -- end frequency for the spectrograms. Overwritten by the 'track' slot

     Modes of operation:
     - "single": Outputs a single spectrogram per component using the min/max times and frequencies provided by the user.
     - "sequential": Outputs a sequential series of spectrograms per component using the number of time bins and the min/max frequencies provided by the user.

     Slots:
     - "fs-fftw": void (Nymph::KTDataPtr) -- Contribute a slice to a FS-FFTW spectrogram. Requires KTFrequencySpectrumDataFFTW.
     - "fs-polar": void (Nymph::KTDataPtr) -- Contribute a slice to a FS-polar spectrogram.  Requires KTFrequencySpectrumDataPolar.
     - "ps": void (Nymph::KTDataPtr) -- Contribute a slice to a power spectrogram.  Requires KTPowerSpectrumData.
     - "psd": void (Nymph::KTDataPtr) -- Contribute a slice to a PSD spectrogram.  Requires KTPowerSpectrumData.
     - "agg-ps": void (Nymph::KTDataPtr) -- Contribute a slice to a power spectrogram.  Requires KTPowerSpectrumData.
     - "agg-psd": void (Nymph::KTDataPtr) -- Contribute a slice to a PSD spectrogram.  Requires KTPowerSpectrumData.
     - "proc-track": void (Nymph::KTDataPtr) -- Contribute a line representing a track; all lines will be written to the root file together, and can be drawn on top of a spectrogram.  Requires KTProcessedTrackData.
     - "write-file": void () -- Write out the ROOT file of any spectrograms that were built.


     */

    class KTROOTSpectrogramWriter : public Nymph::KTWriterWithTypists< KTROOTSpectrogramWriter, KTROOTSpectrogramTypeWriter > //public KTWriter
    {
        public:
            enum Mode
            {
                kSingle,
                kSequential
            };

        public:
            KTROOTSpectrogramWriter(const std::string& name = "root-spectrogram-writer");
            virtual ~KTROOTSpectrogramWriter();

            bool Configure(const scarab::param_node* node);

        public:
            TFile* OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            MEMBERVARIABLEREF(std::string, Filename);
            MEMBERVARIABLEREF(std::string, FileFlag);

            MEMBERVARIABLE(Mode, Mode);

            MEMBERVARIABLE(double, MinTime); // in sec
            MEMBERVARIABLE(double, MaxTime); // in sec

            MEMBERVARIABLE(unsigned, NTimeBins);

            MEMBERVARIABLE(double, MinFreq); // in Hz
            MEMBERVARIABLE(double, MaxFreq); // in Hz

            MEMBERVARIABLE_NOSET(TFile*, File);

            bool OpenAndVerifyFile();
            void WriteFile();

        private:
            KTROOTWriterFileManager* fFileManager;

            Nymph::KTSlotDone fWriteFileSlot;

    };

    //****************************
    // KTROOTSpectrogramTypeWriter
    //****************************

    template< class XDataType >
    void KTROOTSpectrogramTypeWriter::AddFrequencySpectrumDataHelper(Nymph::KTDataPtr data, DataTypeBundle& dataBundle)
    {
        LDEBUG(publog_rsw, "Adding frequency-spectrum-type data");
        KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
        double timeInRun = sliceHeader.GetTimeInRun();
        double sliceLength = sliceHeader.GetSliceLength();
        bool isNewAcq = sliceHeader.GetIsNewAcquisition();
        // Check if this is a slice we should care about.
        // The first slice of interest will contain the writer's min time;
        // The last slice of interest will contain the writer's max time.
        if (fWriter->GetMode() == KTROOTSpectrogramWriter::kSequential || (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime()))
        {
            // Ok, this is a slice we should pay attention to.

            XDataType& fsData = data->Of< XDataType >();
            unsigned nComponents = fsData.GetNComponents();

            int iSpectTimeBin = KTROOTSpectrogramTypeWriter::UpdateSpectrograms(fsData, nComponents, timeInRun, sliceLength, isNewAcq, dataBundle);
            LDEBUG(publog_rsw, "Spectrogram time bin to write to is <" << iSpectTimeBin << ">");
            if (iSpectTimeBin <= 0) return; // do this check here instead of down in the component loop to save time

            // add this slice's data to the spectrogram
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                LWARN(publog_rsw, "Working on component " << iComponent);
                const KTFrequencySpectrum* spectrum = fsData.GetSpectrum(iComponent);
                TH2D* spectrogram = dataBundle.fSpectrograms[iComponent].fSpectrogram;
                LWARN(publog_rsw, "pointers: " << spectrum << "  " << spectrogram);
                unsigned iSpectFreqBin = 0;
                if (iSpectTimeBin > spectrogram->GetNbinsX()) continue;
                //std::cout << "spectrum size: " << spectrum->GetNFrequencyBins() << std::endl;
                //std::cout << "first freq bin: " << spectrograms[iComponent].fFirstFreqBin << "; last freq bin: " << spectrograms[iComponent].fLastFreqBin << std::endl;
                for (unsigned iFreqBin = dataBundle.fSpectrograms[iComponent].fFirstFreqBin; iFreqBin <= dataBundle.fSpectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                {
                    //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << iSpectTimeBin << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                    spectrogram->SetBinContent(iSpectTimeBin, iSpectFreqBin + 1, spectrum->GetAbs(iFreqBin));
                    ++iSpectFreqBin;
                }
            }
        }

        return;
    }

    template< class XDataType >
    void KTROOTSpectrogramTypeWriter::AddPowerSpectrumDataCoreHelper(Nymph::KTDataPtr data, DataTypeBundle& dataBundle)
    {
        LDEBUG(publog_rsw, "Adding power-spectrum-type data");
        KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
        double timeInRun = sliceHeader.GetTimeInRun();
        double sliceLength = sliceHeader.GetSliceLength();
        bool isNewAcq = sliceHeader.GetIsNewAcquisition();
        // Check if this is a slice we should care about.
        // The first slice of interest will contain the writer's min time;
        // The last slice of interest will contain the writer's max time.
        if (fWriter->GetMode() == KTROOTSpectrogramWriter::kSequential || (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime()))
        {
            // Ok, this is a slice we should pay attention to.
            XDataType& fsData = data->Of< XDataType >();
            unsigned nComponents = fsData.GetNComponents();

            int iSpectTimeBin = KTROOTSpectrogramTypeWriter::UpdateSpectrograms(fsData, nComponents, timeInRun, sliceLength, isNewAcq, dataBundle);
            if (iSpectTimeBin <= 0) return; // do this check here instead of down in the component loop to save time

            // add this slice's data to the spectrogram
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTPowerSpectrum* spectrum = fsData.GetSpectrum(iComponent);
                spectrum->ConvertToPowerSpectrum();
                TH2D* spectrogram = dataBundle.fSpectrograms[iComponent].fSpectrogram;
                unsigned iSpectFreqBin = 0;
                if (iSpectTimeBin > spectrogram->GetNbinsX()) continue;
                for (unsigned iFreqBin = dataBundle.fSpectrograms[iComponent].fFirstFreqBin; iFreqBin <= dataBundle.fSpectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                {
                    //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                    spectrogram->SetBinContent(iSpectTimeBin, iSpectFreqBin + 1, (*spectrum)(iFreqBin));
                    ++iSpectFreqBin;
                }
            }
        }

        return;
    }

    template< class XDataType >
    void KTROOTSpectrogramTypeWriter::AddPowerSpectralDensityDataCoreHelper(Nymph::KTDataPtr data, DataTypeBundle& dataBundle)
    {
        LDEBUG(publog_rsw, "Adding power-spectral-density-type data");
        KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
        double timeInRun = sliceHeader.GetTimeInRun();
        double sliceLength = sliceHeader.GetSliceLength();
        bool isNewAcq = sliceHeader.GetIsNewAcquisition();
        // Check if this is a slice we should care about.
        // The first slice of interest will contain the writer's min time;
        // The last slice of interest will contain the writer's max time.
        if (fWriter->GetMode() == KTROOTSpectrogramWriter::kSequential || (timeInRun + sliceLength >= fWriter->GetMinTime() && timeInRun <= fWriter->GetMaxTime()))
        {
            // Ok, this is a slice we should pay attention to.
            XDataType& fsData = data->Of< XDataType >();
            unsigned nComponents = fsData.GetNComponents();

            int iSpectTimeBin = KTROOTSpectrogramTypeWriter::UpdateSpectrograms(fsData, nComponents, timeInRun, sliceLength, isNewAcq, dataBundle);
            if (iSpectTimeBin <= 0) return; // do this check here instead of down in the component loop to save time

            // add this slice's data to the spectrogram
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTPowerSpectrum* spectrum = fsData.GetSpectrum(iComponent);
                spectrum->ConvertToPowerSpectralDensity();
                TH2D* spectrogram = dataBundle.fSpectrograms[iComponent].fSpectrogram;
                unsigned iSpectFreqBin = 0;
                if (iSpectTimeBin > spectrogram->GetNbinsX()) continue;
                for (unsigned iFreqBin = dataBundle.fSpectrograms[iComponent].fFirstFreqBin; iFreqBin <= dataBundle.fSpectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                {
                    //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                    spectrogram->SetBinContent(iSpectTimeBin, iSpectFreqBin + 1, (*spectrum)(iFreqBin));
                    ++iSpectFreqBin;
                }
            }
        }

        return;
    }

} /* namespace Katydid */
#endif /* KTROOTSPECTROGRAMWRITER_HH_ */
