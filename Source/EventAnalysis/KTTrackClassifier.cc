/*
 * KTTrackClassifier.cc
 *
 *  Created on: Dec 13, 2016
 *      Author: ezayas
 */

#include "KTTrackClassifier.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(evlog, "KTTrackClassifier");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTTrackClassifier, "track-classifier");

    KTTrackClassifier::KTTrackClassifier(const std::string& name) :
            KTProcessor(name),
            fMVAFile("foo.weights.xml"),
            fAlgorithm("SomeAlgorithm"),
            fMVACut(0.),
            fClassifySignal("classify", this)
    {
        RegisterSlot( "power-fit", this, &KTTrackClassifier::SlotFunctionPowerFitData );
    }

    KTTrackClassifier::~KTTrackClassifier()
    {
    }

    bool KTTrackClassifier::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMVAFile(node->get_value< std::string >("mva-file", GetMVAFile()));
        SetAlgorithm(node->get_value< std::string >("algorithm", GetAlgorithm()));
        SetMVACut(node->get_value< double >("mva-cut", GetMVACut()));
        
        return true;
    }

    bool KTTrackClassifier::ClassifyTrack( KTProcessedTrackData& trackData, double mva )
    {
        KTINFO(evlog, "Classifying track with MVA classifier = " << mva);
        trackData.SetMVAClassifier( mva );

        if( mva >= GetMVACut() )
        {
            KTDEBUG(evlog, "Classifying track as a signal peak");
            trackData.SetMainband( true );
        }
        else
        {
            KTDEBUG(evlog, "Classifying track as a sideband peak");
            trackData.SetMainband( false );
        }

        if( mva <= -999 )
        {
            KTWARN(evlog, "Classifier value is -999; something probably went wrong computing it");
            return false;
        }

        return true;
    }

} // namespace Katydid
