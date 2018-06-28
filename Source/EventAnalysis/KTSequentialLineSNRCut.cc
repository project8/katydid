/*
 * KTSequentialLineSNRCut.cc
 *
 *  Created on: June 13, 2018
 *      Author: C. Claessens
 */

#include "KTSequentialLineSNRCut.hh"
#include "KTSequentialLineData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(sqlcutlog, "KTSequentialLineSNRCut");

    const std::string KTSequentialLineSNRCut::Result::sName = "seq-line-snr-cut";

    KT_REGISTER_CUT(KTSequentialLineSNRCut);

    KTSequentialLineSNRCut::KTSequentialLineSNRCut(const std::string& name) :
         KTCutOneArg(name),
         fMinTotalSNR(0.0),
         fMinAverageSNR(0.0),
         fWideOrNarrowLine( wide_or_narrow::wide )
    {}

    KTSequentialLineSNRCut::~KTSequentialLineSNRCut()
    {}

    bool KTSequentialLineSNRCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinTotalSNR( node->get_value< double >( "min-total-snr", GetMinTotalSNR() ) );
        SetMinAverageSNR( node->get_value< double >( "min-average-snr", GetMinAverageSNR() ) );
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
                KTERROR(sqlcutlog, "Invalid string for fWideOrNarrow");
            }
        }
        return true;
    }

    bool KTSequentialLineSNRCut::Apply( Nymph::KTData& data, KTSequentialLineData& seqLineData )
    {
        bool isCut = false;
        //seqLineData.CalculateTotalSNR();

        if (fWideOrNarrowLine == wide_or_narrow::narrow)
        {
            if( seqLineData.GetTotalSNR() < GetMinTotalSNR() )
            {
                isCut = true;
            }
            if( seqLineData.GetTotalSNR() / ( seqLineData.GetEndTimeInRunC() - seqLineData.GetStartTimeInRunC() ) < GetMinAverageSNR() )
            {
                isCut = true;
            }
        }
        else
        {
            if( seqLineData.GetTotalWideSNR() < GetMinTotalSNR() )
            {
                isCut = true;
            }
            if( seqLineData.GetTotalWideSNR() / ( seqLineData.GetEndTimeInRunC() - seqLineData.GetStartTimeInRunC() ) < GetMinAverageSNR() )
            {
                isCut = true;
            }
        }

        data.GetCutStatus().AddCutResult< KTSequentialLineSNRCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
