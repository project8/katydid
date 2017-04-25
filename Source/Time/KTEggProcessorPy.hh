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

/*
boost::python::list GetFilenames_wrapper()
{
    boost::python::list result;
    std::vector<boost::filesystem::path, std::allocator<boost::filesystem::path>> v = Katydid::KTEggProcessor::GetFilenames();
    std::vector<boost::filesystem::path, std::allocator<boost::filesystem::path>>::iterator it;
    for (it = v.begin(); it != v.end(); ++it){
        result.append(*it);
    }
    return result;
}
*/

//typedef std::vector<boost::filesystem::path, std::allocator<boost::filesystem::path> > PyListType;

void export_KTEggProcessorPy()
{
    using namespace Katydid;
    using namespace boost::python;

    //class_<PyListType>("FilenameList").def(vector_indexing_suite<PyListType>() );

    class_< Katydid::KTEggProcessor, boost::noncopyable, bases<Nymph::KTProcessor> >("KTEggProcessor")
        .add_property("NSlices", &KTEggProcessor::GetNSlices, &KTEggProcessor::SetNSlices)
        .add_property("ProgressReportInterval", &KTEggProcessor::GetProgressReportInterval, &KTEggProcessor::SetProgressReportInterval)

        //TODO The Filenames type is an std::vector which i don't have a way to deal with yet
        .add_property("Filenames", make_function( &KTEggProcessor::GetFilenames, return_value_policy<copy_const_reference>()), &KTEggProcessor::SetFilenames)

        .add_property("EggReaderType", make_function( &KTEggProcessor::GetEggReaderType, return_value_policy<copy_const_reference>()), &KTEggProcessor::SetEggReaderType)

        .add_property("SliceSize", &KTEggProcessor::GetSliceSize, &KTEggProcessor::SetSliceSize)
        .add_property("Stride", &KTEggProcessor::GetStride, &KTEggProcessor::SetStride)
        .add_property("StartTime", &KTEggProcessor::GetStartTime, &KTEggProcessor::SetStartTime)

        .add_property("NormalizeVoltages", &KTEggProcessor::GetNormalizeVoltages, &KTEggProcessor::SetNormalizeVoltages)
        ;
}

#endif /* KTEGGPROCESSORPY_HH_ */
