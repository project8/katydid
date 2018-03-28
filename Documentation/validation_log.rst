Validation Log
==============

Guidelines
----------

* All new features incorporated into a tagged release should have their validation documented.
  * Document the new feature.
  * Perform tests to validate the new feature.
  * If the feature is slated for incorporation into an official analysis, perform tests to show that the overall analysis works and benefits from this feature.
  * Indicate in this log where to find documentation of the new feature.
  * Indicate in this log what tests were performed, and where to find a writeup of the results.
* Fixes to existing features should also be validated.
  * Perform tests to show that the fix solves the problem that had been indicated.
  * Perform tests to shwo that the fix does not cause other problems.
  * Indicate in this log what tests were performed and how you know the problem was fixed.
  
Template
--------

Version: 
~~~~~~~~

Release Date: 
'''''''''''''

New Features:
'''''''''''''

* Feature 1
    * Details
* Feature 2
    * Details
  
Fixes:
''''''

* Fix 1
    * Details
* Fix 2
    * Details
  
Log
---

Version: *Upcoming Releases*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: TBD
'''''''''''''''''

New Features:
'''''''''''''

* Updates to data accumulator, gain variation processor, gain normalization, and spectrum discriminator
    * Modernized processors that have not been updated in a long time
    * Data accumulator now accumulates variance for frequency-type data
    * Gain variation data now includes variance, which is calculated in gain variation processor
    * Gain normalization is updated to do normalization properly and includes both the  mean and variance
    * Normalized FS data now stores the normalized mean and variance, which can be used by the spectrum discriminator
    * Spectrum discriminator has been modernized and the sigma thresholding fixed
    * Validation scripts added/used: TestBackgroundFlattening, TestDataAccumulator, TestGainNormalization, and TestGainVariationProcessor
* Added some utilities
    * Functions to convert between different complex representations in KTStdComplexFuncs.hh
    * C++ name demangling in KTDemangle.hh
* Added the ability to write out sparse spectrograms from KTKDTreeData in KTBasicROOTWriter
    * Slot is "kd-tree-ss"
    * Validated by comparing to KDTree data written to a ROOT TTree
* Added and implemented the use of the Cicada library for ROOT TTree writing.
    * Validated by adding and running TestROOTTreeWritingViaCicada.cc
* Update of sequential track building
    * Configurable slope method
    * Configurable number of points used by slope methods
    * Different frequency acceptance can be set for second point in line
    * Validation by processing test run (3004) with newly available settings
  
Fixes:
''''''


Version: 2.10.0
~~~~~~~~~~~~~~~

Release Date: TBD
'''''''''''''''''

New Features:
'''''''''''''

* Spectrogram Striper
    * Accepts frequency spectra and groups them into stripes with a given width and stride.
    * Validation programs: TestSpectrogramStriper, TestSpectrogramStriperSwaps
  
Fixes:
''''''


Version: 2.9.2
~~~~~~~~~~~~~~

Release Date: February 20, 2018
'''''''''''''''''''''''''''''''

 
Fixes:
''''''

* Fixed the calculation of the center frequency in the egg3 reader
    * Validated by reading two egg files with EggScanner and verifying that the printed frequencies (min, max, and center) match what I expect from manually examining the contents of the egg files


Version: 2.9.1
~~~~~~~~~~~~~~

Release Date: January 29, 2018
''''''''''''''''''''''''''''''

 
Fixes:
''''''

* Updated the documentation system to remove Sphinx-based API pages
* Fixed problem in KTEgg3Reader where the current record ID was not being kept track of
    * This was only a problem for multi-file runs when reading the non-first files
    * Validated by reading both a first file and a non-first file and seeing that the records were stepped through correctly

Version: 2.9.0
~~~~~~~~~~~~~~

Release Date: January 18, 2018
''''''''''''''''''''''''''''''

New Features:
'''''''''''''

* Auto-building documentation system now fully functional
  
Fixes:
''''''


Version: 2.8.0
~~~~~~~~~~~~~~

Release Date: January 11, 2018
''''''''''''''''''''''''''''''

New Features:
'''''''''''''

* Sequential Track Building
* Overlapping Track Clustering
* Iterative Track Clustering
* Collinear Track Clustering
* 1D Convolution Processor
* Auto-building documentation system
  
Fixes:
''''''


Version: 2.7.3
~~~~~~~~~~~~~~

Release Date: December 19, 2017
'''''''''''''''''''''''''''''''

Fixes:
''''''

* Fixed bug in creating the correct number of SingleChannelADCs
    * Added a copy constructor to KTSingleChannelADC
    * Create the correct number of SingleChannelADCS in KTDAC


Version: 2.7.2
~~~~~~~~~~~~~~

Release Date: October 2, 2017
'''''''''''''''''''''''''''''

New Features:
'''''''''''''

* ROOT Spectrogram Writer: sequential writing mode
    * A new mode of writing was added to the writer that writes sequential spectrograms of a given time size.
    * Documentation is included in the KTROOTSpectrogramWriter header documentation.
    * The new writing mode was tested on a concatenated file from an RSA run. Sequential spectrograms split at the right times according to the setting and acquisition breaks.
    * The old writing mode ("single") continued to function in the same way.
  
Fixes:
''''''

* Egg3 reader: Fixeed filling of frequencies (min/max/center)

* Monarch: updated to v3.4.6
    * Fixed the string-attribute-length bug.
    * Validated by testing on an egg file with a long description attribute. File opened and read correctly.

* Consensus Thresholding NaN fix
    * For vertically-aligned points, the CT algorithm would calculate an NaN slope, but the algorithm did not crash. This special case is now handled without calculating a slope.
    * Validated by observing the debug output of the CT processor when processing a data file, and seeing that no NaN or inf values were present.
  
* Egg3 reader: crashed on reading second file and beyond in multi-file runs
    * The reader was not picking up the first record number in the file, which was non-zero for the second file and beyond in a multi-file run.
    * This was fixed by adjusting the variable that tracks that record number immediately after the first record was read.
    * This was validated by analyzing a non-first-file in a multi-file run and seeing that the record number was correct in the debug output.  Run number 3870 was used.


  
