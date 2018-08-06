/*
 * KTSequentialLinePointDensityCut.hh
 *
 *  Created on: August 06, 2018
 *      Author: C. Claessens
 */

#ifndef KTSEQUENTIALLINEPOINTDENSITYCUT_HH_
#define KTSEQUENTIALLINEPOINTDENSITYCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTSequentialLineData;

    /*
     @class KTSequentialLinePointDensityCut
     @author C. Claessens

     @brief Cuts on points per time

     @details
     KTSequentialLineData objects must have a point density a set value to pass the cut

     Configuration name: "sq-line-density-cut"

     Available configuration values:
     - "min-density": double -- minimum density to accept
     */

    class KTSequentialLinePointDensityCut : public Nymph::KTCutOneArg< KTSequentialLineData >
    {


    public:
        struct Result : Nymph::KTExtensibleCutResult< Result >
        {
            static const std::string sName;
        };

    public:
        KTSequentialLinePointDensityCut(const std::string& name = "seq-line-density-cut");
        ~KTSequentialLinePointDensityCut();

        bool Configure(const scarab::param_node* node);

        MEMBERVARIABLE(double, MinDensity);

    public:
        bool Apply(Nymph::KTData& data, KTSequentialLineData& seqLineData);

    };
} // namespace Katydid

#endif /* KTSequentialLinePointDensityCut_HH_ */
