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
         fValidity(true),
         fMinNPeaks(0),
         fMaxNPeaks(10),
         fMinMean(-10e6),
         fMaxMean(10e6),
         fMinVariance(-1e14),
         fMaxVariance(1e14),
         fMinSkewness(-10e6),
         fMaxSkewness(10e6),
         fMinKurtosis(-10e6),
         fMaxKurtosis(10e6)
    {}

    KTPowerFitCut::~KTPowerFitCut()
    {}

    bool KTPowerFitCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetValidity( node->get_value< bool >( "keep-valid", GetValidity() ) );
        SetMinNPeaks( node->get_value< int >( "min-npeaks", GetMinNPeaks() ) );
        SetMaxNPeaks( node->get_value< int >( "max-npeaks", GetMaxNPeaks() ) );
        SetMinMean( node->get_value< double >( "min-mean", GetMinMean() ) );
        SetMaxMean( node->get_value< double >( "max-mean", GetMaxMean() ) );
        SetMinVariance( node->get_value< double >( "min-variance", GetMinVariance() ) );
        SetMaxVariance( node->get_value< double >( "max-variance", GetMaxVariance() ) );
        SetMinSkewness( node->get_value< double >( "min-skewness", GetMinSkewness() ) );
        SetMaxSkewness( node->get_value< double >( "max-skewness", GetMaxSkewness() ) );
        SetMinKurtosis( node->get_value< double >( "min-kurtosis", GetMinKurtosis() ) );
        SetMaxKurtosis( node->get_value< double >( "max-kurtosis", GetMaxKurtosis() ) );

        return true;
    }

    bool KTPowerFitCut::Apply( Nymph::KTData& data, KTPowerFitData& fitData )
    {
        if( fitData.GetIsValid() != GetValidity() )
        {
            return false;
        }

        if( fitData.GetNPeaks() < GetMinNPeaks() || fitData.GetNPeaks() > GetMaxNPeaks() )
        {
            return false;
        }

        if( fitData.GetAverage() < GetMinMean() || fitData.GetAverage() > GetMaxMean() )
        {
            return false;
        }
        if( fitData.GetVariance() < GetMinVariance() || fitData.GetVariance() > GetMaxVariance() )
        {
            return false;
        }
        if( fitData.GetSkewness() < GetMinSkewness() || fitData.GetSkewness() > GetMaxSkewness() )
        {
            return false;
        }
        if( fitData.GetKurtosis() < GetMinKurtosis() || fitData.GetKurtosis() > GetMaxKurtosis() )
        {
            return false;
        }

        return true;
    }

} // namespace Katydid
