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

    KTLOGGER(egglog, "KTEggProcessor");

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

    bool KTEggProcessor::Configure(const scarab::param_node* node)
    {
        // First determine the egg reader type
        // config file setting
        if (node != NULL)
        {
            SetEggReaderType( node->get_value("egg-reader", GetEggReaderType()) );
        }
        // command line setting (overrides config file, if used)
        if (fCLHandler->IsCommandLineOptSet("use-egg1-reader"))
        {
            SetEggReaderType("egg1");
        }

        // Other settings

        // Config-file settings
        if (node != NULL)
        {
            SetNSlices(node->get_value< unsigned >("number-of-slices", fNSlices));
            SetProgressReportInterval(node->get_value< unsigned >("progress-report-interval", fProgressReportInterval));

            if (node->has("filename"))
            {
                KTDEBUG(egglog, "Adding single file to egg processor");
                fFilenames.clear();
                scarab::path metadataFilename;
                if (node->has("metadata"))
                {
                    metadataFilename = scarab::expand_path(node->get_value( "metadata" ));
                    KTINFO(egglog, "Added metadata file to egg processor: <" << metadataFilename << ">" );
                }
                fFilenames.push_back( std::make_pair(scarab::expand_path(node->get_value( "filename" )), metadataFilename) );
                KTINFO(egglog, "Added file to egg processor:\n" <<
                        "\tegg: <" << fFilenames.back().first << ">\n" <<
                        "\tmetadata: <" << fFilenames.back().second << ">");
            }
            else if (node->has("filenames"))
            {
                KTDEBUG(egglog, "Adding multiple files to egg processor");
                fFilenames.clear();
                const scarab::param_array* tConfigFilenames = node->array_at("filenames");
                const scarab::param_array* tConfigMetadatas = node->array_at("metadata");
                if( tConfigMetadatas != nullptr && tConfigMetadatas->size() != tConfigFilenames->size() )
                {
                    KTERROR(egglog, "Number of egg files (" << tConfigFilenames->size() << ") and metadata files (" << tConfigMetadatas->size() << ") were not the same");
                    return false;
                }
                scarab::path metadataFilename;
                for (unsigned iFile = 0; iFile < tConfigFilenames->size(); ++iFile)
                {
                    if (tConfigMetadatas != nullptr ) metadataFilename = scarab::expand_path((*tConfigMetadatas)[iFile].as_value().as_string());
                    fFilenames.push_back( std::make_pair(scarab::expand_path((*tConfigFilenames)[iFile].as_value().as_string()), metadataFilename) );
                    KTINFO(egglog, "Added file to egg processor:\n" <<
                            "\tegg: <" << fFilenames.back().first << ">\n" <<
                            "\tmetadata: <" << fFilenames.back().second << ">");
                }
            }

            fRequireMetadata = node->get_value< bool >("require-metadata", fRequireMetadata);

            // specify the length of the time series
            fSliceSize = node->get_value< unsigned >("slice-size", fSliceSize);
            // specify the stride (leave unset to make stride == slice size)
            fStride = node->get_value< unsigned >("stride", fSliceSize);
            // specify the time in the run to start
            fStartTime = node->get_value< double >("start-time", fStartTime);
            fStartRecord = node->get_value< unsigned >("start-record", fStartRecord);

            if (fSliceSize == 0)
            {
                KTERROR(egglog, "Slice size MUST be specified");
                return false;
            }

            const scarab::param_node* dacNode = node->node_at("dac");
            if (dacNode != NULL)
            {
                fDAC->Configure(dacNode);
            }

            // whether or not to normalize voltage values, and what the normalization is
            SetNormalizeVoltages(node->get_value< bool >("normalize-voltages", fNormalizeVoltages));
        }

        // Command-line settings
        SetNSlices(fCLHandler->GetCommandLineValue< int >("n-slices", fNSlices));
        if (fCLHandler->IsCommandLineOptSet("egg-file"))
        {
            KTDEBUG(egglog, "Specifying single egg file to egg processor from the CL");
            fFilenames.clear();
            fFilenames.push_back( std::make_pair(scarab::expand_path(fCLHandler->GetCommandLineValue< string >("egg-file")), scarab::path()) );
            KTINFO(egglog, "Added egg file to egg processor: <" << fFilenames.back().first << ">");
        }
        if (fCLHandler->IsCommandLineOptSet("metadata-file"))
        {
            if (fFilenames.size() != 1)
            {
                KTERROR(egglog, "Can only specify metadata file if there's already a single egg file specified" );
                return false;
            }
            KTDEBUG(egglog, "Specifying single metadata file to egg processor from the CL");
            fFilenames.back().second = scarab::expand_path(fCLHandler->GetCommandLineValue< string >("metadata-file"));
            KTINFO(egglog, "Added metadata file to egg processor: <" << fFilenames.back().second << ">");
        }

        return true;
    }


    bool KTEggProcessor::ProcessEgg()
    {
        // Create egg reader and transfer information
        KTEggReader* reader = scarab::factory< KTEggReader >::get_instance()->create(fEggReaderType);
        if (reader == NULL)
        {
            KTERROR(egglog, "Invalid egg reader type: " << fEggReaderType);
            return false;
        }
        reader->Configure(*this);

        if (fFilenames.size() == 0)
        {
            KTERROR(egglog, "No files have been specified");
            return false;
        }

        // ******************************************************************** //
        // Call BreakEgg - this actually opens the file and loads its content
        Nymph::KTDataPtr headerPtr = reader->BreakEgg(fFilenames);
        if (! headerPtr)
        {
            KTERROR(egglog, "Egg did not break");
            delete reader;
            return false;
        }

        KTEggHeader& header = headerPtr->Of< KTEggHeader >();

        // pass the digitizer parameters from the egg header to the DAC
        if (! fDAC->InitializeWithHeader(header))
        {
            KTERROR(egglog, "Unable to initialize the DAC");
            return false;
        }

        fHeaderSignal(headerPtr);
        KTINFO(egglog, "The egg file has been opened successfully and the header was parsed and processed;");
        KTDEBUG(egglog, "From EggProcessor: FileNumber is ;" << header.GetFileNumber());
        KTPROG(egglog, "Proceeding with slice processing");

        if (fNSlices == 0) UnlimitedLoop(reader);
        else LimitedLoop(reader);

        fEggDoneSignal();

        KTProcSummary* summary = new KTProcSummary();
        summary->SetNSlicesProcessed(reader->GetNSlicesProcessed());
        summary->SetNRecordsProcessed(reader->GetNRecordsProcessed());
        summary->SetIntegratedTime(reader->GetIntegratedTime());
        KTDEBUG(egglog, "Summary of processing:\n" <<
                "\tSlices processed: " << summary->GetNSlicesProcessed() << '\n' <<
                "\tRecords processed: " << summary->GetNRecordsProcessed() << '\n' <<
                "\tIntegrated time: " << summary->GetIntegratedTime() << " s");
        if(fNSlices != 0 && summary->GetNSlicesProcessed() != fNSlices)
        {
            KTWARN(egglog, "Could not process the requested number of slices because there was not enough data in the file(s):\n" <<
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
            KTERROR(egglog, "Unable to hatch first slice of data.");
            return;
        }
        while (true)
        {
            KTINFO(egglog, "Hatching slice " << iSlice);

            // Hatch the slice
            if (! HatchNextSlice(reader, nextData))
            {
                data->Of< Nymph::KTData >().SetLastData(true);
                nextSliceIsValid = false;
            }

            if (data->Has< KTRawTimeSeriesData >())
            {
                KTDEBUG(egglog, "Raw time series data is present.");
                fRawDataSignal(data);
                NormalizeData(data);
            }
            if (data->Has< KTTimeSeriesData >())
            {
                KTDEBUG(egglog, "Normalized time series data is present.");
                fDataSignal(data);
            }
            else
            {
                KTWARN(egglog, "No time-series data present in slice");
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

            KTINFO(egglog, "Hatching slice " << iSlice << "/" << fNSlices);

            // Hatch the slice
            if (! HatchNextSlice(reader, data)) break;

            if (iSlice == fNSlices - 1) data->Of< Nymph::KTData >().SetLastData(true);

            if (data->Has< KTRawTimeSeriesData >())
            {
                KTDEBUG(egglog, "Raw time series data is present.");
                fRawDataSignal(data);
                NormalizeData(data);
            }
            if (data->Has< KTTimeSeriesData >())
            {
                KTDEBUG(egglog, "Normalized time series data is present.");
                fDataSignal(data);
            }
            else
            {
                KTWARN(egglog, "No time-series data present in slice");
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
