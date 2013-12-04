/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details KTEggProcessor iterates over the bundles in an Egg file
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTEggProcessor.hh"

#ifdef USE_MONARCH
#include "KTEggReaderMonarch.hh"
#endif

#include "KTCommandLineOption.hh"
#include "KTData.hh"
#include "KTEggHeader.hh"
#include "KTEggReader2011.hh"
#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTProcSummary.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"

using std::string;


namespace Katydid
{
    static KTCommandLineOption< int > sNsCLO("Egg Processor", "Number of slices to process", "n-slices", 'n');
    static KTCommandLineOption< string > sFilenameCLO("Egg Processor", "Egg filename to open", "egg-file", 'e');
    static KTCommandLineOption< bool > sOldReaderCLO("Egg Processor", "Use the 2011 egg reader", "use-2011-egg-reader", 'z');

    KTLOGGER(egglog, "katydid.egg");

    static KTDerivedNORegistrar< KTProcessor, KTEggProcessor > sEggProcRegistrar("egg-processor");

    KTEggProcessor::KTEggProcessor(const std::string& name) :
            KTPrimaryProcessor(name),
            fNSlices(0),
            fProgressReportInterval(1),
            fFilename(""),
            fEggReaderType(kMonarchEggReader),
            fSliceSize(1024),
            fStride(1024),
            fNormalizeVoltages(true),
            fFullVoltageScale(0.5),
            fNADCLevels(256),
            fNormalization(1.),
            fCalculateNormalization(true),
            fTimeSeriesType(kRealTimeSeries),
            fHeaderSignal("header", this),
            fDataSignal("slice", this),
            fEggDoneSignal("egg-done", this),
            fSummarySignal("summary", this)
    {
    }

    KTEggProcessor::~KTEggProcessor()
    {
    }

    Bool_t KTEggProcessor::Configure(const KTPStoreNode* node)
    {
        // First determine the egg reader type
        // config file setting
        if (node != NULL)
        {
            string eggReaderTypeString = node->GetData< string >("egg-reader", "monarch");
            if (eggReaderTypeString == "monarch") SetEggReaderType(kMonarchEggReader);
            else if (eggReaderTypeString == "2011") SetEggReaderType(k2011EggReader);
            else
            {
                KTERROR(egglog, "Illegal string for egg reader type: <" << eggReaderTypeString << ">");
                return false;
            }
        }
        // command line setting (overrides config file, if used)
        if (fCLHandler->IsCommandLineOptSet("use-2011-egg-reader"))
        {
            SetEggReaderType(k2011EggReader);
        }

#ifndef USE_MONARCH
        if (fEggReaderType == kMonarchEggReader)
        {
            KTERROR(egglog, "Monarch is not enabled; please select another egg reader type");
            return false;
        }
#endif

        // Other settings

        // Config-file settings
        if (node != NULL)
        {
            SetNSlices(node->GetData< UInt_t >("number-of-slices", fNSlices));
            SetProgressReportInterval(node->GetData< UInt_t >("progress-report-interval", fProgressReportInterval));
            SetFilename(node->GetData< string >("filename", fFilename));

            // specify the length of the time series
            fSliceSize = node->GetData< UInt_t >("slice-size", fSliceSize);
            // specify the stride (leave unset to make stride == slice size)
            fStride = node->GetData< UInt_t >("stride", fSliceSize);

            if (fSliceSize == 0)
            {
                KTERROR(egglog, "Slice size MUST be specified");
                return false;
            }

            // type of time series
            string timeSeriesTypeString = node->GetData< string >("time-series-type", "real");
            if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
            else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
            else
            {
                KTERROR(egglog, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
                return false;
            }

            // whether or not to normalize voltage values, and what the normalization is
            SetNormalizeVoltages(node->GetData< Bool_t >("normalize-voltages", fNormalizeVoltages));
            if (node->HasData("full-voltage-scale"))
                SetFullVoltageScale(node->GetData< Double_t >("full-voltage-scale", fFullVoltageScale));
            if (node->HasData("n-adc-levels"))
                SetNADCLevels(node->GetData< UInt_t >("n-adc-levels", fNADCLevels));
            if (node->HasData("normalization"))
                SetNormalization(node->GetData< Double_t >("normalization", fNormalization));
        }

        // Command-line settings
        SetNSlices(fCLHandler->GetCommandLineValue< Int_t >("n-slices", fNSlices));
        SetFilename(fCLHandler->GetCommandLineValue< string >("egg-file", fFilename));

        return true;
    }


    Bool_t KTEggProcessor::ProcessEgg()
    {
        KTEggReader* reader = NULL;

        // Create egg reader and transfer information
        if (fEggReaderType == kMonarchEggReader)
        {
#ifdef USE_MONARCH
            KTEggReaderMonarch* eggReaderMonarch = new KTEggReaderMonarch();
            eggReaderMonarch->SetSliceSize(fSliceSize);
            eggReaderMonarch->SetStride(fStride);
            if (fTimeSeriesType == kRealTimeSeries)
                eggReaderMonarch->SetTimeSeriesType(KTEggReaderMonarch::kRealTimeSeries);
            else if (fTimeSeriesType == kFFTWTimeSeries)
                eggReaderMonarch->SetTimeSeriesType(KTEggReaderMonarch::kFFTWTimeSeries);
            reader = eggReaderMonarch;
#else
            KTERROR(egglog, "Monarch is not enabled; please select another egg reader type");
            return false;
#endif
        }
        else
        {
            KTEggReader2011* eggReader2011 = new KTEggReader2011();
            reader = eggReader2011;
        }

        KTEggHeader* header = reader->BreakEgg(fFilename);
        if (header == NULL)
        {
            KTERROR(egglog, "Egg did not break");
            return false;
        }

        fHeaderSignal(header);

        KTINFO(egglog, "The egg file has been opened successfully and the header was parsed and processed;");
        KTPROG(egglog, "Proceeding with slice processing");

        if (fCalculateNormalization) CalculateNormalization();

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

        return true;
    }

    void KTEggProcessor::UnlimitedLoop(KTEggReader* reader)
    {
        UInt_t iSlice = 0, iProgress = 0;
        KTDataPtr data;
        while (kTRUE)
        {
            KTINFO(egglog, "Hatching slice " << iSlice);

            // Hatch the slice
            if (! HatchNextSlice(reader, data)) break;

            if (data->Has< KTTimeSeriesData >())
            {
                KTDEBUG(egglog, "Time series data is present.");
                NormalizeData(data);
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
        }
        return;
    }

    void KTEggProcessor::LimitedLoop(KTEggReader* reader)
    {
        UInt_t iSlice = 0, iProgress = 0;
        KTDataPtr data;
        while (kTRUE)
        {
            if (fNSlices != 0 && iSlice >= fNSlices)
            {
                KTPROG(egglog, iSlice << "/" << fNSlices << " slices hatched; slice processing is complete");
                break;
            }

            KTINFO(egglog, "Hatching slice " << iSlice << "/" << fNSlices);

            // Hatch the slice
            if (! HatchNextSlice(reader, data)) break;

            if (iSlice == fNSlices - 1) data->Of< KTData >().fLastData = true;

            if (data->Has< KTTimeSeriesData >())
            {
                KTDEBUG(egglog, "Time series data is present.");
                NormalizeData(data);
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
                KTPROG(egglog, iSlice << " slices processed (" << Double_t(iSlice) / Double_t(fNSlices) * 100. << " %)");
            }
        }
        return;
    }

    void KTEggProcessor::NormalizeData(KTDataPtr& data) const
    {
        if (fNormalizeVoltages)
        {
            KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
            UInt_t nComponents = tsData.GetNComponents();
            for (UInt_t iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                tsData.GetTimeSeries(iComponent)->Scale(fNormalization);
            }
        }
        return;
    }

} /* namespace Katydid */
