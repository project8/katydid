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
#include "KTData.hh"

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
    };
}

#endif  // __KTHDFTWFFT_HH
