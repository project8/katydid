/**
 @file KTWindowFunction.hh
 @brief Contains KTWindowFunction
 @details Abstract base class for window functions.
 @author: N. S. Oblath
 @date: Created on: Sep 18, 2011
 */

#ifndef KTWINDOWFUNCTION_HH_
#define KTWINDOWFUNCTION_HH_

#include "KTConfigurable.hh"

#include "Rtypes.h"

#include <string>
#include <vector>

class TH1D;

namespace Katydid
{
    class KTPStoreNode;
    class KTTimeSeriesData;

    /*!
     @class KTWindowFunction
     @author N. S. Oblath

     @brief Abstract base class for window functions

     @details
     Available configuration values:
      none
    */

   class KTWindowFunction : public KTConfigurable
    {
        protected:
            enum ParameterName
            {
                kBinWidth,
                kSize,
                kLength
            };

        public:
            KTWindowFunction(const std::string& name = "generic-window-function");
            virtual ~KTWindowFunction();

            virtual Bool_t Configure(const KTPStoreNode* node);
            virtual Bool_t ConfigureWFSubclass(const KTPStoreNode* node) = 0;

            /// Sets fBinWidth to bundle->GetBinWidth(), and adapts the length to be the integral number of bins closest to the current fLength.
            /// Returns the adapted length.
            /// If you also need to set the length, it is recommended that you use AdaptTo(const TBundle*, Double_t) instead of separately setting the length.
            Double_t AdaptTo(const KTTimeSeriesData* tsData);
            /// Sets fBinWidth to bundle->GetBinWidth(), and adapts the length to be the integer multiple of the bin width closest to the given length.
            /// Returns the adapted length.
            Double_t AdaptTo(const KTTimeSeriesData* tsData, Double_t length);

            virtual Double_t GetWeight(Double_t time) const = 0;
            Double_t GetWeight(UInt_t bin) const;

#ifdef ROOT_FOUND
            TH1D* CreateHistogram(const std::string& name = "hWindowFunction") const;
            TH1D* CreateFrequencyResponseHistogram(const std::string& name = "hWFFrequencyResponse") const;
#endif

            Double_t GetLength() const;
            Double_t GetBinWidth() const;
            UInt_t GetSize() const;

            /// Sets fLength to length, and adapts the bin width to be an integral number of bins closest to the current bw.
            /// Returns the adapted bin width.
            /// NOTE: this changes the size of the window!
            Double_t SetLength(Double_t length);
            /// Sets fBinWidth to bw, and adapts the length to be the integral number of bins closest to the current fLength.
            /// Returns the adapted length.
            /// NOTE: this changes the size of the window!
            Double_t SetBinWidth(Double_t bw);
            /// Adapts the given length (length) to the be an integer multiple of the given bin width (bw).
            /// Returns the adapted length.
            /// NOTE: this changes the size of the window!
            Double_t SetBinWidthAndLength(Double_t bw, Double_t length);
            /// Adapts the given bin width to be an integer divisor of the given length (length)
            /// Returns the adapted bin width.
            /// NOTE: this changes the size of the window!
            Double_t SetLengthAndBinWidth(Double_t length, Double_t bw);
            /// Sets the number of bins; leaves fBinWidth as is, and sets fLength accordingly.
            /// Returns the adapted length.
            /// NOTE: this changes the size of the window! (duh)
            Double_t SetSize(UInt_t size);

        protected:
            virtual void RebuildWindowFunction() = 0;

            std::vector< Double_t > fWindowFunction;

            Double_t fLength;
            Double_t fBinWidth;
            UInt_t fSize;

            ParameterName fLastSetParameter;

    };

   inline Double_t KTWindowFunction::GetWeight(UInt_t bin) const
   {
       return bin < fSize ? fWindowFunction[bin] : 0.;
   }

   inline Double_t KTWindowFunction::GetLength() const
   {
       return fLength;
   }

   inline UInt_t KTWindowFunction::GetSize() const
   {
       return fSize;
   }

   inline Double_t KTWindowFunction::GetBinWidth() const
   {
       return fBinWidth;
   }


} /* namespace Katydid */
#endif /* KTWINDOWFUNCTION_HH_ */
