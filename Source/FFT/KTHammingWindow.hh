/**
 @file KTHammingWindow.hh
 @brief Contains KTHammingWindow
 @details Implementation of a Hamming window function.
 @author: N. S. Oblath
 @date: Created on: Sep 18, 2011
 */

#ifndef KTHAMMINGWINDOW_HH_
#define KTHAMMINGWINDOW_HH_

#include "KTWindowFunction.hh"

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

    class KTHammingWindow : public KTWindowFunction
    {
        public:
            KTHammingWindow(const std::string& name = "hamming");
            virtual ~KTHammingWindow();

            virtual bool ConfigureWFSubclass(const KTParamNode* node);

            virtual double GetWeight(double time) const;

            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTHAMMINGWINDOW_HH_ */
