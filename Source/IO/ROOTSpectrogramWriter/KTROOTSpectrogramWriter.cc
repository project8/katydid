/*
 * KTROOTSpectrogramWriter.cc
 *
 *  Created on: Jun 18, 2015
 *      Author: nsoblath
 */

#include "KTROOTSpectrogramWriter.hh"

#include "KTROOTWriterFileManager.hh"

#include "KTCommandLineOption.hh"

#include <algorithm>

using std::string;
using std::stringstream;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTSpectrogramWriter");


    KT_REGISTER_WRITER(KTROOTSpectrogramWriter, "root-spectrogram-writer");
    KT_REGISTER_PROCESSOR(KTROOTSpectrogramWriter, "root-spectrogram-writer");

    static Nymph::KTCommandLineOption< string > sRSWFilenameCLO("ROOT Spectrogram Writer", "ROOT spectrogram writer filename", "rsw-file");

    KTROOTSpectrogramWriter::KTROOTSpectrogramWriter(const std::string& name) :
            KTWriterWithTypists< KTROOTSpectrogramWriter, KTROOTSpectrogramTypeWriter >(name),
            fFilename("spect_output.root"),
            fFileFlag("recreate"),
            fMode(kSingle),
            fMinTime(0.),
            fMaxTime(0.),
            fNTimeBins(0),
            fMinFreq(0.),
            fMaxFreq(0.),
            fFile(NULL),
            fFileManager(KTROOTWriterFileManager::get_instance()),
            fWriteFileSlot("write-file", this, &KTROOTSpectrogramWriter::WriteFile)
    {
    }

    KTROOTSpectrogramWriter::~KTROOTSpectrogramWriter()
    {
        CloseFile();
    }

    bool KTROOTSpectrogramWriter::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->get_value("output-file", fFilename));
            SetFileFlag(node->get_value("file-flag", fFileFlag));

            if (node->has("mode"))
            {
                std::string modeStr = node->get_value("mode");
                if (modeStr == "single") SetMode(kSingle);
                else if (modeStr == "sequential") SetMode(kSequential);
                else
                {
                    KTERROR(publog, "Invalid mode: <" << modeStr << ">");
                    return false;
                }
            }

            SetMinFreq(node->get_value("min-freq", fMinFreq));
            SetMaxFreq(node->get_value("max-freq", fMaxFreq));

            SetNTimeBins(node->get_value("n-time-bins", fNTimeBins));

            SetMinTime(node->get_value("min-time", fMinTime));
            SetMaxTime(node->get_value("max-time", fMaxTime));

            if (fMinTime > fMaxTime)
            {
                fMode = kSequential;
            }

            if (fMode == kSequential && fNTimeBins == 0)
            {
                KTERROR(publog, "Invalid configuration: you must specify a non-zero number of bins if using sequential mode");
                return false;
            }
        }

        // Command-line settings
        SetFilename(fCLHandler->GetCommandLineValue< string >("rsw-file", fFilename));

        return true;
    }

    TFile* KTROOTSpectrogramWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = fFileManager->OpenFile(this, filename.c_str(), flag.c_str());
        return fFile;
    }

    void KTROOTSpectrogramWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            fFileManager->FinishFile(this, fFilename);
            fFile = NULL;
        }
        return;
    }

    bool KTROOTSpectrogramWriter::OpenAndVerifyFile()
    {
        if (fFile == NULL)
        {
            fFile = fFileManager->OpenFile(this, fFilename.c_str(), fFileFlag.c_str());
        }
        if (! fFile->IsOpen())
        {
            fFileManager->DiscardFile(this, fFilename);
            fFile = NULL;
            KTERROR(publog, "Output file <" << fFilename << "> did not open!");
            return false;
        }
        fFile->cd();
        return true;
    }

    void KTROOTSpectrogramWriter::WriteFile()
    {
        KTDEBUG("In write-file slot")
        for (TypeWriterMap::iterator thisTypeWriter = fTypeWriters.begin(); thisTypeWriter != fTypeWriters.end(); ++thisTypeWriter)
        {
            thisTypeWriter->second->OutputSpectrograms();
        }
    }


    //****************************
    // KTROOTSpectrogramTypeWriter
    //****************************

    KTROOTSpectrogramTypeWriter::DataTypeBundle::DataTypeBundle(const std::string& histNameBase) :
            fSpectrograms(),
            fHistNameBase(histNameBase),
            fHistCount(0),
            fNTimeBins(0),
            fTimeAxisMin(0.),
            fTimeAxisMax(0.),
            fCurrentTimeBin(0)
    {
    }

    KTROOTSpectrogramTypeWriter::KTROOTSpectrogramTypeWriter() :
        KTDerivedTypeWriter< KTROOTSpectrogramWriter >()
    {
    }

    KTROOTSpectrogramTypeWriter::~KTROOTSpectrogramTypeWriter()
    {
    }

    int KTROOTSpectrogramTypeWriter::UpdateSpectrograms(const KTFrequencyDomainArrayData& data, unsigned nComponents, double timeInRun, double sliceLength, bool isNewAcq, DataTypeBundle& dataBundle)
    {
        // if fCurrentTimeBin is already negative, it stays that way.  No more histogram writing will take place.
        KTWARN(publog, "in UpdateSpectrograms");
        if (dataBundle.fCurrentTimeBin < 0) return dataBundle.fCurrentTimeBin;

        KTROOTSpectrogramWriter::Mode mode = fWriter->GetMode();

        KTWARN(publog, "checking new acq");
        if (isNewAcq && dataBundle.fSpectrograms.size() != 0)
        {
            dataBundle.fHistCount += 1;
            if (! fWriter->OpenAndVerifyFile())
            {
                KTWARN(publog, "File could not be opened; unable to write spectrograms.  Setting time bin to -1 to prevent further spectrogram creation.");
                dataBundle.fCurrentTimeBin = -1;
                return dataBundle.fCurrentTimeBin;
            }
            else
            {
                if (mode == KTROOTSpectrogramWriter::kSingle)
                {
                    OutputASpectrogramSet(dataBundle, false);
                    dataBundle.fCurrentTimeBin = -1; // writing to the spectrogram is finished, so returning -1 will prevent further histogram filling
                    return dataBundle.fCurrentTimeBin;
                }
                else // mode == KTROOTSpectrogramWriter::kSequential
                {
                    OutputASpectrogramSet(dataBundle, true);
                    dataBundle.fCurrentTimeBin = 0; // it'll get ++1'd later
                }
            }
        }

        KTWARN(publog, "checking if we need to increase spectrogram vec size");
        // resize the dataBundle vector if necessary and initialize the new spectrogram pointers
        if (dataBundle.fSpectrograms.size() < nComponents)
        {
            KTWARN(publog, "yep, we need to increase the size: " << dataBundle.fSpectrograms.size() << " < " << nComponents );
            // Yes, we do need to resize the vector of histograms
            // Get number of components and resize the vector of histograms
            unsigned currentSize = dataBundle.fSpectrograms.size();
            if (currentSize == 0)
            {
                KTWARN(publog, "this is the first time through");
                // this is the first time creating histograms
                dataBundle.fCurrentTimeBin = 0;

                dataBundle.fSpectrograms.resize(nComponents);
                for (unsigned iComponent = currentSize; iComponent < nComponents; ++iComponent)
                {
                    dataBundle.fSpectrograms[iComponent].fSpectrogram = nullptr;
                    dataBundle.fHistCount = 0;
                }

                dataBundle.fTimeAxisMin = timeInRun;

                // calculate the properties of the time axis
                if (mode == KTROOTSpectrogramWriter::kSingle)
                {
                    dataBundle.fNTimeBins = unsigned((fWriter->GetMaxTime() - dataBundle.fTimeAxisMin) / sliceLength) + 1; // the +1 is so that the end time is the first slice ending outside the max time.
                }
                else // mode == KTROOTSpectrogramWriter::kSequential
                {
                    dataBundle.fNTimeBins = fWriter->GetNTimeBins();
                }

                dataBundle.fTimeAxisMax = dataBundle.fTimeAxisMin + sliceLength * (double)dataBundle.fNTimeBins;
                KTWARN(publog, dataBundle.fNTimeBins << "  " << dataBundle.fTimeAxisMin << "  " << dataBundle.fTimeAxisMax);

            } // end if currentSize == 0

            // create the new histograms
            for (auto sdIt = dataBundle.fSpectrograms.begin(); sdIt != dataBundle.fSpectrograms.end(); ++sdIt)
            {
                if (sdIt->fSpectrogram != nullptr) continue;
                unsigned iComponent = sdIt - dataBundle.fSpectrograms.begin();

                // calculate the properties of the frequency axis
                double freqBinWidth = data.GetArray(iComponent)->GetAxis().GetBinWidth();
                const KTAxisProperties< 1 >& axis = data.GetArray(iComponent)->GetAxis();
                sdIt->fFirstFreqBin = std::max< unsigned >(0, axis.FindBin(fWriter->GetMinFreq()));
                sdIt->fLastFreqBin = std::min< unsigned >(axis.GetNBins()-1, axis.FindBin(fWriter->GetMaxFreq()));
                //spectrograms[iComponent].fFirstFreqBin = unsigned((fWriter->GetMinFreq() - data.GetArray(iComponent)->GetAxis().GetBinLowEdge(0)) / freqBinWidth);
                //spectrograms[iComponent].fLastFreqBin = unsigned((fWriter->GetMaxFreq() - data.GetArray(iComponent)->GetAxis().GetBinLowEdge(0)) / freqBinWidth) + 1;
                unsigned nFreqBins = sdIt->fLastFreqBin - sdIt->fFirstFreqBin + 1;
                //double startFreq = spectrograms[iComponent].fFirstFreqBin * freqBinWidth;
                //double endFreq = spectrograms[iComponent].fLastFreqBin * freqBinWidth;
                double startFreq = axis.GetBinLowEdge(sdIt->fFirstFreqBin);
                double endFreq = axis.GetBinLowEdge(sdIt->fLastFreqBin) + freqBinWidth;
                // form the histogram name
                stringstream conv;
                conv << "_" << dataBundle.fHistCount << "_" << iComponent;
                string histName = dataBundle.fHistNameBase + conv.str();
                KTWARN(publog, "Creating histogram called <" << histName << ">");
                KTDEBUG(publog, "Creating new spectrogram histogram for component " << iComponent << ": " << histName << ", " << dataBundle.fNTimeBins << ", " << dataBundle.fTimeAxisMin << ", " << dataBundle.fTimeAxisMax << ", " << nFreqBins << ", " << startFreq << ", " << endFreq);
                sdIt->fSpectrogram = new TH2D(histName.c_str(), "Spectrogram", dataBundle.fNTimeBins, dataBundle.fTimeAxisMin, dataBundle.fTimeAxisMax, nFreqBins, startFreq, endFreq );
                sdIt->fSpectrogram->SetXTitle("Time (s)");
                sdIt->fSpectrogram->SetYTitle(axis.GetAxisLabel().c_str());
                sdIt->fSpectrogram->SetZTitle(data.GetArray(iComponent)->GetOrdinateLabel().c_str());
            }

        } // end if had to resize vector of histograms

        KTWARN(publog, "checking current time bin");
        dataBundle.fCurrentTimeBin += 1;
        if (dataBundle.fCurrentTimeBin > dataBundle.fNTimeBins)
        {
            dataBundle.fHistCount += 1;
            KTDEBUG(publog, "Bin count has triggered output of a spectrogram set; hist count is " << dataBundle.fHistCount);
            if (! fWriter->OpenAndVerifyFile())
            {
                KTWARN(publog, "File could not be opened; unable to write spectrograms.  Setting time bin to -1 to prevent further spectrogram creation.");
                dataBundle.fCurrentTimeBin = -1;
            }
            else
            {
                if (mode == KTROOTSpectrogramWriter::kSingle)
                {
                    KTWARN(publog, "writing in single mode");
                    OutputASpectrogramSet(dataBundle, false);
                    dataBundle.fCurrentTimeBin = -1; // writing to the spectrogram is finished, so returning -1 will prevent further histogram filling
                }
                else // mode == KTROOTSpectrogramWriter::kSequential
                {
                    KTWARN(publog, "writing in sequential mode");
                    OutputASpectrogramSet(dataBundle, true);
                    dataBundle.fCurrentTimeBin = 1;
                }
            }
        }

        return dataBundle.fCurrentTimeBin;
    }

    void KTROOTSpectrogramTypeWriter::OutputASpectrogramSet(DataTypeBundle& dataBundle, bool cloneSpectrograms)
    {
        // this function does not check the root file; it's assumed to be opened and verified already
        KTDEBUG(publog, "Outputting a spectrogram set; cloning? " << cloneSpectrograms);
        for (auto spectIt = dataBundle.fSpectrograms.begin(); spectIt != dataBundle.fSpectrograms.end(); ++spectIt)
        {
            TH2D* spectrogram = spectIt->fSpectrogram;
            KTWARN(publog, "Writing histogram <" << spectrogram->GetName() << ">");
            if (cloneSpectrograms)
            {
                spectIt->fSpectrogram = new TH2D();
                spectrogram->Copy(*spectIt->fSpectrogram);
                spectIt->fSpectrogram->Reset();
                stringstream conv;
                conv << "_" << dataBundle.fHistCount << "_" << spectIt - dataBundle.fSpectrograms.begin();
                string histName = dataBundle.fHistNameBase + conv.str();
                spectIt->fSpectrogram->SetName(histName.c_str());
            }
            spectrogram->SetDirectory(fWriter->GetFile());
            spectrogram->Write();
        }

        if (! cloneSpectrograms) dataBundle.fSpectrograms.clear();
        return;
    }

    void KTROOTSpectrogramTypeWriter::ClearASpectrogramSet(DataTypeBundle& dataBundle)
    {
        while (! dataBundle.fSpectrograms.empty())
        {
            delete dataBundle.fSpectrograms.back().fSpectrogram;
            dataBundle.fSpectrograms.pop_back();
        }
        return;
    }


} /* namespace Katydid */
