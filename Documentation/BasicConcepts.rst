The Basic Idea
===============

The majority of internal Katydid objects belong to one of two groups: data classes and processor classes. Data classes hold information such as a power spectrum or a track object, and processor classes perform some operation which takes one or more data objects as input and output. The basic idea of the Katydid framework is to construct an analysis chain of processors, and specify the types of data they will exchange. With this framework, changes to the analysis chain are made at runtime, while only changes to the internals of a processor or data object require code development. Changes to the analysis chain may include:

- Exchange one set of processors in the chain for a different set
- Retrieve an alternate type of data from a processor
- Adjust an analysis parameter of a processor

A link in the analysis chain is described by a signal-slot connection; signals are the output of processors and slots are their input. This connection has the following form:

`
- signal: processor-name-1:signal-name
  slot: processor-name-2:slot-name
`

Every processor has a list of possible signals and slots, and the connection is only allowed if both the specified signal and slot exist in their respective processors. The possible signals and slots are listed in the comment block before the class declaration of each processor .hh file.
