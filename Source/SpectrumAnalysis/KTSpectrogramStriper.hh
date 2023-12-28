/**
 @file KTSpectrogramStriper.hh
 @brief Contains KTSpectrogramStriper
 @details Collects spectrogram stripes
 @author: N.S. Oblath
 @date: Aug 11, 2017
 */

#ifndef KTSPECTROGRAMSTRIPER_HH_
#define KTSPECTROGRAMSTRIPER_HH_

#include "KTProcessor.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTPowerSpectrumData.hh"
#include "KTSliceHeader.hh"

#include "logger.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"

namespace scarab
{
    class param_node;
}

namespace Katydid
{
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTPowerSpectrum;

    LOGGER(sslog_h, "KTSpectrogramStriper");

    /*!
     @class KTSpectrogramStriper
     @author N.S. Oblath

     @brief Collects individual spectra into stripes of a spectrogram

     @details
     If a spectrogram represents all of the data in a run, this processor breaks that spectrogram into vertical stripes.
     Each stripe covers the entire frequency range and some smaller time range.  Stripes can overlap.

     Configuration name: "spectrogram-striper"

     Available configuration values:
     - "stripe-size": unsigned int -- The size, in slices, of each stripe
     - "overlap": unsigned int -- The number of slices that overlap from one stripe to the next

     Slots:
     - "fs-fftw": void (KTDataPtr) -- Adds an FS-FFTW spectrum to the current (or a new) stripe; Requires KTSliceHeader and KTFrequencySpectrumDataFFTW
     - "fs-polar": void (KTDataPtr) -- Adds an FS-Polar spectrum to the current (or a new) stripe; Requires KTSliceHeader and KTFrequencySpectrumDataPolar
     - "ps: void (KTDataPtr) -- Adds a power spectrum to the current (or a new) stripe; Requires KTSliceHeader and KTPowerSpectrumData

     Signals:
     - "str-fs-fftw": void (KTDataPtr) -- Emitted upon completion of an FS-FFTW stripe, either after collecting the requisite number of spectra, when a new acquisition is starting, or when a file is done (when the "done" slot is used); Guarantees KTMultiFSDataFFTW.
     - "str-fs-polar": void (KTDataPtr) -- Emitted upon completion of an FS-Polar stripe, either after collecting the requisite number of spectra, when a new acquisition is starting, or when a file is done (when the "done" slot is used); Guarantees KTMultiFSDataPolar.
     - "str-ps": void (KTDataPtr) -- Emitted upon completion of a PS stripe, either after collecting the requisite number of spectra, when a new acquisition is starting, or when a file is done (when the "done" slot is used); Guarantees KTMultiPSData.
    */
    class KTSpectrogramStriper : public Nymph::KTProcessor
    {
        public:
            struct CompareTypeInfo
            {
                bool operator() (const std::type_info* lhs, const std::type_info* rhs)
                {
                    return lhs->before(*rhs);
                }
            };

            struct StripeAccumulator
            {
                Nymph::KTDataPtr fDataPtr;
                KTSliceHeader& fSliceHeader;
                unsigned fNextBin;
                bool fFirstAccumulation; // in a run or acquisition

                //void IncrementSlice();
                StripeAccumulator() :
                    fDataPtr(new Nymph::KTData()),
                    fSliceHeader(fDataPtr->Of<KTSliceHeader>()),
                    fNextBin(0),
                    fFirstAccumulation(true)
                {}
            };
            template< class XDataClass >
            struct TypedStripeAccumulator : StripeAccumulator
            {
                XDataClass& fData;
                TypedStripeAccumulator() :
                    StripeAccumulator(),
                    fData(fDataPtr->Of<XDataClass>())
                {}
            };

            typedef std::map< const std::type_info*, StripeAccumulator > AccumulatorMap;
            typedef AccumulatorMap::iterator AccumulatorMapIt;

        public:
            KTSpectrogramStriper(const std::string& name = "spectrogram-striper");
            virtual ~KTSpectrogramStriper();

            bool Configure(const scarab::param_node* node);

            void CalculateSwaps();

            MEMBERVARIABLE(unsigned, StripeSize) // in number of slices
            MEMBERVARIABLE(unsigned, StripeOverlap) // in number of slices

        public:
            bool AddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTW& data);
            bool AddData(KTSliceHeader& header, KTFrequencySpectrumDataPolar& data);
            bool AddData(KTSliceHeader& header, KTPowerSpectrumData& data);

            bool OutputStripes();

            const std::vector< std::pair< unsigned, unsigned > >& Swaps() const;

        private:
            template< class XDataType >
            void PerformSwaps(XDataType& data);

            template< class XDataType >
            TypedStripeAccumulator< XDataType >& GetOrCreateAccumulator();

            template< class XSpectrumDataCore, class XMultiSpectrumDataCore >
            bool CoreAddData(const KTSliceHeader& header, const XSpectrumDataCore& data, StripeAccumulator& stripeDataStruct, XMultiSpectrumDataCore& stripeData);

            // FS FFTW functions
            const KTFrequencySpectrumFFTW* GetSpectrum(const KTFrequencySpectrumDataFFTWCore& data, const unsigned iComponent) const;
            void CopySpectrum(const KTFrequencySpectrumFFTW* source, KTFrequencySpectrumFFTW* dest, unsigned arraySize);

            // FS Polar functions
            const KTFrequencySpectrumPolar* GetSpectrum(const KTFrequencySpectrumDataPolarCore& data, const unsigned iComponent) const;
            void CopySpectrum(const KTFrequencySpectrumPolar* source, KTFrequencySpectrumPolar* dest, unsigned arraySize);

            // PS functions
            const KTPowerSpectrum* GetSpectrum(const KTPowerSpectrumDataCore& data, const unsigned iComponent) const;
            void CopySpectrum(const KTPowerSpectrum* source, KTPowerSpectrum* dest, unsigned arraySize);

            AccumulatorMap fDataMap;
            mutable StripeAccumulator* fLastAccumulatorPtr;
            mutable std::type_info* fLastTypeInfo;

            std::vector< std::pair< unsigned, unsigned > > fSwaps;


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fStripeFSFFTWSignal;
            Nymph::KTSignalData fStripeFSPolarSignal;
            Nymph::KTSignalData fStripePSSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTFrequencySpectrumDataFFTW > fAddFSFFTWSlot;
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTFrequencySpectrumDataPolar > fAddFSPolarSlot;
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTPowerSpectrumData > fAddPSSlot;

    };


    template< class XSpectraType >
    void KTSpectrogramStriper::PerformSwaps(XSpectraType& spectra)
    {
        // calculate min/max times
        double minTime = spectra.GetBinLowEdge(fSwaps[0].first);
        double maxTime = minTime + fStripeSize * spectra.GetBinWidth();

        // send element 0 to a holding buffer
        typename XSpectraType::value_type bufferSpectrum = spectra(0);
        // do mapped swaps
        unsigned nSwaps = fSwaps.size();
        for (unsigned iSwap = 0; iSwap < nSwaps; ++iSwap)
        {
            spectra(fSwaps[iSwap].second) = spectra(fSwaps[iSwap].first);
        }
        // return the pointer in the buffer to the final position
        spectra(fSwaps[nSwaps-1].second) = bufferSpectrum;

        // apply new min and max times
        spectra.SetRange(minTime, maxTime);
        return;
    }

    template< class XDataType >
    KTSpectrogramStriper::TypedStripeAccumulator< XDataType >& KTSpectrogramStriper::GetOrCreateAccumulator()
    {
        const std::type_info* typeInfo = &typeid(XDataType);
        LDEBUG(sslog_h, "Getting or creating <" << typeInfo->name() << ">");
        if (typeInfo != fLastTypeInfo)
        {
            fLastAccumulatorPtr = &fDataMap[typeInfo];
            fLastTypeInfo = const_cast< std::type_info* >(typeInfo);
        }
        return static_cast< TypedStripeAccumulator< XDataType >& >(*fLastAccumulatorPtr);
    }

    template< class XSpectrumDataCore, class XMultiSpectrumDataCore >
    bool KTSpectrogramStriper::CoreAddData(const KTSliceHeader& header, const XSpectrumDataCore& data, StripeAccumulator& stripeDataStruct, XMultiSpectrumDataCore& stripeData)
    {
        unsigned nComponents = data.GetNComponents();

        if (stripeData.GetNComponents() == 0) // this is the first time through this function
        {
            LDEBUG(sslog_h, "This is the first time through CoreAddData for this data type");
            stripeDataStruct.fSliceHeader.CopySliceHeaderOnly(header);
            stripeData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {

                const typename XMultiSpectrumDataCore::spectrum_type* dataFS = GetSpectrum(data, iComponent);
                typename XMultiSpectrumDataCore::multi_spectrum_type* newMultiFS = new typename XMultiSpectrumDataCore::multi_spectrum_type(fStripeSize, header.GetTimeInRun(), header.GetTimeInRun() + fStripeSize * header.GetSliceLength());
                for (unsigned iFS = 0; iFS < fStripeSize; ++iFS)
                {
                    (*newMultiFS)(iFS) = new typename XMultiSpectrumDataCore::spectrum_type(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                    (*newMultiFS)(iFS)->operator*=(double(0.));
                }
                stripeData.SetSpectra(newMultiFS, iComponent);
            }
        }
        else if (header.GetIsNewAcquisition()) // this starts a new acquisition, so it should start a new stripe, ignoring the overlap
        {
            LDEBUG(sslog_h, "This is a new acquisition; will emit signal if there's a partially-filled stripe");

            // emit signal for the current stripe if there is an existing partially-filled stripe
            if (stripeDataStruct.fNextBin != fStripeOverlap || (stripeDataStruct.fFirstAccumulation && stripeDataStruct.fNextBin == fStripeOverlap)) fStripeFSFFTWSignal(stripeDataStruct.fDataPtr);

            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                typename XMultiSpectrumDataCore::multi_spectrum_type* spectra = stripeData.GetSpectra(iComponent);
                // reset the values all to 0
                for (unsigned iSpect = 0; iSpect < spectra->size(); ++iSpect)
                {
                    spectra->operator()(iSpect)->operator*=(0.);
                }
                // set the time axis
                spectra->SetRange(header.GetTimeInRun(), header.GetTimeInRun() + fStripeSize * header.GetSliceLength());
            }

            stripeDataStruct.fSliceHeader.CopySliceHeaderOnly(header);
            stripeDataStruct.fNextBin = 0;
            stripeDataStruct.fFirstAccumulation = true;
        }
        else if (stripeDataStruct.fNextBin == fStripeOverlap  && ! stripeDataStruct.fFirstAccumulation) // this isn't the first time through, but we have a fresh stripe, so we perform the swap to keep the overlap region
        {
            stripeDataStruct.fSliceHeader.CopySliceHeaderOnly(header);
            LDEBUG(sslog_h, "Performing swap for overlap region");
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                typename XMultiSpectrumDataCore::multi_spectrum_type* spectra = stripeData.GetSpectra(iComponent);
                PerformSwaps(*spectra);
                // zero out all spectra after the overlap
                for (unsigned iSpect = fStripeOverlap; iSpect < spectra->size(); ++iSpect)
                {
                    spectra->operator()(iSpect)->operator*=(0.);
                }
            }
        }

        if (nComponents != stripeData.GetNComponents())
        {
            LERROR(sslog_h, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        unsigned arraySize = GetSpectrum(data, 0)->size();
        if (arraySize != (*stripeData.GetSpectra(0))(stripeDataStruct.fNextBin)->size())
        {
            LERROR(sslog_h, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            CopySpectrum(GetSpectrum(data, iComponent), (*stripeData.GetSpectra(iComponent))(stripeDataStruct.fNextBin), arraySize);
        }

        stripeDataStruct.fNextBin += 1;
        if (stripeDataStruct.fNextBin == fStripeSize)
        {
            // emit the signal for this stripe
            LDEBUG(sslog_h, "Finished a stripe; emitting signal");
            fStripeFSFFTWSignal(stripeDataStruct.fDataPtr);
            stripeDataStruct.fNextBin = fStripeOverlap;
            stripeDataStruct.fFirstAccumulation = false;
        }

        return true;
    }


    inline const std::vector< std::pair< unsigned, unsigned > >& KTSpectrogramStriper::Swaps() const
    {
        return fSwaps;
    }

    inline const KTFrequencySpectrumFFTW* KTSpectrogramStriper::GetSpectrum(const KTFrequencySpectrumDataFFTWCore& data, const unsigned iComponent) const
    {
        return data.GetSpectrumFFTW(iComponent);
    }

    inline const KTFrequencySpectrumPolar* KTSpectrogramStriper::GetSpectrum(const KTFrequencySpectrumDataPolarCore& data, const unsigned iComponent) const
    {
        return data.GetSpectrumPolar(iComponent);
    }

    inline const KTPowerSpectrum* KTSpectrogramStriper::GetSpectrum(const KTPowerSpectrumDataCore& data, const unsigned iComponent) const
    {
        return data.GetSpectrum(iComponent);
    }


}
 /* namespace Katydid */
#endif /* KTSPECTROGRAMSTRIPER_HH_ */
