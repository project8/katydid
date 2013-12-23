/*
 * KT2ROOT.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"

#include "KTRawTimeSeries.hh"

#include "TH1I.h"

namespace Katydid
{

    KT2ROOT::KT2ROOT()
    {
    }

    KT2ROOT::~KT2ROOT()
    {
    }

    TH1I* KT2ROOT::CreateHistogram(const KTRawTimeSeries* ts, const std::string& histName = "hRawTimeSeries")
    {

    }

    TH1I* KT2ROOT::CreateAmplitudeDistributionHistogram(const KTRawTimeSeries* ts, const std::string& histName = "hRawTSDist")
    {

    }


} /* namespace Katydid */
