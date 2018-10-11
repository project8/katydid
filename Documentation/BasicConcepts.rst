The Basic Idea
===============

The majority of internal Katydid objects belong to one of two groups: data classes and processor classes. Data classes hold information such as a power spectrum or a track object, and processor classes perform some operation which takes one or more data objects as input and output. The basic idea of the Katydid framework is to construct an analysis chain of processors, and specify the types of data they will exchange. With this modular framework, changes to the analysis chain are made at runtime, while only changes to the internals of a processor or data object require code development. Changes to the analysis chain may include:

- Exchange one set of processors in the chain for a different set
- Retrieve an alternate type of data from a processor
- Adjust an analysis parameter of a processor

Katydid has many, many data and processor classes but they are all specific to Project 8 analysis. Some are used in official analysis, some are used only in alternative analyses, some are old and no longer used, some are broken, and some are only understood by one person. Material is almost ever deleted, as the modular structure makes it beneficial to keep things around since they might see a different use (or a similar one) in the future. However, all of the numerous classes in Katydid inherit from one of a handful of classes in Nymph, which is the submodule that contains the more general framework we've begun to outline. In this page, we will focus on the most important aspects of this framework for a user or a new developer of Katydid. With that basic understanding, you may explore the multitude of processors, data classes, and other objects in Katydid and become familiar with the ones necessary for your analysis.

Signals and Slots
------------------

A link in the analysis chain is described by a signal-slot connection; signals are the output of processors and slots are their input. This connection has the following form:

`
- signal: processor-name-1:signal-name
  slot: processor-name-2:slot-name
`

Every processor has a list of possible signals and slots, and the connection is only allowed if both the specified signal and slot exist in their respective processors. The possible signals and slots are listed along with the associated data objects in the comment block before the class declaration of each processor .hh file.

The Data Pointer
-----------------

You may have already correctly guessed that there is another requirement on the signal-slot connection: they must be associated with the same data object. However, the object which is actually passed between processors is slightly more complicated than an ordinary data class; it is called the data pointer, and it is effectively an ordered list of many data objects. A signal always carries a data pointer, and a slot always receives it; the slot then checks whether the data pointer contains the required data object(s). Most often, a processor appends a data object to the data pointer and then emits a signal with the same data pointer. The data pointer then contains all of the data objects it had previously, and the one (or many) new objects appended by the most recent processor. Somtimes it is necessary for a processor to create a new data pointer; this is usually when there is not a 1-to-1 correspondence between the anticipated input and output (for example, you might only conditionally emit a signal). In these cases, the resulting data pointer will only have the objects appended to it by the processor which created it.

Familiarity with the data pointer is most important for developers, but it is useful for ordinary users as well due to the following consequences on signal-slot connections:

- A processor slot can see all data objects behind it in the chain, to the extent that the 1-to-1 correspondence mentioned above holds. For example, if I connect the output of a FFT processor to a new processor, the new processor can still access the time-domain data which was input to the FFT.
- A slot may require multiple data objects, in which case they must all be contained in the data pointer.
- A data pointer should never contain multiple instances of the same data class, as there is no way of telling the slot which one to use.

Configurable Parameters
------------------------

In addition to allowed signals and slots, there is one more set of properties of a processor: configurable parameters. These are parameters of the analysis performed by the processor, and can be specified at run-time along with the analysis chain. A list of configurable parameters is located in the .hh file of each processor, immediately before the signals and slots. And, like the signals and slots, only those variables which are specifically made so in the processor code will be configurable. As a simple example, if we have a processor which performs a Gaussian smearing on a 2D spectrogram, a natural configurable parameter would be the standard deviation of the kernel function.

Putting it all together
------------------------

With an understanding of all of the above elements, we are now prepared to discuss the configuration file in full. The configuration is a YAML-format file which specifies everything related to the analysis chain. We talk through it more thoroughly on the User Interface page, but here we will outline the basic structure of any config file:

1. Specify & name all processors. You may have multiple instances of the same processor but give them different names.
2. Construct all signal-slot connections.
3. Specify the first processor in the chain. Only certain processors are valid for this assignment.
4. Configure all processor parameters

