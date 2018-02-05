Configuration
==============


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
