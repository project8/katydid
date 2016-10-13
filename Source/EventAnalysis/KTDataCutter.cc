/*
 * KTDataCutter.cc
 *
 *  Created on: Oct 13, 2016
 *      Author: ezayas
 */

#include "KTDataCutter.hh"

#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTProcessedTrackData.hh"
#include "KTLinearFitResult.hh"

namespace Katydid
{
    KTLOGGER(evlog, "KTDataCutter");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTDataCutter, "data-cutter");

    KTDataCutter::KTDataCutter(const std::string& name) :
            KTProcessor(name),
            fMinSidebandSeparation(-1e12),
            fMaxSidebandSeparation(1e12),
            fMinMagnetronFreq(0),
            fMaxMagnetronFreq(1e12),
            fLinearFitSignal("fit-result", this)
    {
        RegisterSlot( "fit-result", this, &KTDataCutter::SlotFunctionFitResult );
    }

    KTDataCutter::~KTDataCutter()
    {
    }

    bool KTDataCutter::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinSidebandSeparation(node->get_value< double >("min-sideband-separation", fMinSidebandSeparation));
        SetMaxSidebandSeparation(node->get_value< double >("max-sideband-separation", fMaxSidebandSeparation));
        SetMinMagnetronFreq(node->get_value< double >("min-magnetron-freq", fMinMagnetronFreq));
        SetMaxMagnetronFreq(node->get_value< double >("max-magnetron-freq", fMaxMagnetronFreq));
        
        return true;
    }

    bool KTDataCutter::CutLinearFitResult( KTLinearFitResult& fitData )
    {
        if( fitData.GetSidebandSeparation( 0 ) < fMinSidebandSeparation )
        {
            return false;
        }
        if( fitData.GetSidebandSeparation( 0 ) > fMaxSidebandSeparation )
        {
            return false;
        }

        if( fitData.GetFFT_peak( 0 ) < fMinMagnetronFreq )
        {
            return false;
        }
        if( fitData.GetFFT_peak( 0 ) > fMaxMagnetronFreq )
        {
            return false;
        }

        return true;
    }

} // namespace Katydid
