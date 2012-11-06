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

#include "KTEventWindowFunction.hh"

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

    class KTHannWindow : public KTEventWindowFunction
    {
        public:
            KTHannWindow();
            KTHannWindow(const KTTimeSeriesChannelData* tsData);
            virtual ~KTHannWindow();

            virtual Bool_t ConfigureEventWindowFunctionSubclass(const KTPStoreNode* node);

            virtual Double_t GetWeight(Double_t time) const;
            virtual Double_t GetWeight(UInt_t bin) const;

        protected:
            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTHANNWINDOW_HH_ */
