/**
 @file KTWindower.hh
 @brief Contains KTWindower
 @details Windows a time series
 @author: N. S. Oblath
 @date: Apr 18, 2013
 */

#ifndef KTWINDOWER_HH_
#define KTWINDOWER_HH_

#include "KTProcessor.hh"

#include "KTLogger.hh"
#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

#include <string>

namespace Katydid
{
    KTLOGGER(fftlog_comp, "katydid.fft");

    class KTData;
    class KTEggHeader;
    class KTPStoreNode;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;
    class KTWindowFunction;

    /*!
     @class KTWindower
     @author N. S. Oblath

     @brief Applies a window function to a time series.

     @details
     KTWindower applies a window function to a time series (either real- or fftw-type)

     NOTE: The windowing is done IN-PLACE! The data object will not be extended.

     Configuration name: "windower"

     Available configuration values:
     - "window-function-type": string -- sets the type of window function to be used
     - "window-function": subtree -- parent node for the window function configuration

     Slots:
     - "header": void (const KTEggHeader* header) -- Initialize the window function from an Egg header
     - "ts-real": void (shared_ptr<KTData>) -- Window the time series; Requires KTTimeSeriesData containing KTTimeSeriesReal; Does not add data; Emits signal "windowed"
     - "ts-fftw": void (shared_ptr<KTData>) -- Window the time series; Requires KTTimeSeriesData containing KTTimeSeriesFFTW; Does not add data; Emits signal "windowed"

     Signals:
     - "windowed": void (shared_ptr<KTData>) -- Emitted upon performance of a windowing; Guarantees KTTimeSeriesData.
    */

    class KTWindower : public KTProcessor
    {
        public:
            KTWindower(const std::string& name = "windower");
            virtual ~KTWindower();

            Bool_t Configure(const KTPStoreNode* node);

            KTWindowFunction* GetWindowFunction() const;
            void SetWindowFunction(KTWindowFunction* wf);

        private:
            KTWindowFunction* fWindowFunction;

        public:
            Bool_t InitializeWindow();
            void InitializeWithHeader(const KTEggHeader* header);

            /// Window the data object's time series (real-type)
            Bool_t WindowDataReal(KTTimeSeriesData& tsData);
            /// Window the data object's time series (fftw-type)
            Bool_t WindowDataFFTW(KTTimeSeriesData& tsData);

            /// Window a single time series (real-type)
            Bool_t ApplyWindow(KTTimeSeriesReal* data) const;
            /// Window a single time series (fftw-type)
            Bool_t ApplyWindow(KTTimeSeriesFFTW* data) const;

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


    inline KTWindowFunction* KTWindower::GetWindowFunction() const
    {
        return fWindowFunction;
    }



} /* namespace Katydid */
#endif /* KTWINDOWER_HH_ */
