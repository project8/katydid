# Tutorial -- Low-Pass Filter Processor

For this tutorial, your goal is to add to Katydid the ability to apply a low-pass filter to frequency-domain data.  You will need to create the processor that performs the filtering, and data objects that will represent post-filtered data.  Your processor should be able to act on 3 data types: KTFrequencySpectrumDataPolar, KTFrequencySpectrumDataFFTW, and KTPowerSpectrumData.

## Get Katydid
1. Clone the git repo: git clone --recursive https://github.com/project8/katydid.git
2. cd katydid
3. Create a branch for your development work: git checkout -b tutorial


## Build Katydid
1. Create a build directory: mkdir build-tutorial
2. Enter that directory: cd build-tutorial
3. Run ccmake: ccmake ..
4. Run the initial configuration step by pressing 'c'
5. Change the build type to DEBUG
6. Configure by pressing 'c'
7. Generate the build files and exit by pressing 'g'
8. Build Katydid: make install
   To speed the build process, if you have multiple cores, you can add a -j argument.

## Development Setup
1. Copy KTProcessorTemplate.hh/cc to the Source/SpectrumAnalysis directory and rename them to create your new processor. Generally the file name should describe the function of the processor (e.g. KTLowPassFilter.hh/cc).
2. Copy the data class files KTFrequencySpectrumDataFFTW.hh/cc, KTFrequencySpectrumDataPolar.hh/cc, and KTPowerSpectrumData.hh/cc in the Source/Data/Transform directory to Source/Data/SpectrumAnalysis to create your new data types. Again, rename them to something which indicates they will represent data that has been processed by the low-pass filter.
3. Build again, to make sure there are no errors. 


## Data
1. Create classes for F-polar, FS-fftw, and PS. If KTNormalizedFSData was copied, this can be done by text-replace.
2. Fix inclusion guard.
3. Fix class-name documentation.
4. Add the class to the CMakeLists.txt in Source/Data.


## Processor
1. Fix inclusion guard. (.hh)
2. Fix class name and class-name documentation. (.hh and .cc)
3. Fix config name. (.hh and .cc)
4. Add brief description. (.hh)
5. Add the time constant parameter using MEMBERVARIABLE (or you can write the variable declaration, and getter and setter methods).  Add KTMemberVariable.hh inclusion. (.hh and .cc)
6. Implement Configure function to set the time constant. (.hh and .cc)
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


## Config
1. Copy Examples/ConfigFiles/KatydidPSConfig.json to a working directory.
2. Add the creation of the LPF processor.
3. Connect the processor between the FFT and the writer.
4. Add the configuration of the processor.


## Run Katydid
1. Customize the input and output filenames.
2. Katydid -c [config file]
