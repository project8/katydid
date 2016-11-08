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
     KTPowerFitData objects must have a parameters `scale` and `width` which fall within the specified ranges to pass the cut

     Configuration name: "power-fit-cut"

     Available configuration values:
     - "min-scale": double -- minimum scale to accept
     - "max-scale": double -- maximum scale to accept
     - "min-width": double -- minimum width to accept
     - "max-width": double -- maximum width to accept
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

        MEMBERVARIABLE(double, MinScale);
        MEMBERVARIABLE(double, MaxScale);
        MEMBERVARIABLE(double, MinWidth);
        MEMBERVARIABLE(double, MaxWidth);

    public:
        bool Apply(Nymph::KTData& data, KTPowerFitData& fitData);

    };
} // namespace Katydid

#endif /* KTPOWERFITCUT_HH_ */
