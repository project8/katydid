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

using std::vector;

namespace Katydid
{

    KTEvent::KTEvent(unsigned nChannels) :
                fSampleRate(0.),
                fRecordLength(0.),
                fBinWidth(1.),
                fRecordSize(0),
                fChannelData(nChannels)
    {
    }

    KTEvent::~KTEvent()
    {
        while (! fChannelData.empty())
        {
            delete fChannelData.back().fRecord;
            fChannelData.pop_back();
        }
    }

#ifdef ROOT_FOUND
    TH1C* KTEvent::CreateEventHistogram(unsigned channelNum) const
    {
        TH1C* hist = new TH1C("hRecord", "Event Record", (int)GetRecordSize(), -0.5*fBinWidth, GetRecordLength() + fBinWidth*0.5);
        for (unsigned int iBin=0; iBin<fChannelData[channelNum].fRecord->size(); iBin++)
        {
            hist->SetBinContent(iBin+1, fChannelData[channelNum].fRecord->at(iBin));
        }
        hist->SetXTitle("Time (s)");
        return hist;
    }

    TH1I* KTEvent::CreateAmplitudeDistributionHistogram(unsigned channelNum) const
    {
        TH1I* hist = new TH1I("hRecordAmpl", "Event Record Amplitude Distribution", 256, -0.5, 255.5);
        for (int iBin=0; iBin<fChannelData[channelNum].fRecord->size(); iBin++)
        {
            hist->Fill((double)(fChannelData[channelNum].fRecord->at(iBin)));
        }
        hist->SetXTitle("ADC Bin");
        return hist;
    }
#endif

} /* namespace Katydid */
