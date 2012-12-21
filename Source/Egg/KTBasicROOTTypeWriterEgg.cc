/*
 * KTBasicROOTTypeWriterEgg.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterEgg.hh"

#include "KTEvent.hh"
#include "KTFactory.hh"
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

    static KTDerivedRegistrar< KTBasicROOTTypeWriter, KTBasicROOTTypeWriterEgg > sBRTWERegistrar("basic-root-type-writer-egg");

    KTBasicROOTTypeWriterEgg::KTBasicROOTTypeWriterEgg() :
            KTBasicROOTTypeWriter()
    {
    }

    KTBasicROOTTypeWriterEgg::~KTBasicROOTTypeWriterEgg()
    {
    }
    /*
    Bool_t KTBasicROOTFileWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileFlag(node->GetData<string>("file-flag", fFileFlag));
        }

        // Command-line settings
        //SetFilename(fCLHandler->GetCommandLineValue< string >("broot-output-file", fTransformFlag));

        return true;
    }
    */


    //*****************
    // Time Series Data
    //*****************

    void KTBasicROOTTypeWriterEgg::Write(const KTTimeSeriesData* data)
    {
        KTEvent* event = data->GetEvent();
        UInt_t eventNumber = 0;
        if (event != NULL) eventNumber = event->GetEventNumber();
        UInt_t nChannels = data->GetNTimeSeries();

        if (! fFileWriter->OpenAndVerifyFile()) return;

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
                powerSpectrum->SetDirectory(fFileWriter->GetFile());
                powerSpectrum->Write();
                KTDEBUG(publog, "Histogram <" << histName << "> written to ROOT file");
            }
        }
        return;
    }

} /* namespace Katydid */
