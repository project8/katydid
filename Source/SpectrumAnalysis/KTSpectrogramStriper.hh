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
    class KTMultiFSDataFFTW;
    class KTPowerSpectrumData;


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
                Nymph::KTDataPtr fData;
                KTSliceHeader& fSliceHeader;
                unsigned fNextBin;

                void IncrementSlice();
                StripeAccumulator() : fData(new Nymph::KTData()), fSliceHeader(fData->Of<KTSliceHeader>()), fNextBin(0)
                {
                }
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
            //bool AddData(KTFrequencySpectrumDataPolar& data);
            bool AddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTW& data);
            //bool AddData(KTPowerSpectrumData& data);

            const std::vector< std::pair< unsigned, unsigned > >& Swaps() const;

        private:
            template< class XDataType >
            void PerformSwaps(XDataType& data);

            template< class XDataType >
            StripeAccumulator& GetOrCreateAccumulator();

            //bool CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataPolarCore& accData);
            bool CoreAddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTWCore& data, StripeAccumulator& stripeDataStruct, KTMultiFSDataFFTW& stripeData);

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
    KTSpectrogramStriper::StripeAccumulator& KTSpectrogramStriper::GetOrCreateAccumulator()
    {
        const std::type_info* typeInfo = &typeid(XDataType);
        if (typeInfo == fLastTypeInfo) return *fLastAccumulatorPtr;
        fLastAccumulatorPtr = &fDataMap[typeInfo];
        fLastTypeInfo = const_cast< std::type_info* >(typeInfo);
        return *fLastAccumulatorPtr;
    }

    inline const std::vector< std::pair< unsigned, unsigned > >& KTSpectrogramStriper::Swaps() const
    {
        return fSwaps;
    }

}
 /* namespace Katydid */
#endif /* KTSPECTROGRAMSTRIPER_HH_ */
