/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details KTEggProcessor iterates over the events in an Egg file
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTEggProcessor.hh"

#include "KTCommandLineOption.hh"
#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTEggReaderMonarch.hh"
#include "KTEggReader2011.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesDataReal.hh"
#include "KTTimeSeriesDataFFTW.hh"

//#include "TCanvas.h"
//#include "TH1.h"

using std::string;

namespace Katydid
{
    static KTCommandLineOption< int > sNEventsCLO("Egg Processor", "Number of events to process", "n-events", 'n');
    static KTCommandLineOption< string > sFilenameCLO("Egg Processor", "Egg filename to open", "egg-file", 'e');
    static KTCommandLineOption< bool > sOldReaderCLO("Egg Processor", "Use the 2011 egg reader", "use-2011-egg-reader", 'z');

    KTLOGGER(egglog, "katydid.egg");

    static KTDerivedRegistrar< KTProcessor, KTEggProcessor > sEggProcRegistrar("egg-processor");

    KTEggProcessor::KTEggProcessor() :
            KTProcessor(),
            fNEvents(0),
            fFilename(""),
            fEggReaderType(kMonarchEggReader),
            fTimeSeriesType(kRealTimeSeries),
            fHeaderSignal(),
            fEventSignal(),
            fEggDoneSignal()
    {
        fConfigName = "egg-processor";

        RegisterSignal("header", &fHeaderSignal, "void (const KTEggHeader*)");
        RegisterSignal("event", &fEventSignal, "void (KTEvent*)");
        RegisterSignal("egg_done", &fEggDoneSignal, "void ()");
    }

    KTEggProcessor::~KTEggProcessor()
    {
    }

    Bool_t KTEggProcessor::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetNEvents(node->GetData< UInt_t >("number-of-events", fNEvents));
            SetFilename(node->GetData< string >("filename", fFilename));

            // egg reader
            string eggReaderTypeString = node->GetData< string >("egg-reader", "monarch");
            if (eggReaderTypeString == "monarch") SetEggReaderType(kMonarchEggReader);
            else if (eggReaderTypeString == "2011") SetEggReaderType(k2011EggReader);
            else
            {
                KTERROR(egglog, "Illegal string for egg reader type: <" << eggReaderTypeString << ">");
                return false;
            }

            // type series
            string timeSeriesTypeString = node->GetData< string >("time-series", "real");
            if (timeSeriesTypeString == "real") SetTimeSeriesType(kRealTimeSeries);
            else if (timeSeriesTypeString == "fftw-complex") SetTimeSeriesType(kFFTWComplexTimeSeries);
            else
            {
                KTERROR(egglog, "Illegal string for time series type: <" << timeSeriesTypeString << ">");
                return false;
            }
        }

        // Command-line settings
        SetNEvents(fCLHandler->GetCommandLineValue< Int_t >("n-events", fNEvents));
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
            egg.SetReader(new KTEggReaderMonarch());
        }
        else
        {
            egg.SetReader(new KTEggReader2011());
        }

        if (! egg.BreakEgg(fFilename))
        {
            KTERROR(egglog, "Egg did not break");
            return false;
        }

        fHeaderSignal(egg.GetHeader());

        KTINFO(egglog, "The egg file has been opened successfully"
                "\n\tand the header parsed and processed;"
                "\n\tProceeding with event processing");

        UInt_t iEvent = 0;
        while (kTRUE)
        {
            if (iEvent >= fNEvents) break;

            KTINFO(egglog, "Event " << iEvent);

            // Hatch the event
            KTEvent* event = egg.HatchNextEvent();
            if (event == NULL) break;

            Bool_t tsDataPresent = false;
            if (event->GetData<KTTimeSeriesDataReal>(KTTimeSeriesDataReal::StaticGetName()) != NULL)
            {
                tsDataPresent = true;
                KTDEBUG(egglog, "Time series data (type: real) is present.");
            }
            if (event->GetData<KTTimeSeriesDataFFTW>(KTTimeSeriesDataFFTW::StaticGetName()) != NULL)
            {
                tsDataPresent = true;
                KTDEBUG(egglog, "Time series data (type: fftw-complex) is present.");
            }
            if (! tsDataPresent)
            {
                KTWARN(egglog, "No time-series data present in event");
                continue;
            }

            /*
            TCanvas* cAmpl = new TCanvas("cAmpl", "cAmpl");
            TH1I* hist = event->CreateEventHistogram();
            hist->Draw();
            cAmpl->WaitPrimitive();
            delete hist;
            delete cAmpl;
            */

            // Pass the event to any subscribers
            fEventSignal(event);

            iEvent++;

            delete event;
        }

        fEggDoneSignal();

        return true;
    }

} /* namespace Katydid */
