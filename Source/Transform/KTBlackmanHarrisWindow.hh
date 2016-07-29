/**
 @file KTBlackmanHarrisWindow.hh
 @brief Contains KTBlackmanHarrisWindow
 @details Implementation of a Blackman-Harris window function.
 @author: N. S. Oblath
 @date: Created on: Apr 18, 2013
 */

#ifndef KTBLACKMANHARRISWINDOW_HH_
#define KTBLACKMANHARRISWINDOW_HH_

#include "KTWindowFunction.hh"

namespace Katydid
{
    using namespace Nymph;
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

            virtual bool ConfigureWFSubclass(const scarab::param_node* node);

            virtual double GetWeight(double time) const;

            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTBLACKMANHARRISWINDOW_HH_ */
