Developers Guide
========================

TODO_DOC: this should probably be split up into the different bits it talks about. Better that each entry has a basic section followed by an advanced section, rather than what we have now: a folder of basic pages followed by a folder of advanced pages. Also, almost all of this is very out of date, even compared to the rest of the docs here. 


Processors and Data
--------------
These are the main two types of classes in Katydid.  Generally, Data classes store particular types of information, and the Processors act on them.  For instance, there is a TimeSeriesData class (actually, a set of them, but more on that later), which gets acted on by one of the FFT Processor classes, producing one of the FrequencySpectrum data classes.

KTEvent
--------------
An event represents all of the information relevant to a time period spanned by a record.  In this case the record size is not necessarily the same as the Monarch record size; it can be specified by the user via the class KTEggProcessor.  As an analysis progresses, different types of data will be produced and attached to the Event.

More about Processors
--------------
Processors can typically be split into three parts:
1. The main purpose of a Processor is to perform some sort of action on one or more types of Data.  Therefore the main part of a Processor will be the methods used to perform that action.  There should be a public interface to these methods so that the Processor can be used manually if desired (i.e. without the signal/slot or configuration mechanisms).
2. In addition to the usual Set methods for setting the parameters of the Processor, a Processor will have a Configure method that allows it to be configured via Katydid's configuration mechanism.
3. A Processor will typically have slots to initiate the running of the processor, and a signal to indicate the production of new data.  These will allow the Processor to be linked to other Processors dynamically.

For new Katydid developers, it is probably easiest to approach creating a new Processor in that order.  First simply develop a class that performs a specific action on a type of Data.  Next implement the Configure method so that your Processor can be automatically configured via Configuration system.  Finally, add the relevant signals and slots.

More about Data
--------------
A Data class is meant to be a set of data that all go together.  For example, there's a time series Data class called KTProgenitorTimeSeriesData that has the time series information plus all of the other associated information from the egg file.  For the most part, there's a close pairing between Data classes and Processor classes, because a Processor typically produces a particular type of Data.  KTCorrelator produces KTCorrelationData, and KTHoughTransform produces KTHoughData.  There are also cases where types of Data are produced by different Processors: KTWignerVille and KTSlidingWindowFFTW produce KTSlidingWindowFSDataFFTW.

Configuration Mechanism
--------------
The purpose of the Configuration mechanism is to allow objects to be setup at runtime via a [JSON](http://www.json.org) file passed to the executable by the user.  A class is made a part of the Configuration mechanism by inheriting it from KTConfigurable (KTProcessor inherits from KTConfigurable, so all Processors are part of the Configuration system).  Please see [[Processors]] for further information. 

All classes that are able to be configured via the configuration system should inherit from KTConfigurable. When making a configurable class, it will need to implement a `Bool_t Configure(const KTParamNode* node)` function, which will be responsible for correctly setting up the class to be used.  See the documentation for the KTParam* classes (KTParam, KTParamValue, KTParamArray, and KTParamNode) for information on how to access the configuration information.  See the patterns for using the parameter system below for suggestions on how to configure your class.

Any configurable class also needs a "config name."  KTConfigurable::fConfigName should be set to something unique (across Katydid) in the derived class's constructor. 

Patterns for using the parameter system
-----------
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
--------------
For more details on creating new processors and custom Katydid components, go through the Tutorial in the source code "Documentation" folder. You may also be interested in the tutorials about how to use the DIRAC file system and batch computing cluster-- those tutorials are on the "hardware" wiki under the DIRAC section. 