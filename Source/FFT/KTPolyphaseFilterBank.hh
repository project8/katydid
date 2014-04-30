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

namespace Katydid
{
    KTLOGGER(fftlog_comp, "KTPolyphaseFilterBank.hh");

    class KTEggHeader;
    class KTParamNode;
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
     - "ts-real": void (KTDataPtr) -- Window the time series; Requires KTTimeSeriesData containing KTTimeSeriesReal; Does not add data; Emits signal "windowed"
     - "ts-fftw": void (KTDataPtr) -- Window the time series; Requires KTTimeSeriesData containing KTTimeSeriesFFTW; Does not add data; Emits signal "windowed"

     Signals:
     - "windowed": void (KTDataPtr) -- Emitted upon performance of a windowing; Guarantees KTTimeSeriesData.
    */

    class KTPolyphaseFilterBank : public KTProcessor
    {
        public:
            KTPolyphaseFilterBank(const std::string& name = "polyphase-filter-bank");
            virtual ~KTPolyphaseFilterBank();

            bool Configure(const KTParamNode* node);

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
            KTDataPtr CreateFilteredDataReal(const KTTimeSeriesData& tsData);
            /// Create a new data object for the filtered time series (fftw-type)
            KTDataPtr CreateFilteredDataFFTW(const KTTimeSeriesData& tsData);

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
            KTSignalData fWindowed;

            //***************
            // Slots
            //***************

        private:
            KTSlotOneArg< void (KTEggHeader*) > fHeaderSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesFFTWSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesRealSlot;

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
