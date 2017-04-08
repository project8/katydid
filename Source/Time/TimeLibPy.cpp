#include <boost/python.hpp>

// include the *Py.hh here


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
    export_
}
