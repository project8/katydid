/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details KTEggProcessor iterates over the bundles in an Egg file
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTEggProcessor.hh"

#include "KTCommandLineOption.hh"
#include "KTData.hh"
#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTEggReaderMonarch.hh"
#include "KTEggReader2011.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"

using std::string;
using boost::shared_ptr;

namespace Katydid
{
    static KTCommandLineOption< int > sNsCLO("Egg Processor", "Number of slices to process", "n-slices", 'n');
    static KTCommandLineOption< string > sFilenameCLO("Egg Processor", "Egg filename to open", "egg-file", 'e');
    static KTCommandLineOption< bool > sOldReaderCLO("Egg Processor", "Use the 2011 egg reader", "use-2011-egg-reader", 'z');

    KTLOGGER(egglog, "katydid.egg");

    static KTDerivedRegistrar< KTProcessor, KTEggProcessor > sEggProcRegistrar("egg-processor");

    KTEggProcessor::KTEggProcessor(const std::string& name) :
            KTPrimaryProcessor(name),
            fNSlices(0),
            fProgressReportInterval(1),
            fFilename(""),
            fEggReaderType(kMonarchEggReader),
            fSliceSize(1024),
            fStride(1024),
            fTimeSeriesType(kRealTimeSeries),
            fHeaderSignal("header", this),
            fDataSignal("slice", this),
            fEggDoneSignal("egg-done", this)
    {
    }

    KTEggProcessor::~KTEggProcessor()
    {
    }

    Bool_t KTEggProcessor::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetNSlices(node->GetData< UInt_t >("number-of-slices", fNSlices));
            SetProgressReportInterval(node->GetData< UInt_t >("progress-report-interval", fProgressReportInterval));
            SetFilename(node->GetData< string >("filename", fFilename));

            // choose the egg reader
            string eggReaderTypeString = node->GetData< string >("egg-reader", "monarch");
            if (eggReaderTypeString == "monarch") SetEggReaderType(kMonarchEggReader);
            else if (eggReaderTypeString == "2011") SetEggReaderType(k2011EggReader);
            else
            {
                KTERROR(egglog, "Illegal string for egg reader type: <" << eggReaderTypeString << ">");
                return false;
            }

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
        }

        // Command-line settings
        SetNSlices(fCLHandler->GetCommandLineValue< Int_t >("n-slices", fNSlices));
        SetFilename(fCLHandler->GetCommandLineValue< string >("egg-file", fFilename));
        if (fCLHandler->IsCommandLineOptSet("use-2011-egg-reader"))
        {
            SetEggReaderType(k2011EggReader);
        }

        return true;
    }


    Bool_t KTEggProcessor::ProcessEgg()
    {
        KTEgg egg;

        if (fEggReaderType == kMonarchEggReader)
        {
            KTEggReaderMonarch* eggReader = new KTEggReaderMonarch();
            eggReader->SetSliceSize(fSliceSize);
            eggReader->SetStride(fStride);
            if (fTimeSeriesType == kRealTimeSeries)
                eggReader->SetTimeSeriesType(KTEggReaderMonarch::kRealTimeSeries);
            else if (fTimeSeriesType == kFFTWTimeSeries)
                eggReader->SetTimeSeriesType(KTEggReaderMonarch::kFFTWTimeSeries);
            egg.SetReader(eggReader);
        }
        else
        {
            KTEggReader2011* eggReader = new KTEggReader2011();
            egg.SetReader(eggReader);
        }

        if (! egg.BreakEgg(fFilename))
        {
            KTERROR(egglog, "Egg did not break");
            return false;
        }

        fHeaderSignal(egg.GetHeader());

        KTINFO(egglog, "The egg file has been opened successfully"
                "\n\tand the header parsed and processed;");
        KTPROG(egglog, "Proceeding with slice processing");

        if (fNSlices == 0) UnlimitedLoop(egg);
        else LimitedLoop(egg);

        fEggDoneSignal();

        return true;
    }

    void KTEggProcessor::UnlimitedLoop(KTEgg& egg)
    {
        UInt_t iSlice = 0, iProgress = 0;
        while (kTRUE)
        {
            KTINFO(egglog, "Hatching slice " << iSlice);

            // Hatch the slice
            shared_ptr<KTData> data = egg.HatchNextSlice();
            if (data.get() == NULL) break;

            if (data->Has< KTTimeSeriesData >())
            {
                KTDEBUG(egglog, "Time series data is present.");
                fDataSignal(data);
            }
            else
            {
                KTWARN(egglog, "No time-series data present in slice");
            }

            iSlice++;
            iProgress++;

            if (iProgress == fProgressReportInterval)
            {
                iProgress = 0;
                KTPROG(egglog, iSlice << " slices processed");
            }
        }
        return;
    }

    void KTEggProcessor::LimitedLoop(KTEgg& egg)
    {
        UInt_t iSlice = 0, iProgress = 0;
        while (kTRUE)
        {
            if (fNSlices != 0 && iSlice >= fNSlices)
            {
                KTPROG(egglog, iSlice << "/" << fNSlices << " slices hatched; slice processing is complete");
                break;
            }

            KTINFO(egglog, "Hatching slice " << iSlice << "/" << fNSlices);

            // Hatch the slice
            shared_ptr<KTData> data = egg.HatchNextSlice();
            if (data.get() == NULL) break;

            if (iSlice == fNSlices - 1) data->Of< KTData >().fLastData = true;

            if (data->Has< KTTimeSeriesData >())
            {
                KTDEBUG(egglog, "Time series data is present.");
                fDataSignal(data);
            }
            else
            {
                KTWARN(egglog, "No time-series data present in slice");
            }

            iSlice++;
            iProgress++;

            if (iProgress == fProgressReportInterval)
            {
                iProgress = 0;
                KTPROG(egglog, iSlice << " slices processed (" << Double_t(iSlice) / Double_t(fNSlices) * 100. << " %)");
            }
        }
        return;
    }

} /* namespace Katydid */
