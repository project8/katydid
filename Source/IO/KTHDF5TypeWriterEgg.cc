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

    static KTTIRegistrar< KTHDF5TypeWriter, KTHDF5TypeWriterEgg > sHDF5TWEggRegistrar;

    KTHDF5TypeWriterEgg::KTHDF5TypeWriterEgg() :
            KTHDF5TypeWriter()
    {
    }

    KTHDF5TypeWriterEgg::~KTHDF5TypeWriterEgg()
    {
    }


    void KTHDF5TypeWriterEgg::RegisterSlots()
    {
        fWriter->RegisterSlot("setup_from_header", this, &KTHDF5TypeWriterEgg::ProcessEggHeader);
        fWriter->RegisterSlot("raw_ts", this, &KTHDF5TypeWriterEgg::WriteRawTimeSeriesData);
        return;
    }


    // *********************
    // Egg Header
    // *********************

    void KTHDF5TypeWriterEgg::ProcessEggHeader(KTEggHeader* header)
    {   
        // TODO(kofron): this should probably belong to the writer.
        // TODO(kofron): storage size should be set here too, not just 8 bit.
        /*
            Inform the writer about the slice size so it can set up
            its buffers and so on.
        */
        if (!fWriter->OpenAndVerifyFile()) return;
        if(header != NULL) {
            fWriter->SetComponents(header->GetNChannels());
            fWriter->SetRawSliceSize(header->GetRawSliceSize());
            fWriter->SetSliceSize(header->GetSliceSize());  

            fWriter->CreateDataspaces();
            fWriter->AddGroup("/raw_data");
        }
    }

    //*****************
    // Raw Time Series Data
    //*****************

    void KTHDF5TypeWriterEgg::WriteRawTimeSeriesData(KTDataPtr data)
    {
        if (! data) return;

        // A buffer to work in.
        unsigned buffer[fWriter->GetRawSliceSize()];

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        std::stringstream ss;
        ss << "/raw_data/" << "slice_" << sliceNumber;
        std::string slice_name;
        ss >> slice_name;        
        H5::DataSet* dset = fWriter->CreateRawTimeSeriesDataSet(slice_name);

        KTRawTimeSeriesData& tsData = data->Of<KTRawTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
           
            const KTRawTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                std::cout << spectrum[0](0) << std::endl;
                for(int i=0; i < fWriter->GetRawSliceSize(); i++) {
                    // TODO(kofron): wat
                    buffer[i] = spectrum[0](i);
                }
                dset->write(buffer, H5::PredType::NATIVE_UINT);
            }
        }
        return;
    }

    //*****************
    // Time Series Data
    //*****************

    void KTHDF5TypeWriterEgg::WriteTimeSeriesData(KTDataPtr data)
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
