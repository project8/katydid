/*
 * KTOldGainNormalization.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTOldGainNormalization.hh"

#include "KTFactory.hh"
#include "KTFrequencySpectrum.hh"
#include "KTPhysicalArray.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTPowerSpectrum.hh"

#include <algorithm>
#include <iostream>

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTOldGainNormalization > sGainNormRegistrar("old-gain-normalization");

    KTOldGainNormalization::KTOldGainNormalization() :
            KTProcessor(),
            fNormalization(NULL)
    {
        fConfigName = "old-gain-normalization";

        RegisterSlot("freq_spect", this, &KTOldGainNormalization::ProcessFrequencySpectrum, "void (UInt_t, KTFrequencySpectrum*)");
    }

    KTOldGainNormalization::~KTOldGainNormalization()
    {
        delete fNormalization;
    }

    Bool_t KTOldGainNormalization::Configure(const KTPStoreNode* node)
    {
        return true;
    }


    void KTOldGainNormalization::PrepareNormalization(KTFrequencySpectrum* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth)
    {
        delete fNormalization;
        fNormalization = new KTFrequencySpectrum(reducedNBins, -0.5*reducedBinWidth, reducedBinWidth * ((Double_t)reducedNBins-0.5));

        Int_t veryLastBinInFullPS = (Int_t)fullArray->size() - 1;
        for (UInt_t iBin=0; iBin<reducedNBins; iBin++)
        {
            Double_t freqBinMin = fNormalization->GetBinLowEdge(iBin);
            Double_t freqBinMax = fNormalization->GetBinLowEdge(iBin+1);
            Int_t firstBinFullPS = std::max((Int_t)fullArray->FindBin(freqBinMin), 0);
            Int_t lastBinFullPS = std::min((Int_t)fullArray->FindBin(freqBinMax), veryLastBinInFullPS);
            //std::cout << iBin << "  " << freqBinMin << "  " << freqBinMax << "  " << firstBinFullPS << "  " << lastBinFullPS << std::endl;
            complexpolar<Double_t> meanBinContent;
            Int_t nBinsInSum = 0;
            for (Int_t iSubBin=firstBinFullPS; iSubBin<=lastBinFullPS; iSubBin++)
            {
                    meanBinContent += (*fullArray)(iSubBin);
                    nBinsInSum++;
            }
            //if (nBinsInSum != 0) meanBinContent /= (Double_t)nBinsInSum;
            (*fNormalization)(iBin) = meanBinContent;
            //cout << "Gain norm bin " << iBin << "  content: " << meanBinContent << endl;
        }


        return;
    }

    void KTOldGainNormalization::ProcessSlidingWindowFFT(KTSlidingWindowFSData* swFSData)
    {
        KTPhysicalArray< 1, KTFrequencySpectrum* >* spectra = swFSData->GetSpectra(0);
        UInt_t nPowerSpectra = spectra->size();
        for (UInt_t iPS=0; iPS<nPowerSpectra; iPS++)
        {
            ProcessFrequencySpectrum(iPS, (*spectra)(iPS));
        }

        return;
    }

    void KTOldGainNormalization::ProcessFrequencySpectrum(UInt_t /*psNum*/, KTFrequencySpectrum* freqSpectrum)
    {
        if (freqSpectrum->size() != fNormalization->size())
        {
            std::cout << "Error in KTOldGainNormalization::ProcessArray: Array sizes do not match!" << std::endl;
            return;
        }

        (*freqSpectrum) /= (*fNormalization);

        return;
    }


} /* namespace Katydid */
