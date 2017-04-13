/*
 * KTImageWriter.cc
 *
 *  Created on: April 12, 2017
 *      Author: nsoblath
 */

#include "KTImageWriter.hh"

#include "KTException.hh"

#include <algorithm>

using std::string;
using std::stringstream;

namespace Katydid
{
    KTLOGGER(publog, "KTImageWriter");

    KT_REGISTER_WRITER(KTImageWriter, "image-writer");
    KT_REGISTER_PROCESSOR(KTImageWriter, "image-writer");

    // TODO: InitializeMagick();

    KTImageWriter::KTImageWriter(const std::string& name) :
            KTWriterWithTypists< KTImageWriter, KTImageTypeWriter >(name),
            fFilenameBase("spectrogram"),
            fFileExtension("tif"),
            fColorPaletteName("grayscale"),
            fMinTime(0.),
            fMaxTime(0.),
            fMinFreq(0.),
            fMaxFreq(0.),
            fWriteFileSlot("write-files", this, &KTImageWriter::WriteFiles)
    {
    }

    KTImageWriter::~KTImageWriter()
    {
    }

    bool KTImageWriter::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilenameBase(node->get_value("filename-base", fFilenameBase));
            SetFileExtension(node->get_value("file-extension", fFileExtension));
            SetMinFreq(node->get_value("min-freq", fMinFreq));
            SetMaxFreq(node->get_value("max-freq", fMaxFreq));
            SetMinTime(node->get_value("min-time", fMinTime));
            SetMaxTime(node->get_value("max-time", fMaxTime));
        }

        return true;
    }

    void KTImageWriter::SetColorPaletteName(const std::string& aName)
    {
        if (aName == "grayscale" || "gray")
        {
            fColorPaletteName = "gray";
        }
        else
        {
            KTWARN(publog, "Unable to set color palette to <" << aName << ">; palette remains set to <" << fColorPaletteName << ">");
        }
        return;
    }

    void KTImageWriter::WriteFiles()
    {
        KTDEBUG(publog, "In write-files slot");

        for (TypeWriterMap::iterator thisTypeWriter = fTypeWriters.begin(); thisTypeWriter != fTypeWriters.end(); ++thisTypeWriter)
        {
            thisTypeWriter->second->OutputSpectrograms();
        }
        return;
    }


    //****************************
    // KTImageTypeWriter
    //****************************

    KTImageTypeWriter::KTImageTypeWriter() :
        KTDerivedTypeWriter< KTImageWriter >()
    {
    }

    KTImageTypeWriter::~KTImageTypeWriter()
    {
    }

    void KTImageTypeWriter::CreateNewSpectrograms(const KTFrequencyDomainArrayData& data, unsigned nComponents, double startTime, double sliceLength, std::vector< SpectrogramData >& spectrograms, string histNameBase)
    {
        if (spectrograms.size() < nComponents)
        {
            // Yes, we do need to resize the vector of histograms
            // Get number of components and resize the vector of histograms
            unsigned currentSize = spectrograms.size();
            spectrograms.resize(nComponents);

            // calculate the properties of the time axis
            unsigned nSlices = unsigned((fWriter->GetMaxTime() - startTime) / sliceLength) + 1; // the +1 is so that the end time is the first slice ending outside the max time.
            double endTime = startTime + sliceLength * (double)nSlices;
            //std::cout << fWriter->GetMaxTime() << "  " << startTime << "  " << sliceLength << "  " << nSlices << "  " << endTime << std::endl;
            for (unsigned iComponent = currentSize; iComponent < nComponents; ++iComponent)
            {
                // calculate the properties of the frequency axis
                double freqBinWidth = data.GetArray(iComponent)->GetAxis().GetBinWidth();
                const KTAxisProperties< 1 >& axis = data.GetArray(iComponent)->GetAxis();
                spectrograms[iComponent].fFirstFreqBin = std::max< unsigned >(0, axis.FindBin(fWriter->GetMinFreq()));
                spectrograms[iComponent].fLastFreqBin = std::min< unsigned >(axis.GetNBins()-1, axis.FindBin(fWriter->GetMaxFreq()));
                unsigned nFreqBins = spectrograms[iComponent].fLastFreqBin - spectrograms[iComponent].fFirstFreqBin + 1;
                double startFreq = axis.GetBinLowEdge(spectrograms[iComponent].fFirstFreqBin);
                double endFreq = axis.GetBinLowEdge(spectrograms[iComponent].fLastFreqBin) + freqBinWidth;
                KTDEBUG(publog, "Creating new spectrogram image for component " << iComponent << ": " << nSlices << ", " << startTime << ", " << endTime << ", " << nFreqBins  << ", " << startFreq << ", " << endFreq);
                spectrograms[iComponent].fSpectrogram = new KTPhysicalArray< 2, double >(nSlices, startTime, endTime, nFreqBins, startFreq, endFreq); //Magick::Image(Magick::Geometry(nSlices, nFreqBins), "white");
            }
        } // done initializing new spectrograms
    }

    KTImageTypeWriter::SpectrogramData::SpectrogramData() :
            fSpectrogram(nullptr),
            fFirstFreqBin(0),
            fLastFreqBin(0),
            fColorTranslator(new ColorTranslatorGray())
    {}

    Magick::Image KTImageTypeWriter::SpectrogramData::CreateSpectrogram()
    {
        unsigned nTimeBins = fSpectrogram->GetNBins(0);
        unsigned nFreqBins = fSpectrogram->GetNBins(1);

        Magick::Image image(Magick::Geometry(nTimeBins, nFreqBins), "white");
        image.modifyImage();
        Magick::PixelPacket* pixels = image.getPixels(0, 0, nFreqBins, nTimeBins);

        unsigned xBin, yBin;
        double minValue = fSpectrogram->GetMaximumBin(xBin, yBin);
        double maxValue = fSpectrogram->GetMinimumBin(xBin, yBin);

        double scaling = 1. / (maxValue - minValue);
        double newValue = 0.;

        for (unsigned iTime = 0; iTime < nTimeBins; ++iTime)
        {
            for (unsigned iFreq = 0; iFreq < nFreqBins; ++iTime)
            {
                *(pixels + iFreq * nTimeBins + iTime) = fColorTranslator->SetColor( ((*fSpectrogram)(iTime, iFreq) - minValue) * scaling );
            }
        }

        image.syncPixels();

        return image;
    }

    void KTImageTypeWriter::SpectrogramData::SetColorPalette(const std::string& paletteName)
    {
        if (paletteName == "gray")
        {
            delete fColorTranslator;
            fColorTranslator = new ColorTranslatorGray();
        }
        else
        {
            throw Nymph::KTException() << "Unknown color palette name: <" << paletteName << ">";
        }
    }


} /* namespace Katydid */
