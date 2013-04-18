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
    /*!
     @class KTRectangularWindow
     @author N. S. Oblath

     @brief Implementation of a rectangular window function.

     @details
     Within the window area, all weights are 1.
     Outside of the window area, the weights are 0.

     Available configuration values:
      - boxcar-size: UInt_t -- size (# of bins) of the interval where the weight is 1; this interval is centered in the window
    */

    class KTRectangularWindow : public KTWindowFunction
    {
        public:
            KTRectangularWindow(const std::string& name = "rectangular");
            virtual ~KTRectangularWindow();

            virtual Bool_t ConfigureWFSubclass(const KTPStoreNode* node);

            virtual Double_t GetWeight(Double_t time) const;

        protected:
            virtual void RebuildWindowFunction();

        public:
            UInt_t GetBoxcarSize() const;
            void SetBoxcarSize(UInt_t size);

        private:
            UInt_t fBoxcarSize;

    };

    inline UInt_t KTRectangularWindow::GetBoxcarSize() const
    {
        return fBoxcarSize;
    }

    inline void KTRectangularWindow::SetBoxcarSize(UInt_t size)
    {
        fBoxcarSize = size;
        return;
    }


} /* namespace Katydid */
#endif /* KTRECTANGULARWINDOW_HH_ */
