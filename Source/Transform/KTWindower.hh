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

#include "logger.hh"
#include "KTSlot.hh"

#include <string>


namespace Katydid
{
    
    class KTEggHeader;
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
     - "header": void (Nymph::KTDataPtr) -- Initialize the window function from an Egg header; Requires KTEggHeader
     - "ts-real": void (Nymph::KTDataPtr) -- Window the time series; Requires KTTimeSeriesData containing KTTimeSeriesReal; Does not add data; Emits signal "windowed"
     - "ts-fftw": void (Nymph::KTDataPtr) -- Window the time series; Requires KTTimeSeriesData containing KTTimeSeriesFFTW; Does not add data; Emits signal "windowed"

     Signals:
     - "windowed": void (Nymph::KTDataPtr) -- Emitted upon performance of a windowing; Guarantees KTTimeSeriesData.
    */

    class KTWindower : public Nymph::KTProcessor
    {
        public:
            KTWindower(const std::string& name = "windower");
            virtual ~KTWindower();

            bool Configure(const scarab::param_node* node);

            KTWindowFunction* GetWindowFunction() const;
            void SetWindowFunction(KTWindowFunction* wf);
            bool SelectWindowFunction(const std::string& windowType);

        private:
            KTWindowFunction* fWindowFunction;

        public:
            bool InitializeWindow(double binWidth, double size);
            bool InitializeWithHeader(KTEggHeader& header);

            /// Window the data object's time series (real-type)
            bool WindowDataReal(KTTimeSeriesData& tsData);
            /// Window the data object's time series (fftw-type)
            bool WindowDataFFTW(KTTimeSeriesData& tsData);

            /// Window a single time series (real-type)
            bool ApplyWindow(KTTimeSeriesReal* ts) const;
            /// Window a single time series (fftw-type)
            bool ApplyWindow(KTTimeSeriesFFTW* ts) const;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fWindowed;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTEggHeader > fHeaderSlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesFFTWSlot;
            Nymph::KTSlotDataOneType< KTTimeSeriesData > fTimeSeriesRealSlot;

    };


    inline KTWindowFunction* KTWindower::GetWindowFunction() const
    {
        return fWindowFunction;
    }



} /* namespace Katydid */
#endif /* KTWINDOWER_HH_ */
