/*
 * KTEvent.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEvent.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <fstream>
#include <iostream>

using std::ifstream;
using std::string;
using std::vector;

namespace Katydid
{

    KTEvent::KTEvent() :
                fSampleRate(0.),
                fRecordLength(0.),
                fBinWidth(1.),
                fTimeStamp(),
                fFrameID(),
                fRecord()
    {
    }

    KTEvent::~KTEvent()
    {
    }

#ifdef ROOT_FOUND
    TH1I* KTEvent::CreateEventHistogram() const
    {
        TH1I* hist = new TH1I("hRecord", "Event Record", (int)GetRecordSize(), -0.5*fBinWidth, GetRecordLength() + fBinWidth*0.5);
        for (unsigned int iBin=0; iBin<fRecord.size(); iBin++)
        {
            hist->SetBinContent(iBin+1, fRecord[iBin]);
        }
        hist->SetXTitle("Time (s)");
        return hist;
    }

    TH1I* KTEvent::CreateAmplitudeDistributionHistogram() const
    {
        TH1I* hist = new TH1I("hRecordAmpl", "Event Record Amplitude Distribution", 256, -0.5, 255.5);
        for (int iBin=0; iBin<fRecord.size(); iBin++)
        {
            hist->Fill((double)(fRecord[iBin]));
        }
        hist->SetXTitle("ADC Bin");
        return hist;
    }
#endif

} /* namespace Katydid */
