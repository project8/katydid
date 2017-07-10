/**
 @file KTImageWriter.hh
 @brief Contains KTImageWriter
 @details Writes images of spectrograms
 @author: N. S. Oblath
 @date: April 12, 2017
 */

#ifndef KTIMAGEWRITER_HH_
#define KTIMAGEWRITER_HH_

#include "KTWriter.hh"

#include "KTFrequencySpectrum.hh"
#include "KTPowerSpectrum.hh"
#include "KTProcessedTrackData.hh"
#include "KTSliceHeader.hh"

#include "KTData.hh"
#include "KTLogger.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include <Magick++.h>

#include <vector>


namespace Katydid
{
    KTLOGGER( publog_rsw, "KTImageWriter" );
    
    class KTFrequencyDomainArrayData;
    class KTImageWriter;

    struct ColorTranslator
    {
            virtual ~ColorTranslator() {}
            virtual Magick::Color& SetColor(double scaled) = 0; // scaled is a value on from 0 to 1
    };

    struct ColorTranslatorGray : ColorTranslator
    {
            virtual ~ColorTranslatorGray() {}
            virtual Magick::Color& SetColor(double scaled)
            {
                fColor.shade(scaled);
                return fColor;
            }
            Magick::ColorGray fColor;
    };

    class KTImageTypeWriter : public Nymph::KTDerivedTypeWriter< KTImageWriter >
    {
        public:
            KTImageTypeWriter();
            virtual ~KTImageTypeWriter();
            virtual void OutputSpectrograms() = 0;

        protected:
            struct SpectrogramData {
                SpectrogramData();
                ~SpectrogramData() {delete fSpectrogram;}
                void SetColorPalette(const std::string& paletteName);
                Magick::Image CreateSpectrogram();
                KTPhysicalArray< 2, double >* fSpectrogram;
                unsigned fFirstFreqBin; // frequency-axis bin 0 is this bin in the incoming data
                unsigned fLastFreqBin; // frequency-axis last-bin is this bin in the incoming data
                ColorTranslator* fColorTranslator;
            };

            /// Checks to see if new spectrograms are needed, and creates them if so
            void CreateNewSpectrograms(const KTFrequencyDomainArrayData& data, unsigned nComponents, double startTime, double sliceLength, std::vector< SpectrogramData >& spectrograms);

            template< typename XDataType >
            void AddFrequencySpectrumDataHelper(Nymph::KTDataPtr data, std::vector< SpectrogramData >& spectrograms);

            template< typename XDataType >
            void AddPowerSpectrumDataCoreHelper(Nymph::KTDataPtr data, std::vector< SpectrogramData >& spectrograms);
            template< typename XDataType >
            void AddPowerSpectralDensityDataCoreHelper(Nymph::KTDataPtr data, std::vector< SpectrogramData >& spectrograms);
    };


    class KTROOTWriterFileManager;

  /*!
     @class KTImageWriter
     @author N. S. Oblath

     @brief Outputs a spectrogram in the form of an image

     @details
     The first use of any of the spectrogram-contribution slots (i.e. not "write-file") will start a spectrogram.

     The "write-files" slot must be called at the end to actually write out the image file.

     Filenames will be constructed as follows: [filename-base]_[data-type specifier]_[component number].[filename-extension]
     For example, for data with two components, where the power spectrum is plotted, the filename base is "/home/me/analysis/my_data", and the file extension is "png", two files will be produced:
        - /home/me/analysis/my_data_ps_0.png
        - /home/me/analysis/my_data_ps_1.png

     Configuration name: "image-writer"

     Available configuration values:
     - "filename-base": string -- The initial part of the filename, including any path information (absolute or relative). See filename construction notes above.
     - "file-extension": string -- The string after the '.' in the filename that specifies the image type.  See below for available options and filename construction notes above.
     - "color-palette": string -- The name of the color palette to be used.  See below for available options.
     - "min-time": double -- start time for the spectrograms.
     - "max-time": double -- end time for the spectrograms.
     - "min-freq": double -- start frequency for the spectrograms.
     - "max-freq": double -- end frequency for the spectrograms.

     Recommended file extension options
     - tif or tiff
     - png
     The full list of acceptable file formats is given in the Supported Image Formats table here: http://www.graphicsmagick.org/formats.html. Only formats that are writable can be used here.

     Color palette options:
     - "grayscale"

     Slots:
     - "fs-fftw": void (Nymph::KTDataPtr) -- Contribute a slice to a FS-FFTW spectrogram. Requires KTFrequencySpectrumDataFFTW.
     - "fs-polar": void (Nymph::KTDataPtr) -- Contribute a slice to a FS-polar spectrogram.  Requires KTFrequencySpectrumDataPolar.
     - "ps": void (Nymph::KTDataPtr) -- Contribute a slice to a power spectrogram.  Requires KTPowerSpectrumData.
     - "psd": void (Nymph::KTDataPtr) -- Contribute a slice to a PSD spectrogram.  Requires KTPowerSpectrumData.
     - "write-files": void () -- Write out the image files of any spectrograms that were built.

    */

    class KTImageWriter : public Nymph::KTWriterWithTypists< KTImageWriter, KTImageTypeWriter >//public KTWriter
    {
        public:
            KTImageWriter(const std::string& name = "root-spectrogram-writer");
            virtual ~KTImageWriter();

            bool Configure(const scarab::param_node* node);

        public:
            MEMBERVARIABLEREF(std::string, FilenameBase);
            MEMBERVARIABLEREF(std::string, FileExtension);

            MEMBERVARIABLEREF_NOSET(std::string, ColorPaletteName);

            MEMBERVARIABLE(double, MinTime); // in sec
            MEMBERVARIABLE(double, MaxTime); // in sec

            MEMBERVARIABLE(double, MinFreq); // in Hz
            MEMBERVARIABLE(double, MaxFreq); // in Hz

        public:
            void SetColorPaletteName(const std::string& aName);

            void WriteFiles();

        private:
            Nymph::KTSlotDone fWriteFileSlot;

    };


    //****************************
    // KTImageTypeWriter
    //****************************

     template< class XDataType >
     void KTImageTypeWriter::AddFrequencySpectrumDataHelper(Nymph::KTDataPtr data, std::vector< SpectrogramData >& spectrograms)
     {
         KTDEBUG( publog_rsw, "Adding frequency-spectrum-type data" );
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

             KTImageTypeWriter::CreateNewSpectrograms(fsData, nComponents, timeInRun, sliceLength, spectrograms);

             // add this slice's data to the spectrogram
             for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
             {
                 const KTFrequencySpectrum* spectrum = fsData.GetSpectrum(iComponent);
                 unsigned iSpectFreqBin = 0;
                 int iSpectTimeBin = spectrograms[iComponent].fSpectrogram->FindBin(1, timeInRun + 0.5*sliceLength);
                 if (iSpectTimeBin <= 0 || iSpectTimeBin > spectrograms[iComponent].fSpectrogram->GetNBins(1)) continue;
                 //std::cout << "spectrum size: " << spectrum->GetNFrequencyBins() << std::endl;
                 //std::cout << "first freq bin: " << spectrograms[iComponent].fFirstFreqBin << "; last freq bin: " << spectrograms[iComponent].fLastFreqBin << std::endl;
                 for (unsigned iFreqBin = spectrograms[iComponent].fFirstFreqBin; iFreqBin <= spectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                 {
                     //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << iSpectTimeBin << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                     spectrograms[iComponent].fSpectrogram->operator()(iSpectTimeBin, iSpectFreqBin) = spectrum->GetAbs(iFreqBin);
                     ++iSpectFreqBin;
                 }
             }
         }

         return;
     }

     template< class XDataType >
     void KTImageTypeWriter::AddPowerSpectrumDataCoreHelper(Nymph::KTDataPtr data, std::vector< SpectrogramData >& spectrograms)
     {
         KTDEBUG( publog_rsw, "Adding power-spectrum-type data" );
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

             KTImageTypeWriter::CreateNewSpectrograms(fsData, nComponents, timeInRun, sliceLength, spectrograms);

             // add this slice's data to the spectrogram
             for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
             {
                 KTPowerSpectrum* spectrum = fsData.GetSpectrum(iComponent);
                 spectrum->ConvertToPowerSpectrum();
                 unsigned iSpectFreqBin = 0;
                 int iSpectTimeBin = spectrograms[iComponent].fSpectrogram->FindBin(1, timeInRun + 0.5*sliceLength);
                 if (iSpectTimeBin <= 0 || iSpectTimeBin > spectrograms[iComponent].fSpectrogram->GetNBins(1)) continue;
                 for (unsigned iFreqBin = spectrograms[iComponent].fFirstFreqBin; iFreqBin <= spectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                 {
                     //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                     spectrograms[iComponent].fSpectrogram->operator()(iSpectTimeBin, iSpectFreqBin) = (*spectrum)(iFreqBin);
                     ++iSpectFreqBin;
                 }
             }
         }

         return;
     }

     template< class XDataType >
     void KTImageTypeWriter::AddPowerSpectralDensityDataCoreHelper(Nymph::KTDataPtr data, std::vector< SpectrogramData >& spectrograms)
     {
         KTDEBUG( publog_rsw, "Adding power-spectral-density-type data" );
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

             KTImageTypeWriter::CreateNewSpectrograms(fsData, nComponents, timeInRun, sliceLength, spectrograms);

             // add this slice's data to the spectrogram
             for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
             {
                 KTPowerSpectrum* spectrum = fsData.GetSpectrum(iComponent);
                 spectrum->ConvertToPowerSpectralDensity();
                 unsigned iSpectFreqBin = 0;
                 int iSpectTimeBin = spectrograms[iComponent].fSpectrogram->FindBin(1, timeInRun + 0.5*sliceLength);
                 if (iSpectTimeBin <= 0 || iSpectTimeBin > spectrograms[iComponent].fSpectrogram->GetNBins(1)) continue;
                 for (unsigned iFreqBin = spectrograms[iComponent].fFirstFreqBin; iFreqBin <= spectrograms[iComponent].fLastFreqBin; ++iFreqBin)
                 {
                     //std::cout << "spectrum bin: " << iFreqBin << "   spectrogram bins (" << fFSFFTWSpectrograms[iComponent].fNextTimeBinToFill << ", " << iSpectFreqBin << "    value: " << spectrum->GetAbs(iFreqBin) << std::endl;
                     spectrograms[iComponent].fSpectrogram->operator()(iSpectTimeBin, iSpectFreqBin) = (*spectrum)(iFreqBin);
                     ++iSpectFreqBin;
                 }
             }
         }

         return;
     }

} /* namespace Katydid */
#endif /* KTIMAGEWRITER_HH_ */
