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
    	void ProcessEggHeader();

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

    	unsigned fNComponents;
    	unsigned fSliceSize;

    	H5::Group* fFFTGroup;
    	H5::Group* fPowerGroup;

    	/*
    	 * power FS and PS
    	 */
    	unsigned fPolarFFTSize;
    	fft_buffer* fPolarFFTBuffer;
    	H5::DataSpace* fPolarFFTDSpace;

    	unsigned fPolarPwrSize;
    	fft_buffer* fPolarPwrBuffer;
    	H5::DataSpace* fPolarPwrDSpace;

    	/* 
    	 * complex (FFTW) FS and PS
    	 */
    	unsigned fCmplxFFTSize;
    	fft_buffer* fCmplxFFTBuffer;
    	H5::DataSpace* fCmplxFFTDSpace;

    	unsigned fCmplxPwrSize;
    	fft_buffer* fCmplxPwrBuffer;
    	H5::DataSpace* fCmplxPwrDSpace;

        /*
         * Power spectrum and Power spectral density
         */
        unsigned fPwrSpecSize;
        fft_buffer* fPwrSpecBuffer;
        H5::DataSpace* fPwrSpecDSpace;

        unsigned fPSDSize;
        fft_buffer* fPSDBuffer;
        H5::DataSpace* fPSDDSpace;

        /* 
         * Group for spectral data
         */
        H5::Group* fSpectraGroup;
    };
}

#endif  // __KTHDFTWFFT_HH
