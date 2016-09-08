/*
 * KTHannWindow.hh
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */
/**
 @file KTHannWindow.hh
 @brief Contains KTHannWindow
 @details Implementation of a Hann window function.
 @author: N. S. Oblath
 @date: Created on: Sep 18, 2011
 */

#ifndef KTHANNWINDOW_HH_
#define KTHANNWINDOW_HH_

#include "KTWindowFunction.hh"

namespace Katydid
{
    
    /*!
     @class KTHannWindow
     @author N. S. Oblath

     @brief Implementation of a Hann window function.

     @details
     Name for specifying this window function: hann

     The Hann window is a general-purpose windowing function often used to select a subset of samples for performing a Fourier Transform.
     It has the advantage of low aliasing with a fairly small loss of resolution.
     Source: Wikipedia: Hann function (http://en.wikipedia.org/wiki/Hann_function, 8/1/2012)

     Available configuration values:
      none
    */

    class KTHannWindow : public KTWindowFunction
    {
        public:
            KTHannWindow(const std::string& name = "hann");
            virtual ~KTHannWindow();

            virtual bool ConfigureWFSubclass(const scarab::param_node* node);

            virtual double GetWeight(double time) const;

            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTHANNWINDOW_HH_ */
