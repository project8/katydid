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

#include "factory.hh"

#include <string>
#include <vector>

class TH1D;


namespace Katydid
{
    
    class KTTimeSeriesData;

    /*!
     @class KTWindowFunction
     @author N. S. Oblath

     @brief Abstract base class for window functions

     @details
     Available configuration values:
      none
    */

   class KTWindowFunction : public Nymph::KTConfigurable
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

            virtual bool Configure(const scarab::param_node* node);
            virtual bool ConfigureWFSubclass(const scarab::param_node* node) = 0;

            /// Sets fBinWidth to bundle->GetBinWidth(), and adapts the length to be the integral number of bins closest to the current fLength.
            /// Returns the adapted length.
            /// If you also need to set the length, it is recommended that you use AdaptTo(const TBundle*, double) instead of separately setting the length.
            double AdaptTo(const KTTimeSeriesData* tsData);

            virtual double GetWeight(double time) const = 0;
            double GetWeight(unsigned bin) const;

#ifdef ROOT_FOUND
            TH1D* CreateHistogram(const std::string& name = "hWindowFunction") const;
#ifdef FFTW_FOUND
            TH1D* CreateFrequencyResponseHistogram(const std::string& name = "hWFFrequencyResponse") const;
#endif
#endif

            double GetLength() const;
            double GetBinWidth() const;
            unsigned GetSize() const;

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
            double SetSize(unsigned size);

            virtual void RebuildWindowFunction() = 0;

        protected:
            std::vector< double > fWindowFunction;

            double fLength;
            double fBinWidth;
            unsigned fSize;

            ParameterName fLastSetParameter;

    };

   inline double KTWindowFunction::GetWeight(unsigned bin) const
   {
       return bin < fSize ? fWindowFunction[bin] : 0.;
   }

   inline double KTWindowFunction::GetLength() const
   {
       return fLength;
   }

   inline unsigned KTWindowFunction::GetSize() const
   {
       return fSize;
   }

   inline double KTWindowFunction::GetBinWidth() const
   {
       return fBinWidth;
   }

#define KT_REGISTER_WINDOWFUNCTION(window_class, window_name) \
        static scarab::registrar< KTWindowFunction, window_class > sWF##window_class##Registrar(window_name);

} /* namespace Katydid */
#endif /* KTWINDOWFUNCTION_HH_ */
