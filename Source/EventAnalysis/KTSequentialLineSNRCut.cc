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
    KTLOGGER(exlog, "KTSequentialLineSNRCut");

    const std::string KTSequentialLineSNRCut::Result::sName = "seq-line-snr-cut";

    KT_REGISTER_CUT(KTSequentialLineSNRCut);

    KTSequentialLineSNRCut::KTSequentialLineSNRCut(const std::string& name) :
         KTCutOneArg(name),
         fMinTotalSNR(0.0),
         fMinAverageSNR(0.0)
    {}

    KTSequentialLineSNRCut::~KTSequentialLineSNRCut()
    {}

    bool KTSequentialLineSNRCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetMinTotalSNR( node->get_value< double >( "min-total-snr", GetMinTotalSNR() ) );
        SetMinAverageSNR( node->get_value< double >( "min-average-snr", GetMinAverageSNR() ) );
        SetWideOrNarrowLine( node->get_value("wide-or-narrow", GetWideOrNarrowLine() ) );

        return true;
    }

    bool KTSequentialLineSNRCut::Apply( Nymph::KTData& data, KTSequentialLineData& seqLineData )
    {
        bool isCut = false;
        seqLineData.CalculateTotalSNR();

        if( seqLineData.GetTotalSNR() < GetMinTotalSNR() )
        {
            isCut = true;
        }
        if( seqLineData.GetTotalSNR() / ( seqLineData.GetEndTimeInRunC() - seqLineData.GetStartTimeInRunC() ) > GetMaxStartTime() )
        {
            isCut = true;
        }
        
        data.GetCutStatus().AddCutResult< KTSequentialLineSNRCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
