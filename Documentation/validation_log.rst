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

Version: x.y.z
~~~~~~~~~~~~~~

Release Date: ?????????????????
'''''''''''''''''''''''''''''''

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
        Propagating the new structure to SparseWaterfallCandidateData and related classes (tested with TestDBScanTrackClustering)
      * KTTrackProcessing: split the KTProcessingTrack processor into two processors: KTTrackProcessingDoubleCuts and KTTrackProcessingWeightedSlope.
        KTTrackProcessingWeightedSlope handles both SequentialLineData and SparseWaterfallCandidateData; while KTTrackProcessingDoubleCuts only connects to SparseWaterfallCandidateData (with HoughData).
        Adding new track properties to the KTProcessedTrackData result (Tested with TestTrackProcessing).
      * KTMultiTrackEventData and KTProcessedTrackData: added member variables for SNR and NUP quantities.
      * KTSequentialLineData: new version of KTSeqLine. Has SNR and NUP member variables. LineTrimming now uses SNR instead of Power.
      * KTSequentialTrackFinder: new slot for KTDiscriminatedPoints1DData only. Signal is now KTSequentialLineData.
      * KTOverlappingTrackClustering and KTIterativeTrrackClustering: new slot and singal for KTSequentialLineData. Both Processors can no longer apply cuts.
      * KTSequentialLineSNRCut and KTSequentialLineNUPCut: can be used to apply cuts on total and average SNR and NUP of KTSequentialLineData.
* Writers update:
      * KTSparseWaterfallCandidateData objects: TDiscriminatedPoint and TSparseWaterfallCandidateData classes have been added.

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


  
