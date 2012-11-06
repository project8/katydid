/**
 @file KTRectangularWindow.hh
 @brief Contains KTRectangularWindow
 @details Implementation of a rectangular window function.
 @author: N. S. Oblath
 @date: Created on: Sep 18, 2011
 */

#ifndef KTRECTANGULARWINDOW_HH_
#define KTRECTANGULARWINDOW_HH_

#include "KTEventWindowFunction.hh"

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
      none
    */

    class KTRectangularWindow : public KTEventWindowFunction
    {
        public:
            KTRectangularWindow();
            KTRectangularWindow(const KTTimeSeriesChannelData* tsData);
            virtual ~KTRectangularWindow();

            virtual Bool_t ConfigureEventWindowFunctionSubclass(const KTPStoreNode* node);

            virtual Double_t GetWeight(Double_t time) const;
            virtual Double_t GetWeight(UInt_t bin) const;

        protected:
            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTRECTANGULARWINDOW_HH_ */
