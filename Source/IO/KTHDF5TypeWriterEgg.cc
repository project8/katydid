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
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesData.hh"

using std::stringstream;
using std::string;

namespace Katydid {
    KTLOGGER(publog, "KTHDF5TypeWriterEgg");

    static KTTIRegistrar< KTHDF5TypeWriter, 
                          KTHDF5TypeWriterEgg > sHDF5TWEggRegistrar;

    KTHDF5TypeWriterEgg::KTHDF5TypeWriterEgg() :
            KTHDF5TypeWriter(),
            fRawTSliceDSpace(NULL),
            fRealTSliceDSpace(NULL),
            fSliceSize(0),
            fRawSliceSize(0),
            fRealTimeBuffer(NULL),
            fRawTimeBuffer(NULL),
            fHeaderProcessed(false)
             {}

    KTHDF5TypeWriterEgg::~KTHDF5TypeWriterEgg() {
            if(fRawTimeBuffer) delete fRawTimeBuffer;
            if(fRealTimeBuffer) delete fRealTimeBuffer;
    }


    void KTHDF5TypeWriterEgg::RegisterSlots() {
        fWriter->RegisterSlot("egg-from-header", 
                              this, 
                              &KTHDF5TypeWriterEgg::ProcessEggHeader);
        fWriter->RegisterSlot("ts-raw", 
                              this, 
                              &KTHDF5TypeWriterEgg::WriteRawTimeSeriesData);
        fWriter->RegisterSlot("ts-real", 
                              this, 
                              &KTHDF5TypeWriterEgg::WriteRealTimeSeriesData);
        return;
    }


    // *********************
    // Egg Header
    // *********************

    void KTHDF5TypeWriterEgg::ProcessEggHeader() {   
        if ( fWriter->OpenAndVerifyFile() && (fHeaderProcessed == false)) {
            KTEggHeader* header = fWriter->GetHeader();
            this->fNComponents = (header->GetNChannels());
            this->fRawSliceSize = (header->GetChannelHeader(0)->GetRawSliceSize());
            this->fSliceSize = (header->GetChannelHeader(0)->GetSliceSize());

            this->fRealTimeBuffer = new double[fSliceSize];
            this->fRawTimeBuffer = new unsigned[fRawSliceSize];

            this->CreateDataspaces();
            this->fRawDataGroup = fWriter->AddGroup("/raw_data");
            this->fRealDataGroup = fWriter->AddGroup("/real_data"); 
        } // if we haven't already processed the header
    }


    void KTHDF5TypeWriterEgg::CreateDataspaces() {
        /*
        If the dataspaces have already been created, this is a no-op.  
        Otherwise, we want to create two dataspaces - 1XM and 1XN, where
        M is the size of a raw time slice, and N is the size of a time slice.
        */
        if(this->fRawTSliceDSpace == NULL
            || this->fRealTSliceDSpace == NULL) {
            hsize_t raw_dims[] = {this->fNComponents, this->fRawSliceSize};
            hsize_t dims[] = {this->fNComponents, this->fSliceSize};

            this->fRawTSliceDSpace = new H5::DataSpace(2, raw_dims);
            this->fRealTSliceDSpace = new H5::DataSpace(2, dims);
        }
    }

    H5::DataSet* KTHDF5TypeWriterEgg::CreateRawTSDSet(const std::string& name) {
        H5::Group* grp = this->fRawDataGroup;
        H5::DSetCreatPropList plist;
        unsigned default_value = 0;
        plist.setFillValue(H5::PredType::NATIVE_UINT, &default_value);
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(),
                                                               H5::PredType::NATIVE_INT,
                                                               *(this->fRawTSliceDSpace),
                                                               plist));
        return dset;
    }

    H5::DataSet* KTHDF5TypeWriterEgg::CreateRealTSDSet(const std::string& name) {
        H5::Group* grp = this->fRealDataGroup;
        H5::DSetCreatPropList plist;
        double default_value = 0.0;
        plist.setFillValue(H5::PredType::NATIVE_DOUBLE, &default_value);
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(),
                                                               H5::PredType::NATIVE_DOUBLE,
                                                               *(this->fRealTSliceDSpace),
                                                               plist));
        return dset;
    }

    // *****************
    // Raw Time Series Data
    // *****************

    void KTHDF5TypeWriterEgg::WriteRawTimeSeriesData(KTDataPtr data) {
        if ( !data ) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }


        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        std::stringstream ss;
        ss << "slice_" << sliceNumber;
        std::string slice_name;
        ss >> slice_name;      
        H5::DataSet* dset = this->CreateRawTSDSet(slice_name);

        KTRawTimeSeriesData& tsData = data->Of<KTRawTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if ( fWriter->OpenAndVerifyFile() ) {
            for (unsigned iComponent=0; iComponent<nComponents; ++iComponent) {
                const KTRawTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
                if (spectrum != NULL) {
                    for (int i = 0; i < this->fRawSliceSize; i++) {
                        // TODO(kofron): wat
                        this->fRawTimeBuffer[i] = spectrum[0](i);
                    }
                    dset->write(fRawTimeBuffer, H5::PredType::NATIVE_UINT);
                } // if spectrum is not NULL
            } // loop over components   
        } // if fWriter is in a sane state
        return;
    }

    // *****************
    // Time Series Data
    // *****************

    void KTHDF5TypeWriterEgg::WriteRealTimeSeriesData(KTDataPtr data) {
        if (!data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        uint64_t sliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        std::stringstream ss;
        ss << "slice_" << sliceNumber;
        std::string slice_name;
        ss >> slice_name;
        H5::DataSet* dset = this->CreateRealTSDSet(slice_name);

        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();
        unsigned nComponents = tsData.GetNComponents();

        if ( !fWriter->OpenAndVerifyFile()) return;

        for (unsigned iC=0; iC<nComponents; ++iC) {
            const KTTimeSeries* spectrum = 
                static_cast<KTTimeSeriesReal*>(tsData.GetTimeSeries(iC));
            if (spectrum != NULL) {
                for (int i = 0; i < this->fRawSliceSize; i++) {
                    // TODO(kofron): wat
                    this->fRealTimeBuffer[i] = spectrum->GetValue(i);
                }
                dset->write(this->fRealTimeBuffer, 
                            H5::PredType::NATIVE_DOUBLE);
            }
        }
        return;
    }

} /* namespace Katydid */
