/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details KTEggProcessor iterates over the bundles in an Egg file
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTEggProcessor.hh"

#include "KTCommandLineOption.hh"
#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTEggReaderMonarch.hh"
#include "KTEggReader2011.hh"
#include "KTBundle.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesChannelData.hh"

using std::string;
using boost::shared_ptr;

namespace Katydid
{
    static KTCommandLineOption< int > sNBundlesCLO("Egg Processor", "Number of bundles to process", "n-bundles", 'n');
    static KTCommandLineOption< string > sFilenameCLO("Egg Processor", "Egg filename to open", "egg-file", 'e');
    static KTCommandLineOption< bool > sOldReaderCLO("Egg Processor", "Use the 2011 egg reader", "use-2011-egg-reader", 'z');

    KTLOGGER(egglog, "katydid.egg");

    static KTDerivedRegistrar< KTProcessor, KTEggProcessor > sEggProcRegistrar("egg-processor");

    KTEggProcessor::KTEggProcessor() :
            KTPrimaryProcessor(),
            fNBundles(0),
            fFilename(""),
            fEggReaderType(kMonarchEggReader),
            fSliceSizeRequest(0),
            fTimeSeriesType(kRealTimeSeries),
            fOutputDataName("time-series"),
            fHeaderSignal(),
            fDataSignal(),
            fBundleSignal(),
            fEggDoneSignal()
    {
        fConfigName = "egg-processor";

        RegisterSignal("header", &fHeaderSignal, "void (const KTEggHeader*)");
        RegisterSignal("data", &fDataSignal, "void (const KTTimeSeriesData*)");
        RegisterSignal("bundle", &fBundleSignal, "boost::shared_ptr<KTBundle>");
        RegisterSignal("egg-done", &fEggDoneSignal, "void ()");
    }

    KTEggProcessor::~KTEggProcessor()
    {
    }

    Bool_t KTEggProcessor::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetNBundles(node->GetData< UInt_t >("number-of-bundles", fNBundles));
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

            // specify the length of the time series (0 for use Monarch's record size)
            fSliceSizeRequest = node->GetData< UInt_t >("time-series-size", fSliceSizeRequest);

            // type of time series
            string timeSeriesTypeString = node->GetData< string >("time-series-type", "real");
            if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
            else if (timeSeriesTypeString == "fftw") SetTimeSeriesType(kFFTWTimeSeries);
            else
            {
                KTERROR(egglog, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
                return false;
            }

            // output data name
            SetOutputDataName(node->GetData< string >("output-data-name", fOutputDataName));
        }

        // Command-line settings
        SetNBundles(fCLHandler->GetCommandLineValue< Int_t >("n-bundles", fNBundles));
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
            eggReader->SetTimeSeriesSizeRequest(fSliceSizeRequest);
            if (fTimeSeriesType == kRealTimeSeries)
                eggReader->SetTimeSeriesType(KTEggReaderMonarch::kRealTimeSeries);
            else if (fTimeSeriesType == kFFTWTimeSeries)
                eggReader->SetTimeSeriesType(KTEggReaderMonarch::kFFTWTimeSeries);
            eggReader->SetOutputDataName(fOutputDataName);
            egg.SetReader(eggReader);
        }
        else
        {
            KTEggReader2011* eggReader = new KTEggReader2011();
            eggReader->SetOutputDataName(fOutputDataName);
            egg.SetReader(eggReader);
        }

        if (! egg.BreakEgg(fFilename))
        {
            KTERROR(egglog, "Egg did not break");
            return false;
        }

        fHeaderSignal(egg.GetHeader());

        KTINFO(egglog, "The egg file has been opened successfully"
                "\n\tand the header parsed and processed;"
                "\n\tProceeding with bundle processing");

        UInt_t iBundle = 0;
        while (kTRUE)
        {
            if (iBundle >= fNBundles) break;

            KTINFO(egglog, "Bundle " << iBundle);

            // Hatch the bundle
            shared_ptr<KTBundle> bundle = egg.HatchNextBundle();
            if (bundle.get() == NULL) break;

            if (iBundle == fNBundles - 1) bundle->SetIsLastBundle(true);

            KTTimeSeriesData* newData = bundle->GetData< KTTimeSeriesData >(fOutputDataName);
            if (newData != NULL)
            {
                KTDEBUG(egglog, "Time series data is present.");
                fDataSignal(newData);
            }
            else
            {
                KTWARN(egglog, "No time-series data present in bundle");
            }

            // Pass the bundle to any subscribers
            fBundleSignal(bundle);

            iBundle++;
        }

        fEggDoneSignal();

        return true;
    }

} /* namespace Katydid */
