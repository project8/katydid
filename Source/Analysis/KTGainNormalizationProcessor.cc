/*
 * KTGainNormalizationProcessor.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTGainNormalizationProcessor.hh"

#include "KTPhysicalArray.hh"
#include "KTPowerSpectrum.hh"

#include <iostream>

namespace Katydid
{

    KTGainNormalizationProcessor::KTGainNormalizationProcessor() :
            fNormalization(NULL)
    {
    }

    KTGainNormalizationProcessor::~KTGainNormalizationProcessor()
    {
        delete fNormalization;
    }

    Bool_t KTGainNormalizationProcessor::ApplySetting(const KTSetting* setting)
    {
        return kFALSE;
    }

    void KTGainNormalizationProcessor::PrepareNormalization(KTPhysicalArray< 1, Double_t >* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth)
    {
        Double_t freqMult = 1.e-6;
        delete fNormalization;
        fNormalization = new KTPhysicalArray< 1, Double_t >(reducedNBins, -0.5*reducedBinWidth*freqMult, reducedBinWidth * ((Double_t)reducedNBins-0.5) * freqMult);

        for (UInt_t iBin=0; iBin<reducedNBins; iBin++)
        {
            Double_t freqBinMin = fNormalization->GetBinLowEdge(iBin);
            Double_t freqBinMax = fNormalization->GetBinLowEdge(iBin+1);
            Int_t firstBinFullPS = fullArray->FindBin(freqBinMin);
            Int_t lastBinFullPS = fullArray->FindBin(freqBinMax);
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

    void KTGainNormalizationProcessor::ProcessPowerSpectrum(UInt_t /*psNum*/, KTPowerSpectrum* powerSpectrum)
    {
        if (powerSpectrum->GetSize() != fNormalization->size())
        {
            std::cout << "Error in KTGainNormalizationProcessor::ProcessArray: Array sizes do not match!" << std::endl;
            return;
        }

        (*powerSpectrum) /= (*fNormalization);

        return;
    }


} /* namespace Katydid */
