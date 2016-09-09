Katydid
=======

Katydid is a data analysis package for Project 8.

The various actions Katydid can perform on data are packaged into Processors, which can
be setup at runtime into any number of configurations to suit any analysis procedure.


Dependencies
------------

**External**
- CMake (3.1 or better)
- Boost (date_time, filesystem, program_options, system, thread; 1.46 or better)
- HDF5 (optional, but required by Monarch3 and for outputing to HDF5 files)
- Protobuf (optional, but required by Monarch2)
- FFTW3 (optional, but required to do FFTs; 3.3 or newer)
- ROOT (optional, but required for some processors and outputing to ROOT files; 6.0 or better)
- MatIO (optional, but required to read the Matlab files generated by the Tektronix RSA; 1.5.0 or better)
- Eigen (not currently used)

**Submodules** (included with Katydid; must be fetched via Git)
- [Monarch](https://github.com/project8/monarch)
  - [Scarab](https://github.com/project8/scarab)
    - yaml-cpp
- [Nymph](https://github.com/project8/nymph)
  - [Scarab](https://github.com/project8/scarab)
    - yaml-cpp

**Distributed Code** (included with Katydid directly)
- nanoflann
- RapidXML


Operating System Support
------------------------

* Mac OS X (usually tested on OS X 10.10 and 10.11)
* Linux (usually tested on Debian Jessie and Scientific Linux 6)


Directory Structure
-------------------

*  cmake - Files that CMake uses to build Katydid.
*  Documentation - Doxygen-based code documentation.
*  Examples - Configuration files, custom applications, and a tutorial.
*  External - A few dependencies that are included with the distribution.
*  Nymph - Submodule
*  OldAnalysis - Dump for old source files.
*  Source
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


Installing
----------

Katydid is installed using CMake.
We recommend an out-of-source build:
```
  >  mkdir build
  >  cd build
```

To configure the installation you can use cmake, ccmake, or cmake-gui.
You should set the variable `CMAKE_BUILD_TYPE` to either `Release`, `Standard`, or `Debug`, in order
of how much text output you would like (from least to most) and how much compiler optimization
should be performed (from most to least).

Variables that start with `Katydid_` will control which parts of Katydid are built.

If using Monarch, if you want to read egg3 files, ensure that `Monarch_BUILD_MONARCH3` is set.
If you want to read egg2 files, ensure that `Monarch_BUILD_MONARCH2` is set.

Here is an example of what you have to do using ccmake for a `Debug` build.

```
ccmake ..
```
Press [c] to configure
Select CMAKE_BUILD_TYPE
Press Return
Write `Debug`
Press Return
Press [c] to configure
Press [g] to generate
Press [q] to quit

To build and install:
```
  >  make
  >  make install
```

The install prefix is specified by the CMake variable `CMAKE_INSTALL_PREFIX`.
The library, binaries, and header files will be installed in the
lib, bin, and include subdirectories. The default install prefix is the
build directory.


Instructions for Use
--------------------

Katydid is typically started with a command like:
```
  > Katydid -c my_config_file.json
```

For more detailed instructions, use `Katydid -h`


Documentation
-------------

Hosted at: http://www.project8.org/katydid


Development
-----------

The Git workflow used is git-flow:
* http://nvie.com/posts/a-successful-git-branching-model/
We suggest that you use the aptly-named git extension, git-flow, which is available from commonly-used package managers:
* https://github.com/nvie/gitflow

Issues should be posted via [GitHub](https://github.com/project8/katydid/issues).
