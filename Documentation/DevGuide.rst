Developers Guide
========================



Overview
----------

The first thing to do before adding a new feature to katydid, is to create a new feature branch.
It is recommended to use the gitflow_ branching module.

After creating a new feature branch, the first step it to identify where the new feature fits into the katydids structure. For more detail, see the FileStructure_ and Modules_ sections below.

Here are some examples of what development would be required if you wanted to...

* ... add a new cut (for example in an event property):
    * make a new cut class in *Source/EventAnalysis/CutClasses*
* ... add a new property to an existing data type:
    * add the new property to the data class
    * make sure that every processor that can create data of this type also adds the new property
    * add the new property in the root writer methods that can write this type of data
* ... add a new data processing/analysis step:
    * create a new processor
    * if this processor procudces a new type of data, add a new data class
    * make a new method in the root writer of your choice so the new data type can be written to a root file


.. _gitflow: https://nvie.com/posts/a-successful-git-branching-model/



.. _Modules:

Modules
---------------

Katydid is divided into several modules, each of which builds a shared-object library.  Each module has a general purpose, and the classes that go into each module pertain primarily to that purpose.

Dependencies between the modules are certainly allowed, though care should be taken to ensure that those dependencies go only in one direction (i.e. be careful to avoid circular dependencies!).

The library filenames are libKatydid[ModuleName].so (or .dylib).

Modules in Katydid each have source files, and you can find conventions of different types of modules in the README of the "Source" directory.



.. _FileStructure:

File structure
----------------
Katydid's source directory contains the following subdirectories.

* Data - Classes that store the data as it gets processed.
* Evaluation - Processors for determining how well an analysis is performing.
* EventAnalysis - Processors that act on individual events (e.g. tracks, clusters of tracks, etc.).
* Executables - Source code for Katydid executables and test programs.
* IO - Writer and reader classes.
* Simulation - An attempt at integrating a simulation into Katydid (use Locust_MC instead)
* SpectrumAnalysis - Processors that act on spectrum-like data.
* Time - Processors that act on data in the time domain.
* Transform - Processors involved in transforming between the time and frequency domains.
* Utility - Generic classes that can be used throughout Katydid.



Processors and Data
--------------------
These are the main two types of classes in Katydid.  Generally, Data classes store particular types of information, and the Processors act on them.  For instance, there is a TimeSeriesData class (actually, a set of them, but more on that later), which gets acted on by one of the FFT Processor classes, producing one of the FrequencySpectrum data classes.


More about Data
~~~~~~~~~~~~~~~~
A Data class is meant to be a set of data that all go together.  For example, there's a time series Data class called KTProgenitorTimeSeriesData that has the time series information plus all of the other associated information from the egg file.  For the most part, there's a close pairing between Data classes and Processor classes, because a Processor typically produces a particular type of Data.  KTCorrelator produces KTCorrelationData, and KTHoughTransform produces KTHoughData.  There are also cases where types of Data are produced by different Processors: KTWignerVille and KTSlidingWindowFFTW produce KTSlidingWindowFSDataFFTW.


More about Processors
~~~~~~~~~~~~~~~~~~~~~
Processors can typically be split into three parts:

1. The main purpose of a Processor is to perform some sort of action on one or more types of Data.  Therefore the main part of a Processor will be the methods used to perform that action.  There should be a public interface to these methods so that the Processor can be used manually if desired (for example in a validation executable).
2. A Processor will have a Configure method that allows it to be configured via Katydid's configuration mechanism.
3. A Processor will typically have slots to initiate the running of the processor, and a signal to indicate the production of new data.  These will allow the Processor to be linked to other Processors dynamically.

For new Katydid developers, it is probably easiest to approach creating a new Processor in that order.  First simply develop a class that performs a specific action on a type of Data.  Next implement the Configure method so that your Processor can be automatically configured via Configuration system.  Finally, add the relevant signals and slots.

For detailed guidance for developping a new processor go checkout this Tutorial_.



Configuration Mechanism
--------------------------
The purpose of the Configuration mechanism is to allow objects to be setup at runtime via a configuration file passed to the executable by the user (see UserInterface_).  A class is made a part of the Configuration mechanism by inheriting it from KTConfigurable (KTProcessor inherits from KTConfigurable, so all Processors are part of the Configuration system).  

All classes that are able to be configured via the configuration system should inherit from KTConfigurable. When making a configurable class, it will need to implement a `Bool_t Configure(const KTParamNode* node)` function, which will be responsible for correctly setting up the class to be used.  See the documentation for the KTParam* classes (KTParam, KTParamValue, KTParamArray, and KTParamNode) for information on how to access the configuration information.  See the patterns for using the parameter system below for suggestions on how to configure your class.

Any configurable class also needs a "config name."  KTConfigurable::fConfigName should be set to something unique (across Katydid) in the derived class's constructor. 

.. _UserInterface: https://katydid.readthedocs.io/en/latest/UserInterface.html

Patterns for using the parameter system
---------------------------------------
Here are some examples of code that has been used to configure different KTConfigurable classes in Katydid.  This is an incomplete list, and if new ways are developed, they should be added here.

* Basic method for getting information from the tree, for the available data types (an exception is thrown if the data cannot be cast to the requested type, or if the data doesn't exist)::

        std::string stringData = node->GetValue("string-data")
        int intData = node->GetValue< int >("int-data")
        double floatData = node->GetValue< double >("float-data")
        bool boolData = node->GetValue< bool >("bool-data")

* Data can be accessed with a default to make setting optional (from KTEggProcessor)::

        SetNSlices(node->GetValue< unsigned >("number-of-slices", fNSlices));

* Optional setting with limited possibilities (from KTEggProcessor)::

        string eggReaderTypeString = node->GetValue("egg-reader", "monarch");
        if (eggReaderTypeString == "monarch") SetEggReaderType(kMonarchEggReader);
        else if (eggReaderTypeString == "2011") SetEggReaderType(k2011EggReader);
        else
        {
            KTERROR(egglog, "Illegal string for egg reader type: <" << eggReaderTypeString << ">");
            return false;
        }

* Require that a setting is present (from KTProcessorToolbox)::

        if (! subNode.Has("type"))
        {
            KTERROR(proclog, "Unable to create processor: no processor type given");
            return false;
        }
        string procType = subNode.GetData("type");

* Iterate over all settings in an array (from KTProcessorToolbox; also requires that the array is present)::

        const KTParamArray* procArray = node->ArrayAt( "processors" );
        if (procArray == NULL)
        {
            KTWARN(proclog, "No processors were specified");
        }
        else
        {
            for( KTParamArray::const_iterator procIt = procArray->Begin(); procIt != procArray->End(); ++procIt )
            {
                /* deal with this element in the array */
            }
        }

* Nested configuration: get a child node and use it to configure a member (from KTCorrelator)::

        /* required configuration */
        fFFT->Configure(node->NodeAt("complex-fftw"));

        /* optional configuration */
        const KTParamNode* windowerNode = node->NodeAt("windower");
        if (windowerNode != NULL)
        {
            fUseWindowFunction = true;
            fWindower->Configure(windowerNode);
        }

* Array of number pairs (from KTCorrelator)::

        const KTParamArray* corrPairs = node->ArrayAt("corr-pairs");
        if (corrPairs != NULL)
        {
            for (KTParamArray::const_iterator pairIt = corrPairs->Begin(); pairIt != corrPairs->End(); ++pairIt)
            {
                if (! ((*pairIt)->IsArray() && (*pairIt)->AsArray().Size() == 2))
                {
                    KTERROR(corrlog, "Invalid pair: " << (*pairIt)->ToString());
                    return false;
                }
                UIntPair pair((*pairIt)->AsArray().GetValue< unsigned >(0), (*pairIt)->AsArray().GetValue< unsigned >(1));
                KTINFO(corrlog, "Adding correlation pair " << pair.first << ", " << pair.second);
                this->AddPair(pair);
            }
        }


Further Reading
-----------------
For more details on creating new processors and custom Katydid components, go through the Tutorial_ in the source code "Example" folder. You may also be interested in the tutorials about how to use the DIRAC file system and batch computing cluster-- those tutorials are on the "hardware" wiki under the DIRAC_ section.

.. _Tutorial: https://github.com/project8/katydid/blob/dockathon_tutorial/Examples/Tutorial/TutorialSteps.md
.. _DIRAC: https://github.com/project8/hardware/wiki/DIRAC