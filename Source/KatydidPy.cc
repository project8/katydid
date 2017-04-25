#include <boost/python.hpp>
#include <Python.h>

/* include each library here */
#include "KTDataLibPy.hh"
#include "KTEvaluationLibPy.hh"
#include "KTEventAnalysisLibPy.hh"
#include "KTIOLibPy.hh"
//Simulation is commented out the the top-level CMakeLists.txt, is it still valid?
//#include "KTSimulationLibPy.hh"
#include "KTSpectrumAnalysisLibPy.hh"
#include "KTTimeLibPy.hh"
#include "KTTransformLibPy.hh"
#include "KTUtilityLibPy.hh"

/* each library needs a c++ class to contain its namespace in python*/
class katydidPyData{};
class katydidPyEvaluation{};
class katydidPyEventAnalysis{};
class katydidPyIO{};
//class katydidPySimulation{};
class katydidPySpectrumAnalysis{};
class katydidPyTime{};
class katydidPyTransform{};
class katydidPyUtility{};

// This should not be here, nymph should have its own python module
//#include "KTDataPy.hh"
//class externals{};

BOOST_PYTHON_MODULE(KatydidPy)
{
    using namespace boost::python;

    {
    scope Data = class_<katydidPyData>("Data");
    }

    {
        scope Evaluation = class_<katydidPyEvaluation>("Evaluation");
        export_KTEvaluationPy();
    }

    {
        scope EventAnalysis = class_<katydidPyEventAnalysis>("EventAnalysis");
        export_KTEventAnalysisPy();
    }

    {
        scope IO = class_<katydidPyIO>("IO");
        export_KTIOPy();
    }

    /*
    {
        scope Simulation = class_<katydidPySimulation>("Simulation");
        export_Simulation();
    }
    */

    {
        scope SpectrumAnalysis = class_<katydidPySpectrumAnalysis>("SpectrumAnalysis");
        export_KTSpectrumAnalysisPy();
    }

    {
        scope Time = class_<katydidPyTime>("Time");
        export_KTTimePy();
    }

    {
        scope Transform = class_<katydidPyTransform>("Transform");
        export_KTTransformPy();
    }

    {
        scope Utility = class_<katydidPyUtility>("Utility");
        export_KTUtilityPy();
    }

    /**********************/
    // These should go into a dedicated nymph python module
    /*{
        scope Externals = class_<externals>("Externals");
        export_DataPy();
    }*/

}
