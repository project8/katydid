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
    KTLOGGER(fftlog_comp, "katydid.fft");

    class KTEggHeader;
    class KTPStoreNode;
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
     - "header": void (const KTEggHeader* header) -- Initialize the window function from an Egg header
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

            Bool_t Configure(const KTPStoreNode* node);

            UInt_t GetNSubsets() const;
            void SetNSubsets(UInt_t subsets);

            UInt_t GetSubsetSize() const;
            void SetSubsetSize(UInt_t size);

        private:
            UInt_t fNSubsets;
            UInt_t fSubsetSize;

        public:
            Bool_t InitializeWindow();
            void InitializeWithHeader(const KTEggHeader* header);

            /// Apply the PFB to the data object's time series (real-type)
            Bool_t ProcessDataReal(const KTTimeSeriesData& tsData);
            /// Apply the PFB to the data object's time series (fftw-type)
            Bool_t ProcessDataFFTW(const KTTimeSeriesData& tsData);

            /// Create a new data object for the filtered time series (real-type)
            KTDataPtr CreateFilteredDataReal(const KTTimeSeriesData& tsData);
            /// Create a new data object for the filtered time series (fftw-type)
            KTDataPtr CreateFilteredDataFFTW(const KTTimeSeriesData& tsData);

            /// Apply PFB to a single time series (real-type); a new time series is produced
            KTTimeSeriesReal* ApplyPFB(const KTTimeSeriesReal* data) const;
            /// Apply PFB to a single time series (fftw-type); a new time series is produced
            KTTimeSeriesFFTW* ApplyPFB(const KTTimeSeriesFFTW* data) const;

        private:
            Bool_t TransferHeaderInformation(const KTSliceHeader& oldHeader, KTSliceHeader& newHeader);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fWindowed;

            //***************
            // Slots
            //***************

        private:
            KTSlotOneArg< void (const KTEggHeader*) > fHeaderSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesFFTWSlot;
            KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesRealSlot;

    };


    inline UInt_t KTPolyphaseFilterBank::GetNSubsets() const
    {
        return fNSubsets;
    }

    inline void KTPolyphaseFilterBank::SetNSubsets(UInt_t subsets)
    {
        fNSubsets = subsets;
        return;
    }

    inline UInt_t KTPolyphaseFilterBank::GetSubsetSize() const
    {
        return fSubsetSize;
    }

    inline void KTPolyphaseFilterBank::SetSubsetSize(UInt_t size)
    {
        fNSubsets = size;
        return;
    }


} /* namespace Katydid */
#endif /* KTPOLYPHASEFILTERBANK_HH_ */
