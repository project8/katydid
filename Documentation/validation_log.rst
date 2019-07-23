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
  * Perform tests to show that the fix does not cause other problems.
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

Version: 2.17.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: July 23, 2019
'''''''''''''''''''''''''''''''

New Features:
'''''''''''''

* Digital beamforming: KTChannelAggregator and KTAggregatedChannelOptimizer
* Fractional FFT: KTFractionalFFT

Fixes:
'''''''''''''

* Fixed issue with `start-record` in KTEggReader.


Version: 2.16.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: June 6, 2019
'''''''''''''''''''''''''''''''

New Features:
'''''''''''''

* N-Tracks, N-Points NUP Cut added
* Added the ability to skip a specified number of records into an egg file
* Added default value option for construction of KTPhysicalArrays and their derived classes
* Added the ability to write the output of the spectrogram collector via the basic ROOT writer

Fixes:
'''''''''''''

* Solving Travis issue [#169](https://github.com/project8/katydid/issues/169): now uses Docker as starting environment
* Fixed spectrogram bin offset problem (between spectrograms and discriminated points)
    * A 1-bin offset was caused by a bin indexing problem (ROOT-histogram indexing, starting at 1, needed to be used)
    * A 1/2 bin offset was caused by a mistake in the spectrum discriminators, when points were assigned frequency-axis values


Version: 2.15.3
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: April 10, 2019
'''''''''''''''''''''''''''''''

Fixes:
'''''''''''''

* Updated Dockerfile for new dependencies


Version: 2.15.2
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: December 21, 2018
'''''''''''''''''''''''''''''''

Fixes:
'''''''''''''

* Updated Dockerfile for new dependencies


Version: 2.15.1
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: December 6, 2018
'''''''''''''''''''''''''''''''

Fixes:
'''''''''''''

* Updated Dockerfile for new dependencies
* Updated Cicada to v1.3.2


Version: 2.15.0 
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: November 30, 2018
'''''''''''''''''''''''''''''''

New Features:
'''''''''''''

* Dockerfile based on the p8compute-dependencies container
* Updated Cicada to v1.3.0
* Fixed ROOT dictionary builds so they don't hard-code source-tree file locations
    * Validated by loading libraries in ROOT and in ROOT via Python


* Version: 2.14.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: October 8, 2018
'''''''''''''''''''''''''''''''

New Features:
'''''''''''''

* New processor: KTDBSCANNoiseFilter
    * Uses DBSCAN algorithm to filter noise from a sparse spectrogram
    * Validated with new test program, TestDBSCANNoiseFiltering
* New slot in KTSequentialTrackFinder: `kd-tree`
    * Validated by running the STF with an example data file
* Added variance-histogram-writing to the Basic ROOT Writer
    * Validated by testing the writer on real data
* Added `proc-track` slot to the Basic ROOT Writer
    * Validated by outputting some tracks and plotting on top of sparse spectrogram data
* Added `kd-tree-scaled` slot to the ROOT Tree Writer to scale time and frequency values to their original units
    * Validated by outputting a KD-Tree and seeing that tracks fit from that data are correctly plotted on top, so the units are right.
* Updated nanoflann to v1.3.0
* TSequentialLineData ROOT tree writer
* New cuts
    * Bin density: cut on number of points per time length in a track
    * Tested in TestSequentialTrackFinder.cc and by reprocessing raw data on dirac
    * NPoints: cut on minimum number of points in the first track of an event
* New cut options
    * NUP and SNR cuts on sequential lines, first tracks of events have the additional option to cut on total NUP / number of points in a track
    * Tested in TestSequentialTrackFinder.cc and by reprocessing raw data on dirac
* Cut classes location
    * Moved all cut classes in EventAnalysis to EventAnalysis/CutClasses
    * Tested by successfully running TestSequentialTrackFinder.cc

Fixes:
''''''

* Everything named with "DBScan" is renamed to "DBSCAN"
* Adding missing branches to TDiscriminated1D


Version: 2.13.0
~~~~~~~~~~~~~~~~~~~~~~~~~

Release Date: July 26, 2018
'''''''''''''''''''''''''''

New Features:
'''''''''''''

* Event Time Cut in Acquisition
    * Cuts events that start outside a specifiable time window within an acquisition.
    

Version: 2.12.0
~~~~~~~~~~~~~~~

Release Date: July 5, 2018
'''''''''''''''''''''''''''''''

New Features
''''''''''''

* Moving the default object name from the Katydid Writer into Cicada:
      * TMultiTrackEventData
      * TProcessedTrackData
      * TProcessedMPTData
      * TClassifierResultsData
* Continuous Integration with Travis:
      * Automatic build of libraries and validation executable
      * Test of one executable (TestRandom)
      * Slack message upon success or failure
* Track SNR and NUP:
      * Adding a KTDiscriminatedPoint structure that would be common to data objects using points obtained by discrimination.
      * Propagating the new structure to SparseWaterfallCandidateData and related classes (tested with TestDBScanTrackClustering)
      * KTSpectrumDiscriminator and KTVariableSpectrumDiscriminator: new member variables of KTDiscriminatedPoints1DData are calculated and set.
      * KTTrackProcessing: split the KTProcessingTrack processor into two processors: KTTrackProcessingDoubleCuts and KTTrackProcessingWeightedSlope.
      * KTTrackProcessingWeightedSlope handles both SequentialLineData and SparseWaterfallCandidateData; while KTTrackProcessingDoubleCuts only connects to SparseWaterfallCandidateData (with HoughData).
      * Adding new track properties to the KTProcessedTrackData result (Tested with TestTrackProcessing).
      * KTMultiTrackEventData and KTProcessedTrackData: added member variables for SNR and NUP quantities.
      * KTSequentialLineData: new version of KTSeqLine. Has SNR and NUP member variables. LineTrimming now uses SNR instead of Power.
      * KTSequentialTrackFinder: new slot for KTDiscriminatedPoints1DData only. Signal is now KTSequentialLineData.
      * KTOverlappingTrackClustering and KTIterativeTrrackClustering: new slot and singal for KTSequentialLineData. Both Processors can no longer apply cuts.
      * KTSequentialLineSNRCut and KTSequentialLineNUPCut: can be used to apply cuts on total and average SNR and NUP of KTSequentialLineData.
      * KTEventFirstTrackSNR and KTEventFirstTrackNUPCut: can be used to apply cuts on total and average SNR and NUP of KTMultiTrackEventData.
* Writers update:
      * KTSparseWaterfallCandidateData objects: TDiscriminatedPoint and TSparseWaterfallCandidateData classes have been added.


Version: 2.11.1
~~~~~~~~~~~~~~~

Release Date: June 22, 2011
''''''''''''''''''''''''''''

Fixes:
''''''

* Nymph upgraded to v1.4.5
* Commented out incorrect calculation of variance in KTGainVariationProcessor.

Version: 2.11.0
~~~~~~~~~~~~~~~

Release Date: April 25, 2018
''''''''''''''''''''''''''''

New Features:
'''''''''''''

* Added KTEventFirstTrackPowerCut
    * Cuts multi-track events with low power/length.
    * Validated by seeing that the threshold changes the number of events passing in a reasonable way.

Fixes:
''''''

* Nymph upgraded to v1.4.4 (Scarab update to v1.5.4)
* Fixed Cicada options
* Improved power calculation in the STF


Version: 2.10.1
~~~~~~~~~~~~~~~

Release Date: March 30, 2018
''''''''''''''''''''''''''''

Fixes:
''''''

* Fixed Cicada and Nymph branches that had somehow been reverted to older versions in the merges before the last commit.
* Fixed a config error in the STF

Version: 2.10.0
~~~~~~~~~~~~~~~

Release Date: March 29, 2018
''''''''''''''''''''''''''''

New Features:
'''''''''''''

* Spectrogram Striper
    * Accepts frequency spectra and groups them into stripes with a given width and stride.
    * Validation programs: TestSpectrogramStriper, TestSpectrogramStriperSwaps
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
* Spectrogram Striper
    * Accepts frequency spectra and groups them into stripes with a given width and stride.
    * Validation programs: TestSpectrogramStriper, TestSpectrogramStriperSwaps
* Update of sequential track building
    * Configurable slope method
    * Configurable number of points used by slope methods
    * Different frequency acceptance can be set for second point in line
    * Validation by processing test run (3004) with newly available settings
* Classifier and Rotate-and-Project Updates
    * Added data structure in the event builder to store the KTDataPtr associated to each track. This allows the propagation of other data objects (specifically of interest are the classifier calculations) through the event builder
    * Added classifiers built on TMVA and DLIB libraries
    * Added a slot in the ROOT Tree writer to simultaneously write tracks and classifier results within each event
    * Moved ROOT-based IO classes to submodule Cicada

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
