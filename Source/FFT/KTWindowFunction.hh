/*
 * KTWindowFunction.hh
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 *
 *      Notes: The width of the function will always be adjusted to make an integral number of bins, given the bin width.
 *             The bin width is always fixed to what the user specifies.
 *             The window is centered around 0; there will always be an equal number of bins above and below 0.
 *             However, the bin numbering starts with 0 at the _left edge_ of the window! (i.e. negative bins are to the left of the window)
 */

#ifndef KTWINDOWFUNCTION_HH_
#define KTWINDOWFUNCTION_HH_

#include "Rtypes.h"

#include <string>

class TH1D;

namespace Katydid
{

    class KTWindowFunction
    {
        public:
            KTWindowFunction();
            virtual ~KTWindowFunction();

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
