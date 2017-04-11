#include <boost/python.hpp>
#include <Python.h>

// include the *Py.hh here
#include "KTDataPy.hh"

#include "KTEggReaderPy.hh"
#include "KTEgg3ReaderPy.hh"
#include "KTRSAMatReaderPy.hh"
#include "KTEggProcessorPy.hh"

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
    export_KTDataPy();
    export_KTEggReaderPy();
    export_KTEgg3ReaderPy();
    export_KTRSAMatReaderPy();
    export_KTEggProcessorPy();
}
