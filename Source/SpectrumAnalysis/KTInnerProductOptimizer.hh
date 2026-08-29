/**
 @file KTInnerProductOptimizer.hh
 @brief Contains KTInnerProductOptimizer
 @details Finds the best matches in the MF SNR matrix
 @author: F. Thomas
 @date: May 5, 2021
 */

#ifndef KTINNERPRODUCT_OPTIMIZER_HH_
#define KTINNERPRODUCT_OPTIMIZER_HH_

#include "KTProcessor.hh"

#include "KTMemberVariable.hh"
#include "KTSlot.hh"

#include <cmath>

namespace scarab
{
    class param_node;
}

namespace Katydid
{

    class KTInnerProductData;

    /*!
     @class KTConvertToTemplate
     @author F. Thomas

     @brief Converts data to Matched Filter templates

     @details
     Converts a matrix of timeseries data to a matrix of normalized Matched Filter templates.
     The normalization includes a noise contribution, which assumes white Gaussian noise.
     Using this normalization the MF output will be a dimensionless SNR.

     For an input timeseries X the normalized template is
     ```
     Y = 1/sqrt(X^H*C^-1*X) C^-1 X
     ```
     where C is the noise covariance matrix.
     With the white Gaussian assumption this simplifies to
     ```
     Y = sqrt(2)/sqrt(X^2*V) * X
     ```
     where V is the Johnson-Nyquist noise variance.

     Configuration name: "template-converter"

     Available configuration values:
     - "T": double -- Noise temperature
     - "bandwidth": double -- Bandwidth of the input data, i.e. half the sampling rate

     Slots:
     - "ts-matrix": void (KTDataPtr) -- Converts timeseries to normalized MF template; Requires KTAggregatedTSMatrixData; Adds KTAggregatedTemplateMatrixDatar; Emits signal "template-matrix"

     Signals:
     - "template-matrix": void (KTDataPtr) -- Emitted conversion; Guarantees KTAggregatedTemplateMatrixData.

    */
    class KTInnerProductOptimizer : public Nymph::KTProcessor
    {
        public:
            KTInnerProductOptimizer(const std::string& name = "inner-product-optimizer");

            bool Configure(const scarab::param_node* node);



        public:
            bool FindOptimum(KTInnerProductData& fData);

        private:

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fOptSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTInnerProductData > fIPSlot;

    };

} /* namespace Katydid */
#endif /* KTINNERPRODUCT_OPTIMIZER_HH_ */
