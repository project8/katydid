# Tutorial -- Low-Pass Filter Processor

## Setup
1. Copy KTProcessorTemplate.hh/cc files to the appropriate location (Source/SpectrumAnalysis) and new filenames.
2. Copy a data class files (e.g. KTNormalizedFSData.hh/cc) to the appropriate location (Source/Data/SpectrumAnalysis) and new filenames.


## Data
1. Create classes for FS-polar, FS-fftw, and PS. If KTNormalizedFSData was copied, this can be done by text-replace.
2. Fix inclusion guard.
3. Fix class-name documentation.

## Processor
1. Fix inclusion guard. (.hh)
2. Fix class name and class-name documentation. (.hh and .cc)
3. Fix config name. (.hh and .cc)
4. Add brief description. (.hh)
5. Add mean and sigma parameters using MEMBERVARIABLE.  Add KTMemberVariable.hh inclusion. (.hh and .cc)
6. Implement Configure function to set mean and sigma. (.hh and .cc)
7. Create and implement Convolve functions: (.hh and .cc)
    ```
    bool Filter(KTFrequencySpectrumDataPolar& fsData);
    bool Filter(KTFrequencySpectrumDataFFTW& fsData);
    bool Filter(KTPowerSpectrumData& psData);
    ```
8. Add signal/slot interface. (.hh and .cc)
9. Finish documentation. (.hh)