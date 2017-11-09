/*
 * KTRPClassifier.cc
 *
 *  Created on: Nov 9, 2017
 *      Author: ezayas
 */

#include "KTRPClassifier.hh"

#include "KTLogger.hh"

#include "KTProcessedTrackData.hh"
#include "KTPowerFitData.hh"
#include "KTRPTrackData.hh"

#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTRPClassifier");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTRPClassifier, "rp-classifier");

    KTRPClassifier::KTRPClassifier(const std::string& name) :
            KTProcessor(name),
            fRPTrackSignal("rp-track", this)
    {
        RegisterSlot( "track", this, &KTRPClassifier::SlotFunctionTrack );
    }

    KTRPClassifier::~KTRPClassifier()
    {
    }

    bool KTRPClassifier::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        return true;
    }

    bool KTRPClassifier::FillRPTrack( KTProcessedTrackData& trackData, KTPowerFitData& pfData, KTRPTrackData& rpTrackData )
    {
        rpTrackData = trackData;

        rpTrackData.SetIsValid( pfData.GetIsValid() );
        rpTrackData.SetMainPeak( pfData.GetMainPeak() );
        rpTrackData.SetNPeaks( pfData.GetNPeaks() );
        rpTrackData.SetAverage( pfData.GetAverage() );
        rpTrackData.SetRMS( pfData.GetRMS() );
        rpTrackData.SetSkewness( pfData.GetSkewness() );
        rpTrackData.SetKurtosis( pfData.GetKurtosis() );
        rpTrackData.SetNormCentral( pfData.GetNormCentral() );
        rpTrackData.SetMeanCentral( pfData.GetMeanCentral() );
        rpTrackData.SetSigmaCentral( pfData.GetSigmaCentral() );
        rpTrackData.SetMaximumCentral( pfData.GetMaximumCentral() );
        rpTrackData.SetRMSAwayFromCentral( pfData.GetRMSAwayFromCentral() );
        rpTrackData.SetCentralPowerFraction( pfData.GetCentralPowerFraction() );

        return true;
    }

} // namespace Katydid
