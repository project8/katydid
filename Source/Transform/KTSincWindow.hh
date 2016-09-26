/**
 @file KTSincWindow.hh
 @brief Contains KTSincWindow
 @details Implementation of a sinc window function.
 @author: N. S. Oblath
 @date: Created on: Apr 18, 2013
 */

#ifndef KTSINCWINDOW_HH_
#define KTSINCWINDOW_HH_

#include "KTWindowFunction.hh"

namespace Katydid
{
    
    /*!
     @class KTSincWindow
     @author N. S. Oblath

     @brief Implementation of a sinc window function.

     @details
     Within the window area, all weights are 1.
     Outside of the window area, the weights are 0.

     Available configuration values:
      none
    */

    class KTSincWindow : public KTWindowFunction
    {
        public:
            KTSincWindow(const std::string& name = "sinc");
            virtual ~KTSincWindow();

            virtual bool ConfigureWFSubclass(const scarab::param_node* node);

            virtual double GetWeight(double time) const;

            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTSINCWINDOW_HH_ */
