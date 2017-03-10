/*
 * KTHDF5TypeWriterTime.cc
 *
 *  Created on: Sept 9, 2014
 *      Author: nsoblath
 */

#include <cstring>
#include <sstream>

#include "KTEggHeader.hh"
#include "KTHDF5TypeWriterTime.hh"
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
    KTLOGGER(publog, "KTHDF5TypeWriterTime");

    static Nymph::KTTIRegistrar< KTHDF5TypeWriter, 
                          KTHDF5TypeWriterTime > sHDF5TWEggRegistrar;

    KTHDF5TypeWriterTime::KTHDF5TypeWriterTime() :
            KTHDF5TypeWriter(),
            fRawTSliceDSpace(NULL),
            fRealTSliceDSpace(NULL),
            fRawDataGroup(NULL),
            fRealDataGroup(NULL),
            fRealTimeBuffer(NULL),
            fRawTimeBuffer(NULL)
     {}

    KTHDF5TypeWriterTime::~KTHDF5TypeWriterTime()
    {
            delete [] fRawTimeBuffer;
            delete [] fRealTimeBuffer;
    }


    void KTHDF5TypeWriterTime::RegisterSlots()
    {
        fWriter->RegisterSlot("ts-raw", this, &KTHDF5TypeWriterTime::WriteRawTimeSeriesData);
        fWriter->RegisterSlot("ts-real", this, &KTHDF5TypeWriterTime::WriteRealTimeSeriesData);
        return;
    }


    H5::DataSet* KTHDF5TypeWriterTime::CreateRawTSDSet(const std::string& name)
    {
        H5::Group* grp = fRawDataGroup;
        H5::DSetCreatPropList plist;
        unsigned default_value = 0;
        plist.setFillValue(H5::PredType::NATIVE_UINT, &default_value);
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(), H5::PredType::NATIVE_INT, *fRawTSliceDSpace, plist));
        return dset;
    }

    H5::DataSet* KTHDF5TypeWriterTime::CreateRealTSDSet(const std::string& name)
    {
        H5::Group* grp = fRealDataGroup;
        H5::DSetCreatPropList plist;
        double default_value = 0.0;
        plist.setFillValue(H5::PredType::NATIVE_DOUBLE, &default_value);
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(), H5::PredType::NATIVE_DOUBLE, *fRealTSliceDSpace, plist));
        return dset;
    }

    // *****************
    // Raw Time Series Data
    // *****************

    void KTHDF5TypeWriterTime::WriteRawTimeSeriesData(Nymph::KTDataPtr data)
    {
        KTSliceHeader& header = data->Of< KTSliceHeader >();
        KTRawTimeSeriesData& tsData = data->Of< KTRawTimeSeriesData >();

        unsigned nComponents = tsData.GetNComponents();
        unsigned sliceSize = header.GetRawSliceSize();

        if (fRawTSliceDSpace == NULL)
        {
            hsize_t dims[] = {nComponents, sliceSize};
            fRawTSliceDSpace = new H5::DataSpace(2, dims);
            fRawDataGroup = fWriter->AddGroup("/raw_ts");
            fRawTimeBuffer = new unsigned(header.GetRawSliceSize());
        }

        uint64_t sliceNumber = header.GetSliceNumber();
        std::stringstream ss;
        ss << "slice_" << sliceNumber;
        std::string slice_name;
        ss >> slice_name;      
        H5::DataSet* dset = CreateRawTSDSet(slice_name);

        if ( fWriter->OpenAndVerifyFile() )
        {
            for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
            {
                const KTRawTimeSeries* spectrum = tsData.GetTimeSeries(iComponent);
                if (spectrum != NULL)
                {
                    for (int i = 0; i < sliceSize; ++i)
                    {
                        // TODO(kofron): wat
                        fRawTimeBuffer[i] = spectrum[0](i);
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

    void KTHDF5TypeWriterTime::WriteRealTimeSeriesData(Nymph::KTDataPtr data)
    {
        KTSliceHeader& header = data->Of<KTSliceHeader>();
        KTTimeSeriesData& tsData = data->Of<KTTimeSeriesData>();

        unsigned nComponents = tsData.GetNComponents();
        unsigned sliceSize = header.GetSliceSize();

        if (fRealTSliceDSpace == NULL)
        {
            hsize_t dims[] = {nComponents, sliceSize};
            fRealTSliceDSpace = new H5::DataSpace(2, dims);
            fRealDataGroup = fWriter->AddGroup("/real_ts");
            fRealTimeBuffer = new double(header.GetSliceSize());
        }

        uint64_t sliceNumber = header.GetSliceNumber();
        std::stringstream ss;
        ss << "slice_" << sliceNumber;
        std::string slice_name;
        ss >> slice_name;
        H5::DataSet* dset = CreateRealTSDSet(slice_name);

        if ( !fWriter->OpenAndVerifyFile()) return;

        for (unsigned iC=0; iC<nComponents; ++iC)
        {
            const KTTimeSeries* spectrum = static_cast<KTTimeSeriesReal*>(tsData.GetTimeSeries(iC));
            if (spectrum != NULL) {
                for (int i = 0; i < sliceSize; ++i)
                {
                    // TODO(kofron): wat
                    fRealTimeBuffer[i] = spectrum->GetValue(i);
                }
                dset->write(fRealTimeBuffer, H5::PredType::NATIVE_DOUBLE);
            }
        }
        return;
    }

} /* namespace Katydid */
