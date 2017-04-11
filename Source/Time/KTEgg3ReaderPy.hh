/*
 * KTEgg3ReaderPy.hh
 *
 *  Created on: April 07, 2017
 *      Author: laroque
 */

#ifndef KTEGG3READERPY_HH_
#define KTEGG3READERPY_HH_

//#include <boost/python/return_value_policy.hpp>
//#include <boost/python/copy_const_reference.hpp>

#include "KTEgg3Reader.hh"

void export_KTEgg3ReaderPy()
{
    using namespace Katydid;
    using namespace boost::python;
    class_<Katydid::KTEgg3Reader, boost::noncopyable>("KTEgg3Reader",init<>())
        // use .add_property for getters and setters
        .add_property("SliceSize", &KTEgg3Reader::GetSliceSize, &KTEgg3Reader::SetSliceSize)
        .add_property("Stride", &KTEgg3Reader::GetStride, &KTEgg3Reader::SetStride)

        // public methods
        .def("BreakAnEgg", &KTEgg3Reader::BreakAnEgg, "Open an egg file from a path")
        .def("HatchNextSlice", &KTEgg3Reader::HatchNextSlice, "Retrieves the next time slice")
        .def("CloseEgg", &KTEgg3Reader::CloseEgg, "close the file")
        //TODO does the following exist?
        //.def("GetMaxChannels", &KTEgg3Reader::GetMaxChannels, "Get the number of channels")
        .def("GetSampleRateUnitsInHz", &KTEgg3Reader::GetSampleRateUnitsInHz, "return the number of samples per second")
        //TODO do these exist?
        //.def("GetFullVoltageScale", &KTEgg3Reader::GetFullVoltageScale, "return dynamic range in volts")
        //.def("GetNADCLevels", &KTEgg3Reader::GetNADCLevels, "return the digitizer bit depth")
        .def("GetRecordSize", &KTEgg3Reader::GetRecordSize, "Get the number of samples per record in the egg file")
        //Come back and fill in these doc strings TODO!!!
        .def("GetBinWidth", &KTEgg3Reader::GetBinWidth)
        .def("GetTimeInRun", &KTEgg3Reader::GetTimeInRun)
        .def("GetIntegratedTime", &KTEgg3Reader::GetIntegratedTime)
        .def("GetNSlicesProcessed", &KTEgg3Reader::GetNSlicesProcessed)
        .def("GetNRecordsProcessed", &KTEgg3Reader::GetNRecordsProcessed)
        //TODO the following requires wrapping MonarchReadState
        //.def("GetReadState", &KTEgg3Reader::GetReadState)
        .def("GetAcqTimeInRun", &KTEgg3Reader::GetAcqTimeInRun)
        ;
}

#endif /* KTEGG3READERPY_HH_ */
