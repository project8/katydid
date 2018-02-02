Modules
==============

Katydid is divided into several modules, each of which builds a shared-object library.  Each module has a general purpose, and the classes that go into each module pertain primarily to that purpose.

Dependencies between the modules are certainly allowed, though care should be taken to ensure that those dependencies go only in one direction (i.e. be careful to avoid circular dependencies!).

The library filenames are libKatydid[ModuleName].so (or .dylib).


TODO_DOC: this section might better fit in the Doxygen files? Perhaps delete this page. 

The modules in Katydid are:
* **Analysis** -- Classes that perform various higher-level analyses of time- and frequency-space data.
* **Candidates** -- Classes that are responsible for identifying and analyzing candidates
* **Core** -- Classes that comprise the various systems within Katydid (e.g. the system of data classes, or the configuration file system).
* **Data** -- A collection of all of the data classes that are used by the various modules. They are all here to reduce interdependencies between the modules.
* **Egg** -- Classes used to read egg files and create events with the basic time-series data.
* **Evaluation** -- Classes used to do meta-analysis of candidates and how well analyses are performing compared to MC truth
* **FFT** -- Classes for performing Fast Fourier Transforms.
* **Input** -- Classes used to read various types of data from files or other sources
* **Output** -- Classes used to output various types of data.
* **Simulation** -- Classes that can generate time series
* **Utility** -- General-use classes that are convenient and typically used throughout Katydid.
