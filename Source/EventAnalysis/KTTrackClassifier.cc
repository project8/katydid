/*
 * KTTrackClassifier.cc
 *
 *  Created on: Dec 13, 2016
 *      Author: ezayas, L. Saldana
 */

#include "KTTrackClassifier.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(evlog, "KTTrackClassifier");

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
        reader->AddVariable( "CentralPowerRatio", &fCentralPowerRatio );
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
        fCentralPowerRatio = (float)(pfData.GetCentralPowerRatio());
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

    // Register the processor
    KT_REGISTER_PROCESSOR(KTDLIBClassifier, "dlib-classifier");

    KTDLIBClassifier::KTDLIBClassifier(const std::string& name) :
            KTProcessor(name),
            fDFFile("foo_df.dat"),
            fClassifySignal("classify", this)
    {
        RegisterSlot( "power-fit", this, &KTDLIBClassifier::SlotFunctionPowerFitData );
    }

    KTDLIBClassifier::~KTDLIBClassifier()
    {
    }

    bool KTDLIBClassifier::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetDFFile(node->get_value< std::string >("df-file",GetDFFile()));
        
        return true;
    }

    bool KTDLIBClassifier::ClassifyTrack(KTClassifierResultsData& resultData, double label )
    {   
        if( label == 0 )
        {
            resultData.SetMCH( true );
        }
        else if( label == 1 )
        {
            resultData.SetMCL( true );
        }
        else if( label == 2 )
        {
            resultData.SetSB( true );
        }
        else
        {
            KTERROR(avlog_hh, "Could not assign appropriate classification label; something went wrong");
            return false;
        }

        return true;
    }

    void KTDLIBClassifier::SlotFunctionPowerFitData( Nymph::KTDataPtr data )
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

        KTProcessedTrackData& ptData = data->Of<KTProcessedTrackData>();
        KTPowerFitData& pfData = data->Of< KTPowerFitData>();
        
        // Set up classifier features
        fPower = (float)(ptData.GetTotalPower());
        fSlope = (float)(ptData.GetSlope());
        fTimeLength = (float)(ptData.GetTimeLength());
        fAverage = (float)(pfData.GetAverage());
        fRMS = (float)(pfData.GetRMS());
        fRMSAwayFromCentral = (float)(pfData.GetRMSAwayFromCentral());
        fKurtosis = (float)(pfData.GetKurtosis());
        fSkewness = (float)(pfData.GetSkewness());
        fCentralPowerFraction = (float)(pfData.GetCentralPowerFraction()); 
        fNPeaks = (float)(pfData.GetNPeaks());
        fMaxCentral = (float)(pfData.GetMaximumCentral());
        fMeanCentral = (float)(pfData.GetMeanCentral());
        fNormCentral = (float)(pfData.GetNormCentral());
        fSigmaCentral = (float)(pfData.GetSigmaCentral());
        
        typedef matrix<double,14,1> sample_type;
        sample_type classifierFeatures; // set up 14-dim vector of classification features
        classifierFeatures(0) = fPower;
        classifierFeatures(1) = fSlope;
        classifierFeatures(2) = fTimeLength;
        classifierFeatures(3) = fAverage;
        classifierFeatures(4) = fRMS;
        classifierFeatures(5) = fRMSAwayFromCentral;
        classifierFeatures(6) = fKurtosis;
        classifierFeatures(7) = fSkewness;
        classifierFeatures(8) = fCentralPowerFraction;
        classifierFeatures(9) = fNPeaks;
        classifierFeatures(10) = fMaxCentral;
        classifierFeatures(11) = fMeanCentral;
        classifierFeatures(12) = fNormCentral;
        classifierFeatures(13) = fSigmaCentral;
        
        // Some helpful type definitions from dlib
        typedef one_vs_all_trainer<any_trainer< sample_type, double > > ova_trainer;
        typedef radial_basis_kernel<sample_type> rbf_kernel;
        typedef one_vs_all_decision_function<ova_trainer,decision_function<rbf_kernel>> decision_funct_type;
        typedef normalized_function<decision_funct_type> normalized_decision_funct_type;
        normalized_decision_funct_type decisionFunction; // declare normalized ova decision function for SVM with radial basis kernel
        try
        {
            KTDEBUG(avlog_hh,"DF File = " << fDFFile);
            deserialize(fDFFile) >> decisionFunction; // load train decision function
        }
        catch(...)
        {
            KTERROR(avlog_hh, "Unable to read the decision function from file. Aborting");
            return
        }

        double classificationLabel = decisionFunction(classifierFeatures); // classify track with trained decision function, i.e gives label for example 0, 1 or 2

        if( !ClassifyTrack( data->Of< KTClassifierResultsData >(), classificationLabel) )
        {
            KTERROR(avlog_hh, "Something went wrong with assigning classification label to track");
            return;
        }

        KTINFO(avlog_hh, "Classification finished!");

        // Emit signal
        fClassifySignal( data );
    }

} // namespace Katydid
