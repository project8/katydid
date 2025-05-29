/*
 * KTCavityNoiseGenerator.hh
 *
 *  Created on: May 28, 2025
 *      Author: ehtkarim
*/

#ifndef KTCAVITYNOISEGENERATOR_HH_
#define KTCAVITYNOISEGENERATOR_HH_

#include "KTGaussianNoiseGenerator.hh"

#include <string>

namespace Katydid
{

    /*! @class KTCavityNoiseGenerator
     *  @brief Generates cavity noise in a time series
     */
    class KTCavityNoiseGenerator : public KTGaussianNoiseGenerator
    {
        public:
            KTCavityNoiseGenerator(const std::string& name = "cavity-noise-generator");
            virtual ~KTCavityNoiseGenerator();

            virtual bool ConfigureDerivedGenerator(const scarab::param_node* node);

        protected:
            struct ModelPars
            {
                double f0;
                double Q_L;
                double Q0;
                double A;
                double T_line_start;
                double T_line_end;
                double T_cav;
                double T_isol;
                double epsilon;
                double f_lo;

                ModelPars();
            };

            ModelPars   fPars;
            std::string fTransformFlag;
            double      fNoiseScaling;

            double NoisePSD(double f) const;

        public:
            virtual bool GenerateTS(KTTimeSeriesData& data);
    };

} /* namespace Katydid */

#endif /* KTCAVITYNOISEGENERATOR_HH_ */
