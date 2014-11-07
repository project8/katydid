# Tutorial -- Low-Pass Filter Processor

## Get Katydid
1. Clone the git repo: git clone https://github.com/project8/katydid.git
2. Download the submodules: git submodule update --init --recursive
2.5. Checkout the tutorial branch: git checkout Tutorial-Complete
3. Create a branch to work in: git branch tutorial
4. Checkout the branch: git checkout tutorial


## Setup
1. Copy KTProcessorTemplate.hh/cc files to the appropriate location (Source/SpectrumAnalysis) and new filenames.
2. Copy a data class files (e.g. KTNormalizedFSData.hh/cc) to the appropriate location (Source/Data/SpectrumAnalysis) and new filenames.


## Data
1. Create classes for FS-polar, FS-fftw, and PS. If KTNormalizedFSData was copied, this can be done by text-replace.
2. Fix inclusion guard.
3. Fix class-name documentation.
4. Add the class to the CMakeLists.txt in Source/Data.


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
10. Add the class to the CMakeLists.txt file in Source/Data/SpectrumAnalysis.


## Writer
1. In Source/IO/KTBasicROOTTypeWriterAnalysis.hh, copy WriteNormalized[FSDataPolar/FSDataFFTW/PSData] to WriteLowPassFiltered[FSDataPolar/FSDataFFTW/PSData]
2. Do the same copying in the .cc file.
3. Change the class names in the functions appropriately.
4. Add a #include line for the data class header.
5. Add Register lines for the new functions (i.e. copy and modify the relevant WriteNormalized lines)


## Build Katydid
1. Create a build directory: mkdir build-tutorial
2. Enter that directory: cd build-tutorial
3. Run ccmake: ccmake ..
4. Change the build type to DEBUG
5. Configure by pressing 'c'
6. Generate the build files and exit by pressing 'g'
7. Build Katydid: make install
   To speed the build process, if you have multiple cores, you can add a -j[# cores] argument.


## Config
1. Copy Examples/ConfigFiles/KatydidPSConfig.json to a working directory.
2. Add the creation of the LPF processor.
3. Connect the processor between the FFT and the writer.
4. Add the configuration of the processor.


## Run Katydid
1. Customize the input and output filenames.
2. Katydid -c [config file]