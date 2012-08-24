/*
 * KTFFTTypes.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */


#include "KTFFTTypes.hh"

namespace Katydid
{

#ifdef ROOT_FOUND
    TH1D* CreatePowerSpectrumHistFromFreqSpect(const std::string& histName, const KTFrequencySpectrum* freqSpect)
    {
        UInt_t nPoints = freqSpect->GetNBins();
        TH1D* newHist = new TH1D(histName.c_str(), histName.c_str(), nPoints, freqSpect->GetRangeMin(), freqSpect->GetRangeMax());
        for (Int_t iPoint=0; iPoint<nPoints; iPoint++)
        {
            newHist->SetBinContent(iPoint+1, norm((*freqSpect)[iPoint]));
        }
        return newHist;
    }
#endif

}
