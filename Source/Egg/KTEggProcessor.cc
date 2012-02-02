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

//#include "TCanvas.h"
//#include "TH1.h"

#include <iostream>

using std::string;
using std::cout;
using std::endl;

namespace Katydid
{

    KTEggProcessor::KTEggProcessor() :
            fNEvents(0),
            fHeaderSignal(),
            fEventSignal()
    {
    }

    KTEggProcessor::~KTEggProcessor()
    {
    }

    Bool_t KTEggProcessor::ApplySetting(const KTSetting* setting)
    {
        if (setting->GetName() == "NEvents")
        {
            this->SetNEvents(setting->GetValue< UInt_t >());
            return kTRUE;
        }
        return kFALSE;
    }

    Bool_t KTEggProcessor::ProcessEgg(const string& fileName)
    {
        KTEgg egg;
        egg.SetFileName(fileName);
        if (! egg.BreakEgg())
        {
            cout << "Error: Egg did not break" << endl;
            return false;
        }
        if (! egg.ParseEggHeader())
        {
            cout << "Error: Header did not parse" << endl;
            return false;
        }

        fHeaderSignal(egg.GetHeaderInfo());

        UInt_t iEvent = 0;
        while (kTRUE)
        {
            if (iEvent >= fNEvents) break;

            cout << "Event " << iEvent << endl;

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

        return true;
    }

} /* namespace Katydid */
