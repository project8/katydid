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

#include "KTPythonMacros.hh"

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

//KTEggReader::path_vev (Katydid::KTEggProcessorPy::*GetFilenames_var)(

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
        PROPERTYMEMBER(KTEggProcessor, NSlices)
        PROPERTYMEMBER(KTEggProcessor, ProgressReportInterval)

        PROPERTYMEMBER_RETPOLICY( KTEggProcessor, Filenames, reference_existing_object )

        PROPERTYMEMBER_RETPOLICY( KTEggProcessor, EggReaderType, copy_const_reference )

        PROPERTYMEMBER(KTEggProcessor, SliceSize)
        PROPERTYMEMBER(KTEggProcessor, Stride)
        PROPERTYMEMBER(KTEggProcessor, StartTime)

        PROPERTYMEMBER(KTEggProcessor, NormalizeVoltages)
        ;
}

#endif /* KTEGGPROCESSORPY_HH_ */
