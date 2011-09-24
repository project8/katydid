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

            /// Sets fBinWidth to event->GetBinWidth(), and adapts the length to be the integral number of bins closest to the current fLength.
            /// Returns the adapted length.
            /// If you also need to set the length, it is recommended that you use AdaptTo(const TEvent*, Double_t) instead of separately setting the length.
            virtual Double_t AdaptTo(const KTEvent* event);
            /// Sets fBinWidth to event->GetBinWidth(), and adapts the length to be the integer multiple of the bin width closest to the given length.
            /// Returns the adapted length.
            virtual Double_t AdaptTo(const KTEvent* event, Double_t length);

            virtual Double_t GetWeight(Double_t time) const = 0;
            virtual Double_t GetWeight(Int_t bin) const = 0;

            virtual TH1D* CreateHistogram() const;
            virtual TH1D* CreateFrequencyResponseHistogram() const;

            virtual Double_t GetLength() const;
            virtual Int_t GetSize() const;

            virtual Double_t GetBinWidth() const;

            /// Adapts the given length to be the integer multiple of fBinWidth.
            /// Returns the adapted width.
            /// If you also need to set the bin width, it is highly recommended that you use SetBinWidthAndLength(Double_t, Double_t).
            virtual Double_t SetLength(Double_t length);
            /// Sets fBinWidth to bw, and adapts the length to be the integral number of bins closest to the current fLength.
            /// Returns the adapted length.
            /// It is recommended that you have already set fLength before calling this function.
            virtual Double_t SetBinWidth(Double_t bw);
            ///Adapts the given length to the be an integer multiple of the given bin width (bw).
            /// Returns the adapted length.
            Double_t SetBinWidthAndLength(Double_t bw, Double_t length);
            ///Sets fWidthInBins directly; leaves fBinWidth as is, and sets fWidth accordingly.
            ///Returns the adapted length.
            virtual Double_t SetSize(Int_t size);

        protected:
            virtual void RebuildWindowFunction() = 0;

            TArrayD* fWindowFunction;

            Double_t fLength;
            Double_t fBinWidth;

            Int_t fSize;


    };

} /* namespace Katydid */
#endif /* KTEVENTWINDOWFUNCTION_HH_ */
