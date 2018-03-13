/*
 * KTDLIBClassifier.cc
 *
 *  Created on: Feb 18, 2018
 *      Author: ezayas, L. Saldana
 */

#include "KTDLIBClassifier.hh"

#include "KTLogger.hh"

#include "KTProcessedTrackData.hh"
#include "KTPowerFitData.hh"

// Undefine to avoid conflict between dlib and scarab logger macros
#undef LINFO
#undef LPROG
#undef LWARN
#undef LERROR
#undef LFATAL
#undef LASSERT
#undef LTRACE
#undef LDEBUG

// dlib stuff
#include <dlib/matrix.h>
#include <dlib/svm_threaded.h>
#include <dlib/svm.h>

namespace Katydid
{

    KTLOGGER(evlog_hh, "KTDLIBClassifier");

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
            resultData.SetMCH( 1 );
        }
        else if( label == 1 )
        {
            resultData.SetMCL( 1 );
        }
        else if( label == 2 )
        {
            resultData.SetSB( 1 );
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
        
        typedef dlib::matrix<double,14,1> sample_type;
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
        typedef dlib::one_vs_all_trainer<dlib::any_trainer< sample_type, double > > ova_trainer;
        typedef dlib::radial_basis_kernel<sample_type> rbf_kernel;
        typedef dlib::one_vs_all_decision_function<ova_trainer,dlib::decision_function<rbf_kernel>> decision_funct_type;
        typedef dlib::normalized_function<decision_funct_type> normalized_decision_funct_type;
        normalized_decision_funct_type decisionFunction; // declare normalized ova decision function for SVM with radial basis kernel
        try
        {
            KTDEBUG(avlog_hh,"DF File = " << fDFFile);
            dlib::deserialize(fDFFile) >> decisionFunction; // load train decision function
        }
        catch(...)
        {
            KTERROR(avlog_hh, "Unable to read the decision function from file. Aborting");
            return;
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
