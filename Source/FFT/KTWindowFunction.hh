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
            /// If you also need to set the length, it is recommended that you use AdaptTo(const TBundle*, double) instead of separately setting the length.
            double AdaptTo(const KTTimeSeriesData* tsData);

            virtual double GetWeight(double time) const = 0;
            double GetWeight(UInt_t bin) const;

#ifdef ROOT_FOUND
            TH1D* CreateHistogram(const std::string& name = "hWindowFunction") const;
#ifdef FFTW_FOUND
            TH1D* CreateFrequencyResponseHistogram(const std::string& name = "hWFFrequencyResponse") const;
#endif
#endif

            double GetLength() const;
            double GetBinWidth() const;
            UInt_t GetSize() const;

            /// Sets fLength to length, and adapts the bin width to be an integral number of bins closest to the current bw.
            /// Returns the adapted bin width.
            /// NOTE: this changes the size of the window!
            double SetLength(double length);
            /// Sets fBinWidth to bw, and adapts the length to be the integral number of bins closest to the current fLength.
            /// Returns the adapted length.
            /// NOTE: this changes the size of the window!
            double SetBinWidth(double bw);
            /// Adapts the given length (length) to the be an integer multiple of the given bin width (bw).
            /// Returns the adapted length.
            /// NOTE: this changes the size of the window!
            double SetBinWidthAndLength(double bw, double length);
            /// Adapts the given bin width to be an integer divisor of the given length (length)
            /// Returns the adapted bin width.
            /// NOTE: this changes the size of the window!
            double SetLengthAndBinWidth(double length, double bw);
            /// Sets the number of bins; leaves fBinWidth as is, and sets fLength accordingly.
            /// Returns the adapted length.
            /// NOTE: this changes the size of the window! (duh)
            double SetSize(UInt_t size);

            virtual void RebuildWindowFunction() = 0;

        protected:
            std::vector< double > fWindowFunction;

            double fLength;
            double fBinWidth;
            UInt_t fSize;

            ParameterName fLastSetParameter;

    };

   inline double KTWindowFunction::GetWeight(UInt_t bin) const
   {
       return bin < fSize ? fWindowFunction[bin] : 0.;
   }

   inline double KTWindowFunction::GetLength() const
   {
       return fLength;
   }

   inline UInt_t KTWindowFunction::GetSize() const
   {
       return fSize;
   }

   inline double KTWindowFunction::GetBinWidth() const
   {
       return fBinWidth;
   }


} /* namespace Katydid */
#endif /* KTWINDOWFUNCTION_HH_ */
