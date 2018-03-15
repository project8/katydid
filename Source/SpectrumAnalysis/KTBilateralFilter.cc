/*
 * KTBilateralFilter.cc
 *
 *  Created on: Mar 7, 2018
 *      Author: N. Buzinsky
 */

#include "KTBilateralFilter.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"

#include "KTLogger.hh"
#include "KTBilateralFilteredData.hh"
#include "KTMath.hh"

#include "KTMultiFSDataFFTW.hh"

#include "param.hh"

#include <iostream>

namespace Katydid
{
    KTLOGGER(gclog, "KTBilateralFilter");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTBilateralFilter, "bilateral-filter");

    KTBilateralFilter::KTBilateralFilter(const std::string& name) :
            KTProcessor(name),
            fSigmaPixels(0.),
            fSigmaRange(0.),
            nIters(0),
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
        int nComponents = fsData.GetNComponents();

        KTFrequencySpectrumDataFFTW& newData = fsData.Of< KTFrequencySpectrumDataFFTW >().SetNComponents(nComponents);
        //KTBilateralFilterFSDataFFTW& newData = fsData.Of< KTBilateralFilterFSDataFFTW >().SetNComponents(nComponents);
        ++nIters;

        
        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpectrum = Filter(fsData.GetSpectra(iComponent));

            //if (newSpectrum == NULL)
            //{
            //    KTERROR(gclog, "Bilateral filter of spectrum " << iComponent << " failed for some reason. Continuing processing.");
            //    continue;
            //}
            
            KTDEBUG(gclog, "Computed bilateral filter");

            newData.SetSpectrum(newSpectrum, iComponent);
        }
        //KTINFO(gclog, "Completed bilateral filter of " << nComponents << " frequency spectra (FFTW)");

        return true;
    }

    //(*fs)(0);
    KTFrequencySpectrumFFTW* KTBilateralFilter::Filter(const KTMultiFSFFTW * frequencySpectrum) const
    {
        KTDEBUG(gclog, "Creating new FS for filtered data");
        int nBins = frequencySpectrum->size();
        KTFrequencySpectrumFFTW* newSpectrum = new KTFrequencySpectrumFFTW(nBins, (*frequencySpectrum)(0)->GetRangeMin(), (*frequencySpectrum)(0)->GetRangeMax());
        newSpectrum->SetNTimeBins((*frequencySpectrum)(0)->GetNTimeBins());
        //std::cout<<nBins<<std::endl; //number of bins in time domain
        //std::cout<<(*frequencySpectrum)(0)->size()<<std::endl; //4096, number of freq. bins
        std::cout<<nIters<<std::endl;

        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            (*newSpectrum)(iBin)[0] = 1;
            (*newSpectrum)(iBin)[1] = 1;
        }

        return newSpectrum;
    }

} /* namespace Katydid */
