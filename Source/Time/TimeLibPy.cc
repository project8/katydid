#include <boost/python.hpp>
#include <Python.h>

// include the *Py.hh here
#include "KTEggReaderPy.hh"
#include "KTEgg3ReaderPy.hh"

// I'm creating a module, but should probably actuall do:
/*
class time_namespace{};
void export_time()
{
    scope time = class_<time_namespace>("time");
    <<< then everything that follows >>>
}
*/
BOOST_PYTHON_MODULE(katydidPy)
{
    export_KTEggReaderPy();
    export_KTEgg3ReaderPy();
}
