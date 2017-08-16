/*
 * KTSpectrogramStriper.cc
 *
 *  Created on: Aug 11, 2017
 *      Author: N.S. Oblath
 */

#include "KTSpectrogramStriper.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"

#include "KTLogger.hh"

#include "param.hh"


namespace Katydid
{
    KTLOGGER(sslog, "KTSpectrogramStriper");

    KT_REGISTER_PROCESSOR(KTSpectrogramStriper, "spectrogram-striper");

    KTSpectrogramStriper::KTSpectrogramStriper(const std::string& name) :
            KTProcessor(name),
            fStripeSize(100),
            fStripeOverlap(0),
            fDataMap(),
            fLastAccumulatorPtr(),
            fStripeSignal("stripe", this),
            fAddFSFFTWSlot("fs-fftw", this, &KTSpectrogramStriper::AddData)
    {
    }

    KTSpectrogramStriper::~KTSpectrogramStriper()
    {
    }

    bool KTSpectrogramStriper::Configure(const scarab::param_node* node)
    {
        if (node != NULL)
        {
            SetStripeSize(node->get_value("stripe-size", GetStripeSize()));
            SetStripeOverlap(node->get_value("overlap", GetStripeOverlap()));
        }

        CalculateSwaps();

        return true;
    }

    void KTSpectrogramStriper::CalculateSwaps()
    {
        KTWARN(sslog, "Calculating swaps");
        fSwaps.clear();

        unsigned nSwaps = fStripeSize;
        fSwaps.resize(nSwaps);

        unsigned swapDist = fStripeSize - fStripeOverlap;

        unsigned destination = 0;
        for (unsigned iSwap = 0; iSwap < nSwaps; ++iSwap)
        {
            unsigned source = destination + swapDist;
            if (source >= fStripeSize ) source -= fStripeSize;
            fSwaps[iSwap] = std::pair< unsigned, unsigned >(source, destination);
            KTDEBUG(sslog, "Swap pair: " << source << " --> " << destination);
            destination = source;
        }
    }

    bool KTSpectrogramStriper::AddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTW& data)
    {
        StripeAccumulator& accDataStruct = GetOrCreateAccumulator< KTFrequencySpectrumDataFFTW >();
        KTMultiFSDataFFTW& accData = accDataStruct.fData->Of< KTMultiFSDataFFTW >();
        return CoreAddData(header, data, accDataStruct, accData);
        return false;
    }

    bool KTSpectrogramStriper::CoreAddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTWCore& data, StripeAccumulator& stripeDataStruct, KTMultiFSDataFFTW& stripeData)
    {
        unsigned nComponents = data.GetNComponents();

        KTSliceHeader& stripeHeader = stripeDataStruct.fData->Of< KTSliceHeader >();
        if (stripeData.GetNComponents() == 0) // this is the first time through this function
        {
            stripeData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {

                KTFrequencySpectrumFFTW* dataFS = data.GetSpectrumFFTW(iComponent);
                KTMultiFSFFTW* newMultiFS = new KTMultiFSFFTW(fStripeSize, header.GetTimeInRun(), header.GetTimeInRun() + fStripeSize * header.GetSliceLength());
                for (unsigned iFS = 0; iFS < fStripeSize; ++iFS)
                {
                    (*newMultiFS)(iFS) = new KTFrequencySpectrumFFTW(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                    (*newMultiFS)(iFS)->operator*=(double(0.));
                }
                stripeData.SetSpectra(newMultiFS, iComponent);
            }
        }
        else if (stripeDataStruct.fNextBin == 0) // this isn't the first time through, but we have a fresh stripe
        {
            // TODO: reorganize existing frequency spectra
            // TODO: redo time limits of the x-axis?
        }

        //accDataStruct.BumpSliceNumber();


        if (nComponents != stripeData.GetNComponents())
        {
            KTERROR(sslog, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrumFFTW(0)->size();
        if (arraySize != (*stripeData.GetSpectra(0))(stripeDataStruct.fNextBin)->size())
        {
            KTERROR(sslog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpect = data.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumFFTW* avSpect = (*stripeData.GetSpectra(iComponent))(stripeDataStruct.fNextBin);
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin)[0] = (*newSpect)(iBin)[0];
                (*avSpect)(iBin)[1] = (*newSpect)(iBin)[1];
            }
        }

        return true;
    }



} /* namespace Katydid */
