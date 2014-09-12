/*
 * KTHDF5TypeWriterEgg.cc
 *
 *  Created on: Sept 9, 2014
 *      Author: nsoblath
 */

#include <cstring>
#include <sstream>

#include "KTHDF5TypeWriterEgg.hh"
#include "KTEggHeader.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"
#include "KTRawTimeSeries.hh"
#include "KTRawTimeSeriesData.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesData.hh"

using std::stringstream;
using std::string;

namespace Katydid {
    KTLOGGER(publog, "KTHDF5TypeWriterEgg");

    static KTTIRegistrar< KTHDF5TypeWriter, 
                          KTHDF5TypeWriterEgg > sHDF5TWEggRegistrar;

    KTHDF5TypeWriterEgg::KTHDF5TypeWriterEgg() :
            KTHDF5TypeWriter(),
            raw_time_slice_dspace(NULL),
            time_slice_dspace(NULL),
            slice_size(0),
            raw_slice_size(0) {}

    KTHDF5TypeWriterEgg::~KTHDF5TypeWriterEgg() {
    }


    void KTHDF5TypeWriterEgg::RegisterSlots() {
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
            this->n_components = (header->GetNChannels());
            this->raw_slice_size = (header->GetRawSliceSize());
            this->slice_size = (header->GetSliceSize());  

            this->CreateDataspaces();
            this->working_group = fWriter->AddGroup("/raw_data");
        }
    }


    void KTHDF5TypeWriterEgg::CreateDataspaces() {
        /*
        If the dataspaces have already been created, this is a no-op.  
        Otherwise, we want to create two dataspaces - 1XM and 1XN, where
        M is the size of a raw time slice, and N is the size of a time slice.
        */
        if(this->raw_time_slice_dspace == NULL 
            || this->time_slice_dspace == NULL) {
            hsize_t raw_dims[] = {this->n_components, this->raw_slice_size};
            hsize_t dims[] = {this->n_components, this->slice_size};

            this->raw_time_slice_dspace = new H5::DataSpace(2, raw_dims);
            this->time_slice_dspace = new H5::DataSpace(2, dims);
        }
    }

    H5::DataSet* KTHDF5TypeWriterEgg::CreateRawTSDSet(const std::string& name) {
        H5::Group* grp = this->working_group;
        H5::DSetCreatPropList plist;
        plist.setFillValue(H5::PredType::NATIVE_INT, 0);
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(),
                                                               H5::PredType::NATIVE_INT,
                                                               *(this->raw_time_slice_dspace),
                                                               plist));
        return dset;
    }

    //*****************
    // Raw Time Series Data
    //*****************

    void KTHDF5TypeWriterEgg::WriteRawTimeSeriesData(KTDataPtr data)
    {
        if (! data) return;

        // A buffer to work in.
        unsigned buffer[this->raw_slice_size];

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        std::stringstream ss;
        ss << "slice_" << sliceNumber;
        std::string slice_name;
        ss >> slice_name;        
        H5::DataSet* dset = this->CreateRawTSDSet(slice_name);

        KTRawTimeSeriesData& tsData = data->Of<KTRawTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
           
            const KTRawTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
            if (spectrum != NULL)
            {
                std::cout << spectrum[0](0) << std::endl;
                for(int i=0; i < this->raw_slice_size; i++) {
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
