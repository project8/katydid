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
- Eigen (3.3 or better recommended; may work with earlier versions)
- FFTW3 (optional, but required to do FFTs; 3.3 or newer)
- ROOT (optional, but required for some processors and outputing to ROOT files; 6.0 or better)
- MatIO (optional, but required to read the Matlab files generated by the Tektronix RSA; 1.5.0 or better)
- GraphicsMagick (optional, but required for writing images)
- Eigen (not currently used)

**Submodules** (included with Katydid; must be fetched via Git)
- [Monarch](https://github.com/project8/monarch)
  - [Scarab](https://github.com/project8/scarab)
    - yaml-cpp
- [Nymph](https://github.com/project8/nymph)
  - [Scarab](https://github.com/project8/scarab)
    - yaml-cpp
- [Cicada](https://github.com/project8/cicada)
  - [Scarab](https://github.com/project8/scarab)
    - yaml-cpp

**Distributed Code** (included with Katydid directly)
- nanoflann
- RapidXML


Operating System Support
------------------------

* Mac OS X
* Linux (usually tested on Ubuntu)
* Docker (based on Ubuntu 20.04)


Directory Structure
-------------------

*  cmake - Files that CMake uses to build Katydid.
*  Documentation - Doxygen-based code documentation.
*  Examples - Configuration files, custom applications, and a tutorial.
*  External - A few dependencies that are included with the distribution.
*  Nymph - Submodule
*  OldAnalysis - Dump for old source files.
*  Source - current source files (more docs in that folder)



Tips on Installing the Dependencies
-----------------------------------

MacOS: [Homebrew](https://brew.sh/) is a convenient package manager. Before using it for the installation, make sure that the version available on brew is compatible with what is listed in the dependency list above. [CMake](http://brewformulas.org/Cmake), [Boost](http://brewformulas.org/Boost), [Hdf5](http://brewformulas.org/Hdf5), [Protobuf](http://brewformulas.org/Protobuf), and [Root](http://brewformulas.org/root) are all available through brew via the instructions in these links.


Installing
----------

The following steps will build Katydid from scratch.  Starting with a terminal window . . .

1. Clone the repository and make a build directory as recommended above. You will also have to initialize the submodules.
  ```
  $ git clone "https://github.com/project8/Katydid"
  $ cd Katydid
  $ git submodule update --init --recursive
  $ mkdir build
  ```

2. To configure the installation you can use cmake, ccmake, or cmake-gui.

  For a first configuration, using either ccmake or cmake-gui is highly recommended.  The following instructions are for ccmake, but the steps with cmake-gui would be approximately the same.
  ```
  $ cd build
  $ ccmake ..
  ```

  You will be prompted to press [c] to configure, and the window will fill up with several options. 

  You should set the CMake variable `CMAKE_BUILD_TYPE` to either `RELEASE`, `STANDARD`, or `DEBUG` (default), in order
  of how much text output you would like (from least to most) and how much compiler optimization
  should be performed (from most to least). You may wish to to activate C++14 support with USE_CPP14=ON if your ROOT version was built with C++14 support.

  Variables that start with `Katydid_` will control which parts of Katydid are built.

  If using Monarch, if you want to read egg3 files, ensure that `Monarch_BUILD_MONARCH3` is set.
  If you want to read egg2 files, ensure that `Monarch_BUILD_MONARCH2` is set.

  The install prefix is specified by the CMake variable `CMAKE_INSTALL_PREFIX`.
  The library, binaries, and header files will be installed in the
  lib, bin, and include subdirectories. The default install prefix is the
  build directory.

  After you've finished, if you've changed anything press [c] again to configure.  Then [g] to generate and exit.

3. Build and install.

  ```
  $ make install
  ```

  Or if you want to take advantage of parallel building to get things done faster:
  ```
  $ make -j install
  ```

  If the compiler runs into errors during the build, first check that you've updated the submodules and that you have all of the required dependencies installed (many are called "optional" on this page, but if you want to build without them you must also specify this in the cmake window). If you made a change to the dependencies or submodules, you may have to wipe the build directory and start again from step 1; simply writing `make install` again will not always work. 


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

Reference guide included with Katydid at Documentation/ReferenceGuide/html/index.html


Development
-----------

The Git workflow used is git-flow:
* http://nvie.com/posts/a-successful-git-branching-model/
We suggest that you use the aptly-named git extension, git-flow, which is available from commonly-used package managers:
* https://github.com/nvie/gitflow

Issues should be posted via [GitHub](https://github.com/project8/katydid/issues).
