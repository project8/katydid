/*
 * KTPowerFitCut.hh
 *
 *  Created on: Nov 8, 2016
 *      Author: ezayas
 */

#ifndef KTPOWERFITCUT_HH_
#define KTPOWERFITCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTPowerFitData;

    /*
     @class KTPowerFitCut
     @author E. Zayas

     @brief Keeps power fit results with specified fit parameters

     @details
     Supports cutting on validity, number of peaks, and first four moments

     Configuration name: "power-fit-cut"

     Available configuration values:
     - "keep-valid": bool -- validity to accept
     - "min-npeaks": int -- minimum number of peaks to accept
     - "max-npeaks": int -- maximum number of peaks to accept
     - "min-average": double -- minimum mean to accept
     - "max-average": double -- maximum mean to accept
     - "min-rms": double -- minimum rms to accept
     - "max-rms": double -- maximum rms to accept
     - "min-skewness": double -- minimum skewness to accept
     - "max-skewness": double -- maximum skewness to accept
     - "min-kurtosis": double -- minimum kurtosis to accept
     - "max-kurtosis": double -- maximum kurtosis to accept
    */

    class KTPowerFitCut : public Nymph::KTCutOneArg< KTPowerFitData >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTPowerFitCut(const std::string& name = "power-fit-cut");
        ~KTPowerFitCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(bool, Validity);
        MEMBERVARIABLE(int, MinNPeaks);
        MEMBERVARIABLE(int, MaxNPeaks);
        MEMBERVARIABLE(double, MinAverage);
        MEMBERVARIABLE(double, MaxAverage);
        MEMBERVARIABLE(double, MinRMS);
        MEMBERVARIABLE(double, MaxRMS);
        MEMBERVARIABLE(double, MinSkewness);
        MEMBERVARIABLE(double, MaxSkewness);
        MEMBERVARIABLE(double, MinKurtosis);
        MEMBERVARIABLE(double, MaxKurtosis);

    public:
        bool Apply(Nymph::KTData& data, KTPowerFitData& fitData);

    };
} // namespace Katydid

#endif /* KTPOWERFITCUT_HH_ */
