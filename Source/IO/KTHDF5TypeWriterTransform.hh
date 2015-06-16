/*
 * KTHDF5TypeWriterTransform.hh
 *
 *  Created on: 9/12/2014
 *      Author: J.N. Kofron
 * 
 */

#ifndef __KTHDF5TYPEWRITERTRANSFORM_HH
#define __KTHDF5TYPEWRITERTRANSFORM_HH

#include "KTHDF5Writer.hh"
#include "KTEggHeader.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"

#include "boost/multi_array.hpp"

namespace Katydid {
	class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTMultiFSDataPolar;
    class KTMultiFSDataFFTW;
    class KTPowerSpectrumData;

    class KTHDF5TypeWriterTransform: public KTHDF5TypeWriter {
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
        MEMBERVARIABLE(bool, FirstSliceHasBeenWritten);
        MEMBERVARIABLE(bool, CompressFFTFlag);

   	/*
   	 * Frequency Spectrum Data
   	 */
	public:
        void WriteFrequencySpectrumDataPolar(KTDataPtr data);
        void WriteFrequencySpectrumDataFFTW(KTDataPtr data);
        void WriteFrequencySpectrumDataPolarPhase(KTDataPtr data);
        void WriteFrequencySpectrumDataFFTWPhase(KTDataPtr data);
        void WriteFrequencySpectrumDataPolarPower(KTDataPtr data);
        void WriteFrequencySpectrumDataFFTWPower(KTDataPtr data);
        void WriteFrequencySpectrumDataPolarMagnitudeDistribution(KTDataPtr data);
        void WriteFrequencySpectrumDataFFTWMagnitudeDistribution(KTDataPtr data);
        void WriteFrequencySpectrumDataPolarPowerDistribution(KTDataPtr data);
        void WriteFrequencySpectrumDataFFTWPowerDistribution(KTDataPtr data);

        /*
         * Power Spectrum Data
         */
        void WritePowerSpectrum(KTDataPtr data);
        void WritePowerSpectralDensity(KTDataPtr data);
        void WritePowerSpectrumDistribution(KTDataPtr data);
        void WritePowerSpectralDensityDistribution(KTDataPtr data);


        /*
         * TF Data
         */
        void WriteTimeFrequencyDataPolar(KTDataPtr data);
        void WriteTimeFrequencyDataPolarPhase(KTDataPtr data);
        void WriteTimeFrequencyDataPolarPower(KTDataPtr data);


        /*
		 * Multispectral data
         */
    public:
        void WriteMultiFSDataPolar(KTDataPtr data);
        void WriteMultiFSDataFFTW(KTDataPtr data);

    /*
     * Internal data members
     */
    private:
    	typedef boost::multi_array<double, 4> fft_buffer;
        typedef boost::multi_array<double, 1> freq_buffer;
    	void PrepareHDF5File(const std::string& SlotName);
    	H5::DataSet* CreateDSet(const std::string& name,
    							const H5::Group* grp,
    							const H5::DataSpace& ds);

    	unsigned fNChannels;
        unsigned fNComponents;
        unsigned fNumberOfSlices;
        unsigned fSliceSize;

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
