/**
 @file KTEggProcessorPy.hh
 @brief Contains python wrapper of KTEggProcessor
 @details Iterates over slices in an Egg file.
 @author: B. H. LaRoque
 @date: April 10, 2017
 */

#ifndef KTEGGPROCESSORPY_HH_
#define KTEGGPROCESSORPY_HH_

#include "KTEggProcessor.hh"


void export_KTEggProcessorPy()
{
    using namespace Katydid;
    using namespace boost::python;
    class_<Katydid::KTEggProcessor, boost::noncopyable>("KTEggProcessor")
        .add_property("NSlices", &KTEggProcessor::GetNSlices, &KTEggProcessor::SetNSlices)
        .add_property("ProgressReportInterval", &KTEggProcessor::GetProgressReportInterval, &KTEggProcessor::SetProgressReportInterval)

        //TODO build error casting the arg/return types
        //.add_property("Filenames", &KTEggProcessor::GetFilenames, &KTEggProcessor::SetFilenames)
        //.add_property("EggReaderType", &KTEggProcessor::GetEggReaderType, &KTEggProcessor::SetEggReaderType)

        .add_property("SliceSize", &KTEggProcessor::GetSliceSize, &KTEggProcessor::SetSliceSize)
        .add_property("Stride", &KTEggProcessor::GetStride, &KTEggProcessor::SetStride)
        .add_property("StartTime", &KTEggProcessor::GetStartTime, &KTEggProcessor::SetStartTime)

        .add_property("NormalizeVoltages", &KTEggProcessor::GetNormalizeVoltages, &KTEggProcessor::SetNormalizeVoltages)
        ;
}

#endif /* KTEGGPROCESSORPY_HH_ */
