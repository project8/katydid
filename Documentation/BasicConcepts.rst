The Basic Idea
===============

The majority of internal Katydid objects belong to one of two groups: data classes and processor classes. Data classes hold information such as a power spectrum or a track object, and processor classes perform some operation which takes one or more data objects as input and output. The basic idea of the Katydid framework is to construct an analysis chain of processors, and specify the types of data they will exchange. With this modular framework, changes to the analysis chain are made at runtime, while only changes to the internals of a processor or data object require code development. Changes to the analysis chain may include:

- Exchange one set of processors in the chain for a different set
- Retrieve an alternate type of data from a processor
- Adjust an analysis parameter of a processor

Katydid has many, many data and processor classes but they are all specific to Project 8 analysis. Some are used in official analysis, some are used only in alternative analyses, some are old and no longer used, some are broken, and some are only understood by one person. Material is almost ever deleted, as the modular structure makes it beneficial to keep things around since they might see a different use (or a similar one) in the future. However, all of the numerous classes in Katydid inherit from one of a handful of classes in Nymph, which is the submodule that contains the more general framework we've begun to outline. In this page, we will focus on the most important aspects of this framework for a user or a new developer of Katydid. With that basic understanding, you may explore the multitude of processors, data classes, and other objects in Katydid and become familiar with the ones necessary for your analysis.

Signals and Slots
------------------

A link in the analysis chain is described by a signal-slot connection; signals are the output of processors and slots are their input. This connection has the following form::

  - signal: processor-name-1:signal-name
    slot: processor-name-2:slot-name

Every processor has a list of possible signals and slots, and the connection is only allowed if both the specified signal and slot exist in their respective processors. **The possible signals and slots are listed along with the associated data objects in the comment block before the class declaration of each processor .hh file.**

The Data Pointer
-----------------

You may have already correctly guessed that there is another requirement on the signal-slot connection: they must be associated with the same data object. However, the object which is actually passed between processors is slightly more complicated than an ordinary data class; it is called the data pointer, and it is effectively an ordered list of many data objects. A signal always carries a data pointer, and a slot always receives it; the slot then checks whether the data pointer contains the required data object(s). Most often, a processor appends a new data object to the data pointer and then emits a signal with the same data pointer. The data pointer then contains all of the data objects it had previously, and the one (or many) new objects appended by the most recent processor. Sometimes it is necessary for a processor to create a new data pointer; this is usually when there is not a 1-to-1 correspondence between the anticipated input and output (for example, you might only conditionally emit a signal). In these cases, the resulting data pointer will only have the objects appended to it by the processor which created it.

Familiarity with the data pointer is most important for developers, but it is useful for ordinary users as well due to the following consequences on signal-slot connections:

- A processor slot can see all data objects behind it in the chain, to the extent that the 1-to-1 correspondence mentioned above holds. For example, if I connect the output of a FFT processor to a new processor, the new processor can still access the time-domain data which was input to the FFT.
- A slot may require multiple data objects, in which case they must all be contained in the data pointer.
- A data pointer should never contain multiple instances of the same data class, as there is no way of telling the slot which one to use.

Configurable Parameters
------------------------

In addition to allowed signals and slots, there is one more set of properties of a processor: configurable parameters. These are parameters of the analysis performed by the processor, and can be specified at run-time along with the analysis chain. A list of configurable parameters is located in the .hh file of each processor, immediately before the signals and slots. And, like the signals and slots, only those variables which are specifically made so in the processor code will be configurable. As a simple example, if we have a processor which performs a Gaussian smearing on a 2D spectrogram, a natural configurable parameter would be the standard deviation of the kernel function.

In general, we will not specify every possible parameter for every processor. Those which are not set in the configuration file will take on a default value which is given in the constructor of the processor class. **These default values are not always meant to be sensible, only functional**. For example, the default maximum frequency for the discriminator processor is 1.0 (Hz), which is entirely useless for any real data (0-100MHz for ROACH data and 50-150MHz for RSA data). If something is not behving as expected, you may want to check these default values and see whether there is anything you left out of the config file that should be specified.

Putting it all together
------------------------

With an understanding of all of the above elements, we are now prepared to discuss the configuration file in full. The configuration is a YAML-format file which specifies everything related to the analysis chain. We talk through it more thoroughly on the User Interface page, but here we will outline the basic structure of any config file:

1. Specify & name all processors. You may have multiple instances of the same processor but give them different names.
2. Construct all signal-slot connections.
3. Specify the first processor in the chain. Only certain processors are valid for this assignment.
4. Configure all processor parameters

Below is a graphic depicting the analysis chain from a simple example configuration file:

.. image:: katydid-example.png
   :width: 600

The text is labelled by the four steps above, and this config file can be found in this repository under: ``Examples/ConfigFiles/KatydidPSConfig.yaml``. Processors are shown in blue, and the data passed between them in orange. The signal and slot names are also indicated, which you should compare with the text in section 2 of the config file (note also that processors here are called by their names, not their types). This example takes one slice of an egg file, computes the FFT, and then the associated power spectrum. The analysis chain terminates with the root writer processor, which writes the power spectrum to a TH1 object in a ROOT file and does not emit any signals.

Directory Structure
--------------------

The processors and data classes in Katydid are organized into the following directories. Note that not all of the directories which exist are listed here; as I already made a point to mention, there are *many, many things* in Katydid and even as a developer you will only be familiar with a small subset of them. That sort of goes for directories too.

* Examples/ConfigFiles - Sample configuration files meant to demonstrate a certain type of processing chain
* Examples/Tutorial - Files for the development tutorial, which guides the user through creating a new processor and associated data objects to perform a low-pass filter operation
* Nymph - Submodule containing the backend framework
* Source - All of the source code lives here
* Source/Transform - Processor classes involved in transforming between the time and frequency domains
* Source/SpectrumAnalysis - Processor classes that act on spectrum-like data
* Source/EventAnalysis - Processor classes that act on individual events (e.g. tracks, clusters of tracks, etc.)
* Source/Data/Transform - Data classes related to time/frequency transforms
* Source/Data/SpectrumAnalysis - Spectrum-like data classes
* Source/Data/EventAnalysis - Data related to individual events (e.g. tracks)
* Source/IO - Processors to read or write data objects. Note that the egg reader processors are in Source/Time, not here.
* Source/Utility - Generic classes that are used throughout Katydid

An addendum: Cut Classes
-------------------------

Now that we have a decent understanding of the basics, there is one other fairly large family of classes that I've thusfar neglected to mention: in addition to processors and data, there are cut classes. Cut classes are just like ordinary processors, but they have a special function: evaluate some property of the received data, and "cut" if that property fails to meet some requirement. Cut classes have three signals: ``pass``, ``fail``, and ``all``. One of the two former signals is emitted conditionally if the object passes (or fails) whatever cut is performed in the class; the last is emitted always. In all cases, a ``CutStatus`` is added to the data pointer; this cut status can be utilized by other processors if needed.

The slot name for every cut class is ``apply``, and the processor name is ``apply-cut``. Thus, in the configuration file we would initialize the cut class with::

  - type: apply-cut
    name: cuts

And then connect it with::

  - signal: "tr-proc:track"
    slot: "cuts:apply"
  - signal: "cuts:pass"
    slot: "rootw:processed-track"

The above example makes a cut on a track property which we have not yet specified; we do this the same way we configure parameters for an ordinary processor, with one extra step to identify the cut class::

  cuts:
      track-time-cut:
          min-time: 0.01

This tells the cut processor to use a cut of type ``track-time-cut`` with the ``min-time`` parameter set to 10 milliseconds, and send only those tracks which pass (with a start time of at least 10 milliseconds) to the root writer.

All cut classes are currently located in Source/EventAnalysis/CutClasses.

Where to go from here?
-----------------------

If you are a new user or developer, we hope this page has helped to provide a basic grasp of the concepts behind Katydid analysis. We have discussed the signal-slot mechanism, the data pointer, and the general form of a configuration file. Please don't hesitate to provide any feedback to us about your experience.

Next, we want to get started with using Katydid. Here are a few pages in this folder to take you in that direction:

- Installation.rst: instructions to download and compile Katydid and its dependencies
- QuickStart.rst: a walkthrough to execute Katydid with a simple config file, read/interpret the output, and then make an adjustment to the config file
- DevGuide.rst: a more thorough introduction to the development side of Katydid
