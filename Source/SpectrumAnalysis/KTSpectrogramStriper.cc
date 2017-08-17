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
            fLastAccumulatorPtr(nullptr),
            fLastTypeInfo(nullptr),
            fSwaps(),
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
        KTMultiFSDataFFTWCore& accData = accDataStruct.fDataPtr->Of< KTMultiFSDataFFTW >();
        return CoreAddData(header, static_cast< KTFrequencySpectrumDataFFTWCore& >(data), accDataStruct, accData);
    }
/*
    bool KTSpectrogramStriper::CoreAddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTWCore& data, StripeAccumulator& stripeDataStruct, KTMultiFSDataFFTWCore& stripeData)
    {
        unsigned nComponents = data.GetNComponents();

        if (stripeData.GetNComponents() == 0) // this is the first time through this function
        {
            stripeDataStruct.fSliceHeader.CopySliceHeaderOnly(header);
            stripeData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {

                KTMultiFSDataFFTWCore::spectrum_type* dataFS = data.GetSpectrumFFTW(iComponent);
                KTMultiFSDataFFTWCore::multi_spectrum_type* newMultiFS = new KTMultiFSDataFFTW::multi_spectrum_type(fStripeSize, header.GetTimeInRun(), header.GetTimeInRun() + fStripeSize * header.GetSliceLength());
                for (unsigned iFS = 0; iFS < fStripeSize; ++iFS)
                {
                    (*newMultiFS)(iFS) = new KTMultiFSDataFFTWCore::spectrum_type(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                    (*newMultiFS)(iFS)->operator*=(double(0.));
                }
                stripeData.SetSpectra(newMultiFS, iComponent);
            }
        }
        else if (header.GetIsNewAcquisition()) // this starts a new acquisition, so it should start a new stripe, ignoring the overlap
        {
            // emit signal for the current stripe if there is an existing partially-filled stripe
            if (stripeDataStruct.fNextBin != fStripeOverlap) fStripeSignal(stripeDataStruct.fDataPtr);

            stripeDataStruct.fSliceHeader.CopySliceHeaderOnly(header);
            stripeDataStruct.fNextBin = 0;

        }
        else if (stripeDataStruct.fNextBin == fStripeOverlap) // this isn't the first time through, but we have a fresh stripe
        {
            stripeDataStruct.fSliceHeader.CopySliceHeaderOnly(header);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                PerformSwaps(*stripeData.GetSpectra(iComponent));
            }
        }

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
            KTMultiFSDataFFTWCore::spectrum_type* newSpect = data.GetSpectrumFFTW(iComponent);
            KTMultiFSDataFFTWCore::spectrum_type* avSpect = (*stripeData.GetSpectra(iComponent))(stripeDataStruct.fNextBin);
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin)[0] = (*newSpect)(iBin)[0];
                (*avSpect)(iBin)[1] = (*newSpect)(iBin)[1];
            }
        }

        stripeDataStruct.fNextBin += 1;
        if (stripeDataStruct.fNextBin == fStripeSize)
        {
            fStripeSignal(stripeDataStruct.fDataPtr);
            stripeDataStruct.fNextBin = fStripeOverlap;
        }

        return true;
    }
*/
    const KTFrequencySpectrumFFTW* KTSpectrogramStriper::GetSpectrum(const KTFrequencySpectrumDataFFTWCore& data, const unsigned iComponent) const
    {
        return data.GetSpectrumFFTW(iComponent);
    }

    void KTSpectrogramStriper::CopySpectrum(const KTFrequencySpectrumFFTW* source, KTFrequencySpectrumFFTW* dest, unsigned arraySize)
    {
        for (unsigned iBin = 0; iBin < arraySize; ++iBin)
        {
            (*dest)(iBin)[0] = (*source)(iBin)[0];
            (*dest)(iBin)[1] = (*source)(iBin)[1];
        }
    }

} /* namespace Katydid */
