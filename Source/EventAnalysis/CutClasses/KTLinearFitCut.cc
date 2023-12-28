/*
 * KTLinearFitCut.cc
 *
 *  Created on: Nov 8, 2016
 *      Author: ezayas
 */

#include "KTLinearFitCut.hh"
#include "KTLinearFitResult.hh"

#include "logger.hh"

namespace Katydid
{
    LOGGER(exlog, "KTLinearFitCut");

    const std::string KTLinearFitCut::Result::sName = "linear-fit-cut";

    KT_REGISTER_CUT(KTLinearFitCut);

    KTLinearFitCut::KTLinearFitCut(const std::string& name) :
         KTCutOneArg(name),
         fMinSidebandSep(2e6)
    {}

    KTLinearFitCut::~KTLinearFitCut()
    {}

    bool KTLinearFitCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinSidebandSep( node->get_value< double >( "min-sep", GetMinSidebandSep() ) );

        return true;
    }

    bool KTLinearFitCut::Apply( Nymph::KTData& data, KTLinearFitResult& fitData )
    {
        bool isCut = true;

        if( fitData.GetSidebandSeparation() > GetMinSidebandSep() )
        {
            isCut = false;
        }
        if( fitData.GetSidebandSeparation() < -1 * GetMinSidebandSep() )
        {
            isCut = false;
        }

        data.GetCutStatus().AddCutResult< KTLinearFitCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
