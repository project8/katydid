/*
 * KTGainNormalization.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTGainNormalization.hh"

#include "KTPhysicalArray.hh"
#include "KTPowerSpectrum.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFFT.hh"

#include "TMath.h"

#include <iostream>

namespace Katydid
{

    KTGainNormalization::KTGainNormalization() :
            KTProcessor(),
            KTConfigurable(),
            fNormalization(NULL)
    {
        fConfigName = "gain-normalization";

        RegisterSlot("power_spect", this, &KTGainNormalization::ProcessPowerSpectrum);
    }

    KTGainNormalization::~KTGainNormalization()
    {
        delete fNormalization;
    }

    Bool_t KTGainNormalization::Configure(const KTPStoreNode* node)
    {
        return true;
    }


    void KTGainNormalization::PrepareNormalization(KTPhysicalArray< 1, Double_t >* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth)
    {
        Double_t freqMult = 1.e-6;
        delete fNormalization;
        fNormalization = new KTPhysicalArray< 1, Double_t >(reducedNBins, -0.5*reducedBinWidth*freqMult, reducedBinWidth * ((Double_t)reducedNBins-0.5) * freqMult);

        Int_t veryLastBinInFullPS = (Int_t)fullArray->GetNBins() - 1;
        for (UInt_t iBin=0; iBin<reducedNBins; iBin++)
        {
            Double_t freqBinMin = fNormalization->GetBinLowEdge(iBin);
            Double_t freqBinMax = fNormalization->GetBinLowEdge(iBin+1);
            Int_t firstBinFullPS = TMath::Max((Int_t)fullArray->FindBin(freqBinMin), 0);
            Int_t lastBinFullPS = TMath::Min((Int_t)fullArray->FindBin(freqBinMax), veryLastBinInFullPS);
            //std::cout << iBin << "  " << freqBinMin << "  " << freqBinMax << "  " << firstBinFullPS << "  " << lastBinFullPS << std::endl;
            Double_t meanBinContent = 0.;
            Int_t nBinsInSum = 0;
            for (Int_t iSubBin=firstBinFullPS; iSubBin<=lastBinFullPS; iSubBin++)
            {
                    meanBinContent += (*fullArray)[iSubBin];
                    nBinsInSum++;
            }
            //if (nBinsInSum != 0) meanBinContent /= (Double_t)nBinsInSum;
            (*fNormalization)[iBin] = meanBinContent;
            //cout << "Gain norm bin " << iBin << "  content: " << meanBinContent << endl;
        }


        return;
    }

    void KTGainNormalization::ProcessSlidingWindowFFT(KTSlidingWindowFFT* fft)
    {
        UInt_t nPowerSpectra = fft->GetNPowerSpectra();
        for (UInt_t iPS=0; iPS<nPowerSpectra; iPS++)
        {
            ProcessPowerSpectrum(iPS, fft->GetPowerSpectrum(iPS));
        }

        return;
    }

    void KTGainNormalization::ProcessPowerSpectrum(UInt_t /*psNum*/, KTPowerSpectrum* powerSpectrum)
    {
        if (powerSpectrum->GetSize() != fNormalization->size())
        {
            std::cout << "Error in KTGainNormalization::ProcessArray: Array sizes do not match!" << std::endl;
            return;
        }

        (*powerSpectrum) /= (*fNormalization);

        return;
    }


} /* namespace Katydid */
