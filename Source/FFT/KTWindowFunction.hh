/**
 @file KTWindowFunction.hh
 @brief Contains KTWindowFunction
 @details Abstract base class for window functions.
 @author: N. S. Oblath
 @date: Created on: Sep 18, 2011
 */

#ifndef KTWINDOWFUNCTION_HH_
#define KTWINDOWFUNCTION_HH_

#include "Rtypes.h"

#include <string>

class TH1D;

namespace Katydid
{
    class KTPStoreNode;

    /*!
     @class KTWindowFunction
     @author N. S. Oblath

     @brief Abstract base class for window functions

     @details
     Name for specifying this window function: rectangular

     Available configuration values:
     \li \c length -- length of the window in time units
     \li \c bin_width -- length of the each bin in time units
     \li \c size -- number of bins in the window

     @note
     The width of the function will always be adjusted to make an integral number of bins, given the bin width.
     The bin width is always fixed to what the user specifies.
     The window is centered around 0; there will always be an equal number of bins above and below 0.
     However, the bin numbering starts with 0 at the _left edge_ of the window! (i.e. negative bins are to the left of the window)

    */

    class KTWindowFunction
    {
        public:
            KTWindowFunction();
            virtual ~KTWindowFunction();

            Bool_t Configure(const KTPStoreNode* node);
            virtual Bool_t ConfigureWindowFunctionSubclass(const KTPStoreNode* node) = 0;

            virtual Double_t GetWeight(Double_t) const = 0;
            virtual Double_t GetWeight(UInt_t) const = 0;

            virtual TH1D* CreateHistogram(const std::string& name) const = 0;
            virtual TH1D* CreateHistogram() const = 0;
            virtual TH1D* CreateFrequencyResponseHistogram(const std::string& name) const = 0;
            virtual TH1D* CreateFrequencyResponseHistogram() const = 0;

            virtual Double_t GetLength() const = 0;
            virtual UInt_t GetSize() const = 0;
            virtual Double_t GetBinWidth() const = 0;

            virtual Double_t SetLength(Double_t) = 0;
            virtual Double_t SetBinWidth(Double_t) = 0;
            virtual Double_t SetSize(UInt_t) = 0;

            ClassDef(KTWindowFunction, 2);
    };

} /* namespace Katydid */
#endif /* KTWINDOWFUNCTION_HH_ */
