/**
 @file KTPolyphaseFilterBank.hh
 @brief Contains KTPolyphaseFilterBank
 @details Applies the polyphase filter bank
 @author: N. S. Oblath
 @date: Apr 18, 2013
 */

#ifndef KTPOLYPHASEFILTERBANK_HH_
#define KTPOLYPHASEFILTERBANK_HH_

#include "KTProcessor.hh"

#include "KTData.hh"
#include "KTLogger.hh"
#include "KTSlot.hh"

#include <string>


namespace Nymph
{
    class scarab::param_node;
}
;

namespace Katydid
{
    
    KTLOGGER(fftlog_comp, "KTPolyphaseFilterBank.hh");

    class KTEggHeader;
    class KTSliceHeader;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;
    class KTWindowFunction;

    /*!
     @class KTPolyphaseFilterBank
     @author N. S. Oblath

     @brief Applies a polyphase filter bank to a time series

     @details
     KTPolyphaseFilterBank applies a polyphase filter bank to a time series (either real- or fftw-type)

     NOTE: This creates a completely new data object, with a new slice header.

     Configuration name: "polyphase-filter-bank"

     Available configuration values:
     - "n-subsets": unsigned int -- sets the number of subsets to be used in the filter bank (use is mutually exclusive with subset-size)
     - "subset-size": unsigned int -- sets the size of the subsets to be used in the filter bank (use is mutually exclusive with n-subsets)

     Slots:
     - "header": void (KTEggHeader*) -- Initialize the window function from an Egg header
     - "ts-real": void (Nymph::KTDataPtr) -- Window the time series; Requires KTTimeSeriesData containing KTTimeSeriesReal; Does not add data; Emits signal "windowed"
     - "ts-fftw": void (Nymph::KTDataPtr) -- Window the time series; Requires KTTimeSeriesData containing KTTimeSeriesFFTW; Does not add data; Emits signal "windowed"

     Signals:
     - "windowed": void (Nymph::KTDataPtr) -- Emitted upon performance of a windowing; Guarantees KTTimeSeriesData.
    */

    class KTPolyphaseFilterBank : public Nymph::KTProcessor
    {
        public:
            KTPolyphaseFilterBank(const std::string& name = "polyphase-filter-bank");
            virtual ~KTPolyphaseFilterBank();

            bool Configure(const scarab::param_node* node);

            unsigned GetNSubsets() const;
            void SetNSubsets(unsigned subsets);

            unsigned GetSubsetSize() const;
            void SetSubsetSize(unsigned size);

        private:
            unsigned fNSubsets;
            unsigned fSubsetSize;

        public:
            bool InitializeWindow();
            void InitializeWithHeader(KTEggHeader* header);

            /// Apply the PFB to the data object's time series (real-type)
            bool ProcessDataReal(const KTTimeSeriesData& tsData);
            /// Apply the PFB to the data object's time series (fftw-type)
            bool ProcessDataFFTW(const KTTimeSeriesData& tsData);

            /// Create a new data object for the filtered time series (real-type)
            Nymph::KTDataPtr CreateFilteredDataReal(const KTTimeSeriesData& tsData);
            /// Create a new data object for the filtered time series (fftw-type)
            Nymph::KTDataPtr CreateFilteredDataFFTW(const KTTimeSeriesData& tsData);

            /// Apply PFB to a single time series (real-type); a new time series is produced
            KTTimeSeriesReal* ApplyPFB(const KTTimeSeriesReal* data) const;
            /// Apply PFB to a single time series (fftw-type); a new time series is produced
            KTTimeSeriesFFTW* ApplyPFB(const KTTimeSeriesFFTW* data) const;

        private:
            bool TransferHeaderInformation(const KTSliceHeader& oldHeader, KTSliceHeader& newHeader);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fWindowed;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotOneArg< void (KTEggHeader*) > fHeaderSlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesFFTWSlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesRealSlot;

    };


    inline unsigned KTPolyphaseFilterBank::GetNSubsets() const
    {
        return fNSubsets;
    }

    inline void KTPolyphaseFilterBank::SetNSubsets(unsigned subsets)
    {
        fNSubsets = subsets;
        return;
    }

    inline unsigned KTPolyphaseFilterBank::GetSubsetSize() const
    {
        return fSubsetSize;
    }

    inline void KTPolyphaseFilterBank::SetSubsetSize(unsigned size)
    {
        fNSubsets = size;
        return;
    }


} /* namespace Katydid */
#endif /* KTPOLYPHASEFILTERBANK_HH_ */
