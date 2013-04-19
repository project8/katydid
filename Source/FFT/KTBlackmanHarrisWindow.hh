/**
 @file KTBlackmanHarrisWindow.hh
 @brief Contains KTBlackmanHarrisWindow
 @details Implementation of a Blackman-Harris window function.
 @author: N. S. Oblath
 @date: Created on: Apr 18, 2013
 */

#ifndef KTHAMMINGWINDOW_HH_
#define KTHAMMINGWINDOW_HH_

#include "KTWindowFunction.hh"

namespace Katydid
{
    /*!
     @class KTBlackmanHarrisWindow
     @author N. S. Oblath

     @brief Implementation of a Blackman-Harris window function.

     @details
     Name for specifying this window function: blackman-harris

     The Blackman-Harris window is a generalization of the Hamming-family of window functions.
     Source: Wikipedia: Window function (http://en.wikipedia.org/wiki/Window_function#BlackmanHarris_window, 8/1/2012)

     Available configuration values:
      none
    */

    class KTBlackmanHarrisWindow : public KTWindowFunction
    {
        public:
            KTBlackmanHarrisWindow(const std::string& name = "blackman-harris");
            virtual ~KTBlackmanHarrisWindow();

            virtual Bool_t ConfigureWFSubclass(const KTPStoreNode* node);

            virtual Double_t GetWeight(Double_t time) const;

            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTHAMMINGWINDOW_HH_ */
