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

#include "KTSliceHeader.hh"

#include "KTLogger.hh"
#include "KTMemberVariable.hh"
#include "KTSlot.hh"

namespace scarab
{
    class param_node;
}

namespace Katydid
{
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataFFTWCore;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataPolarCore;
    class KTFrequencySpectrumFFTW;
    class KTMultiFSDataFFTW;
    class KTMultiFSDataFFTWCore;
    class KTPowerSpectrumData;

    KTLOGGER(sslog_h, "KTSpectrogramStriper");

    /*!
     @class KTSpectrogramStriper
     @author [name]

     @brief [brief class description]

     @details
     [detailed class description]

     Configuration name: "[config-name]"

     Available configuration values:
     - "some-name": [type] -- [what it does]

     Slots:
     - "[slot-name]": void (KTDataPtr) -- [what it does]; Requires [input data type]; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "[signal-name]": void (KTDataPtr) -- Emitted upon [whatever was done]; Guarantees [output data type].
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

                //void IncrementSlice();
                StripeAccumulator() : fDataPtr(new Nymph::KTData()), fSliceHeader(fDataPtr->Of<KTSliceHeader>()), fNextBin(0)
                {}
            };
            template< class XDataClass >
            struct TypedStripeAccumulator : StripeAccumulator
            {
                XDataClass& fData;
                TypedStripeAccumulator() : StripeAccumulator(), fData(fDataPtr->Of<XDataClass>())
                {}
            };

            typedef std::map< const std::type_info*, StripeAccumulator* > AccumulatorMap;
            typedef AccumulatorMap::iterator AccumulatorMapIt;

        public:
            KTSpectrogramStriper(const std::string& name = "spectrogram-striper");
            virtual ~KTSpectrogramStriper();

            bool Configure(const scarab::param_node* node);

            void CalculateSwaps();

            MEMBERVARIABLE(unsigned, StripeSize) // in number of slices
            MEMBERVARIABLE(unsigned, StripeOverlap) // in number of slices

        public:
            //bool AddData(KTFrequencySpectrumDataPolar& data);
            bool AddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTW& data);
            //bool AddData(KTPowerSpectrumData& data);

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

            //bool CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataPolarCore& accData);

            //bool CoreAddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTWCore& data, StripeAccumulator& stripeDataStruct, KTMultiFSDataFFTWCore& stripeData);

            //bool CoreAddData(KTPowerSpectrumData& data, Accumulator& accDataStruct, KTPowerSpectrumData& accData);

            AccumulatorMap fDataMap;
            mutable StripeAccumulator* fLastAccumulatorPtr;
            mutable std::type_info* fLastTypeInfo;

            std::vector< std::pair< unsigned, unsigned > > fSwaps;


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fStripeSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTSliceHeader, KTFrequencySpectrumDataFFTW > fAddFSFFTWSlot;

    };


    template< class XSpectraType >
    void KTSpectrogramStriper::PerformSwaps(XSpectraType& spectra)
    {
        // calculate min/max times
        double minTime = spectra.GetBinLowEdge(fStripeOverlap - 1);
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
        spectra(fSwaps[nSwaps-1].first) = bufferSpectrum;

        // apply new min and max times
        spectra.SetRange(minTime, maxTime);
        return;
    }

    template< class XDataType >
    KTSpectrogramStriper::TypedStripeAccumulator< XDataType >& KTSpectrogramStriper::GetOrCreateAccumulator()
    {
        const std::type_info* typeInfo = &typeid(XDataType);
        if (typeInfo != fLastTypeInfo)
        {
            fLastAccumulatorPtr = fDataMap[typeInfo];
            fLastTypeInfo = const_cast< std::type_info* >(typeInfo);
        }
        return static_cast< TypedStripeAccumulator< XDataType >& >(*fLastAccumulatorPtr);;
    }

    template< class XSpectrumDataCore, class XMultiSpectrumDataCore >
    bool KTSpectrogramStriper::CoreAddData(const KTSliceHeader& header, const XSpectrumDataCore& data, StripeAccumulator& stripeDataStruct, XMultiSpectrumDataCore& stripeData)
    {
        KTWARN(sslog_h, "In CoreAddData");
        unsigned nComponents = data.GetNComponents();

        if (stripeData.GetNComponents() == 0) // this is the first time through this function
        {
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
            KTERROR(sslog_h, "Numbers of components in the average and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrumFFTW(0)->size();
        if (arraySize != (*stripeData.GetSpectra(0))(stripeDataStruct.fNextBin)->size())
        {
            KTERROR(sslog_h, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            CopySpectrum(data.GetSpectrumFFTW(iComponent), (*stripeData.GetSpectra(iComponent))(stripeDataStruct.fNextBin), arraySize);
        }

        stripeDataStruct.fNextBin += 1;
        if (stripeDataStruct.fNextBin == fStripeSize)
        {
            fStripeSignal(stripeDataStruct.fDataPtr);
            stripeDataStruct.fNextBin = fStripeOverlap;
        }

        return true;
    }


    inline const std::vector< std::pair< unsigned, unsigned > >& KTSpectrogramStriper::Swaps() const
    {
        return fSwaps;
    }

}
 /* namespace Katydid */
#endif /* KTSPECTROGRAMSTRIPER_HH_ */
