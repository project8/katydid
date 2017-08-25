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

* ROOT Spectrogram Writer: sequential writing mode
  * A new mode of writing was added to the writer that writes sequential spectrograms of a given time size.
  * Documentation is included in the KTROOTSpectrogramWriter header documentation.
  * The new writing mode was tested on a concatenated file from an RSA run. Sequential spectrograms split at the right times according to the setting and acquisition breaks.
  * The old writing mode ("single") continued to function in the same way.
  
#### Fixes:

* Fix 1
  * Details
* Fix 2
  * Details
  
