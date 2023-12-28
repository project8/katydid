/*
 * KTDLIBClassifier.cc
 *
 *  Created on: Feb 18, 2018
 *      Author: ezayas, L. Saldana
 */

#include "KTDLIBClassifier.hh"

namespace Katydid
{

    LOGGER(evlog_hh, "KTDLIBClassifier");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTDLIBClassifier, "dlib-classifier");

    KTDLIBClassifier::KTDLIBClassifier(const std::string& name) :
            KTProcessor(name),
            fDFFile("foo_df.dat"),
            fDecisionFunction(),
            fIsInitialized(false),
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

    bool KTDLIBClassifier::Initialize()
    {
        if( ! fIsInitialized )
        {
            try
            {
                LDEBUG(avlog_hh,"DF File = " << fDFFile);
                dlib::deserialize(fDFFile) >> fDecisionFunction; // load train decision function
                fIsInitialized = true;
            }
            catch(dlib::serialization_error& e)
            {
                LERROR(avlog_hh, "Unable to read the decision function from file. Aborting\n" + e.info);
                fIsInitialized = false;
            }
        }

        return fIsInitialized;
    }

    bool KTDLIBClassifier::ClassifyTrack( KTProcessedTrackData& ptData, KTPowerFitData& pfData )
    {
        if( ! Initialize() )
        {
            return false;
        }

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
        
        


        KTClassifierResultsData& resultData = pfData.Of< KTClassifierResultsData >();
        int classificationLabel = std::round(fDecisionFunction(classifierFeatures)); // classify track with trained decision function, i.e gives label for example 0, 1 or 2
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
            LERROR(avlog_hh, "Could not assign appropriate classification label; something went wrong");
            return false;
        }

        LINFO(avlog_hh, "Classification finished!");
        return true;
    }

} // namespace Katydid
