/*
 * KTSequentialLinePointDensityCut.cc
 *
 *  Created on: June 13, 2018
 *      Author: C. Claessens
 */

#include "KTSequentialLinePointDensityCut.hh"
#include "KTSequentialLineData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(sqlcutlog, "KTSequentialLinePointDensityCut");

    const std::string KTSequentialLinePointDensityCut::Result::sName = "seq-line-density-cut";

    KT_REGISTER_CUT(KTSequentialLinePointDensityCut);

    KTSequentialLinePointDensityCut::KTSequentialLinePointDensityCut(const std::string& name) :
         KTCutOneArg(name),
         fMinDensity(0.0)

    {}

    KTSequentialLinePointDensityCut::~KTSequentialLinePointDensityCut()
    {}

    bool KTSequentialLinePointDensityCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinDensity( node->get_value< double >( "min-density", GetMinDensity() ) );

        return true;
    }

    bool KTSequentialLinePointDensityCut::Apply( Nymph::KTData& data, KTSequentialLineData& seqLineData )
    {
        bool isCut = false;

        if( (seqLineData.GetNPoints() / (seqLineData.GetEndTimeInRunC() - seqLineData.GetStartTimeInRunC() ) ) < GetMinDensity() )
        {
            isCut = true;
        }
        data.GetCutStatus().AddCutResult< KTSequentialLinePointDensityCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
