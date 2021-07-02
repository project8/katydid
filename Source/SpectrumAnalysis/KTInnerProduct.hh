/**
 @file KTInnerProduct.hh
 @brief Contains KTInnerProduct
 @details Applies the Matched Filter inner product for multiple channels
 @author: F. Thomas
 @date: Apr 28, 2021
 */

#ifndef KTINNERPRODUCT_HH_
#define KTINNERPRODUCT_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include <eigen3/Eigen/Dense>

namespace scarab
{
    class param_node;
}

namespace Katydid
{

	class KTAggregatedTemplateMatrixData;
	class KTAggregatedTSMatrixData;

    /*!
     @class KTInnerProduct
     @author F. Thomas

     @brief Applies the Matched Filter inner product for multiple channels

     @details
     Frequency-domain implementation of a single-pole RC low-pass filter.  
     This is no-doubt a non-ideal implementation, but demonstrates the features of a processor very well.

     The relationship between the cutoff frequency, f_c, and the RC constant is:
     ```
     RC = 1 / 2*pi*f_c
     ```

     Configuration name: "low-pass-filter"

     Available configuration values:
     - "rc": double -- RC time constant of the filter

     Slots:
     - "fs-polar": void (KTDataPtr) -- Applies a low-pass filter; Requires KTFrequencySpectrumDataPolar; Adds KTLowPassFilteredFSDataPolar; Emits signal "fs-polar"
     - "ts-fftw": void (KTDataPtr) -- Applies a low-pass filter; Requires KTFrequencySpectrumDataFFTW; Adds KTLowPassFilteredFSDataFFTW; Emits signal "fs-fftw"

     Signals:
     - "snr": void (KTDataPtr) -- Emitted upon low-pass filtering; Guarantees KTLowPassFilteredPSData.
    */
    class KTInnerProduct : public Nymph::KTProcessor
    {
        public:
            KTInnerProduct(const std::string& name = "inner-product");
            bool Configure(const scarab::param_node* node);


        public:
            bool SetTemplates(KTAggregatedTemplateMatrixData& fTemplate);
            bool Multiply(KTAggregatedTSMatrixData& fData);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fSnrSignal;

            Eigen::ArrayXXcd fTemplateMatrix;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTAggregatedTSMatrixData > fDataSlot;
            Nymph::KTSlotDataOneType< KTAggregatedTemplateMatrixData > fTemplateSlot;

    };

} /* namespace Katydid */
#endif /* KTINNERPRODUCT_HH_ */
