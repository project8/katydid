/*
 * KTHDF5TypeWriterTransform.hh
 @details

Saves FFTs and Power Spectra to HDF5 files.

All FFTs from a given Egg or Mat file are saved inside a single HDF5 dataset (instead of one dataset per slice).

The dataset will have 4 dimensions:
1 - Number of channels (sometimes called fNComponents in other modules)
2 - Number of slices
3 - Number of parts (real-only: 1;  real and imaginary: 2;  polar amplitude and angle: 2)
4 - Number of samples in slice

The slot determines what FFT or Power Spectrum will be written to file.
it’s one unique slot for each possible output:
fs-fftw -> complex FFT
fs-polar -> polar FFT
ps -> Power Spectrum
psd -> PSD
etc...

Inputs:
output-file: full path with name for output file.  Preferred extension is h5.
use-compression: true or false (default: false).  Uses the built-in hdf5 compression.

Example:
    ...
    { "type": "hdf5-writer",   "name": "h5w" },
    ...
    {
        "signal": "fft:fft",
        "slot": "h5w:fs-fftw"
    }
    ...
    "h5w":
    {
        "output-file": "/temp/fft_20150523T1206_TopCoil1p0A_CF1050MHz_000000001_to_000000014.h5",
        "use-compression": true
    }


 @author: J.N. Kofron, 
 @date: 9/12/2014
 @edited: 6/17/2015, L. de Viveiros
 */


#ifndef __KTHDF5TYPEWRITERTRANSFORM_HH
#define __KTHDF5TYPEWRITERTRANSFORM_HH

#include "KTHDF5Writer.hh"
#include "KTEggHeader.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"

#include "boost/multi_array.hpp"

namespace Katydid
{

	class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTMultiFSDataPolar;
    class KTMultiFSDataFFTW;
    class KTPowerSpectrumData;

    class KTHDF5TypeWriterTransform: public KTHDF5TypeWriter
    {
    /*
     * Constructors and Destructors
     */
    public:
    	KTHDF5TypeWriterTransform();
    	virtual ~KTHDF5TypeWriterTransform();

    	void RegisterSlots();

    /*
     * Internal configuration
     */
    public:
    	void ProcessEggHeader();
        void PrepareHDF5File();
        MEMBERVARIABLE(bool, FirstSliceHasBeenWritten);
        MEMBERVARIABLE(bool, UseCompressionFlag);

   	/*
   	 * Frequency Spectrum Data
   	 */
	public:
        void WriteFrequencySpectrumDataPolar(Nymph::KTDataPtr data);
        void WriteFrequencySpectrumDataFFTW(Nymph::KTDataPtr data);
        //void WriteFrequencySpectrumDataPolarPhase(Nymph::KTDataPtr data);
        //void WriteFrequencySpectrumDataFFTWPhase(Nymph::KTDataPtr data);
        void WriteFrequencySpectrumDataPolarPower(Nymph::KTDataPtr data);
        void WriteFrequencySpectrumDataFFTWPower(Nymph::KTDataPtr data);
        //void WriteFrequencySpectrumDataPolarMagnitudeDistribution(Nymph::KTDataPtr data);
        //void WriteFrequencySpectrumDataFFTWMagnitudeDistribution(Nymph::KTDataPtr data);
        //void WriteFrequencySpectrumDataPolarPowerDistribution(Nymph::KTDataPtr data);
        //void WriteFrequencySpectrumDataFFTWPowerDistribution(Nymph::KTDataPtr data);

        /*
         * Power Spectrum Data
         */
        void WritePowerSpectrum(Nymph::KTDataPtr data);
        void WritePowerSpectralDensity(Nymph::KTDataPtr data);
        //void WritePowerSpectrumDistribution(Nymph::KTDataPtr data);
        //void WritePowerSpectralDensityDistribution(Nymph::KTDataPtr data);


        /*
         * TF Data
         */
        //void WriteTimeFrequencyDataPolar(Nymph::KTDataPtr data);
        //void WriteTimeFrequencyDataPolarPhase(Nymph::KTDataPtr data);
        //void WriteTimeFrequencyDataPolarPower(Nymph::KTDataPtr data);


        /*
		 * Multispectral data
         */
        //void WriteMultiFSDataPolar(Nymph::KTDataPtr data);
        //void WriteMultiFSDataFFTW(Nymph::KTDataPtr data);

    /*
     * Internal data members
     */
    private:
    	typedef boost::multi_array<double, 4> fft_buffer;
        typedef boost::multi_array<double, 1> freq_buffer;
    	H5::DataSet* CreateDSet(const std::string& name, const H5::Group* grp, const H5::DataSpace& ds);

    	unsigned fNChannels;
        unsigned fNParts;
        unsigned fNumberOfSlices;
        unsigned fSliceSize;
        unsigned fSliceNumber;
        std::string fSpectrumName;

        H5::Group* fSpectraGroup;
    	H5::Group* fFFTGroup;
        H5::DataSet* fDSet;
        H5::DataSet* fDSet_FreqArray;

    	unsigned fFFTSize;
    	fft_buffer* fFFTBuffer;
    	H5::DataSpace* fFFTDataSpace;

        // Frequency Arrays (the "X-Axis" of the FFT)
        freq_buffer* fFFTFreqArrayBuffer;
        H5::DataSpace* fFFTFreqArrayDataSpace;

        // Dataspace dimensions
        hsize_t ds_dims[4];
        hsize_t ds_maxdims[4];

    };
}

#endif  // __KTHDF5TYPEWRITERTRANSFORM_HH
