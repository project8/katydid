/*
 * KTTMVAClassifier.cc
 *
 *  Created on: Dec 13, 2016
 *      Author: ezayas
 */

#include "KTTMVAClassifier.hh"

#include "KTLogger.hh"

#include "KTPowerFitData.hh"

namespace Katydid
{
    KTLOGGER(evlog, "KTTMVAClassifier");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTTMVAClassifier, "tmva-classifier");

    KTTMVAClassifier::KTTMVAClassifier(const std::string& name) :
            KTProcessor(name),
            fMVAFile("foo.weights.xml"),
            fAlgorithm("SomeAlgorithm"),
            fMVACut(0.),
            fClassifySignal("classify", this)
    {
        RegisterSlot( "power-fit", this, &KTTMVAClassifier::SlotFunctionPowerFitData );
    }

    KTTMVAClassifier::~KTTMVAClassifier()
    {
    }

    bool KTTMVAClassifier::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMVAFile(node->get_value< std::string >("mva-file", GetMVAFile()));
        SetAlgorithm(node->get_value< std::string >("algorithm", GetAlgorithm()));
        SetMVACut(node->get_value< double >("mva-cut", GetMVACut()));
        
        return true;
    }

    bool KTTMVAClassifier::ClassifyTrack( KTProcessedTrackData& trackData, double mva )
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

    void KTTMVAClassifier::SlotFunctionPowerFitData( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present

        if (! data->Has< KTProcessedTrackData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTProcessedTrackData >!");
            return;
        }

        if (! data->Has< KTPowerFitData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTPowerFitData >!");
            return;
        }

        // Set up reader

        reader = new TMVA::Reader();

        reader->AddVariable( "Average", &fAverage );
        reader->AddVariable( "RMS", &fRMS );
        reader->AddVariable( "Skewness", &fSkewness );
        reader->AddVariable( "Kurtosis", &fKurtosis );
        reader->AddVariable( "NormCentral", &fNormCentral );
        reader->AddVariable( "MeanCentral", &fMeanCentral );
        reader->AddVariable( "SigmaCentral", &fSigmaCentral );
        reader->AddVariable( "NPeaks", &fNPeaks );
        reader->AddVariable( "CentralPowerFraction", &fCentralPowerFraction );
        reader->AddVariable( "RMSAwayFromCentral", &fRMSAwayFromCentral );

        try
        {
            KTDEBUG(avlog_hh, "Algorithm = " << fAlgorithm);
            KTDEBUG(avlog_hh, "MVA File = " << fMVAFile);

            reader->BookMVA( fAlgorithm, fMVAFile );
        }
        catch(...)
        {
            KTERROR(avlog_hh, "Invalid reader configuration; please make sure the algorithm is correct and the file exists. Aborting");
            return;
        }

        KTINFO(avlog_hh, "Successfully set up TMVA reader");

        KTPowerFitData& pfData = data->Of< KTPowerFitData >();

        // Assign variables
        fAverage = (float)(pfData.GetAverage());
        fRMS = (float)(pfData.GetRMS());
        fSkewness = (float)(pfData.GetSkewness());
        fKurtosis = (float)(pfData.GetKurtosis());
        fNormCentral = (float)(pfData.GetNormCentral());
        fMeanCentral = (float)(pfData.GetMeanCentral());
        fSigmaCentral = (float)(pfData.GetSigmaCentral());
        fNPeaks = (float)(pfData.GetNPeaks());
        fCentralPowerFraction = (float)(pfData.GetCentralPowerFraction());
        fRMSAwayFromCentral = (float)(pfData.GetRMSAwayFromCentral());

        double mvaValue = reader->EvaluateMVA( fAlgorithm );
        KTDEBUG(avlog_hh, "Evaluated MVA classifier = " << mvaValue);

        // Call function
        if( !ClassifyTrack( data->Of< KTProcessedTrackData >(), mvaValue ) )
        {
            KTERROR(avlog_hh, "Something went wrong analyzing data of type < KTProcessedTrackData >");
            return;
        }

        KTINFO(avlog_hh, "Classification finished!");

        // Emit signal
        fClassifySignal( data );
    
        return;
    }
} // namespace Katydid
