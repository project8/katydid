/*
 * KTHDF5TypeWriterEgg.cc
 *
 *  Created on: Sept 9, 2014
 *      Author: nsoblath
 */

#include "KTHDF5TypeWriterEgg.hh"

#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

#include <cstring>
#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTHDF5TypeWriterEgg");

    static KTTIRegistrar< KTROOTTreeTypeWriter, KTHDF5TypeWriterEgg > sHDF5TWEggRegistrar;

    KTHDF5TypeWriterEgg::KTHDF5TypeWriterEgg() :
            KTROOTTreeTypeWriter()
    {
    }

    KTHDF5TypeWriterEgg::~KTHDF5TypeWriterEgg()
    {
    }


    void KTHDF5TypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("header", this, &KTHDF5TypeWriterEgg::WriteEggHeader);
        return;
    }


    //*********************
    // Egg Header
    //*********************

    void KTHDF5TypeWriterEgg::WriteEggHeader(KTEggHeader* header)
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        // any other checks needed?

        // fill in header info


        return;
    }

    //*****************
    // Raw Time Series Data
    //*****************

    void KTBasicROOTTypeWriterEgg::WriteRawTimeSeriesData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTRawTimeSeriesData& tsData = data->Of<KTRawTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTRawTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                // write time series here
            }
        }
        return;
    }

    //*****************
    // Time Series Data
    //*****************

    void KTBasicROOTTypeWriterEgg::WriteTimeSeriesData(KTDataPtr data)
    {
        if (! data) return;

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                // write time series here
            }
        }
        return;
    }

} /* namespace Katydid */
