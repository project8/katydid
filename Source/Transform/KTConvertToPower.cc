/*
 * KTConvertToPower.cc
 *
 *  Created on: nsoblath
 *      Author: Aug 1, 2014
 */

#include "KTConvertToPower.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTChirpSpaceFFT.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTChirpSpaceDataFFT.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTChannelAggregatedData.hh"
#include "KTLogger.hh"

#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(pslog, "katydid.fft");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTConvertToPower, "convert-to-power");

    KTConvertToPower::KTConvertToPower(const std::string& name) :
            KTProcessor(name),
            fFSPToPSSlot("fs-polar-to-ps", this, &KTConvertToPower::ToPowerSpectrum, &fPowerSpectrumSignal),
            fFSPToPSDSlot("fs-polar-to-psd", this, &KTConvertToPower::ToPowerSpectralDensity, &fPowerSpectralDensitySignal),
            fFSFToPSSlot("fs-fftw-to-ps", this, &KTConvertToPower::ToPowerSpectrum, &fPowerSpectrumSignal),
            fFSFToPSDSlot("fs-fftw-to-psd", this, &KTConvertToPower::ToPowerSpectralDensity, &fPowerSpectralDensitySignal),
	    fCSFToPSDSlot("fs-fft-to-psd", this, &KTConvertToPower::ToPowerSpectralDensity, &fPowerSpectralDensitySignal),
            fAggFSFToPSSlot("aggfs-fftw-to-ps", this, &KTConvertToPower::ToPowerSpectrum, &fPowerSpectrumSignal),
            fAggFSFToPSDSlot("aggfs-fftw-to-psd", this, &KTConvertToPower::ToPowerSpectralDensity, &fPowerSpectralDensitySignal),
            fPSDToPSSlot("psd-to-ps", this, &KTConvertToPower::ToPowerSpectrum, &fPowerSpectrumSignal),
            fPSToPSDSlot("ps-to-psd", this, &KTConvertToPower::ToPowerSpectralDensity, &fPowerSpectralDensitySignal),
            fPowerSpectrumSignal("ps", this),
            fPowerSpectralDensitySignal("psd", this)
    {
    }

    KTConvertToPower::~KTConvertToPower()
    {
    }

    bool KTConvertToPower::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        // no parameters

        return true;
    }

    bool KTConvertToPower::ToPowerSpectrum(KTFrequencySpectrumDataPolar& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTPowerSpectrumData& psData = data.Of< KTPowerSpectrumData >().SetNComponents(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent)->CreatePowerSpectrum();
            spectrum->ConvertToPowerSpectrum();
            psData.SetSpectrum(spectrum, iComponent);
        }
        return true;
    }
    
    bool KTConvertToPower::ToPowerSpectralDensity(KTFrequencySpectrumDataPolar& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTPowerSpectrumData& psData = data.Of< KTPowerSpectrumData >().SetNComponents(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent)->CreatePowerSpectrum();
            spectrum->ConvertToPowerSpectralDensity();
            psData.SetSpectrum(spectrum, iComponent);
        }
        return true;
    }

    bool KTConvertToPower::ToPowerSpectrum(KTFrequencySpectrumDataFFTW& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTPowerSpectrumData& psData = data.Of< KTPowerSpectrumData >().SetNComponents(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent)->CreatePowerSpectrum();
            spectrum->ConvertToPowerSpectrum();
            psData.SetSpectrum(spectrum, iComponent);
        }
        return true;
    }

    bool KTConvertToPower::ToPowerSpectralDensity(KTFrequencySpectrumDataFFTW& data)
    {
	printf("Entering ToPowerSpectralDensity (DataFFTW)\n");
        unsigned nComponents = data.GetNComponents();
        KTPowerSpectrumData& psData = data.Of< KTPowerSpectrumData >().SetNComponents(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent)->CreatePowerSpectrum();
            spectrum->ConvertToPowerSpectralDensity();
            psData.SetSpectrum(spectrum, iComponent);
        }
        return true;
    }
   
    bool KTConvertToPower::ToPowerSpectralDensity(KTChirpSpaceDataFFT& data)
    {
        printf("Entering ToPowerSpectralDensity (ChirpSpaceDataFFT)\n");
        unsigned nComponents = data.GetNComponents();
	printf("ToPowerSpectralDensity Check A\n");
        KTPowerSpectrumData& psData = data.Of< KTPowerSpectrumData >().SetNComponents(nComponents);
	printf("ToPowerSpectralDensity Check B\n");
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
	    printf("For loop check B.1, %d / %d\n", iComponent+1, nComponents);
	    //KTPowerSpectrum* TestSpectrum = data.GetSpectrum(iComponent,1);
	    //printf("For loop check B.1.1, %d / %d\n", iComponent+1, nComponents);
	    printf("Spectrum Time Bins: %d\n", data.GetSpectrum(0,0)->GetNTimeBins());
            KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent,0)->CreatePowerSpectrum();
	    printf("For loop check B.2, %d / %d\n", iComponent+1, nComponents);
            spectrum->ConvertToPowerSpectralDensity();
	    printf("For loop check B.3, %d / %d\n", iComponent+1, nComponents);
            psData.SetSpectrum(spectrum, iComponent);
	    printf("For loop check B.4, %d / %d\n", iComponent+1, nComponents);
        }
	printf("Exiting ToPowerSpectralDensity (ChirpSpaceDataFFT)\n");
        return true;
    }


 
    bool KTConvertToPower::ToPowerSpectrum(KTAggregatedFrequencySpectrumDataFFTW& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTAggregatedPowerSpectrumData& psData = data.Of< KTAggregatedPowerSpectrumData >().SetNComponents(nComponents);
        double activeRadius=data.GetActiveRadius();
        psData.SetActiveRadius(activeRadius);
        psData.SetNAxialPositions(data.GetNAxialPositions());
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent)->CreatePowerSpectrum();
            spectrum->ConvertToPowerSpectrum();
            psData.SetSpectrum(spectrum, iComponent);
            double gridLocationX, gridLocationY, gridLocationZ;
            data.GetGridPoint(iComponent, gridLocationX, gridLocationY, gridLocationZ);
            psData.SetGridPoint(iComponent, gridLocationX, gridLocationY, gridLocationZ);
            psData.SetSummedGridPower(iComponent,*(std::max_element(spectrum->begin(), spectrum->end())));
        }
        psData.SetOptimizedGridPointValue(data.GetOptimizedGridPoint(),data.GetOptimizedGridValue());
        return true;
    }
    
    bool KTConvertToPower::ToPowerSpectralDensity(KTAggregatedFrequencySpectrumDataFFTW& data)
    {
        unsigned nComponents = data.GetNComponents();
        KTAggregatedPowerSpectrumData& psData = data.Of< KTAggregatedPowerSpectrumData >().SetNComponents(nComponents);
        double activeRadius=data.GetActiveRadius();
        psData.SetActiveRadius(activeRadius);
        psData.SetNAxialPositions(data.GetNAxialPositions());
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* spectrum = data.GetSpectrum(iComponent)->CreatePowerSpectrum();
            spectrum->ConvertToPowerSpectralDensity();
            psData.SetSpectrum(spectrum, iComponent);
            double gridLocationX, gridLocationY,gridLocationZ;
            data.GetGridPoint(iComponent,gridLocationX,gridLocationY,gridLocationZ);
            psData.SetGridPoint(iComponent,gridLocationX,gridLocationY,gridLocationZ);
            psData.SetSummedGridPower(iComponent,*(std::max_element(spectrum->begin(), spectrum->end())));
        }
        psData.SetOptimizedGridPointValue(data.GetOptimizedGridPoint(),data.GetOptimizedGridValue());
        return true;
    }

    bool KTConvertToPower::ToPowerSpectrum(KTPowerSpectrumData& data)
    {
        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            data.GetSpectrum(iComponent)->ConvertToPowerSpectrum();
        }
        return true;
    }

    bool KTConvertToPower::ToPowerSpectralDensity(KTPowerSpectrumData& data)
    {
	printf("Entering ToPowerSpectralDensity (Data)\n");
        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            data.GetSpectrum(iComponent)->ConvertToPowerSpectralDensity();
        }
        return true;
    }

} /* namespace Katydid */
