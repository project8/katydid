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

            MEMBERVARIABLE(unsigned, StripeSize) // in number of slices
            MEMBERVARIABLE(unsigned, StripeOverlap) // in number of slices

        public:
            //bool AddData(KTFrequencySpectrumDataPolar& data);
            bool AddData(KTSliceHeader& header, KTFrequencySpectrumDataFFTW& data);
            //bool AddData(KTPowerSpectrumData& data);

            const std::vector< std::pair< unsigned, unsigned > >& Swaps() const;

        private:
            void CalculateSwaps();

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

    inline const std::vector< std::pair< unsigned, unsigned > >& KTSpectrogramStriper::Swaps() const
    {
        return fSwaps;
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

}
 /* namespace Katydid */
#endif /* KTSPECTROGRAMSTRIPER_HH_ */
