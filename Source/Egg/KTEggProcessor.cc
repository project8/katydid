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

using std::string;
using boost::shared_ptr;

namespace Katydid
{
    static KTCommandLineOption< int > sNsCLO("Egg Processor", "Number of bundles to process", "n-bundles", 'n');
    static KTCommandLineOption< string > sFilenameCLO("Egg Processor", "Egg filename to open", "egg-file", 'e');
    static KTCommandLineOption< bool > sOldReaderCLO("Egg Processor", "Use the 2011 egg reader", "use-2011-egg-reader", 'z');

    KTLOGGER(egglog, "katydid.egg");

    static KTDerivedRegistrar< KTProcessor, KTEggProcessor > sEggProcRegistrar("egg-processor");

    KTEggProcessor::KTEggProcessor() :
            KTPrimaryProcessor(),
            fNSlices(0),
            fFilename(""),
            fEggReaderType(kMonarchEggReader),
            fSliceSizeRequest(0),
            fTimeSeriesType(kRealTimeSeries),
            fHeaderSignal(),
            fDataSignal(),
            //fSignal(),
            fEggDoneSignal()
    {
        fConfigName = "egg-processor";

        RegisterSignal("header", &fHeaderSignal, "void (const KTEggHeader*)");
        RegisterSignal("data", &fDataSignal, "void (boost::shared_ptr<KTData>)");
        //RegisterSignal("bundle", &fBundleSignal, "boost::shared_ptr<KTBundle>");
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
            SetNSlices(node->GetData< UInt_t >("number-of-slices", fNSlices));
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
        }

        // Command-line settings
        SetNSlices(fCLHandler->GetCommandLineValue< Int_t >("n-bundles", fNSlices));
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
                "\n\tand the header parsed and processed;"
                "\n\tProceeding with bundle processing");

        UInt_t iSlice = 0;
        while (kTRUE)
        {
            if (iSlice >= fNSlices) break;

            KTINFO(egglog, "Slice " << iSlice);

            // Hatch the bundle
            shared_ptr<KTData> data = egg.HatchNextSlice();
            if (data.get() == NULL) break;

            if (iSlice == fNSlices - 1) data->Of< KTData >().fLastData(true);

            KTTimeSeriesData* newData = data->Of< KTTimeSeriesData >();
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
            //fBundleSignal(bundle);

            iSlice++;
        }

        fEggDoneSignal();

        return true;
    }

} /* namespace Katydid */
