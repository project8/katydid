/*
 * KTSequentialLineNUPCut.cc
 *
 *  Created on: June 13, 2018
 *      Author: C. Claessens
 */

#include "KTSequentialLineNUPCut.hh"
#include "KTSequentialLineData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(sqlcutlog, "KTSequentialLineNUPCut");

    const std::string KTSequentialLineNUPCut::Result::sName = "seq-line-nup-cut";

    KT_REGISTER_CUT(KTSequentialLineNUPCut);

    KTSequentialLineNUPCut::KTSequentialLineNUPCut(const std::string& name) :
         KTCutOneArg(name),
         fMinTotalNUP(0.0),
         fMinAverageNUP(0.0),
         fWideOrNarrowLine("wide")
    {}

    KTSequentialLineNUPCut::~KTSequentialLineNUPCut()
    {}

    bool KTSequentialLineNUPCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinTotalNUP( node->get_value< double >( "min-total-nup", GetMinTotalNUP() ) );
        SetMinAverageNUP( node->get_value< double >( "min-average-nup", GetMinAverageNUP() ) );
        SetWideOrNarrowLine( node->get_value("wide-or-narrow", GetWideOrNarrowLine() ) );

        return true;
    }

    bool KTSequentialLineNUPCut::Apply( Nymph::KTData& data, KTSequentialLineData& seqLineData )
    {
        bool isCut = false;
        //seqLineData.CalculateTotalNUP();

        if (fWideOrNarrowLine == "narrow")
        {
            if( seqLineData.GetTotalNUP() < GetMinTotalNUP() )
            {
                isCut = true;
            }
            if( seqLineData.GetTotalNUP() / ( seqLineData.GetEndTimeInRunC() - seqLineData.GetStartTimeInRunC() ) < GetMinAverageNUP() )
            {
                isCut = true;
            }
        }
        else if (fWideOrNarrowLine == "wide")
        {
            if( seqLineData.GetTotalWideNUP() < GetMinTotalNUP() )
            {
                isCut = true;
            }
            if( seqLineData.GetTotalWideNUP() / ( seqLineData.GetEndTimeInRunC() - seqLineData.GetStartTimeInRunC() ) < GetMinAverageNUP() )
            {
                isCut = true;
            }
        }
        else
        {
            KTERROR(sqlcutlog, "Invalid string for fWideOrNarrowLine: "<< fWideOrNarrowLine);
        }
        data.GetCutStatus().AddCutResult< KTSequentialLineNUPCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
