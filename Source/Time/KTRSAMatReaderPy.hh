/*
 * KTRSAMatReaderPy.hh
 *
 *  Created on: April 10, 2017
 *      Author: laroque
 */

#ifndef KTRSAMATREADERPY_HH_
#define KTRSAMATREADERPY_HH_

#include "KTRSAMatReader.hh"

void export_KTRSAMatReaderPy()
{
    using namespace Katydid;
    using namespace boost::python;
    class_<Katydid::KTRSAMatReader, boost::noncopyable>("KTRSAMatReader",init<>())
        // use .add_property for getters and setters
        .add_property("SliceSize", &KTRSAMatReader::GetSliceSize, &KTRSAMatReader::SetSliceSize)
        .add_property("Stride", &KTRSAMatReader::GetStride, &KTRSAMatReader::SetStride)

        // public methods
        .def("BreakAnEgg", &KTRSAMatReader::BreakAnEgg, "Open an egg file from a path")
        .def("HatchNextSlice", &KTRSAMatReader::HatchNextSlice, "Retrieves the next time slice")
        .def("CloseEgg", &KTRSAMatReader::CloseEgg, "close the file")

        .def("GetMaxChannels", &KTRSAMatReader::GetMaxChannels, "Get the number of channels")
        .def("GetSampleRateUnitsInHz", &KTRSAMatReader::GetSampleRateUnitsInHz, "return the number of samples per second")
        //TODO do these exist?
        //.def("GetFullVoltageScale", &KTRSAMatReader::GetFullVoltageScale, "return dynamic range in volts")
        //.def("GetNADCLevels", &KTRSAMatReader::GetNADCLevels, "return the digitizer bit depth")
        .def("GetRecordSize", &KTRSAMatReader::GetRecordSize, "Get the number of samples per record in the egg file")
        //Come back and fill in these doc strings TODO!!!
        .def("GetBinWidth", &KTRSAMatReader::GetBinWidth)
        .def("GetTimeInRun", &KTRSAMatReader::GetTimeInRun)
        .def("GetIntegratedTime", &KTRSAMatReader::GetIntegratedTime)
        .def("GetTimeInAcq", &KTRSAMatReader::GetTimeInAcq)
        .def("GetNSlicesProcessed", &KTRSAMatReader::GetNSlicesProcessed)
        .def("GetNRecordsProcessed", &KTRSAMatReader::GetNRecordsProcessed)
        //TODO the following requires wrapping MonarchReadState
        //.def("GetReadState", &KTRSAMatReader::GetReadState)

        // This is named GetTimeInAcq in this class
        //.def("GetAcqTimeInRun", &KTRSAMatReader::GetAcqTimeInRun)
        ;
}

#endif /* KTRSAMatREADERPY_HH_ */
