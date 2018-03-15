/*
 * KTDLIBClassifier.cc
 *
 *  Created on: Feb 18, 2018
 *      Author: ezayas, L. Saldana
 */

#include "KTDLIBClassifier.hh"

namespace Katydid
{

    KTLOGGER(evlog_hh, "KTDLIBClassifier");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTDLIBClassifier, "dlib-classifier");

    KTDLIBClassifier::KTDLIBClassifier(const std::string& name) :
            KTProcessor(name),
            fDFFile("foo_df.dat"),
            fClassifiedSignal("classified", this),
            fPowerFitSlot("power-fit", this, &KTDLIBClassifier::ClassifyTrack, &fClassifiedSignal)
    {
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

    bool KTDLIBClassifier::ClassifyTrack( KTProcessedTrackData& ptData, KTPowerFitData& pfData, KTClassifierResultsData& resultData)
    {
        sample_type classifierFeatures; // set up 14-dim vector of classification features
        classifierFeatures(0) = (double)(ptData.GetTotalPower());
        classifierFeatures(1) = (double)(ptData.GetSlope());
        classifierFeatures(2) = (double)(ptData.GetTimeLength());
        classifierFeatures(3) = (double)(pfData.GetAverage());
        classifierFeatures(4) = (double)(pfData.GetRMS());
        classifierFeatures(5) = (double)(pfData.GetRMSAwayFromCentral());
        classifierFeatures(6) = (double)(pfData.GetKurtosis());
        classifierFeatures(7) = (double)(pfData.GetSkewness());
        classifierFeatures(8) = (double)(pfData.GetCentralPowerFraction());
        classifierFeatures(9) = (double)(pfData.GetNPeaks());
        classifierFeatures(10) = (double)(pfData.GetMaximumCentral());
        classifierFeatures(11) = (double)(pfData.GetMeanCentral());
        classifierFeatures(12) = (double)(pfData.GetNormCentral());
        classifierFeatures(13) = (double)(pfData.GetSigmaCentral());
        
        normalized_decision_funct_type decisionFunction; // declare normalized ova decision function for SVM with radial basis kernel
        try
        {
            KTDEBUG(avlog_hh,"DF File = " << fDFFile);
            dlib::deserialize(fDFFile) >> decisionFunction; // load train decision function
        }
        catch(...)
        {
            KTERROR(avlog_hh, "Unable to read the decision function from file. Aborting");
            return false;
        }

        int classificationLabel = std::round(decisionFunction(classifierFeatures)); // classify track with trained decision function, i.e gives label for example 0, 1 or 2
                                                                                    // round to nearest integer for comparison

        if( classificationLabel == 0 )
        {
            resultData.SetMainCarrierHigh( 1 );
        }
        else if( classificationLabel == 1 )
        {
            resultData.SetMainCarrierLow( 1 );
        }
        else if( classificationLabel == 2 )
        {
            resultData.SetSideBand( 1 );
        }
        else
        {
            KTERROR(avlog_hh, "Could not assign appropriate classification label; something went wrong");
            return false;
        }

        KTINFO(avlog_hh, "Classification finished!");
        return true;
    }

} // namespace Katydid
