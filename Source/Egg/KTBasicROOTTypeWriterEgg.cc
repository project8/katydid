/*
 * KTBasicROOTTypeWriterEgg.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTTypeWriterEgg.hh"

#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include "TH1.h"

#include <sstream>

using boost::shared_ptr;

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
        fWriter->RegisterSlot("ts", this, &KTBasicROOTTypeWriterEgg::WriteTimeSeriesData, "void (shared_ptr<KTData>)");
        return;
    }


    //*****************
    // Time Series Data
    //*****************

    void KTBasicROOTTypeWriterEgg::WriteTimeSeriesData(boost::shared_ptr<KTData> data)
    {
        if (! data) return;

        ULong64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        UInt_t nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                stringstream conv;
                conv << "histTS_" << sliceNumber << "_" << iComponent;
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
