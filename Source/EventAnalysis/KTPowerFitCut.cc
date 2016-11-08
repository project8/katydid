/*
 * KTPowerFitCut.cc
 *
 *  Created on: Nov 8, 2016
 *      Author: ezayas
 */

#include "KTPowerFitCut.hh"
#include "KTPowerFitData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(exlog, "KTPowerFitCut");

    const std::string KTPowerFitCut::Result::sName = "power-fit-cut";

    KT_REGISTER_CUT(KTPowerFitCut);

    KTPowerFitCut::KTPowerFitCut(const std::string& name) :
         KTCutOneArg(name),
         fMinScale(0.),
         fMaxScale(1000.),
         fMinWidth(0.),
         fMaxWidth(10e6)
    {}

    KTPowerFitCut::~KTPowerFitCut()
    {}

    bool KTPowerFitCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinScale( node->get_value< double >( "min-scale", GetMinScale() ) );
        SetMaxScale( node->get_value< double >( "max-scale", GetMaxScale() ) );
        SetMinWidth( node->get_value< double >( "min-width", GetMinWidth() ) );
        SetMaxWidth( node->get_value< double >( "max-width", GetMaxWidth() ) );

        return true;
    }

    bool KTPowerFitCut::Apply( Nymph::KTData& data, KTPowerFitData& fitData )
    {
        bool isCut = true;

        if( fitData.GetScale() > GetMinScale() && fitData.GetScale() < GetMaxScale() && fitData.GetWidth() > GetMinWidth() && fitData.GetWidth() < GetMaxWidth() )
        {
            isCut = false;
        }

        data.GetCutStatus().AddCutResult< KTPowerFitCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
