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
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

//#include "TCanvas.h"
//#include "TH1.h"

using std::string;

namespace Katydid
{
    static KTCommandLineOption< int > sNEventsCLO("Egg Processor", "Number of events to process", "n-events", 'n');
    static KTCommandLineOption< string > sFilenameCLO("Egg Processor", "Egg filename to open", "egg-file", 'e');
    static KTCommandLineOption< bool > sOldReaderCLO("Egg Processor", "Use the old egg reader for 2011 data", "use-old-egg-reader", 'z');

    KTLOGGER(egglog, "katydid.egg");

    KTEggProcessor::KTEggProcessor() :
            fNEvents(0),
            fFilename(""),
            fEggReaderType(kMonarchEggReader),
            fHeaderSignal(),
            fEventSignal(),
            fEggDoneSignal()
    {
        RegisterSignal("header", &fHeaderSignal);
        RegisterSignal("event", &fEventSignal);
        RegisterSignal("egg_done", &fEggDoneSignal);
    }

    KTEggProcessor::~KTEggProcessor()
    {
    }

    Bool_t KTEggProcessor::Configure(const KTPStoreNode* node)
    {
        SetNEvents(node->GetData< UInt_t >("number_of_events", 0));
        SetFilename(node->GetData< string >("filename", ""));

        // egg reader
        string eggReaderTypeString = node->GetData< string >("egg_reader", "monarch");
        if (eggReaderTypeString == "monarch") SetEggReaderType(kMonarchEggReader);
        else if (eggReaderTypeString == "2011") SetEggReaderType(k2011EggReader);
        else return false;

        return true;
    }


    Bool_t KTEggProcessor::ApplySetting(const KTSetting* setting)
    {
        if (setting->GetName() == "NEvents")
        {
            this->SetNEvents(setting->GetValue< UInt_t >());
            return kTRUE;
        }
        if (setting->GetName() == "Filename")
        {
            this->SetFilename(setting->GetValue< string >());
            return kTRUE;
        }
        return kFALSE;
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

        UInt_t iEvent = 0;
        while (kTRUE)
        {
            if (iEvent >= fNEvents) break;

            KTINFO(egglog, "Event " << iEvent);

            // Hatch the event
            KTEvent* event = egg.HatchNextEvent();
            if (event == NULL) break;

            /*
            TCanvas* cAmpl = new TCanvas("cAmpl", "cAmpl");
            TH1I* hist = event->CreateEventHistogram();
            hist->Draw();
            cAmpl->WaitPrimitive();
            delete hist;
            delete cAmpl;
            */

            // Pass the event to any subscribers
            fEventSignal(iEvent, event);

            iEvent++;

            delete event;
        }

        fEggDoneSignal();

        return true;
    }

} /* namespace Katydid */
