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
#include "KTFactory.hh"
#include "KTProcSummary.hh"
#include "KTParam.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTTimeSeriesData.hh"
#include "KTSliceHeader.hh"

using std::string;


namespace Katydid
{
    static KTCommandLineOption< int > sNsCLO("Egg Processor", "Number of slices to process", "n-slices", 'n');
    static KTCommandLineOption< string > sFilenameCLO("Egg Processor", "Egg filename to open", "egg-file", 'e');
    static KTCommandLineOption< bool > sOldReaderCLO("Egg Processor", "Use the egg1 (2011) reader", "use-egg1-reader", 'z');

    KTLOGGER(egglog, "KTEggProcessor");

    KT_REGISTER_PROCESSOR(KTEggProcessor, "egg-processor");

    KTEggProcessor::KTEggProcessor(const std::string& name) :
            KTPrimaryProcessor(name),
            fNSlices(0),
            fProgressReportInterval(1),
            fFilename(""),
            fEggReaderType("none"),
            fSliceSize(1024),
            fStride(1024),
            fStartTime(0.),
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

    bool KTEggProcessor::Configure(const KTParamNode* node)
    {
        // First determine the egg reader type
        // config file setting
        if (node != NULL)
        {
            SetEggReaderType( node->GetValue("egg-reader", GetEggReaderType()) );
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
            SetNSlices(node->GetValue< unsigned >("number-of-slices", fNSlices));
            SetProgressReportInterval(node->GetValue< unsigned >("progress-report-interval", fProgressReportInterval));
            SetFilename(node->GetValue("filename", fFilename));

            // specify the length of the time series
            fSliceSize = node->GetValue< unsigned >("slice-size", fSliceSize);
            // specify the stride (leave unset to make stride == slice size)
            fStride = node->GetValue< unsigned >("stride", fSliceSize);
            // specify the time in the run to start
            fStartTime = node->GetValue< double >("start-time", fStartTime);

            if (fSliceSize == 0)
            {
                KTERROR(egglog, "Slice size MUST be specified");
                return false;
            }

            const KTParamNode* dacNode = node->NodeAt("dac");
            if (dacNode != NULL)
            {
                fDAC->Configure(dacNode);
            }

            // whether or not to normalize voltage values, and what the normalization is
            SetNormalizeVoltages(node->GetValue< bool >("normalize-voltages", fNormalizeVoltages));
        }

        // Command-line settings
        SetNSlices(fCLHandler->GetCommandLineValue< int >("n-slices", fNSlices));
        SetFilename(fCLHandler->GetCommandLineValue< string >("egg-file", fFilename));

        return true;
    }


    bool KTEggProcessor::ProcessEgg()
    {
        // Create egg reader and transfer information
        KTEggReader* reader = KTFactory< KTEggReader >::GetInstance()->Create(fEggReaderType);
        if (reader == NULL)
        {
            KTERROR(egglog, "Invalid egg reader type: " << fEggReaderType);
            return false;
        }
        reader->Configure(*this);

        // ******************************************************************** //
        // Call BreakEgg - this actually opens the file and loads its content
        KTDataPtr headerPtr = reader->BreakEgg(fFilename);
        if (! headerPtr)
        {
            KTERROR(egglog, "Egg did not break");
            delete reader;
            return false;
        }

        KTEggHeader& header = headerPtr->Of< KTEggHeader >();

        // pass the digitizer parameters from the egg header to the DAC
        fDAC->InitializeWithHeader(&header);


        fHeaderSignal(headerPtr);
        KTINFO(egglog, "The egg file has been opened successfully and the header was parsed and processed;");
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
        fSummarySignal(summary);
        delete summary;

        delete reader;

        return true;
    }

    void KTEggProcessor::UnlimitedLoop(KTEggReader* reader)
    {
        unsigned iSlice = 0, iProgress = 0;
        KTDataPtr data, nextData;
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
                data->Of< KTData >().SetLastData(true);
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
        KTDataPtr data;
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

            if (iSlice == fNSlices - 1) data->Of< KTData >().SetLastData(true);

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

    void KTEggProcessor::NormalizeData(KTDataPtr& data)
    {
        if (fNormalizeVoltages)
        {
            fDAC->ConvertData(data->Of< KTSliceHeader >(), data->Of< KTRawTimeSeriesData >());
        }
        return;
    }

} /* namespace Katydid */
