/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details KTEggProcessor iterates over the events in an Egg file
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTEggProcessor.hh"

#include "KTEgg.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

//#include "TCanvas.h"
//#include "TH1.h"

using std::string;

namespace Katydid
{
    KTLOGGER(egglog, "katydid.egg");

    KTEggProcessor::KTEggProcessor() :
            fNEvents(0),
            fHeaderSignal(),
            fEventSignal()
    {
    }

    KTEggProcessor::~KTEggProcessor()
    {
    }

    Bool_t KTEggProcessor::Configure(const KTPStoreNode* node)
    {
        SetNEvents(node->GetData< UInt_t >("number_of_events", 0));
        SetFilename(node->GetData< string >("filename", ""));
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
        egg.SetFileName(fFilename);
        if (! egg.BreakEgg())
        {
            KTERROR(egglog, "Egg did not break");
            return false;
        }
        if (! egg.ParseEggHeader())
        {
            KTERROR(egglog, "Header did not parse");
            return false;
        }

        fHeaderSignal(egg.GetHeaderInfo());

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
