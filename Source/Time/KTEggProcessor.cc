/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details KTEggProcessor iterates over the bundles in an Egg file
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTEggProcessor.hh"

#include "KTCommandLineOption.hh"
#include "KTDAC.hh"
#include "KTData.hh"
#include "KTEggHeader.hh"
#include "KTEggReader.hh"
#include "KTProcSummary.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTTimeSeriesData.hh"
#include "KTSliceHeader.hh"

using std::string;


namespace Katydid
{
    static Nymph::KTCommandLineOption< int > sNsCLO("Egg Processor", "Number of slices to process", "n-slices", 'n');
    static Nymph::KTCommandLineOption< string > sFilenameCLO("Egg Processor", "Egg filename to open", "egg-file", 'e');
    static Nymph::KTCommandLineOption< string > sMetadataCLO("Egg Processor", "Metadata filename to open", "metadata-file", 'm');
    static Nymph::KTCommandLineOption< bool > sOldReaderCLO("Egg Processor", "Use the egg1 (2011) reader", "use-egg1-reader", 'z');

    LOGGER(egglog, "KTEggProcessor");

    KT_REGISTER_PROCESSOR(KTEggProcessor, "egg-processor");

    KTEggProcessor::KTEggProcessor(const std::string& name) :
            KTPrimaryProcessor(name),
            fNSlices(0),
            fProgressReportInterval(1),
            fFilenames(),
            fEggReaderType("none"),
            fRequireMetadata(false),
            fSliceSize(1024),
            fStride(1024),
            fStartTime(0.),
            fStartRecord(0),
            fDAC(new KTDAC()),
            fNormalizeVoltages(true),
            fHeaderSignal("header", this),
            fRawDataSignal("raw-ts", this),
            fDataSignal("ts", this),
            fEggDoneSignal("egg-done", this),
            fSummarySignal("summary", this)
    {
    }

    KTEggProcessor::~KTEggProcessor()
    {
        delete fDAC;
    }

    bool KTEggProcessor::Configure(const scarab::param_node& node)
    {
        // First determine the egg reader type
        // config file setting
        SetEggReaderType( node.get_value("egg-reader", GetEggReaderType()) );
        // command line setting (overrides config file, if used)
        if (fCLHandler->IsCommandLineOptSet("use-egg1-reader"))
        {
            SetEggReaderType("egg1");
        }

        // Other settings

        // Config-file settings
            SetNSlices(node.get_value("number-of-slices", fNSlices));
            SetProgressReportInterval(node.get_value("progress-report-interval", fProgressReportInterval));

            if (node.has("filename"))
            {
                LDEBUG(egglog, "Adding single file to egg processor");
                fFilenames.clear();
                scarab::path metadataFilename(scarab::expand_path(node.get_value("metadata", "")));
                if (! metadataFilename.empty()) LINFO(egglog, "Added metadata file to egg processor: <" << metadataFilename << ">" );
                fFilenames.push_back( std::make_pair(scarab::expand_path(node["filename"]().as_string()), metadataFilename) );
                LINFO(egglog, "Added file to egg processor:\n" <<
                        "\tegg: <" << fFilenames.back().first << ">\n" <<
                        "\tmetadata: <" << fFilenames.back().second << ">");
            }
            else if (node.has("filenames"))
            {
                LDEBUG(egglog, "Adding multiple files to egg processor");
                fFilenames.clear();
                const scarab::param_array& tFilenames = node["filenames"].as_array();
                const scarab::param_array* tMetadataPtrs = nullptr;
                if (node.has("metadata") && node["metadata"].is_array())
                {
                    tMetadataPtrs = &node["metadata"].as_array();
                }
                if (tMetadataPtrs != nullptr && tMetadataPtrs->size() != tFilenames.size())
                {
                    LERROR(egglog, "Number of egg files (" << tFilenames.size() << ") and metadata files (" << tMetadataPtrs->size() << ") were not the same");
                    return false;
                }
                scarab::path metadataFilename;
                for (unsigned iFile = 0; iFile < tFilenames.size(); ++iFile)
                {
                    if (tMetadataPtrs != nullptr) metadataFilename = scarab::expand_path((*tMetadataPtrs)[iFile].as_value().as_string());
                    fFilenames.push_back( std::make_pair(scarab::expand_path(tFilenames[iFile].as_value().as_string()), metadataFilename) );
                    LINFO(egglog, "Added file to egg processor:\n" <<
                            "\tegg: <" << fFilenames.back().first << ">\n" <<
                            "\tmetadata: <" << fFilenames.back().second << ">");
                }
            }

            fRequireMetadata = node.get_value("require-metadata", fRequireMetadata);

            // specify the length of the time series
            fSliceSize = node.get_value("slice-size", fSliceSize);
            // specify the stride (leave unset to make stride == slice size)
            fStride = node.get_value("stride", fSliceSize);
            // specify the time in the run to start
            fStartTime = node.get_value("start-time", fStartTime);
            fStartRecord = node.get_value("start-record", fStartRecord);

            if (fSliceSize == 0)
            {
                LERROR(egglog, "Slice size MUST be specified");
                return false;
            }

            if (node.has("dac"))
            {
                fDAC->Configure(node["dac"].as_node());
            }

            // whether or not to normalize voltage values, and what the normalization is
            SetNormalizeVoltages(node.get_value("normalize-voltages", fNormalizeVoltages));

        // Command-line settings
        SetNSlices(fCLHandler->GetCommandLineValue("n-slices", fNSlices));
        if (fCLHandler->IsCommandLineOptSet("egg-file"))
        {
            LDEBUG(egglog, "Specifying single egg file to egg processor from the CL");
            fFilenames.clear();
            fFilenames.push_back( std::make_pair(scarab::expand_path(fCLHandler->GetCommandLineValue< string >("egg-file")), scarab::path()) );
            LINFO(egglog, "Added egg file to egg processor: <" << fFilenames.back().first << ">");
        }
        if (fCLHandler->IsCommandLineOptSet("metadata-file"))
        {
            if (fFilenames.size() != 1)
            {
                LERROR(egglog, "Can only specify metadata file if there's already a single egg file specified" );
                return false;
            }
            LDEBUG(egglog, "Specifying single metadata file to egg processor from the CL");
            fFilenames.back().second = scarab::expand_path(fCLHandler->GetCommandLineValue< string >("metadata-file"));
            LINFO(egglog, "Added metadata file to egg processor: <" << fFilenames.back().second << ">");
        }

        return true;
    }


    bool KTEggProcessor::ProcessEgg()
    {
        // Create egg reader and transfer information
        KTEggReader* reader = scarab::factory< KTEggReader >::get_instance()->create(fEggReaderType);
        if (reader == NULL)
        {
            LERROR(egglog, "Invalid egg reader type: " << fEggReaderType);
            return false;
        }
        reader->Configure(*this);

        if (fFilenames.size() == 0)
        {
            LERROR(egglog, "No files have been specified");
            return false;
        }

        // ******************************************************************** //
        // Call BreakEgg - this actually opens the file and loads its content
        Nymph::KTDataPtr headerPtr = reader->BreakEgg(fFilenames);
        if (! headerPtr)
        {
            LERROR(egglog, "Egg did not break");
            delete reader;
            return false;
        }

        KTEggHeader& header = headerPtr->Of< KTEggHeader >();

        // pass the digitizer parameters from the egg header to the DAC
        if (! fDAC->InitializeWithHeader(header))
        {
            LERROR(egglog, "Unable to initialize the DAC");
            return false;
        }

        fHeaderSignal(headerPtr);
        LINFO(egglog, "The egg file has been opened successfully and the header was parsed and processed;");
        KTPROG(egglog, "Proceeding with slice processing");

        if (fNSlices == 0) UnlimitedLoop(reader);
        else LimitedLoop(reader);

        fEggDoneSignal();

        KTProcSummary* summary = new KTProcSummary();
        summary->SetNSlicesProcessed(reader->GetNSlicesProcessed());
        summary->SetNRecordsProcessed(reader->GetNRecordsProcessed());
        summary->SetIntegratedTime(reader->GetIntegratedTime());
        LDEBUG(egglog, "Summary of processing:\n" <<
                "\tSlices processed: " << summary->GetNSlicesProcessed() << '\n' <<
                "\tRecords processed: " << summary->GetNRecordsProcessed() << '\n' <<
                "\tIntegrated time: " << summary->GetIntegratedTime() << " s");
        if(fNSlices != 0 && summary->GetNSlicesProcessed() != fNSlices)
        {
            LWARN(egglog, "Could not process the requested number of slices because there was not enough data in the file(s):\n" <<
                    "\tSlices requested: " << fNSlices << '\n' <<
                    "\tSlices processed: " << summary->GetNSlicesProcessed());
        }
        fSummarySignal(summary);
        delete summary;

        delete reader;

        return true;
    }

    void KTEggProcessor::UnlimitedLoop(KTEggReader* reader)
    {
        unsigned iSlice = 0, iProgress = 0;
        Nymph::KTDataPtr data, nextData;
        bool nextSliceIsValid = true;
        if (! HatchNextSlice(reader, data))
        {
            LERROR(egglog, "Unable to hatch first slice of data.");
            return;
        }
        while (true)
        {
            LINFO(egglog, "Hatching slice " << iSlice);

            // Hatch the slice
            if (! HatchNextSlice(reader, nextData))
            {
                data->Of< Nymph::KTData >().SetLastData(true);
                nextSliceIsValid = false;
            }

            if (data->Has< KTRawTimeSeriesData >())
            {
                LDEBUG(egglog, "Raw time series data is present.");
                fRawDataSignal(data);
                NormalizeData(data);
            }
            if (data->Has< KTTimeSeriesData >())
            {
                LDEBUG(egglog, "Normalized time series data is present.");
                fDataSignal(data);
            }
            else
            {
                LWARN(egglog, "No time-series data present in slice");
            }

            ++iSlice;
            ++iProgress;

            if (iProgress == fProgressReportInterval)
            {
                iProgress = 0;
                KTPROG(egglog, iSlice << " slices processed");
            }

            if (! nextSliceIsValid) break;
            data = nextData;
        }
        return;
    }

    void KTEggProcessor::LimitedLoop(KTEggReader* reader)
    {
        unsigned iSlice = 0, iProgress = 0;
        Nymph::KTDataPtr data;
        while (true)
        {
            if (fNSlices != 0 && iSlice >= fNSlices)
            {
                KTPROG(egglog, iSlice << "/" << fNSlices << " slices hatched; slice processing is complete");
                break;
            }

            LINFO(egglog, "Hatching slice " << iSlice << "/" << fNSlices);

            // Hatch the slice
            if (! HatchNextSlice(reader, data)) break;

            if (iSlice == fNSlices - 1) data->Of< Nymph::KTData >().SetLastData(true);

            if (data->Has< KTRawTimeSeriesData >())
            {
                LDEBUG(egglog, "Raw time series data is present.");
                fRawDataSignal(data);
                NormalizeData(data);
            }
            if (data->Has< KTTimeSeriesData >())
            {
                LDEBUG(egglog, "Normalized time series data is present.");
                fDataSignal(data);
            }
            else
            {
                LWARN(egglog, "No time-series data present in slice");
            }

            ++iSlice;
            ++iProgress;

            if (iProgress == fProgressReportInterval)
            {
                iProgress = 0;
                KTPROG(egglog, iSlice << " slices processed (" << double(iSlice) / double(fNSlices) * 100. << " %)");
            }
        }
        return;
    }

    void KTEggProcessor::NormalizeData(Nymph::KTDataPtr& data)
    {
        if (fNormalizeVoltages)
        {
            fDAC->ConvertData(data->Of< KTSliceHeader >(), data->Of< KTRawTimeSeriesData >());
        }
        return;
    }

} /* namespace Katydid */
