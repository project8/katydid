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
         fMinAverage(-10.),
         fMaxAverage(10.),
         fMinRMS(0.),
         fMaxRMS(10.),
         fMinSkewness(-1000.),
         fMaxSkewness(1000.),
         fMinKurtosis(-1000.),
         fMaxKurtosis(1000.)
    {}

    KTPowerFitCut::~KTPowerFitCut()
    {}

    bool KTPowerFitCut::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        SetValidity( node->get_value< bool >( "keep-valid", GetValidity() ) );
        SetMinNPeaks( node->get_value< int >( "min-npeaks", GetMinNPeaks() ) );
        SetMaxNPeaks( node->get_value< int >( "max-npeaks", GetMaxNPeaks() ) );
        SetMinAverage( node->get_value< double >( "min-average", GetMinAverage() ) );
        SetMaxAverage( node->get_value< double >( "max-average", GetMaxAverage() ) );
        SetMinRMS( node->get_value< double >( "min-rms", GetMinRMS() ) );
        SetMaxRMS( node->get_value< double >( "max-rms", GetMaxRMS() ) );
        SetMinSkewness( node->get_value< double >( "min-skewness", GetMinSkewness() ) );
        SetMaxSkewness( node->get_value< double >( "max-skewness", GetMaxSkewness() ) );
        SetMinKurtosis( node->get_value< double >( "min-kurtosis", GetMinKurtosis() ) );
        SetMaxKurtosis( node->get_value< double >( "max-kurtosis", GetMaxKurtosis() ) );

        return true;
    }

    bool KTPowerFitCut::Apply( Nymph::KTData& data, KTPowerFitData& fitData )
    {
        bool isCut = false;

        if( (fitData.GetIsValid() == 1 && GetValidity() == false) || (fitData.GetIsValid() == 0 && GetValidity() == true) )
        {
            isCut = true;
        }

        if( fitData.GetNPeaks() < GetMinNPeaks() || fitData.GetNPeaks() > GetMaxNPeaks() )
        {
            isCut = true;
        }

        if( fitData.GetAverage() < GetMinAverage() || fitData.GetAverage() > GetMaxAverage() )
        {
            isCut = true;
        }
        if( fitData.GetRMS() < GetMinRMS() || fitData.GetRMS() > GetMaxRMS() )
        {
            isCut = true;
        }
        if( fitData.GetSkewness() < GetMinSkewness() || fitData.GetSkewness() > GetMaxSkewness() )
        {
            isCut = true;
        }
        if( fitData.GetKurtosis() < GetMinKurtosis() || fitData.GetKurtosis() > GetMaxKurtosis() )
        {
            isCut = true;
        }

        data.GetCutStatus().AddCutResult< KTPowerFitCut::Result >(isCut);

        return isCut;
    }

} // namespace Katydid
