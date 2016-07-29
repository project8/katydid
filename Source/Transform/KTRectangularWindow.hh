/**
 @file KTRectangularWindow.hh
 @brief Contains KTRectangularWindow
 @details Implementation of a rectangular window function.
 @author: N. S. Oblath
 @date: Created on: Sep 18, 2011
 */

#ifndef KTRECTANGULARWINDOW_HH_
#define KTRECTANGULARWINDOW_HH_

#include "KTWindowFunction.hh"

namespace Katydid
{
    using namespace Nymph;
    /*!
     @class KTRectangularWindow
     @author N. S. Oblath

     @brief Implementation of a rectangular window function.

     @details
     Within the window area, all weights are 1.
     Outside of the window area, the weights are 0.

     Available configuration values:
      - boxcar-size: unsigned -- size (# of bins) of the interval where the weight is 1; this interval is centered in the window
    */

    class KTRectangularWindow : public KTWindowFunction
    {
        public:
            KTRectangularWindow(const std::string& name = "rectangular");
            virtual ~KTRectangularWindow();

            virtual bool ConfigureWFSubclass(const scarab::param_node* node);

            virtual double GetWeight(double time) const;

            virtual void RebuildWindowFunction();

        public:
            unsigned GetBoxcarSize() const;
            void SetBoxcarSize(unsigned size);

        private:
            unsigned fBoxcarSize;

    };

    inline unsigned KTRectangularWindow::GetBoxcarSize() const
    {
        return fBoxcarSize;
    }

    inline void KTRectangularWindow::SetBoxcarSize(unsigned size)
    {
        fBoxcarSize = size;
        return;
    }


} /* namespace Katydid */
#endif /* KTRECTANGULARWINDOW_HH_ */
