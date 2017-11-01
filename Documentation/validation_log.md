# Validation Log

## Guidelines

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
  
## Template

### Version: 

#### Release Date: 

#### New Features:

* Feature 1
    * Details
* Feature 2
    * Details
  
#### Fixes:

* Fix 1
    * Details
* Fix 2
    * Details
  
## Log

### Version: *Upcoming Releases*

#### Release Date: TBD

#### New Features:

  
#### Fixes:


### Version: 2.7.2

#### Release Date: October 2, 2017

#### New Features:

* ROOT Spectrogram Writer: sequential writing mode
    * A new mode of writing was added to the writer that writes sequential spectrograms of a given time size.
    * Documentation is included in the KTROOTSpectrogramWriter header documentation.
    * The new writing mode was tested on a concatenated file from an RSA run. Sequential spectrograms split at the right times according to the setting and acquisition breaks.
    * The old writing mode ("single") continued to function in the same way.
  
#### Fixes:

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


  
