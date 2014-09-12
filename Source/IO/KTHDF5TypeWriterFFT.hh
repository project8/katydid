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
    	H5::DataSet* CreatePolarFFTDSet(const std::string& name);

    	void CreateDataspaces();

    	unsigned n_components;
    	unsigned slice_size;

    	H5::Group* fft_group;

    	unsigned polar_fft_size;
    	boost::multi_array<double, 2>* polar_fft_buffer;
    	H5::DataSpace* polar_fft_dspace;
    };
}

#endif  // __KTHDFTWFFT_HH
