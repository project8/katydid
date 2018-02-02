Developers Guide
========================

TODO_DOC: this should probably be split up into the different bits it talks about. Better that each entry has a basic section followed by an advanced section, rather than what we have now: a folder of basic pages followed by a folder of advanced pages. 

# Concepts

## Processors and Data
These are the main two types of classes in Katydid.  Generally, Data classes store particular types of information, and the Processors act on them.  For instance, there is a TimeSeriesData class (actually, a set of them, but more on that later), which gets acted on by one of the FFT Processor classes, producing one of the FrequencySpectrum data classes.

## KTEvent
An event represents all of the information relevant to a time period spanned by a record.  In this case the record size is not necessarily the same as the Monarch record size; it can be specified by the user via the class KTEggProcessor.  As an analysis progresses, different types of data will be produced and attached to the Event.

## More about Processors
Processors can typically be split into three parts:
1. The main purpose of a Processor is to perform some sort of action on one or more types of Data.  Therefore the main part of a Processor will be the methods used to perform that action.  There should be a public interface to these methods so that the Processor can be used manually if desired (i.e. without the signal/slot or configuration mechanisms).
2. In addition to the usual Set methods for setting the parameters of the Processor, a Processor will have a Configure method that allows it to be configured via Katydid's configuration mechanism.
3. A Processor will typically have slots to initiate the running of the processor, and a signal to indicate the production of new data.  These will allow the Processor to be linked to other Processors dynamically.

For new Katydid developers, it is probably easiest to approach creating a new Processor in that order.  First simply develop a class that performs a specific action on a type of Data.  Next implement the Configure method so that your Processor can be automatically configured via Configuration system.  Finally, add the relevant signals and slots.

## More about Data
A Data class is meant to be a set of data that all go together.  For example, there's a time series Data class called KTProgenitorTimeSeriesData that has the time series information plus all of the other associated information from the egg file.  For the most part, there's a close pairing between Data classes and Processor classes, because a Processor typically produces a particular type of Data.  KTCorrelator produces KTCorrelationData, and KTHoughTransform produces KTHoughData.  There are also cases where types of Data are produced by different Processors: KTWignerVille and KTSlidingWindowFFTW produce KTSlidingWindowFSDataFFTW.

## Configuration Mechanism
The purpose of the Configuration mechanism is to allow objects to be setup at runtime via a [JSON](http://www.json.org) file passed to the executable by the user.  A class is made a part of the Configuration mechanism by inheriting it from KTConfigurable (KTProcessor inherits from KTConfigurable, so all Processors are part of the Configuration system).  Please see [[Processors]] for further information. 

## Signal/Slot Mechanism


## Output Mechanism


# Useful Details

# How do I . . .

* [[make a new Processor?|Creating a Processor]]
* [[add a new Data class?|Data]]