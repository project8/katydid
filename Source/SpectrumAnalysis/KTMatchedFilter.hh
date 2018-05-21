/**
 @file KTMatchedFilter.hh
 @brief Contains KTMatchedFilter
 @details Applies a matched filter to a template bank
 @author: N. Buzinsky
 @date: May 21, 2018
 */

#ifndef KTMATCHEDFILTER_HH_
#define KTMATCHEDFILTER_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTSlot.hh"
#include "KTTimeSeriesFFTW.hh"

#include <cmath>

namespace scarab
{
    class param_node;
}

namespace Katydid
{
    class KTTimeSeriesData;
    //class KTTimeSeriesFFTW;

    /*!
     @class KTMatchedFilter
     @author N. Buzinsky

     @brief Applies a matched filter to between data and simulated template bank

     @details
     First pass implementation of filter for detection of general signals, not just wavetrains

     Configuration name: "matched-filter"

     Available configuration values:
     - "domain": string -- time or frequency
     - "template-bank": string -- path to template bank XXXX?
     - "step": int -- Step Size
     - "window-size": int -- window size

     Slots:
     - "ts-data": void (KTDataPtr) -- Applies a matched filter iff ts-hypothesis is also available; Requires XXXX; Adds YYYY; Emits signal ZZZZ if both are present
     - "ts-hypothesis": void (KTDataPtr) -- Applies a matched filter iff ts-data is also available; Requires XXXX; Adds YYYY; Emits signal ZZZZ if both are present

     Signals:
     - "fs-polar": void (KTDataPtr) -- Emitted upon low-pass filtering; Guarantees KTMatchedFilteredFSDataPolar.
    */
    class KTMatchedFilter : public Nymph::KTProcessor
    {
        public:
            KTMatchedFilter(const std::string& name = "matched-filter");
            virtual ~KTMatchedFilter();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(unsigned, WindowSize);

        public:
            bool HypothesisSetup(KTTimeSeriesData& tsData);
            bool Filter(KTTimeSeriesData& tsData);
            double SNR(KTTimeSeriesFFTW* tsData);
            double InnerProduct(KTTimeSeriesFFTW* tsData);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSSignal;
            Nymph::KTSignalData fTSHypothesisSignal;
            Nymph::KTSignalData fTSDataSignal;

            //KTTimeSeriesData  fHypothesis;
            //
            MEMBERVARIABLE(bool, HasHypothesis);
            MEMBERVARIABLE(bool, HasData);
            MEMBERVARIABLE(double, HypothesisCachedInnerProduct);

            MEMBERVARIABLE(KTTimeSeriesFFTW, Hypothesis);

            //***************
            // Slots
            //***************

        private:
            //FIX ME???
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTSDataSlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTSHypothesisSlot;

    };

}
 /* namespace Katydid */
#endif /* KTMatchedFILTER_HH_ */
