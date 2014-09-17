/*
 * KTHDF5TypeWriterFFT.hh
 *
 *  Created on: 9/12/2014
 *      Author: J.N. Kofron
 * 
 */

#ifndef __KTHDFTWFFT_HH
#define __KTHDFTWFFT_HH

#include "KTHDF5Writer.hh"
#include "KTEggHeader.hh"
#include "KTData.hh"

#include "boost/multi_array.hpp"

namespace Katydid {
	class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTMultiFSDataPolar;
    class KTMultiFSDataFFTW;
    class KTPowerSpectrumData;

    class KTHDF5TypeWriterFFT: public KTHDF5TypeWriter {
    /*
     * Constructors and Destructors
     */
    public:
    	KTHDF5TypeWriterFFT();
    	virtual ~KTHDF5TypeWriterFFT();

    	void RegisterSlots();

    /*
     * Internal configuration
     */
    public:
    	void ProcessEggHeader(KTEggHeader* header);

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
    	typedef boost::multi_array<double, 2> fft_buffer;
    	H5::DataSet* CreatePolarFFTDSet(const std::string& name);
    	H5::DataSet* CreatePolarPowerDSet(const std::string& name);
    	H5::DataSet* CreateComplexFFTDSet(const std::string& name);
    	H5::DataSet* CreateComplexPowerDSet(const std::string& name);
        H5::DataSet* CreatePowerSpecDSet(const std::string& name);
        H5::DataSet* CreatePSDDSet(const std::string& name);
    	H5::DataSet* CreateDSet(const std::string& name, 
    							const H5::Group* grp,
    							const H5::DataSpace& ds);

    	void CreateDataspaces();

    	unsigned n_components;
    	unsigned slice_size;

    	H5::Group* fft_group;
    	H5::Group* power_group;

    	/*
    	 * power FS and PS
    	 */
    	unsigned polar_fft_size;
    	fft_buffer* polar_fft_buffer;
    	H5::DataSpace* polar_fft_dspace;

    	unsigned polar_pwr_size;
    	fft_buffer* polar_pwr_buffer;
    	H5::DataSpace* polar_pwr_dspace;

    	/* 
    	 * complex (FFTW) FS and PS
    	 */
    	unsigned cmplx_fft_size;
    	fft_buffer* cmplx_fft_buffer;
    	H5::DataSpace* cmplx_fft_dspace;

    	unsigned cmplx_pwr_size;
    	fft_buffer* cmplx_pwr_buffer;
    	H5::DataSpace* cmplx_pwr_dspace;

        /*
         * Power spectrum and Power spectral density
         */
        unsigned pwr_spec_size;
        fft_buffer* pwr_spec_buffer;
        H5::DataSpace* pwr_spec_dspace;

        unsigned psd_size;
        fft_buffer* psd_buffer;
        H5::DataSpace* psd_dspace;

        /* 
         * Group for spectral data
         */
        H5::Group* spectra_group;
    };
}

#endif  // __KTHDFTWFFT_HH
