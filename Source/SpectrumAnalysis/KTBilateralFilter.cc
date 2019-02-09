/*
 * KTBilateralFilter.cc
 *
 *  Created on: Mar 7, 2018
 *      Author: N. Buzinsky
 */

#include "KTBilateralFilter.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"

#include "KTBilateralFilteredData.hh"

#include "KTLogger.hh"
#include "KTMath.hh"

#include "KTMultiFSDataFFTW.hh"
#include "param.hh"

#include <iostream>
#include <algorithm>

namespace Katydid
{
    KTLOGGER(gclog, "KTBilateralFilter");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTBilateralFilter, "bilateral-filter");

    KTBilateralFilter::KTBilateralFilter(const std::string& name) :
            KTProcessor(name),
            fSigmaPixels(2.),
            fSigmaRange(0.05),
            fFSFFTWSignal("fs-fftw", this),
            fFSFFTWSlot("str-fs-fftw", this, &KTBilateralFilter::Filter, &fFSFFTWSignal)
    {
    }

    KTBilateralFilter::~KTBilateralFilter()
    {
    }

    bool KTBilateralFilter::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSigmaPixels(node->get_value("sigma-pixels", GetSigmaPixels()));
        SetSigmaRange(node->get_value("sigma-range", GetSigmaRange()));

        return true;
    }

    bool KTBilateralFilter::Filter(KTMultiFSDataFFTW& fsData)
    {
        const int nComponents = fsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = fsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);
        //KTBilateralFilteredFSDataFFTW& newData = fsData.Of< KTBilateralFilteredFSDataFFTW>().SetNComponents(nComponents);
        
        if (fSigmaRange == 0 || fSigmaPixels == 0)
        {
                KTERROR(gclog, "Bilateral filter has zero for SigmaRange");
        }
        
        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpectrum = Filter(fsData.GetSpectra(iComponent));

            if (newSpectrum == NULL)
            {
                KTERROR(gclog, "Bilateral filter of spectrum " << iComponent << " failed for some reason. Continuing processing.");
                continue;
            }
            
            KTDEBUG(gclog, "Computed bilateral filter");
            newData.SetSpectrum(newSpectrum, iComponent);
        }
        KTINFO(gclog, "Completed bilateral filter of " << nComponents << " frequency spectra (FFTW)");

        return true;
    }

    KTFrequencySpectrumFFTW* KTBilateralFilter::Filter(const KTMultiFSFFTW * frequencySpectrum) const
    {
        KTDEBUG(gclog, "Creating new FS for filtered data");

        const int nTimeBins = frequencySpectrum->size();
        const int nFrequencyBins = (*frequencySpectrum)(0)->size();

        const int centerTimeBin = (nTimeBins%2) ? ((nTimeBins-1)/2): (nTimeBins - 2)/2;

        const int nSigmaPixels = 3;

        int timeRange[2] = { std::max(0, centerTimeBin - 3* int(fSigmaPixels)), std::min( nTimeBins , centerTimeBin + 3 * int(fSigmaRange)) };
        int frequencyRange[2];

        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nFrequencyBins, (*frequencySpectrum)(0)->GetRangeMin(), (*frequencySpectrum)(0)->GetRangeMax());

        fftw_complex filterWeightNumerator;
        double filterWeightDenominator;

        fftw_complex pixelPower[2];
        double pixelWeight;
        
        for (int iCenterFrequencyBin = 0; iCenterFrequencyBin < nFrequencyBins; ++iCenterFrequencyBin)
        { 
            frequencyRange[0] = std::max(0, iCenterFrequencyBin - 3 * int(fSigmaPixels));
            frequencyRange[1] = std::min( nFrequencyBins, iCenterFrequencyBin + 3 * int(fSigmaPixels));

            filterWeightNumerator[0] = 0;
            filterWeightNumerator[1] = 0;
            filterWeightDenominator = 0;

            for(int iTimeBin = timeRange[0] ; iTimeBin < timeRange[1]; ++iTimeBin)
            {
                for(int iFrequencyBin = frequencyRange[0]; iFrequencyBin < frequencyRange[1]; ++iFrequencyBin)
                {
                    pixelPower[0][0] = (*(*frequencySpectrum)(centerTimeBin))(iCenterFrequencyBin)[0];
                    pixelPower[0][1] = (*(*frequencySpectrum)(centerTimeBin))(iCenterFrequencyBin)[1];
                    pixelPower[1][0] = (*(*frequencySpectrum)(iTimeBin))(iFrequencyBin)[0];
                    pixelPower[1][1] = (*(*frequencySpectrum)(iTimeBin))(iFrequencyBin)[1];

                    pixelWeight = GaussianWeightRange( pixelPower[0] , pixelPower[1] ) * GaussianWeightPixels(centerTimeBin, iCenterFrequencyBin, iTimeBin, iFrequencyBin );

                    filterWeightNumerator[0] += pixelWeight * pixelPower[1][0];
                    filterWeightNumerator[1] += pixelWeight * pixelPower[1][1];
                    filterWeightDenominator += pixelWeight;
                }
            }
            
            (*newSpectrum)(iCenterFrequencyBin)[0] = filterWeightNumerator[0] / filterWeightDenominator;
            (*newSpectrum)(iCenterFrequencyBin)[1] = filterWeightNumerator[1] / filterWeightDenominator;
        }

        return newSpectrum;
    }

    double KTBilateralFilter::GaussianWeightRange(const fftw_complex &I1, const fftw_complex &I2) const
    {
        return exp( - (pow(I1[0] - I2[0], 2.) + pow(I1[1] - I2[1], 2.)) / (2. * pow(fSigmaRange, 2.) ) );
    }

    double KTBilateralFilter::GaussianWeightPixels(const double &i, const double &j,const double &k,const double &l) const
    {
        return exp( - ( pow(i-k, 2.) +  pow(j - l, 2. )) / (2. * pow(fSigmaPixels, 2.) ) );
    }

} /* namespace Katydid */
