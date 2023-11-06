/**
 @file KTConvertToTemplate.hh
 @brief Contains KTConvertToTemplate
 @details Converts timeseries data to a matched filter template
 @author: F. Thomas
 @date: May 4, 2021
 */

#ifndef KTCONVERTTOTEMPLATE_HH_
#define KTCONVERTTOTEMPLATE_HH_

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

    class KTAggregatedTSMatrixData;

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
    class KTConvertToTemplate : public Nymph::KTProcessor
    {
        public:
            KTConvertToTemplate(const std::string& name = "template-converter");

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, NoiseTemperature);
            MEMBERVARIABLE(double, Bandwidth);
            MEMBERVARIABLE(double, NoiseStd);


        public:
            bool Convert(KTAggregatedTSMatrixData& fData);

        private:

            void CalcNoiseStd();

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTAggregatedTSMatrixData > fTSSlot;

    };

} /* namespace Katydid */
#endif /* KTCONVERTTOTEMPLATE_HH_ */
