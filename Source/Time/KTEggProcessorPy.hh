/**
 @file KTEggProcessorPy.hh
 @brief Contains python wrapper of KTEggProcessor
 @details Iterates over slices in an Egg file.
 @author: B. H. LaRoque
 @date: April 10, 2017
 */

#ifndef KTEGGPROCESSORPY_HH_
#define KTEGGPROCESSORPY_HH_

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "KTEggProcessor.hh"

// Wrappers etc.
typedef std::vector<boost::filesystem::path, std::allocator<boost::filesystem::path> > PyListType;

struct BoostFilesystemPath_to_python_str
{
    static PyObject* convert(boost::filesystem::path const& s)
        {
            return boost::python::incref(
                boost::python::object(
                    s.native()).ptr());
            /*
            std::string* s1 = &s.generic_string();
            PyObject* bpo = boost::python::object(s1);
            boost::python::incref(bpo);
            return bpo;
            */
        }
};
struct BoostFilesystemPath_from_python_str
{
    static void* convertible(PyObject* obj_ptr)
    {
        if (!PyString_Check(obj_ptr)) return 0;
        return obj_ptr;
    }

    static void construct(PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
    {
        // Extract the character data
        const char* value = PyString_AsString(obj_ptr);
        // Verify that it is a string
        assert(value);
        //grab pointer to memery in which to construct new one
        void* storage = ( (boost::python::converter::rvalue_from_python_storage<boost::filesystem::path>*) data)->storage.bytes;
        new (storage) boost::filesystem::path(value);
        data->convertible = storage;
    }

    BoostFilesystemPath_from_python_str()
    {
        boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<boost::filesystem::path>());
    }
};

// namespace exports
void export_KTEggProcessorPy()
{
    using namespace Katydid;
    using namespace boost::python;
    to_python_converter<boost::filesystem::path, BoostFilesystemPath_to_python_str>();
    BoostFilesystemPath_from_python_str();

    class_<PyListType>("FilenameList")
        .def(vector_indexing_suite<PyListType>())
        ;

    class_< Katydid::KTEggProcessor, boost::noncopyable, bases<Nymph::KTProcessor> >("KTEggProcessor")
        .add_property("NSlices", &KTEggProcessor::GetNSlices, &KTEggProcessor::SetNSlices)
        .add_property("ProgressReportInterval", &KTEggProcessor::GetProgressReportInterval, &KTEggProcessor::SetProgressReportInterval)

        //TODO The Filenames type is an std::vector which i don't have a way to deal with yet
        .add_property("Filenames", make_function( &KTEggProcessor::GetFilenames, return_value_policy<copy_const_reference>()), &KTEggProcessor::SetFilenames)
        //.add_property("Filenames", &KTEggProcessor::GetFilenames, &KTEggProcessor::SetFilenames)

        .add_property("EggReaderType", make_function( &KTEggProcessor::GetEggReaderType, return_value_policy<copy_const_reference>()), &KTEggProcessor::SetEggReaderType)

        .add_property("SliceSize", &KTEggProcessor::GetSliceSize, &KTEggProcessor::SetSliceSize)
        .add_property("Stride", &KTEggProcessor::GetStride, &KTEggProcessor::SetStride)
        .add_property("StartTime", &KTEggProcessor::GetStartTime, &KTEggProcessor::SetStartTime)

        .add_property("NormalizeVoltages", &KTEggProcessor::GetNormalizeVoltages, &KTEggProcessor::SetNormalizeVoltages)
        ;
}

#endif /* KTEGGPROCESSORPY_HH_ */
