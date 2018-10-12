/*
 * KTLinearFitCut.hh
 *
 *  Created on: Nov 8, 2016
 *      Author: ezayas
 */

#ifndef KTLINEARFITCUT_HH_
#define KTLINEARFITCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTLinearFitResult;

    /*
     @class KTLinearFitCut
     @author E. Zayas

     @brief Keeps fit results with significant sideband separation

     @details
     KTLinearFitResult objects must have a sideband separation magnitude which exceeds the specified minimum to pass the cut

     Configuration name: "linear-fit-cut"

     Available configuration values:
     - "min-sep": double -- minimum separation to accept
    */

    class KTLinearFitCut : public Nymph::KTCutOneArg< KTLinearFitResult >
    {

    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTLinearFitCut(const std::string& name = "linear-fit-cut");
        ~KTLinearFitCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinSidebandSep);

    public:
        bool Apply(Nymph::KTData& data, KTLinearFitResult& fitData);

    };
} // namespace Katydid

#endif /* KTLINEARFITCUT_HH_ */
