Katydid Source
=======

Katydid is a data analysis package for Project 8. These source files define analysis pieces. 


Dependencies
------------
Same as Katydid dependencies. 


Directory Structure
------------

Folders: 
*  Data - Classes that store the data as it gets processed.
*  Evaluation - Processors for determining how well an analysis is performing.
*  EventAnalysis - Processors that act on individual events (e.g. tracks, clusters of tracks, etc.).
*  Executables - Source code for Katydid executables and test programs.
*  IO - Writer and reader classes.
*  Simulation - An attempt at integrating a simulation into Katydid (use Locust_MC instead)
*  SpectrumAnalysis - Processors that act on spectrum-like data.
*  Time - Processors that act on data in the time domain.
*  Transform - Processors involved in transforming between the time and frequency domains.
*  Utility - Generic classes that can be used throughout Katydid.

Files: 
*  KatydidPy.cc - Python bindings for Katydid classes

Use
------------
Build Katydid, then use a config file to run analysis scripts. See Tutorial for more details. 