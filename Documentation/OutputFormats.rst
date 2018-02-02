Output Formats
=============

# ROOT

Katydid has a variety of ROOT writers:
- "basic-root-writer" 

# HDF5
Katydid supports writing data to the HDF5 file format, which can be opened with various Python plotting tools.  The data are structured according to their type, so that
the HDF5 file will have the following (data dependent) structure:

* Spectra - stored in /spectra
  * Frequency Spectra - stored in /spectra/frequency
    * Polar frequency spectra are called PolarFS_N where N is the number of the slice which the frequency spectrum was calculated over.  The data are a 2MxN array, where N is equal to half the length of the original slice plus one, and M is equal to the number of components in the original time series data.  In memory, to retrieve frequency bin i from component j, data[j][i] is the absolute value, and data[j+i][i] is the phase (argument).
* Time Series - stored in /time_series
* Candidates - stored in /candidates

