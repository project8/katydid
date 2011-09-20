/*
 * KTEventWindowFunction.hh
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#ifndef KTEVENTWINDOWFUNCTION_HH_
#define KTEVENTWINDOWFUNCTION_HH_

#include "KTWindowFunction.hh"

class TArrayD;

namespace Katydid
{
    class KTEvent;

    class KTEventWindowFunction : public KTWindowFunction
    {
        public:
            KTEventWindowFunction();
            KTEventWindowFunction(const KTEvent* event);
            virtual ~KTEventWindowFunction();

            /// Sets fBinWidth to event->GetBinWidth(), and adapts the width to be the integral number of bins closest to the current fWidth.
            /// Returns the adapted width.
            /// If you also need to set the width, it is recommended that you use AdaptTo(const TEvent*, Double_t) instead of separately setting the width.
            virtual Double_t AdaptTo(const KTEvent* event);
            /// Sets fBinWidth to event->GetBinWidth(), and adapts the width to be the integer multiple of the bin width closest to the given width.
            /// Returns the adapted width.
            virtual Double_t AdaptTo(const KTEvent* event, Double_t width);

            virtual Double_t GetWeight(Double_t time) const = 0;
            virtual Double_t GetWeight(Int_t bin) const = 0;

            virtual TH1D* CreateHistogram() const;
            virtual TH1D* CreateFrequencyResponseHistogram() const;

            virtual Double_t GetWidth() const;
            virtual Int_t GetWidthInBins() const;

            Double_t GetBinWidth() const;

            /// Adapts the given width to be the integer multiple of fBinWidth.
            /// Returns the adapted width.
            /// If you also need to set the bin width, it is highly recommended that you use SetBinWidthAndWidth(Double_t, Double_t).
            Double_t SetWidth(Double_t width);
            /// Sets fBinWidth to bw, and adapts the width to be the integral number of bins closest to the current fWidth.
            /// Returns the adapted width.
            /// It is recommended that you have already set fWidth before calling this function.
            Double_t SetBinWidth(Double_t bw);
            ///Adapts the given width to the be an integer mulitple of the given bin width (bw).
            /// Returns the adapted width.
            Double_t SetBinWidthAndWidth(Double_t bw, Double_t width);

        protected:
            virtual void RebuildWindowFunction() = 0;

            TArrayD* fWindowFunction;

            Double_t fWidth;
            Double_t fBinWidth;

            Int_t fWidthInBins;


    };

} /* namespace Katydid */
#endif /* KTEVENTWINDOWFUNCTION_HH_ */
