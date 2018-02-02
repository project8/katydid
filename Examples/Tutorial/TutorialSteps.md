# Tutorial -- Low-Pass Filter Processor

For this tutorial, your goal is to add to Katydid the ability to apply a low-pass filter to frequency-domain data.  You will need to create the processor that performs the filtering, and data objects that will represent post-filtered data.  Your processor should be able to act on 3 data types: KTFrequencySpectrumDataPolar, KTFrequencySpectrumDataFFTW, and KTPowerSpectrumData. For this type of processor, the output and input data types are going to be virtually identical. However, due to the current design of the software, the output types must differ from the input types. Consequently, the processor will output three new types you have to create.

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
1. Copy KTProcessorTemplate.hh/cc to the Source/SpectrumAnalysis directory and rename them to create our new processor. Generally the file name should describe the function of the processor (e.g. KTLowPassFilter.hh/cc).
2. Copy the data class files KTFrequencySpectrumDataFFTW.hh/cc, KTFrequencySpectrumDataPolar.hh/cc, and KTPowerSpectrumData.hh/cc in the Source/Data/Transform directory to Source/Data/SpectrumAnalysis to create our new data types. Again, rename them to something which indicates they will represent data that has been processed by the low-pass filter.
3. Build again, to make sure there are no errors. 

## Data
The data classes that we have just copied each contain essentially the same structure. There is a Core class, which is declared in the header file and contains the spectrum object as well as public methods to Get and Set any component of the spectrum. Then, there is the actual data class which inherits from the appropriate Core data class and contains only a SetNComponents method.

The low-pass filtered data will have exactly the same structure as these spectrum data classes, so it will be sufficient to make them inherit from the Core classes just the same. Obviously we do not need to redefine the Core data classes, so in each of the new data classes we should:

1. Remove the Core data class and all its inline functions from the header file, and simply add an inclusion at the top (e.g `#include KTFrequencySpectrumDataFFTW.hh`).
2. Replace every instance of the class name with the new class name you have chosen. This can be (almost completely) done by text-replace once the Core data class lines have been removed. Remember that the class must still inherit from the Core data class with the old name.
3. Similarly, remove the Core data class constructor/destructor from the source file of each new data class and replace the class name with our low-pass filtered data class names.
4. Replace the definition lines at the top of the header file (e.g. `#ifndef KTFREQUENCYSPECTRUMFFTW_HH_`) with the new class name.

At this point, we should have a working set of new spectrum data classes which inherit from the Core classes in the same way as our input data. Add the new data classes to the CMakeLists.txt file in Source/Data and re-build to ensure the data classes were created without error.

## Processor
The processor template contains dummy objects for each of the essential components of a processor: member variables, slots, and signals. Member variables are parameters of the processor which are often (but not always) configurable at run-time with the configuration file. Slots will determine the type of input data that the processor is allowed to accept, and signals will determine the output data types. We have to replace the dummy objects here with the slots, signals, and member variables that we will need for a low-pass filter.

### Slots and Signals
As we have already discussed, there will be 3 slots and 3 signals corresponding to the 3 types of input and output data. Change the names such as dummy-slot-1, etc. everywhere they appear to reflect the data types that they will be handling. The slots themselves are declared in the header file at the end of the class definition, and here we must specify the real input data class for each slot, for example:

`Nymph::KTSlotDataOneType< KTFrequencySpectrumDataFFTW > fFrequencySpectrumFFTWSlot;`

The slots are initialized in the source file, and in their initialization they each point to a method. This is the method that will be executed when the processor receives its input data to this slot; it is where the "meat" of the processor code will go. First, simply change the method names and the input data classes to something less dumb. We will finish setting up the framework of the processor before we move on to developing these methods.

Although the signals emit the output, explicitly specifying the output type when declaring or initialising the signals is not necessary. Nevertheless, the slots must be connected to their respective signal objects upon initialisation. This is achieved by providing a fourth argument to the slot constructor, which is the address of the signal, as can be seen in the following example:
`fSlot("slot", this, &KTFilter::Filter, &fSignal)`

### Member Variables
For a low-pass filter we need only one configurable parameter, which is the time constant. Thus, we can remove from the processor template anything to do with the 2nd and 3rd dummy member variables, and replace the lines to do with MemberVariable1 with something to represent the time constant. Member variables are parsed from the configuration file in the Configure method; make sure to also edit this method to use the more appropriate name you've chosen for the time constant.

At this point, add the processor to the CMakeLists.txt file in Source/SpectrumAnalysis, and re-build to check for errors and debug. If the build fails, make sure that you have included the headers corresponding to the three input data types in the processor header. Indeed, the types of the slots depend on those, and the compiler will not be able to build your processor if it does not know the memory layout of its slots. Now we will move on to the actual function of the processor.

### Slot Method
By now, we have developed all of the framework for the processor and we are ready to implement the actual low-pass filter. This will happen separately for each data type, in the methods which you have assigned to each slot in their initializations. The actual code at this point, finally, is up to you to write without explicit guidance. However, there are some things the method needs to accomplish in a specific way:

1. Create the new data type and append it to the list of objects contained in the KTDataPtr. This is accomplished by calling the `.Of< NewDataType >` method on the initial data object (the argument to the slot method), e.g.:

`KTLowPassFilteredDataFSFFTW& newData = data.Of< KTLowPassFilteredDataFSFFTW >();`

2. Set `NComponents` of the new data object to the same as that of the old data object.
3. Loop through the components of the incoming data and operate on an individual spectrum using the `GetSpectrum` methods to obtain an initial spectrum and `SetSpectrum` to assign one to the new data.

Since the individual spectrum objects are contained in the Core data classes, these will be the same type for both the initial and filtered data.

## Writer
The last step is to add the ability to write our new data class, so we can see the output of our new processor. We will once again use what exists already for the input data as a starting point; the writers for these classes are located in Source/IO/BasicROOTFileWriter/KTBasicROOTTypeWriterTransform.hh/cc.

In the header file, we must add a declaration of a method for each of the three data types we'd like to write, in the same fashion as the numerous other methods already listed. The argument to these methods is a KTDataPtr, because they will serve as the direct slot functions.

In the source file, we must register the new slots in the constructor and create the slot methods. These can be almost exactly the same as the methods already in place; that is, copy the slot method for `KTFrequencySpectrumDataFFTW`, change instances of the data object accordingly (but the spectrum objects can remain the same), and then do the same for the other two data types. 

## Configure and Run Katydid
At last, we are ready to run Katydid and make use of our new processor. There is a configuration file in this directory which is already set up to perform a simple Fourier transform and low-pass filter; simply take the following steps to adapt this config file for your build and run it:

1. Copy the config file to a working directory outside of the Katydid repo folder
2. Change the strings which describe the filter processor if they do not match the choices you have made: the processor name `low-pass-filter`, the slot name `ps`, the signal name `ps` (for these two you should use the power spectrum signal/slot names), the writer slot name `ps`, and the reciprocal time constant `rc`.
3. Specify an input egg file in configuration of the egg processor.
4. Run Katydid with this configuration file: `Katydid -c LPFConfig.json`
