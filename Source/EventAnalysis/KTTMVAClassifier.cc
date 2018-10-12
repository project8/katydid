/*
 * KTTMVAClassifier.cc
 *
 *  Created on: Dec 13, 2016
 *      Author: ezayas
 */

#include "KTTMVAClassifier.hh"

#include "KTLogger.hh"

#include "KTPowerFitData.hh"
#include "KTProcessedTrackData.hh"

#include "TMVA/Reader.h"

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
            fReader(nullptr),
            fAverage(0.),
            fRMS(0.),
            fSkewness(0.),
            fKurtosis(0.),
            fNormCentral(0.),
            fMeanCentral(0.),
            fSigmaCentral(0.),
            fNPeaks(0.),
            fCentralPowerFraction(0.),
            fRMSAwayFromCentral(0.),
            fClassifySignal("classify", this),
            fClassifySlot("power-fit", this, &KTTMVAClassifier::ClassifyTrack, &fClassifySignal)
    {
    }

    KTTMVAClassifier::~KTTMVAClassifier()
    {
        delete fReader;
    }

    bool KTTMVAClassifier::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMVAFile(node->get_value< std::string >("mva-file", GetMVAFile()));
        SetAlgorithm(node->get_value< std::string >("algorithm", GetAlgorithm()));
        SetMVACut(node->get_value< double >("mva-cut", GetMVACut()));
        
        return true;
    }

    bool KTTMVAClassifier::ClassifyTrack(KTProcessedTrackData& trackData, KTPowerFitData& powerFitData)
    {

        if (fReader == nullptr)
        {
            // Set up reader

            fReader = new TMVA::Reader();

            fReader->AddVariable( "Average", &fAverage );
            fReader->AddVariable( "RMS", &fRMS );
            fReader->AddVariable( "Skewness", &fSkewness );
            fReader->AddVariable( "Kurtosis", &fKurtosis );
            fReader->AddVariable( "NormCentral", &fNormCentral );
            fReader->AddVariable( "MeanCentral", &fMeanCentral );
            fReader->AddVariable( "SigmaCentral", &fSigmaCentral );
            fReader->AddVariable( "NPeaks", &fNPeaks );
            fReader->AddVariable( "CentralPowerFraction", &fCentralPowerFraction );
            fReader->AddVariable( "RMSAwayFromCentral", &fRMSAwayFromCentral );

            try
            {
                KTDEBUG(avlog_hh, "Algorithm = " << fAlgorithm);
                KTDEBUG(avlog_hh, "MVA File = " << fMVAFile);

                fReader->BookMVA( fAlgorithm, fMVAFile );
            }
            catch(...)
            {
                KTERROR(avlog_hh, "Invalid reader configuration; please make sure the algorithm is correct and the file exists. Aborting");
                return false;
            }
        }

        KTINFO(avlog_hh, "Successfully set up TMVA reader");

        // Assign variables
        fAverage = powerFitData.GetAverage();
        fRMS = powerFitData.GetRMS();
        fSkewness = powerFitData.GetSkewness();
        fKurtosis = powerFitData.GetKurtosis();
        fNormCentral = powerFitData.GetNormCentral();
        fMeanCentral = powerFitData.GetMeanCentral();
        fSigmaCentral = powerFitData.GetSigmaCentral();
        fNPeaks = powerFitData.GetNPeaks();
        fCentralPowerFraction = powerFitData.GetCentralPowerFraction();
        fRMSAwayFromCentral = powerFitData.GetRMSAwayFromCentral();

        double mvaValue = fReader->EvaluateMVA( fAlgorithm );
        KTDEBUG(avlog_hh, "Evaluated MVA classifier = " << mvaValue);

        // Classify
        KTINFO(evlog, "Classifying track with MVA classifier = " << mvaValue);
        trackData.SetMVAClassifier( mvaValue );

        if( mvaValue >= GetMVACut() )
        {
            KTDEBUG(evlog, "Classifying track as a signal peak");
            trackData.SetMainband( true );
        }
        else
        {
            KTDEBUG(evlog, "Classifying track as a sideband peak");
            trackData.SetMainband( false );
        }

        if( mvaValue <= -999 )
        {
            KTWARN(evlog, "Classifier value is -999; something probably went wrong computing it");
        }
        KTINFO(avlog_hh, "Classification finished!");
    
        return true;
    }
} // namespace Katydid
