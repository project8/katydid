/*
 * KTSequentialLineNUPCut.cc
 *
 *  Created on: June 13, 2018
 *      Author: C. Claessens
 */

#include "KTSequentialLineNUPCut.hh"
#include "KTSequentialLineData.hh"

#include "logger.hh"

namespace Katydid
{
    LOGGER(sqlcutlog, "KTSequentialLineNUPCut");

    const std::string KTSequentialLineNUPCut::Result::sName = "seq-line-nup-cut";

    KT_REGISTER_CUT(KTSequentialLineNUPCut);

    KTSequentialLineNUPCut::KTSequentialLineNUPCut(const std::string& name) :
         KTCutOneArg(name),
         fMinTotalNUP(0.0),
         fMinAverageNUP(0.0),
         fWideOrNarrowLine( wide_or_narrow::wide ),
         fTimeOrBinAverage( time_or_bin_average::time )
    {}

    KTSequentialLineNUPCut::~KTSequentialLineNUPCut()
    {}

    bool KTSequentialLineNUPCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinTotalNUP( node->get_value< double >( "min-total-nup", GetMinTotalNUP() ) );
        SetMinAverageNUP( node->get_value< double >( "min-average-nup", GetMinAverageNUP() ) );
        if (node->has("wide-or-narrow"))
        {
            if (node->get_value("wide-or-narrow") == "wide")
            {
                SetWideOrNarrowLine(wide_or_narrow::wide);
            }
            else if (node->get_value("wide-or-narrow") == "narrow")
            {
                SetWideOrNarrowLine(wide_or_narrow::narrow);
            }
            else
            {
                LERROR(sqlcutlog, "Invalid string for fWideOrNarrow");
                return false;
            }
        }
        if (node->has("time-or-bin-average"))
        {
            if (node->get_value("time-or-bin-average") == "time")
            {
                SetTimeOrBinAverage(time_or_bin_average:: time);
            }
            else if (node->get_value("time-or-bin-average") == "bin")
            {
                SetTimeOrBinAverage(time_or_bin_average::bin);
            }
            else
            {
                LERROR(sqlcutlog, "Invalid string for fTimeOrBinAverage");
                return false;
            }
        }
        return true;
    }

    bool KTSequentialLineNUPCut::Apply( Nymph::KTData& data, KTSequentialLineData& seqLineData )
    {
        bool isCut = false;
        //seqLineData.CalculateTotalNUP();

        if (fWideOrNarrowLine == wide_or_narrow::narrow)
        {
            if( seqLineData.GetTotalNUP() < GetMinTotalNUP() )
            {
                isCut = true;
            }
            if ( fTimeOrBinAverage == time_or_bin_average::time )
            {
                if( seqLineData.GetTotalNUP() / ( seqLineData.GetEndTimeInRunC() - seqLineData.GetStartTimeInRunC() ) < GetMinAverageNUP() )
                {
                    isCut = true;
                }
            }
            else
            {
                if( seqLineData.GetTotalNUP() / ( seqLineData.GetNPoints() ) < GetMinAverageNUP() )
                {
                    isCut = true;
                }
            }
        }
        else
        {
            if( seqLineData.GetTotalWideNUP() < GetMinTotalNUP() )
            {
                isCut = true;
            }
            if ( fTimeOrBinAverage == time_or_bin_average::time )
            {
                if( seqLineData.GetTotalWideNUP() / ( seqLineData.GetEndTimeInRunC() - seqLineData.GetStartTimeInRunC() ) < GetMinAverageNUP() )
                {
                    isCut = true;
                }
            }
            else
            {
                if( seqLineData.GetTotalWideNUP() / ( seqLineData.GetNPoints() ) < GetMinAverageNUP() )
                {
                    isCut = true;
                }
            }
        }

        data.GetCutStatus().AddCutResult< KTSequentialLineNUPCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
