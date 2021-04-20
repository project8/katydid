/*
 * KTSpectrogramStriper.cc
 *
 *  Created on: Aug 11, 2017
 *      Author: N.S. Oblath
 */

#include "KTSpectrogramStriper.hh"

#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTMultiFSDataPolar.hh"
#include "KTMultiPSData.hh"
#include "KTPowerSpectrum.hh"

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
            fStripeFSFFTWSignal("str-fs-fftw", this),
            fStripeFSPolarSignal("str-fs-polar", this),
            fStripePSSignal("str-ps", this),
            fAddFSFFTWSlot("fs-fftw", this, &KTSpectrogramStriper::AddData),
            fAddFSPolarSlot("fs-polar", this, &KTSpectrogramStriper::AddData),
            fAddPSSlot("ps", this, &KTSpectrogramStriper::AddData)
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
        KTDEBUG(sslog, "Calculating swaps");
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

    bool KTSpectrogramStriper::AddData(KTSliceHeader& header, KTFrequencySpectrumDataPolar& data)
    {
        StripeAccumulator& accDataStruct = GetOrCreateAccumulator< KTFrequencySpectrumDataPolar >();
        KTMultiFSDataPolarCore& accData = accDataStruct.fDataPtr->Of< KTMultiFSDataPolar >();
        return CoreAddData(header, static_cast< KTFrequencySpectrumDataPolarCore& >(data), accDataStruct, accData);
    }

    bool KTSpectrogramStriper::AddData(KTSliceHeader& header, KTPowerSpectrumData& data)
    {
        StripeAccumulator& accDataStruct = GetOrCreateAccumulator< KTPowerSpectrumData >();
        KTMultiPSDataCore& accData = accDataStruct.fDataPtr->Of< KTMultiPSData >();
        return CoreAddData(header, static_cast< KTPowerSpectrumDataCore& >(data), accDataStruct, accData);
    }

    bool KTSpectrogramStriper::OutputStripes()
    {
        KTINFO(sslog, "Outputting all histograms");
        for (AccumulatorMapIt accIt = fDataMap.begin(); accIt != fDataMap.end(); ++accIt)
        {
            KTDEBUG(sslog, "Checking <" << accIt->first->name() << "> for final outputting");
            if (accIt->second.fNextBin != fStripeOverlap) fStripeFSFFTWSignal(accIt->second.fDataPtr);
        }
        return true;
    }

    // Why this function? The copy constructor of KTFrequencySpectrumFFTW should be responsible for this
    void KTSpectrogramStriper::CopySpectrum(const KTFrequencySpectrumFFTW* source, KTFrequencySpectrumFFTW* dest, unsigned arraySize)
    {
        for (unsigned iBin = 0; iBin < arraySize; ++iBin)
        {
            dest->SetRect(iBin, source->GetReal(iBin), source->GetImag(iBin));
        }
    }

    void KTSpectrogramStriper::CopySpectrum(const KTFrequencySpectrumPolar* source, KTFrequencySpectrumPolar* dest, unsigned arraySize)
    {
        for (unsigned iBin = 0; iBin < arraySize; ++iBin)
        {
            (*dest)(iBin).set_polar((*source)(iBin).abs(), (*source)(iBin).arg());
        }
    }

    void KTSpectrogramStriper::CopySpectrum(const KTPowerSpectrum* source, KTPowerSpectrum* dest, unsigned arraySize)
    {
        for (unsigned iBin = 0; iBin < arraySize; ++iBin)
        {
            (*dest)(iBin) = (*source)(iBin);
        }
    }

} /* namespace Katydid */
