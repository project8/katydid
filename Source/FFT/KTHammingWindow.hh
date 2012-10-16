/**
 @file KTHammingWindow.hh
 @brief Contains KTHammingWindow
 @details Implementation of a Hamming window function.
 @author: N. S. Oblath
 @date: Created on: Sep 18, 2011
 */

#ifndef KTHAMMINGWINDOW_HH_
#define KTHAMMINGWINDOW_HH_

#include "KTEventWindowFunction.hh"

namespace Katydid
{
    /*!
     @class KTHammingWindow
     @author N. S. Oblath

     @brief Implementation of a Hamming window function.

     @details
     Name for specifying this window function: hamming

     The Hamming window has the same functional form as the Hann window, with parameters optimized to minimize the
     nearest side lobe (side lobe height is about 1/5 that of the Hann window).
     Source: Wikipedia: Window function (http://en.wikipedia.org/wiki/Window_function#Hamming_window, 8/1/2012)

     Available configuration values:
      none
    */

    class KTHammingWindow : public KTEventWindowFunction
    {
        public:
            KTHammingWindow();
            KTHammingWindow(const KTTimeSeriesDataReal* tsData);
            virtual ~KTHammingWindow();

            virtual Bool_t ConfigureEventWindowFunctionSubclass(const KTPStoreNode* node);

            virtual Double_t GetWeight(Double_t time) const;
            virtual Double_t GetWeight(UInt_t bin) const;

        protected:
            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTHAMMINGWINDOW_HH_ */
