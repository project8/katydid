Output Formats
===============

ROOT
-----------

Katydid has a variety of ROOT writers:

 * "basic-root-writer_"
 * "root-tree-writer_"
 * "multislice-root-writer_"
 * "root-spectrogram-writer_"

To write output from katydid processors with one of the writer, the signal of the processor needs to be connected to the slot of the corresponding slot of the wirter.


.. _basic-root-writer: https://katydid.readthedocs.io/en/stable/_static/class_katydid_1_1_k_t_basic_r_o_o_t_file_writer.html
.. _root-tree-writer: https://katydid.readthedocs.io/en/stable/_static/class_katydid_1_1_k_t_r_o_o_t_tree_writer.html
.. _multislice-root-writer: https://katydid.readthedocs.io/en/stable/_static/class_katydid_1_1_k_t_multi_slice_r_o_o_t_writer.html
.. _root-spectrogram-writer: https://katydid.readthedocs.io/en/stable/_static/class_katydid_1_1_k_t_r_o_o_t_spectrogram_writer.html

*root-tree-writer*
~~~~~~~~~~~~~~~~~~~~

The *root-tree-writer* can be used to write the output from various processors in root tree format.
For example, *discriminated-points*, *processed-tracks* and *multi-peak-events* can all be written to the same root file using this writer.


*basic-root-writer*
~~~~~~~~~~~~~~~~~~~~

The *basic-root-writer* can be used to write out power spectra, gain variation, sparse spectrograms as well as output from event analysis.


HDF5
----------

**Since 2018, HDF5 is no longer supported. New data types can no longer be written in HDF5 format.**


Katydid supports writing data to the HDF5 file format, which can be opened with various Python plotting tools.  The data are structured according to their type, so that
the HDF5 file will have the following (data dependent) structure:

* Spectra - stored in /spectra
    - Frequency Spectra - stored in /spectra/frequency  
        ~ Polar frequency spectra are called PolarFS_N where N is the number of the slice which the frequency spectrum was calculated over.  The data are a 2MxN array, where N is equal to half the length of the original slice plus one, and M is equal to the number of components in the original time series data.  In memory, to retrieve frequency bin i from component j, data[j][i] is the absolute value, and data[j+i][i] is the phase (argument).
* Time Series - stored in /time_series
* Candidates - stored in /candidates

