/*
 * KTBasicROOTTypeWriterEgg.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterEgg.hh"

#include "KTEvent.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include "TH1.h"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedTIRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterEgg > sBRTWERegistrar;

    KTBasicROOTTypeWriterEgg::KTBasicROOTTypeWriterEgg() :
            KTBasicROOTTypeWriter()
            //KTTypeWriterEgg()
    {
    }

    KTBasicROOTTypeWriterEgg::~KTBasicROOTTypeWriterEgg()
    {
    }


    void KTBasicROOTTypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("ts-data", this, &KTBasicROOTTypeWriterEgg::WriteTimeSeriesData, "void (const KTTimeSeriesData*)");
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTBasicROOTTypeWriterEgg::WriteTimeSeriesData(const KTTimeSeriesData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nChannels = data->GetNTimeSeries();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTTimeSeries* spectrum = data->GetTimeSeries(iChannel);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTS_" << eventNumber << "_" << iChannel;
                string histName;
                conv >> histName;
                TH1D* powerSpectrum = spectrum->CreateHistogram(histName);
                powerSpectrum->SetDirectory(fWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

} /* namespace Katydid */
